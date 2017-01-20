extern "C" {
    
#include "eat.h"
#include "files.h"

}
/* General purpose graphics engine.
 * Contains function to load and manage static and moving objects.
 */


int ClipX = 0, ClipY = 0;

/* Attributes handled by the system */
BOOL use_window = FALSE, save_back = FALSE, use_scaling = FALSE, use_clipping = FALSE;

#include <list>
#include <algorithm>

static std::list<anim_t *> DrawList;
static std::list<std::pair<void *, int> > GfxList;


BOOL InitAnimSystem(void)
{
    D(bug("Start: InitAnimSystem!\n"));

    if (!use_window) {
        if (!ClipX || !ClipY) {
            ClipX = os_get_screen_width();
            ClipY = os_get_screen_height();
        }
    }

    return TRUE;
}

template <typename T>
bool InList(const std::list<T *> &l, T *ptr) {
    return std::find(l.begin(), l.end(), ptr) != l.end();
}

/* Questa funzione si occupa di disegnare TUTTI i Bob, prima di disegnarli
    salva lo sfondo, ma solo se l'oggetto si e' spostato
 */

void DrawAnimObj(void)
{
#if defined(_MSC_VER) && _MSC_VER <= 1600
    for each (auto obj in DrawList) {
#else
    for (auto obj: DrawList) {
#endif
        int16_t cf = obj->current_frame;

        if (!use_clipping) {
            if (save_back && obj->bg && obj->moved
                /* obj->x_back!=obj->x_pos||obj->y_back!=obj->y_pos */ ) {
                bltchunkybitmap(main_bitmap, obj->x_pos, obj->y_pos,
                                obj->bg, 0, 0, obj->max_width,
                                obj->max_height, bitmap_width,
                                obj->max_width);

                obj->x_back = obj->x_pos;
                obj->y_back = obj->y_pos;

                obj->moved = FALSE;
            }

            bltanimobj(obj->Frames[cf], main_bitmap, obj->x_pos,
                       obj->y_pos, bitmap_width);
        } else {
            int xs, ys, xd = obj->x_pos, yd = obj->y_pos, w =
                obj->Widths[cf], h = obj->Heights[cf], clipped = FALSE;

            if (xd < 0) {
                xs = (-xd);

                clipped = TRUE;

                if (xs >= obj->Widths[cf])
                    continue;

                w += xd;
                xd = 0;
            } else {
                xs = 0;

                if (xd > (ClipX - w)) {
                    clipped = TRUE;

                    w -= (xd + w - ClipX);

                    if (w <= 0)
                        continue;
                }
            }

            if (yd < 0) {
                clipped = TRUE;

                ys = (-yd);
                h += yd;
                yd = 0;

                if (ys >= obj->Heights[cf])
                    continue;
            } else {
                ys = 0;

                if (yd > (ClipY - h)) {
                    clipped = TRUE;

                    h -= (yd + h - ClipY);

                    if (h <= 0)
                        continue;
                }
            }

            if (clipped)
                bltanimobjclipped(obj->Frames[cf], xs, ys, main_bitmap, xd,
                                  yd, w, h, bitmap_width);
            else
                bltanimobj(obj->Frames[cf], main_bitmap, xd, yd,
                           bitmap_width);
        }
    }
}

/*
 * Find an anim object in the drawlist
 */

BOOL InAnimList(anim_t *obj) {
    return InList(DrawList, obj);
}

void RemAnimObj(anim_t *obj) {
    DrawList.remove(obj);
}
/*
 *  Add an anim object to the drawlist and set its position
 *  and frame.
 */

void AddAnimObj(anim_t * obj, int16_t x, int16_t y, int16_t frame)
{
    DrawList.push_back(obj);

    if (save_back) {
        obj->x_back = x;
        obj->y_back = y;
    }

    obj->x_pos = x;
    obj->y_pos = y;
    obj->current_frame = frame;
    obj->bottom = y + obj->Heights[frame];
    obj->moved = FALSE;

    if (save_back)
        bltchunkybitmap(main_bitmap, obj->x_back, obj->y_back, obj->bg,
                        0, 0, obj->max_width, obj->max_height,
                        bitmap_width, obj->max_width);
}

/*
 * Sort the drawlist to the correct object priorities
 */

void SortDrawList(void)
{
    anim_t *best;
    int16_t best_bottom;
    std::list<anim_t*> TempList;

    while (!DrawList.empty()) {
        best = NULL;
        best_bottom = 30000;

#if defined(_MSC_VER) && _MSC_VER <= 1600
        for each (auto o in DrawList) {
#else
        for (auto o: DrawList) {
#endif
            if (o->Flags & AOBJ_BEHIND) {
                best = o;
                break;
            } else if (o->bottom < best_bottom && !(o->Flags & AOBJ_OVER)) {
                best_bottom = o->bottom;
                best = o;
            }
        }

        if (best) {
// Stacco il nodo dalla lista.

            DrawList.remove(best);

// Lo attacco in coda alla templist
            TempList.push_back(best);
        } else {
// Non c'e' best, quindi probabilmente sono finiti i nodi della lista o ci sono solo nodi OVER

            while (!DrawList.empty()) {
                best = DrawList.front();

                DrawList.pop_front();

// Lo attacco in coda alla templist
                TempList.push_back(best);
            }
            break;
        }
    }
    DrawList = TempList;
}




/* Remove all anim_t from the display, use this only together with save_back */
void ClearAnimObj(void)
{
    /* Leggo la lista al contrario per cancellare correttamente tutto */

    for (auto it = DrawList.rbegin(); it != DrawList.rend(); ++it) {
        anim_t *obj = *it;
        if (obj->bg) {
            bltchunkybitmap(obj->bg, 0, 0, main_bitmap, obj->x_back,
                            obj->y_back, obj->max_width, obj->max_height,
                            obj->max_width, bitmap_width);
        }
    }
}

gfx_t *LoadGfxObject(const char *_name, int32_t * pens)
{
    char name[1024];
    FILE *fh;
    int i;
    uint8_t *planes[8];
    BOOL ok = TRUE;

    sprintf(name, GAME_DIR "%s", _name);

    D(bug("Loading %s...", name));

    if (gfx_t *obj = (gfx_t*)calloc(1, sizeof(gfx_t))) {
        if ((fh = fopen(name, "rb"))) {
            char buffer[4];

            fread(buffer, 4, 1, fh);

            if (strncmp(buffer, "GGFX" /*-*/ , 4)) {
                fclose(fh);
                D(bug("File is not a gfx_t!\n"));
                free(obj);
                return NULL;
            }

            obj->width = fread_u16(fh);
            obj->height = fread_u16(fh);
            obj->realdepth = fread_u16(fh);

            obj->pens = NULL;
            fseek(fh, (1 << obj->realdepth) * 3, SEEK_CUR);


            if ((obj->bmap = (uint8_t*)malloc(obj->width * obj->height))) {
                int planesize = BITRASSIZE(obj->width, obj->height);
                if ((planes[0] = (uint8_t *)malloc(planesize * obj->realdepth))) {
                    for (i = 1; i < obj->realdepth; i++) 
                        planes[i] = (unsigned char *)planes[0] + i * planesize;
                }
                else {
                    free(obj->bmap);
                    ok = FALSE;
                }
            } else {
                D(bug("No memory for main bitmap!\n"));
                ok = FALSE;
            }

            if (ok) {
                for (i = 0; i < obj->realdepth; i++)
                    fread(planes[i], BITRASSIZE(obj->width, obj->height),
                          1, fh);

                do_p2c(planes, obj->bmap, obj->width, obj->height, obj->realdepth, obj->pens);

                /* 15/06/04 - AC: After converting bitplanes in a bitmap, we should
                 * free them.
                 */
                free(planes[0]);

                fclose(fh);

                D(bug
                  ("GfxObject %ld x %ld x %ld\n", obj->width, obj->height,
                   obj->realdepth));

                GfxList.push_back(std::make_pair(obj, TYPE_GFXOBJ));

                if (pens)
                    obj->pens = NULL;

                return obj;
            } 
            fclose(fh);

        } else {
            D(bug("File not found...\n"));
        }

        free(obj);
    } else {
        D(bug("Out of memory.\n"));
    }

    return NULL;
}

anim_t *LoadAnimObject(const char *name, int32_t * pens)
{
    char buffer[4];
    anim_t *obj;
    FILE *fh;
    int i;

    obj = (anim_t*)calloc(1, sizeof(anim_t));
    if(!obj)
        return NULL;

    if(!(fh = fopen(name, "rb"))) {
        D(bug("Unable to open %s for reading...\n", name));
        free(obj);
        return NULL;
    }

    D(bug("Loading %s...", name));

    fread(buffer, 4, 1, fh);
    if(strncmp(buffer, "GOBJ", 4) && strncmp(buffer, "GOBC", 4))
    {
        D(bug("File is not an anim_t!\n"));
        fclose(fh);
        free(obj);
        return NULL;
    }

    obj->nframes = fread_u16(fh);
    obj->max_width = fread_u16(fh);
    obj->max_height = fread_u16(fh);
    obj->real_depth = fread_u16(fh);
    obj->bg = NULL;

    if (save_back) {
        obj->bg = (uint8_t*)malloc(obj->max_width * obj->max_height);
        if(!obj->bg)
        {
            D(bug("Unable to allocate object saveback...\n"));
            fclose(fh);
            free(obj);
            return NULL;
        }
    }

    // we ignore the palette
    fseek(fh, (1 << obj->real_depth) * 3, SEEK_CUR);

    if ((obj->Frames = (MChunky **)calloc(obj->nframes, sizeof(APTR))))
    {
        if ((obj->Widths = (int*)malloc(obj->nframes * sizeof(int))))
        {
            if ((obj->Heights = (int*)malloc(obj->nframes * sizeof(int))))
            {
                BOOL ok = TRUE;

                D(bug
                  ("Loading frames (%ld), mh: %ld mw: %ld rd: %ld\n",
                   obj->nframes, obj->max_width,
                   obj->max_height, obj->real_depth));

                for (i = 0; i < obj->nframes; i++) {
                    obj->Widths[i] = fread_u16(fh);
                    obj->Heights[i] = fread_u16(fh);

//                  D(bug("Conversion to mchunky of %s/%d...", name, i));
                    if (!
                            (obj->Frames[i] =
                             convert_mchunky(fh, NULL,
                                 obj->Widths[i],
                                 obj->Heights[i],
                                 obj->real_depth,
                                 obj->pens))) {
                        ok = FALSE;
                        D(bug("Not enough memory for animobj bitmap!\n"));
                        break;
                    }
//                  D(bug("OK\n"));
                }

                fclose(fh);

                if (!ok) {
                    FreeAnimObj(obj);
                    return NULL;
                }

                D(bug
                  ("Anim Object %s: %ld x %ld / %ld (%ld frames)\n",
                   name, obj->max_width, obj->max_height,
                   obj->real_depth, obj->nframes));

                GfxList.push_back(std::make_pair(obj, TYPE_ANIMOBJ));

                return obj;

            } else {
                D(bug("Non c'e' memoria per obj->Heights.\n"));
            }

            free(obj->Widths);
        } else {
            D(bug("Non c'e' memoria per obj->Widths.\n"));
        }

        free(obj->Frames);
        free(obj);
    } else {
        D(bug("Non c'e' memoria per obj->Frames.\n"));
        free(obj);
    }

    fclose(fh);

    return NULL;
}

void FreeGfxObj(gfx_t * obj)
{
    D(bug("FreeGfxObj - width: %ld\n", obj->width));

#if defined(_MSC_VER) && _MSC_VER <= 1600
    for each( auto &itm in GfxList) {
#else
    for (auto &itm: GfxList) {
#endif
        if (itm.first == obj) {
            GfxList.remove(itm);
            break;
        }
    }
    
    if (obj->Palette) {
        free(obj->Palette);

        if (obj->pens) 
            free(obj->pens);
    }
    if (obj->bmap)
        free(obj->bmap);

    free(obj);
}

void FreeAnimObj(anim_t * obj)
{
    D(bug
      ("FreeAnimObj - frames:%ld flags:%ld\n", obj->nframes,
       obj->Flags));

    DrawList.remove(obj);

#if defined(_MSC_VER) && _MSC_VER <= 1600
    for each (auto &itm in GfxList) {
#else
    for (auto &itm: GfxList) {
#endif
        if (itm.first == obj) {
            GfxList.remove(itm);
            break;
        }
    }

    if (obj->bg)
        free(obj->bg);

    if ((obj->Flags & AOBJ_CLONED) != 0)
        goto fine;

    for (int i = 0; i < obj->nframes; i++)
        if (obj->Frames[i])
            free_mchunky(obj->Frames[i]);

    if ((obj->Flags & AOBJ_COPIED) == 0) {
        if (obj->Widths)
            free(obj->Widths);
        if (obj->Heights)
            free(obj->Heights);
        if (obj->sb)
            free(obj->sb);

    }

    if (obj->pens && ((obj->Flags & AOBJ_SHAREPENS) == 0)) 
        free(obj->pens);

    if (obj->Frames)
        free(obj->Frames);


    if (obj->Palette)
        free(obj->Palette);

  fine:
    free(obj);
}

void FreeGraphics(void)
{
    D(bug("Entering loop...\n"));

    while (!GfxList.empty()) {
        auto &itm = GfxList.front();
        switch (itm.second) {
        case TYPE_GFXOBJ:
            FreeGfxObj((gfx_t *)itm.first);
            break;
        case TYPE_ANIMOBJ:
            FreeAnimObj((anim_t *)itm.first);
            break;
        default:
            D(bug("WARNING Freeing unknown resource!\n"));
            GfxList.pop_front();
            break;
        }
    }
}

void RemapAnimObjColor(anim_t * o, uint8_t source_color, uint8_t dest_color)
{
    int k;
    uint8_t pens[256];

    for (k = 0; k < 256; k++) {
        pens[k] = k;
    }

    pens[source_color] = dest_color;

    for (k = 0; k < o->nframes; k++) {
        RemapMChunkyColors(o->Frames[k], pens);
    }
}

void RemapMColor(struct MChunky *c, uint8_t source_color, uint8_t dest_color)
{
    uint8_t pens[256];

    for (int k = 0; k < 256; k++)
        pens[k] = k;

    pens[source_color] = dest_color;

    RemapMChunkyColors(c, pens);
}

BOOL LoadIFFPalette(char *filename)
{
    FILE *fh;
    uint32_t pal[256 * 3 + 2];
    char buffer[8];
    BOOL rc = FALSE;

    if ((fh = fopen(filename, "rb"))) {
        int cmap_len;
        int i, j, c, colors = 256;
        long l;

        fseek(fh, 0, SEEK_END);
        l = ftell(fh);
        fseek(fh, 0, SEEK_SET);

        fread(buffer, 4, 1, fh);

        if (!strncmp(buffer, "FORM" /*-*/ , 4)) {
            fseek(fh, 4, SEEK_CUR);
            fread(buffer, 4, 1, fh);

            if (!strncmp(buffer, "ILBM" /*-*/ , 4)) {
                for (i = 12; i < l; i += 4) {
                    fread(buffer, 4, 1, fh);

                    if (!strncmp(buffer, "CMAP" /*-*/ , 4)) {
                        cmap_len = fread_u32(fh) / 3;

                        if (cmap_len > colors) {
                            D(bug
                              ("Attenzione l'immagine ha piu' colori dello schermo!\n"));
                        }

                        c = std::min<int>(colors, cmap_len);

                        D(bug
                          ("Loading %ld colors from %s...\n", c, filename));

                        for (j = 0; j < c * 3; j++)
                            pal[j + 1] = (uint32_t)fread_u8(fh) << 24;

                        pal[0] = (uint32_t)c << 16;
                        pal[c * 3 + 1] = 0;
                        os_load_palette(pal);

                        rc = TRUE;
                        i = (int) l + 1;
                    }
                }

                if (i == l) {
                    D(bug("Non trovo il chunk CMAP\n"));
                }
            } else {
                D(bug("Non e' un file ILBM.\n"));
            }
        } else {
            D(bug("Not an IFF file.\n"));
        }

        fclose(fh);
    } else {
        D(bug("Palette file <%s> not found!\n", filename));
    }

    return rc;
}

anim_t *CloneAnimObj(anim_t * obj)
{
    if (anim_t *o = (anim_t*)malloc(sizeof(anim_t))) {
        memcpy(o, obj, sizeof(anim_t));
        o->Flags |= AOBJ_CLONED;

        if (save_back)
            if (!(o->bg = (uint8_t *)malloc(o->max_width * o->max_height))) {
                free(o);
                return NULL;
            }

        GfxList.push_back(std::make_pair(o, TYPE_ANIMOBJ));
        return o;
    }

    return NULL;
}

anim_t *CopyAnimObj(anim_t * obj)
{
    if (anim_t *o = (anim_t *)malloc(sizeof(anim_t))) {
        BOOL ok = TRUE;

        memcpy(o, obj, sizeof(anim_t));

// XXX this is a problem on pocketpc, still have to understand why
        o->Flags = AOBJ_COPIED | AOBJ_SHAREPENS;
                        
        o->pens = NULL;
        o->Palette = NULL;

        if (!(o->Frames = (MChunky**)malloc(o->nframes * sizeof(struct MChunky *)))) {
            free(o);
            return NULL;
        }

        if (save_back)
            if (!(o->bg = (uint8_t*)malloc(o->max_width * o->max_height))) {
                free(o->Frames);
                free(o);
                return NULL;
            }

        for (int i = 0; i < o->nframes; i++) {
            if (!(o->Frames[i] = CloneMChunky(obj->Frames[i])))
                ok = FALSE;

            if (ok == FALSE) {
                int j;

                for (j = 0; j < i; j++)
                    free_mchunky(o->Frames[i]);

                break;
            }
        }

        if (ok) {
            GfxList.push_back(std::make_pair(o, TYPE_ANIMOBJ));
            return o;
        }

        if (save_back && o->bg)
            free(o->bg);

        free(o->Frames);
        free(o);
    }

    return NULL;
}

void LoadGfxObjPalette(const char *name)
{
    uint32_t palette[256 * 3 + 2];
    FILE *fh;
    uint32_t i;
    uint16_t temp, depth;

    if ((fh = fopen(name, "rb"))) {
        fread_u32(fh); /* ignored */
        fread_u16(fh); /* ignored */
        fread_u16(fh); /* ignored */

        temp = fread_u16(fh); /* real depth */
        depth = 1 << std::min<int16_t>(8, temp);

        for (i = 0; i < depth * 3; i++)
            palette[i + 1] = (uint32_t)fread_u8(fh) << 24;

        palette[0] = (uint32_t)depth << 16;
        palette[depth * 3 + 1] = 0;
        os_load_palette(palette);

        D(bug("Loaded %ld colors from gfx_t %s\n", depth, name));

        fclose(fh);
    }
}

void RemapColor(uint8_t *b, uint8_t old, uint8_t newv, int size)
{
    while(size--)
    {
        if(*b == old)
            *b = newv;
        b++;
    }
}

void RemapColors(uint8_t *b, int32_t *pens, int size)
{
    while(size--)
    {
        *b = (uint8_t)pens[*b];
        b++;
    }
}

// ScreenSwap spostata in os_video.c perche' troppo os_dependent!

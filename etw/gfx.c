#include "eat.h"

/* General purpose graphics engine.
 * Contains function to load and manage static and moving objects.
 */

static void AddNode(struct MyList *l, APTR ptr, BYTE type);

int ClipX = 0, ClipY = 0;
int screen_depth;

/* Switch gestiti dal sistema, in seguito potrebbero essere inseriti
    come tags della chiamata InitAnimSystem
 */

BOOL double_buffering = FALSE, public_screen = FALSE;
BOOL use_remapping = TRUE, use_window = FALSE;
BOOL save_back = FALSE, use_scaling = FALSE, use_clipping = FALSE;
BOOL force_single = FALSE;

struct MyList GfxList, DrawList, TempList;


/* Liste necessarie per la gestione degli oggetti */

BOOL InitAnimSystem(void)
{
    D(bug("Start: InitAnimSystem!\n"));

    MyNewList(&DrawList);
    MyNewList(&GfxList);

    if (!public_screen && !use_window) {
        if (!ClipX || !ClipY) {
            ClipX = os_get_screen_width();
            ClipY = os_get_screen_height();
        }

        if (!force_single)
            double_buffering = os_create_dbuffer();
    }

    return TRUE;
}

BOOL InList(struct MyList * l, APTR ptr)
{
    register struct MyNode *n;

    for (n = l->lh_Head; n->ln_Succ; n = n->ln_Succ) {
        if (n == ptr)
            return TRUE;
    }

    return FALSE;
}

struct MyNode *InAList(struct MyList *l, APTR ptr)
{
    register struct MyNode *n;

    for (n = l->lh_Head; n->ln_Succ; n = n->ln_Succ) {
        if (n->ln_Name == ptr)
            return n;
    }

    return NULL;
}



/* Questa funzione si occupa di disegnare TUTTI i Bob, prima di disegnarli
    salva lo sfondo, ma solo se l'oggetto si e' spostato
 */

void DrawAnimObj(void)
{
    register anim_t *obj;
    register WORD cf;

    for (obj = (anim_t *) DrawList.lh_Head; obj->node.mln_Succ;
         obj = (anim_t *) obj->node.mln_Succ) {
        cf = obj->current_frame;

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

BOOL InAnimList(anim_t *obj)
{
    struct MyNode *n;
    
    for (n = DrawList.lh_Head; n->ln_Succ != NULL; n = n->ln_Succ) {
        if (n == (struct MyNode *)obj)
            return TRUE;
    }

    return FALSE;
}

/*
 *  Add an anim object to the drawlist and set its position
 *  and frame.
 */

void AddAnimObj(anim_t * obj, WORD x, WORD y, WORD frame)
{
    MyAddTail(&DrawList, (struct MyNode *) obj);

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
    register anim_t *o, *best;
    register WORD best_bottom;

// Non uso NewList per la velocita'!

    TempList.lh_TailPred = (struct MyNode *) &TempList;
    TempList.lh_Head = (struct MyNode *) &TempList.lh_Tail;
    TempList.lh_Tail = NULL;

    while (DrawList.lh_TailPred != (struct MyNode *) &DrawList) {
        best = NULL;
        best_bottom = 30000;

        for (o = (anim_t *) DrawList.lh_Head; o->node.mln_Succ;
             o = (anim_t *) o->node.mln_Succ) {
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

            best->node.mln_Succ->mln_Pred = best->node.mln_Pred;
            best->node.mln_Pred->mln_Succ = best->node.mln_Succ;

// Lo attacco in coda alla templist

#ifdef OLDCODE
            best->node.mln_Pred =
                (struct MyMinNode *) TempList.lh_TailPred;
            best->node.mln_Succ = (struct MyMinNode *) &TempList.lh_Tail;
            best->node.mln_Succ->mln_Pred = best->node.mln_Pred->mln_Succ =
                (struct MyMinNode *) best;
#else
            best->node.mln_Succ = (struct MyMinNode *) &TempList.lh_Tail;
            best->node.mln_Pred =
                (struct MyMinNode *) TempList.lh_TailPred;
            TempList.lh_TailPred->ln_Succ = (struct MyNode *) best;
            TempList.lh_TailPred = (struct MyNode *) best;
#endif
        } else {
// Non c'e' best, quindi probabilmente sono finiti i nodi della lista o ci sono solo nodi OVER

            while (DrawList.lh_TailPred != (struct MyNode *) &DrawList) {
                best = (anim_t *) DrawList.lh_Head;

// Stacco il nodo dalla list

                best->node.mln_Succ->mln_Pred = best->node.mln_Pred;
                best->node.mln_Pred->mln_Succ = best->node.mln_Succ;

// Lo attacco in coda alla templist

#ifdef OLDCODE
                best->node.mln_Pred =
                    (struct MyMinNode *) TempList.lh_TailPred;
                best->node.mln_Succ =
                    (struct MyMinNode *) &TempList.lh_Tail;
                best->node.mln_Succ->mln_Pred =
                    best->node.mln_Pred->mln_Succ =
                    (struct MyMinNode *) best;
#else
                best->node.mln_Succ =
                    (struct MyMinNode *) &TempList.lh_Tail;
                best->node.mln_Pred =
                    (struct MyMinNode *) TempList.lh_TailPred;
                TempList.lh_TailPred->ln_Succ = (struct MyNode *) best;
                TempList.lh_TailPred = (struct MyNode *) best;
#endif
            }
            break;
        }
    }

// Necessario perche' non e' possibile copiare una lista!

    if (TempList.lh_TailPred != (struct MyNode *) &TempList) {
        DrawList.lh_Head = TempList.lh_Head;
        DrawList.lh_Head->ln_Pred = (struct MyNode *) &DrawList;
        DrawList.lh_TailPred = TempList.lh_TailPred;
        DrawList.lh_TailPred->ln_Succ =
            (struct MyNode *) &DrawList.lh_Tail;
    }
}




/* Remove all anim_t from the display */


void ClearAnimObj(void)
{
    register anim_t *obj;

    /* Leggo la lista al contrario per cancellare correttamente tutto */

    for (obj = (anim_t *) DrawList.lh_TailPred; obj->node.mln_Pred;
         obj = (anim_t *) obj->node.mln_Pred) {
        if (obj->bg) {
            bltchunkybitmap(obj->bg, 0, 0, main_bitmap, obj->x_back,
                            obj->y_back, obj->max_width, obj->max_height,
                            obj->max_width, bitmap_width);

/*
            if(double_buffering)
            {
                obj->x_back=obj->x_pos;
                obj->y_back=obj->y_pos;
            }
*/
        }
    }
}

gfx_t *LoadGfxObject(char *_name, int32_t * pens, uint8_t * dest)
{
    char name[1024];
    gfx_t *obj;
    FILE *fh;
    int i;
    uint16_t temp;
    uint8_t *planes[8];
    BOOL ok = TRUE;

    sprintf(name, GAME_DIR "%s", _name);

    D(bug("Loading %s...", name));

    if ((obj = calloc(1, sizeof(gfx_t)))) {
        if ((fh = fopen(name, "rb"))) {
            char buffer[4];

            fread(buffer, 4, 1, fh);

            if (strncmp(buffer, "GGFX" /*-*/ , 4)) {
                fclose(fh);
                D(bug("File is not a gfx_t!\n"));
                free(obj);
                return NULL;
            }

            fread(&temp, sizeof(uint16_t), 1, fh);
            SWAP16(temp);
            obj->width = temp;
            fread(&temp, sizeof(uint16_t), 1, fh);
            SWAP16(temp);
            obj->height = temp;
            fread(&temp, sizeof(uint16_t), 1, fh);
            SWAP16(temp);
            obj->realdepth = temp;

            if (use_remapping && !pens) {
                if ((obj->Palette = malloc((1 << obj->realdepth) * 3))) {
                    fread(obj->Palette, sizeof(char) * 3,
                          (1 << obj->realdepth), fh);

                    if ((obj->pens =
                        malloc((1 << obj->realdepth) * sizeof(long)))) {
                        for (i = 0; i < (1 << obj->realdepth); i++) {
                            obj->pens[i] = obtain_pen(obj->Palette[i * 3],
                                                      obj->Palette[i * 3 +
                                                                   1],
                                                      obj->Palette[i * 3 +
                                                                   2]);
                        }
                    } else {
                        use_remapping = FALSE;
                        D(bug
                          ("Remapping disabled: low memory.\n"));
                    }
                } else {
                    use_remapping = FALSE;
                    D(bug
                      ("Remapping disabled: low memoriu.\n"));
                }
            } else {
                obj->pens = pens;
                fseek(fh, (1 << obj->realdepth) * 3, SEEK_CUR);
            }

            if (dest) {
                obj->bmap = dest;
            } else {
                if ((obj->bmap = malloc(obj->width * obj->height))) {
                    int planesize = BITRASSIZE(obj->width, obj->height);
                    if ((planes[0] = malloc(planesize * obj->realdepth))) {
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
            }


            if (ok) {
                for (i = 0; i < obj->realdepth; i++)
                    fread(planes[i], BITRASSIZE(obj->width, obj->height),
                          1, fh);

                do_p2c(planes, obj->bmap, obj->width, obj->height,
                       obj->realdepth, obj->pens);

                /* 15/06/04 - AC: After converting bitplanes in a bitmap, we should
                 * free them.
                 */
                free(planes[0]);

                fclose(fh);

                D(bug
                  ("GfxObject %ld x %ld x %ld\n", obj->width, obj->height,
                   obj->realdepth));

                AddNode(&GfxList, obj, TYPE_GFXOBJ);

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

anim_t *LoadAnimObject(char *name, int32_t * pens)
{
    char buffer[4];
    anim_t *obj;
    FILE *fh, *fo = NULL;
    BOOL convert = FALSE;
    int i;

    obj = calloc(1, sizeof(anim_t));
    if(!obj)
        return NULL;

// Routine che cambia dir/name.obj in newgfx/name.objc e vede se esiste gia'
// l'oggetto convertito.

    {
        char bb[100], *c;

        c = name + strlen(name) - 1;

        while (*(c - 1) != '/')
            c--;

        sprintf(bb, NEWGFX_DIR "%s", c);

        bb[strlen(bb) - 1] = 'c';

        if (!(fh = fopen(bb, "rb"))) {
            D(bug("chunky animobj %s not found...\n",  bb));

            if ((fh = fopen(name, "rb"))) {
                convert = TRUE;

#ifndef WINCE
                if (!(fo = fopen(bb, "wb")))
                    D(bug("*** Unable to write to %s\n", bb));
#endif
            }
        }
    }

    if(!fh)
    {
        D(bug("Unable to open file...\n"));
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

    if(fo)
        fwrite("GOBC", 4, 1, fo);

    fread(&obj->nframes, sizeof(uint16_t), 1, fh);
    fread(&obj->max_width, sizeof(uint16_t), 1, fh);
    fread(&obj->max_height, sizeof(uint16_t), 1, fh);
    fread(&obj->real_depth, sizeof(uint16_t), 1, fh);
    if (fo)
    {
        fwrite(&obj->nframes, sizeof(uint16_t), 1, fo);
        fwrite(&obj->max_width, sizeof(uint16_t), 1, fo);
        fwrite(&obj->max_height, sizeof(uint16_t), 1, fo);
        fwrite(&obj->real_depth, sizeof(uint16_t), 1, fo);
    }
    SWAP16(obj->nframes);
    SWAP16(obj->max_width);
    SWAP16(obj->max_height);
    SWAP16(obj->real_depth);

    obj->bg = NULL;

    if (save_back)
    {
        obj->bg = malloc(obj->max_width * obj->max_height);
        if(!obj->bg)
        {
            D(bug("Non riesco ad allocare il saveback...\n"));
            fclose(fh);
            free(obj);
            return NULL;
        }
    }

// this code is out of date with the current ETW game
#ifdef undef
    if (use_scaling)
    {
        obj->sb = malloc(obj->max_width * obj->max_height);
        if(!obj->sb)
        {
            if(obj->bg)
                free(obj->bg);

            D(bug("Non riesco ad allocare lo scale buffer..\n"));
            fclose(fh);
            free(obj);
            return NULL;
        }
    }
#endif

    if (use_remapping && !pens)
    {
        if ((obj->Palette = malloc((1 << obj->real_depth) * 3)))
        {
            fread(obj->Palette, sizeof(char) * 3,
                  (1 << obj->real_depth), fh);
            if (fo)
                fwrite(obj->Palette, sizeof(char) * 3,
                       (1 << obj->real_depth), fo);

            if ((obj->pens =
                malloc((1 << obj->real_depth) * sizeof(int32_t)))) {
                for (i = 0; i < (1 << obj->real_depth); i++) {
                    obj->pens[i] =
                        obtain_pen(obj->Palette[i * 3],
                                   obj->Palette[i * 3 + 1],
                                   obj->Palette[i * 3 + 2]);
                }
            } else {
                use_remapping = FALSE;
                D(bug
                  ("Remapping disabilitato per problemi di memoria.\n"));
            }
        } else {
            use_remapping = FALSE;
            D(bug
              ("Remapping disabilitato per problemi di memoria.\n"));
        }
    } else {
        if (!fo)
            fseek(fh, (1 << obj->real_depth) * 3, SEEK_CUR);
        else {
            char *c = malloc((1 << obj->real_depth) * 3);

            if (c) {
                fread(c, sizeof(char) * 3, (1 << obj->real_depth),
                      fh);
                if (fo)
                    fwrite(c, sizeof(char) * 3,
                           (1 << obj->real_depth), fo);
                free(c);
            }
        }
    }

    if (use_remapping && pens) {
        obj->Flags |= AOBJ_SHAREPENS;
        obj->pens = pens;
    }

    if ((obj->Frames = calloc(obj->nframes, sizeof(APTR)))) {
        if ((obj->Widths = malloc(obj->nframes * sizeof(int)))) {
            if ((obj->Heights =
                malloc(obj->nframes * sizeof(int)))) {
                BOOL ok = TRUE;
                unsigned short tempw;

                D(bug
                  ("Loading frames (%ld), mh: %ld mw: %ld rd: %ld\n",
                   obj->nframes, obj->max_width,
                   obj->max_height, obj->real_depth));

                for (i = 0; i < obj->nframes; i++) {
                    fread(&tempw, sizeof(int16_t), 1, fh);
                    if (fo)
                        fwrite(&tempw, sizeof(int16_t), 1, fo);
                    SWAP16(tempw);
                    obj->Widths[i] = tempw;

                    fread(&tempw, sizeof(int16_t), 1, fh);
                    if (fo)
                        fwrite(&tempw, sizeof(int16_t), 1, fo);
                    SWAP16(tempw);
                    obj->Heights[i] = tempw;

                    if (convert) {
//                        D(bug("Conversion to mchunky of %s/%d...", name, i));
                        if (!
                            (obj->Frames[i] =
                             convert_mchunky(fh, fo,
                                             obj->Widths[i],
                                             obj->Heights[i],
                                             obj->real_depth,
                                             obj->pens))) {
                            ok = FALSE;
                            D(bug
                              ("Non c'e' memoria per le bitmap!\n"));
                            break;
                        }
//                                D(bug("OK\n"));
                    } else {
                        if (!
                            (obj->Frames[i] =
                             load_mchunky(fh, obj->Heights[i],
                                          obj->pens))) {
                            ok = FALSE;
                            D(bug
                              ("Non c'e' memoria per le bitmap!\n"));
                            break;
                        }
                    }
                }

                if (fo)
                    fclose(fo);

                fclose(fh);

                if (!ok) {
                    FreeAnimObj(obj);
                    return NULL;
                }

                D(bug
                  ("Anim Object %s: %ld x %ld / %ld (%ld frames)\n",
                   name, obj->max_width, obj->max_height,
                   obj->real_depth, obj->nframes));

                AddNode(&GfxList, obj, TYPE_ANIMOBJ);

                return obj;

            } else {
                D(bug("Non c'e' memoria per obj->Heights.\n"));
            }

            free(obj->Widths);
        } else {
            D(bug("Non c'e' memoria per obj->Widths.\n"));
        }

        free(obj->Frames);
    } else {
        D(bug("Non c'e' memoria per obj->Frames.\n"));
    }

    if (fo)
        fclose(fo);

    fclose(fh);

    return NULL;
}

void FreeGfxObj(gfx_t * obj)
{
    struct MyNode *n;

    D(bug("FreeGfxObj - width: %ld\n", obj->width));

    if ((n = InAList(&GfxList, obj))) {
        MyRemove(n);
        free(n);
    }

    if (obj->Palette) {
        free(obj->Palette);

        if (obj->pens) {
            int i;

            for (i = 0; i < (1 << obj->realdepth); i++)
                release_pen(obj->pens[i]);

            free(obj->pens);
        }
    }
    if (obj->bmap)
        free(obj->bmap);

    free(obj);
}

void FreeAnimObj(anim_t * obj)
{
    int i;
    struct MyNode *n;

    D(bug
      ("FreeAnimObj - frames:%ld flags:%ld\n", obj->nframes,
       obj->Flags));

    if (InList(&DrawList, obj))
        MyRemove((struct MyNode *) obj);

    if ((n = InAList(&GfxList, obj))) {
        MyRemove(n);
        free(n);
    }

    if (obj->bg)
        free(obj->bg);

    if ((obj->Flags & AOBJ_CLONED) != 0)
        goto fine;

    for (i = 0; i < obj->nframes; i++)
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

    if (obj->pens && ((obj->Flags & AOBJ_SHAREPENS) == 0)) {
        for (i = 0; i < (1 << obj->real_depth); i++)
            release_pen(obj->pens[i]);

        free(obj->pens);
    }

    if (obj->Frames)
        free(obj->Frames);


    if (obj->Palette)
        free(obj->Palette);

  fine:
    free(obj);
}

void FreeGraphics(void)
{
    struct MyNode *n, *next;


    if (double_buffering) {
        D(bug("Freeing double buffering...\n"));
        os_free_dbuffer();
    }
    
    D(bug("Entering loop...\n"));

    for (n = GfxList.lh_Head; n->ln_Succ; n = next) {
        next = n->ln_Succ;

        MyRemove(n);

        switch (n->ln_Type) {
        case TYPE_GFXOBJ:
            D(bug("Freeing a gfx_t.\n"));
            FreeGfxObj((gfx_t *) n->ln_Name);
            break;
        case TYPE_ANIMOBJ:
            D(bug("Freeing a anim_t.\n"));
            FreeAnimObj((anim_t *) n->ln_Name);
            break;
        case TYPE_RASTPORT:
            D(bug("Freeing a RastPort.\n"));
            free(n->ln_Name);
            break;
        case TYPE_SCREENBUFFER:
            D(bug("Freeing a screen buffer...\n"));
// Viene gia' fatta in os_free_dbuffer();
            break;
        case TYPE_BITMAP:
            D(bug("Freeing a bitmap.\n"));
            free(n->ln_Name);
            break;
        default:
            D(bug("WARNING Freeing unknown resource!\n"));
            break;
        }

        free(n);
    }
}

void RemapAnimObjColor(anim_t * o, UBYTE source_color, UBYTE dest_color)
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

void RemapMColor(struct MChunky *c, UBYTE source_color, UBYTE dest_color)
{
    register int k;
    uint8_t pens[256];

    for (k = 0; k < 256; k++) {
        pens[k] = k;
    }

    pens[source_color] = dest_color;

    RemapMChunkyColors(c, pens);
}

void AddNode(struct MyList *l, APTR ptr, BYTE type)
{
    struct MyNode *n;

    if ((n = malloc(sizeof(struct MyNode)))) {
        n->ln_Type = type;
        n->ln_Name = ptr;
        MyAddHead(l, n);
    }
}

BOOL LoadIFFPalette(char *filename)
{
    FILE *fh;
    uint32_t palette[256 * 3 + 2];
    char buffer[8];
    BOOL rc = FALSE;

    if ((fh = fopen(filename, "rb"))) {
        uint32_t cmap_len;
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
                        fread(&cmap_len, sizeof(uint32_t), 1, fh);
                        SWAP32(cmap_len);
                        cmap_len /= 3;

                        if (cmap_len > colors) {
                            D(bug
                              ("Attenzione l'immagine ha piu' colori dello schermo!\n"));
                        }

                        c = min(colors, cmap_len);

                        D(bug
                          ("Loading %ld colors from %s...\n", c, filename));

                        for (j = 0; j < c * 3; j++) {
                            unsigned char cc;

                            fread(&cc, sizeof(char), 1, fh);
                            palette[j + 1] = (uint32_t)cc << 24;
                        }

                        palette[0] = (uint32_t)c << 16;
                        palette[c * 3 + 1] = 0;
                        os_load_palette(palette);

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
    anim_t *o;

    if ((o = malloc(sizeof(anim_t)))) {
        memcpy(o, obj, sizeof(anim_t));
        o->Flags |= AOBJ_CLONED;
        o->node.mln_Succ = o->node.mln_Pred = NULL;

        if (save_back)
            if (!(o->bg = malloc(o->max_width * o->max_height))) {
                free(o);
                return NULL;
            }

        AddNode(&GfxList, o, TYPE_ANIMOBJ);
        return o;

    }

    return NULL;
}

anim_t *CopyAnimObj(anim_t * obj)
{
    anim_t *o;

    if ((o = malloc(sizeof(anim_t)))) {
        BOOL ok = TRUE;
        register int i;

        memcpy(o, obj, sizeof(anim_t));
        o->node.mln_Succ = o->node.mln_Pred = NULL;


// XXX this is a problem on pocketpc, still have to understand why
        o->Flags = AOBJ_COPIED | AOBJ_SHAREPENS;
                        
        o->pens = NULL;
        o->Palette = NULL;

        if (!
            (o->Frames =
             malloc(o->nframes * sizeof(struct MChunky *)))) {
            free(o);
            return NULL;
        }

        if (save_back)
            if (!(o->bg = malloc(o->max_width * o->max_height))) {
                free(o->Frames);
                free(o);
                return NULL;
            }

        for (i = 0; i < o->nframes; i++) {
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
            AddNode(&GfxList, o, TYPE_ANIMOBJ);
            return o;
        }

        if (save_back && o->bg)
            free(o->bg);

        free(o->Frames);
        free(o);
    }

    return NULL;
}

int RemapIFFPalette(char *filename, int32_t *Pens)
{
    FILE *fh;
    char buffer[8];
    unsigned char r, g, b;
    uint32_t cmap_len = 0;

    if ((fh = fopen(filename, "rb"))) {
        long i, j, colors = 256;
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
                        fread(&cmap_len, sizeof(uint32_t), 1, fh);
                        SWAP32(cmap_len);

                        cmap_len /= 3;

                        if (cmap_len > colors) {
                            D(bug
                              ("Attenzione l'immagine ha piu' colori dello schermo!\n"));
                        }

                        for (j = 0; j < cmap_len; j++) {
                            fread(&r, 1, sizeof(char), fh);
                            fread(&g, 1, sizeof(char), fh);
                            fread(&b, 1, sizeof(char), fh);

                            Pens[j] = obtain_pen(r, g, b);
                        }

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
            D(bug("Non e' un file IFF.\n"));
        }
        fclose(fh);
    } else {
        D(bug("Palette file <%s> not found!\n", filename));
    }

    return cmap_len;
}


void LoadGfxObjPalette(char *name)
{
    uint32_t palette[256 * 3 + 2];
    FILE *fh;
    uint32_t i;
    uint16_t temp, depth;

    if ((fh = fopen(name, "rb"))) {
        fread(&i, sizeof(uint32_t), 1, fh);
        fread(&temp, sizeof(uint16_t), 1, fh);
        fread(&temp, sizeof(uint16_t), 1, fh);
        fread(&temp, sizeof(uint16_t), 1, fh);    // Questa e' realdepth
        SWAP16(temp);

        depth = min(screen_depth, temp);

        depth = (1 << depth);

        for (i = 0; i < depth * 3; i++)
        {
            unsigned char c;

            fread(&c, sizeof(char), 1, fh);
            palette[i + 1] = (uint32_t)c << 24;
        }

        palette[0] = (uint32_t)depth << 16;
        palette[depth * 3 + 1] = 0;
        os_load_palette(palette);

        D(bug("Loading %ld colors from gfx_t %s\n", depth, name));

        for (i = 0; i < depth; i++)
            lock_pen(i);

        fclose(fh);
    }
}

void FreeIFFPalette(void)
{
    long depth, i;

    depth = (1 << screen_depth);

    D(bug("Freed %ld colors...\n", depth));

    for(i = 0; i < depth; i++)
        release_pen(i);
}

void RemapColor(uint8_t *b, uint8_t old, uint8_t new, int size)
{
    while(size--)
    {
        if(*b == old)
            *b = new;
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

#include "eat.h"

/* 
 * Questo modulo contiene le varie routine di blitting di gfx/animobj chunky 
 * e le routine di caricamento/conversione/copia degli mchunky, sottoelementi
 * degli animobj che sono invece trattati in gfx.c
 *
 */

// Routine generica di blitting...

#ifndef WINCE
#   define SMART_MCHUNKY
#endif

void bltchunkybitmap(uint8_t * src, int xs, int ys, uint8_t * dest, int xd, int yd,
                     int w, int h, int srcmod, int destmod)
{
    register int i;

    src += (ys * srcmod);
    src += xs;

    dest += (yd * destmod);
    dest += xd;

    if (srcmod == destmod && srcmod == w) {
        memcpy(dest, src, w*h);
        return;
    }

    srcmod -= w;
    destmod -= w;

    while(h--) {
        i = w;

        while(i--)
            *dest++ = *src++;

#ifdef __CODEGUARD__
        /* AC: 17/06/04 - Check if bottom reached */
        if(h != 0)
        {
#endif
            src += srcmod;
            dest += destmod;

#ifdef __CODEGUARD__
        }
#endif
    }
}

void bltanimobjscale(struct scaleAnimObjArgs *args)
{
    register struct ALine *line = args->src->FirstLine;
    struct ABlock *block;
    uint8_t *dest = args->dest + args->xd; 
    register int line_offset, i;
    uint8_t x_ref[800], y_ref[600]; // XXX this is a limit to the animobj size!
    dest += (args->yd*args->destmod);
  
    if (args->ws > sizeof(x_ref) ||
        args->hs > sizeof(y_ref) ) {
        D(bug("Unable to scale such a big animobj!\n"));
        return;
    }
    
    MakeRef(x_ref, args->ws, args->wd);
    MakeRef(y_ref, args->hs, args->hd);
    
    while(args->ys--)
        line = line->Next;

    if (args->hs <= 0)
        return;

    while (args->hs >= 0 && line) {
        args->hs--;

        while (y_ref[args->hs]--) {
            uint8_t *d = dest;
            block = line->FirstBlock;
            line_offset = -args->xs;

            while (block) {
                if (block->Buffer) {
                    for (i = 0; i < block->Length; i++, line_offset++)
                        if (line_offset >= 0 && line_offset < args->ws) {
                            uint8_t k = x_ref[line_offset];

                            while (k--) 
                                *d++ = block->Buffer[i];
                        }
                }
                else {
                    for (i = 0; i < block->Length; i++, line_offset++)
                        if (line_offset >= 0 && line_offset < args->ws)
                            d += x_ref[line_offset];
                }

                block = block->Next;
            }

            dest += args->destmod;
        }
        line = line->Next;
    } 
}

void bltbitmap_x(uint8_t * src, int xs, int ys, uint8_t * dest, int xd, int yd,
                 int w, int h, int srcmod, int destmod, unsigned char noblit)
{
    register int i;

    src += (ys * srcmod);
    src += xs;

    dest += (yd * destmod);
    dest += xd;

    srcmod -= w;
    destmod -= w;

    while(h--) {
        i = w;

        while(i--) {
            if(*src != noblit)
                *dest = *src;

            src++;
            dest++;
        }

        src += srcmod;
        dest += destmod;
    }
}

void do_p2c(unsigned char **p, uint8_t * b, int width, int height, int depth,
            int32_t *pens)
{
    register int k, i, source_color, current_bit;

    if(pens)
    {
        for(k = 0; k < BITRASSIZE(width, height); k++)
        {
            for(current_bit = 128; current_bit > 0; current_bit >>= 1)
            {
                source_color = 0;

                for(i = 0; i < depth; i++)
                {
                    if(p[i][k] & current_bit)
                        source_color |= (1 << i); 
                }

                *b++ = pens[source_color];
            }
        }
    }
    else
    {
        for(k = 0; k < BITRASSIZE(width, height); k++)
        {
            for(current_bit = 128; current_bit > 0; current_bit >>= 1)
            {
                source_color = 0;

                for(i = 0; i < depth; i++)
                {
                    if(p[i][k] & current_bit)
                        source_color |= (1 << i); 
                }

                *b++ = source_color;
            }
        }
    }
}

void bltanimobjclipped(struct MChunky *src, int xs, int ys, uint8_t * dest,
                       int xd, int yd, int w, int h, int destmod)
{
    register struct ALine *line = src->FirstLine;
    register struct ABlock *block;
    register int line_offset, i;

    dest += xd;
    dest += (yd*destmod);

    while(ys > 0)    {
        line = line->Next;
        ys--;
    }

    while (h > 0 && line) {
        block = line->FirstBlock;
        line_offset = -xs;

        while (block) {
            if (block->Buffer) {
                for (i = 0; i < block->Length; i++, line_offset++)
                    if (line_offset >= 0 && line_offset < w)
                        dest[line_offset] = block->Buffer[i];
            }
            else
                line_offset += block->Length;

            block = block->Next;
        }
        
        dest += destmod;
        line = line->Next;
        h--;
    }
}

void bltanimobj(struct MChunky *src, uint8_t * dest, int xd, int yd, int destmod)
{
    register struct ALine *line = src->FirstLine;
    register struct ABlock *block;
    register int line_offset, i;

    dest += xd;
    dest += (yd * destmod);

    while(line) {
        block = line->FirstBlock;
        line_offset = 0;

        while(block) {
            if(block->Buffer) {
                for(i = 0; i < block->Length; i++, line_offset++)
                    dest[line_offset] = block->Buffer[i];
            }
            else
                line_offset += block->Length;

            block = block->Next;
        }

        dest += destmod;

        line = line->Next;
    }
}

#ifdef SMART_MCHUNKY
void free_mchunky(struct MChunky *c)
{
    free(c->FirstLine);
    free(c);
}
#else
void free_mchunky(struct MChunky *c)
{
    struct ALine *l, *lnext;
    struct ABlock *b, *bnext;

    l = c->FirstLine;

    while(l)
    {
        b = l->FirstBlock;

        while(b)
        {
            if(b->Buffer)
                free(b->Buffer);

            bnext = b->Next;
            free(b);
            b = bnext;
        }
        lnext = l->Next;
        free(l);
        l = lnext;
    }
    free(c);
}
#endif

// #define SUPER_DEBUG

#ifdef SMART_MCHUNKY
BOOL create_mchunky(struct MChunky *m, uint16_t *blks, int16_t *bufs,
                    uint8_t *t_data, int32_t *pens)
{
    uint8_t *c;
    int i;

    if(pens)
        for(i = 0; i < m->buffers; i++)
            t_data[i] = pens[t_data[i]];

// Il +4 per sicurezza

#ifdef SUPER_DEBUG
    D(bug("Alloco %ld bytes per l'mchunk\n", sizeof(struct ABlock)*m->blocks+sizeof(struct ALine)*m->lines+m->buffers+4));
#endif

    if ((c = calloc(sizeof(struct ABlock) * m->blocks + sizeof(struct ALine) * m->lines + m->buffers + 4, 1))) {
        int k = 0;
        struct ALine *pl = NULL, *l;
        struct ABlock *pb, *b;

        for(i = 0; i < m->lines; i++) {
            l = (struct ALine *)c;
            c += sizeof(struct ALine);

            if(!pl) {
                m->FirstLine = l;
                pl = l;
            }
            else {
                pl->Next = l;
                pl = l;
            }

            pb = NULL;

            while(blks[i]) {
                b = (struct ABlock *)c;
                c += sizeof(struct ABlock);

                if(!pb) {
                    l->FirstBlock = b;
                    pb = b;
                }
                else {
                    pb->Next = b;
                    pb = b;
                }

                if(bufs[k] > 0) {
                    b->Buffer=c;
                    memcpy(b->Buffer, t_data, bufs[k]);
                    t_data += bufs[k];
                    c += bufs[k];
                    b->Length = bufs[k];
                }
                else {
                    b->Buffer = NULL;
                    b->Length = -bufs[k];
                }
                k++;

                blks[i]--;
            }
        }

#ifdef SUPER_DEBUG
        D(bug("Ho usato %ld bytes...\n", (long int)c - (long int)m->FirstLine));
#endif
        return TRUE;
    }

    return FALSE;
}

#else

BOOL create_mchunky(struct MChunky *m, uint16_t *blks, int16_t *bufs,
                    uint8_t *t_data, int32_t *pens)
{
    int i, k = 0;
    struct ALine *pl = NULL, *l;
    struct ABlock *pb, *b;

    if(pens)
        for(i = 0; i < m->buffers; i++)
            t_data[i] = pens[t_data[i]];

    for(i = 0; i < m->lines; i++)
    {
        l = (struct ALine *)malloc(sizeof(struct ALine));
        l->Next = NULL;
        l->FirstBlock = NULL;

        if(!pl)
        {
            m->FirstLine = l;
            pl = l;
        }
        else
        {
            pl->Next = l;
            pl = l;
        }

        pb = NULL;

        while(blks[i])
        {
            b = (struct ABlock *)malloc(sizeof(struct ABlock));
            b->Next = NULL;

            if(!pb)
            {
                l->FirstBlock = b;
                pb = b;
            }
            else
            {
                pb->Next = b;
                pb = b;
            }

            if(bufs[k] > 0)
            {
                b->Buffer = malloc(bufs[k]);
                memcpy(b->Buffer, t_data, bufs[k]);
                t_data += bufs[k];
                b->Length = bufs[k];
            }
            else
            {
                b->Buffer = NULL;
                b->Length = -bufs[k];
            }

            k++;

            blks[i]--;
        }
    }

    return TRUE;
}
#endif

void save_mchunky(struct MChunky *m, uint16_t *blks, int16_t *bufs,
                  uint8_t *t_data, FILE *f)
{

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
//    int i;
#endif

//    SWAP16(m->blocks);
    fwrite(&m->blocks, sizeof(int16_t), 1, f);
//    SWAP16(m->blocks);

//    SWAP32(m->buffers);
    fwrite(&m->buffers, sizeof(int32_t), 1, f);
//    SWAP32(m->buffers);
/*
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    for(i=0;i<m->lines;i++)
        SWAP16(blks[i]);

    for(i=0;i<m->blocks;i++)
        SWAP16(bufs[i]);
#endif
*/
    fwrite(blks, m->lines, sizeof(int16_t), f);
    fwrite(bufs, m->blocks, sizeof(int16_t), f);
/*
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    for(i=0;i<m->lines;i++)
        SWAP16(blks[i]);

    for(i=0;i<m->blocks;i++)
        SWAP16(bufs[i]);
#endif
*/
    fwrite(t_data, m->buffers, 1, f);
}

struct MChunky *load_mchunky(FILE *f, int height, int32_t *pens)
{
    struct MChunky *m;
    uint8_t *tmp, *t_data;
    uint16_t *blks;
    int16_t *bufs;
    int size;

    m = malloc(sizeof(struct MChunky));
    if(!m)
        goto error;

    m->lines = height;
    fread(&m->blocks, sizeof(int16_t), 1, f);
//  SWAP16(m->blocks);
    fread(&m->buffers, sizeof(int32_t), 1, f);
//  SWAP32(m->buffers);

    size = (m->lines + m->blocks) * sizeof(int16_t) + m->buffers;
    tmp = malloc(size);
    if(!tmp)
    {
        free(m);
        goto error;
    }
    fread(tmp, size, 1, f);

    blks = (uint16_t *)tmp;
    bufs = (int16_t *)(tmp + m->lines * sizeof(int16_t));
    t_data = tmp + (m->lines + m->blocks) * sizeof(int16_t);

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    // Swap m->lines+m->blocks items in tmp
#endif

    if(!create_mchunky(m, blks, bufs, t_data, pens))
    {
        free(tmp);
        free(m);
        goto error;
    }

    free(tmp);
    return m;

error:
    D(bug("Memory error in load_mchunky!\n"));
    return NULL;
}


struct MChunky *convert_mchunky(FILE *f, FILE *fo, int width, int height,
                                int depth, int32_t *pens)
{
    int planesize=BITRASSIZE(width, height), i, k;
    unsigned char *temp, *planes[9];
    uint8_t * chunky;
    int start_blocks = 200;
    int start_datas = 500;
    struct MChunky *m;
    uint8_t *t_data;
    uint16_t *blks;
    int16_t *bufs;

    width = (((width + 15) >> 4) << 4);
    
    if(!(temp = malloc(planesize * (depth + 1) + width * height)))
        return NULL;

    chunky = &temp[planesize * (depth + 1)];

    if(!(m = malloc(sizeof(struct MChunky)))) {
        free(temp);
        return NULL;
    }

    for(i = 0; i < depth; i++)
        planes[i] = &temp[i * planesize];

    fread(temp, planesize * (depth + 1), 1, f);

    planes[8] = &temp[depth * planesize];


// Niente remapping in conversione!

    do_p2c(planes, chunky, width, height, depth, NULL);


    if ((blks = calloc(sizeof(int16_t), height))) {
        if ((t_data = malloc(start_datas * sizeof(uint8_t)))) {
            if ((bufs = malloc(start_blocks * sizeof(int16_t)))) {
                uint8_t linebuffer[640], *chunky_line = chunky;
                int s;
                uint8_t * linea = planes[8];


#ifdef SUPER_DEBUG
                int ln = 1;
                D(bug("elaboro %ldx%ld (%ld)\n", width, height, width));
#endif

                m->lines = m->blocks = 0;
                m->buffers = 0;

                while(height) {
                    // Creo una linea della maschera, prima a 0...

                    memset(linebuffer,0, width);

                    s = 0;

                    // Qui setto i pixel neri a 1....
                    k = 0;

#ifdef SUPER_DEBUG
                    D(bug("- Linea %ld\n", ln));
                    ln++;
#endif
                    while(k < width) {
                        for(i = 128; i > 0; i >>= 1) {
                            if(*linea & i)
                                linebuffer[s] = 1;

                            s++;
                        }

                        linea++;

                        k += 8;
                    }

                    s = 0;

                    while(s<width) {
                        // controllo quanti spazi bianchi ci sono.

                        k = 0;

                        while(linebuffer[s] == 0 && s < width) {
                            k++;
                            s++;
                        }

                        if(s < width) {
                            if(k > 0) {
#ifdef SUPER_DEBUG
                                D(bug("    Blocco vuoto (%ld)\n", k));
#endif
                                if(m->blocks >= (start_blocks - 1)) {
                                    start_blocks <<= 1;
                                    bufs=realloc(bufs, start_blocks * sizeof(int16_t));
                                }

                                bufs[m->blocks] = -k;
                                m->blocks++;
                                blks[m->lines]++;
                            }

                            k = 0;

                            while(linebuffer[s] != 0 && s < width) {
                                k++;
                                s++;
                            }

                            if(k > 0) {
#ifdef SUPER_DEBUG

                                D(bug("    Blocco buffer (%ld)\n", k));
#endif
                                if(m->blocks >= (start_blocks - 1)) {
                                    start_blocks <<= 1;
                                    bufs = realloc(bufs, start_blocks * sizeof(int16_t));
                                }

                                bufs[m->blocks] = k;
                                m->blocks++;
                                blks[m->lines]++;

                                if(m->buffers >= (start_datas - k - 1)) {
                                    start_datas <<= 1;
                                    t_data=realloc(t_data, start_datas);
                                }

                                memcpy(&t_data[m->buffers], &chunky_line[s - k], k);
                                m->buffers += k;
                            }
                        }
                    }
                    chunky_line += width;
                    m->lines++;
                    height--;
                }

                // Dopo la conversione devo anche crearlo!

                if(fo)
                    save_mchunky(m, blks, bufs, t_data, fo);

                if(create_mchunky(m, blks, bufs, t_data, pens)) {
                    free(bufs);
                    free(temp);
                    free(blks);
                    free(t_data);

                    return m;
                }
                free(bufs);
            }
            free(t_data);
        }
        free(blks);
    }

    free(temp);
    free(m);

    return NULL;
}

#ifdef SMART_MCHUNKY

struct MChunky *CloneMChunky(struct MChunky *c)
{
    struct ALine *l, *nl, *pnl = NULL;
    struct ABlock *b, *nb, *pnb;
    struct MChunky *n;
    unsigned char *buf;

    if(!(n = malloc(sizeof(struct MChunky))))
        return NULL;

    if(!(buf = calloc(c->lines * sizeof(struct ALine) + c->blocks * sizeof(struct ABlock) + c->buffers + 4, 1)))
    {
        free(n);    
        D(bug("Non c'e' memoria per clonare il MChunk!\n"));
        return NULL;
    }

    n->lines = c->lines;
    n->blocks = c->blocks;
    n->buffers = c->buffers;

    l = c->FirstLine;

    while(l) {
        nl = (struct ALine *)buf;    
        buf += sizeof(struct ALine);

        if (!pnl) {
            n->FirstLine = nl;
            pnl = nl;
        }
        else {
            pnl->Next = nl;
            pnl = pnl->Next;
        }
    
        b = l->FirstBlock;

// il blocco precedente non deve esserci all'inizio di una linea!

        pnb = NULL;

        while(b)
        {
            nb = (struct ABlock *)buf;
            buf += sizeof(struct ABlock);

            if(!pnb)
            {
                nl->FirstBlock = nb;
                pnb = nb;
            }
            else
            {
                pnb->Next = nb;
                pnb = pnb->Next;
            }
            
            if(b->Buffer)
            {
                nb->Buffer = buf;
                memcpy(nb->Buffer, b->Buffer, b->Length);
                buf += b->Length;
            }

            nb->Length = b->Length;
            b = b->Next;
        }

        l = l->Next;
    }    

    return n;
}
#else

struct MChunky *CloneMChunky(struct MChunky *c)
{
    struct ALine *l, *nl, *pnl = NULL;
    struct ABlock *b, *nb, *pnb;
    struct MChunky *n;

    if(!(n = malloc(sizeof(struct MChunky))))
        return NULL;
    
    n->lines = c->lines;
    n->blocks = c->blocks;
    n->buffers = c->buffers;

    l = c->FirstLine;

    while(l) {
        nl = (struct ALine *)malloc(sizeof(struct ALine));    

        if (!pnl) {
            n->FirstLine = nl;
            pnl = nl;
        }
        else {
            pnl->Next = nl;
            pnl = pnl->Next;
        }
    
        b = l->FirstBlock;

// il blocco precedente non deve esserci all'inizio di una linea!

        pnb = NULL;

        while(b) {
            nb = (struct ABlock *)malloc(sizeof(struct ABlock));

            if(!pnb) {
                nl->FirstBlock = nb;
                pnb = nb;
            }
            else {
                pnb->Next = nb;
                pnb = pnb->Next;
            }
            
            if(b->Buffer) {
                nb->Buffer = malloc(b->Length);
                memcpy(nb->Buffer, b->Buffer, b->Length);
            }

            nb->Length = b->Length;
            b = b->Next;
        }

        l = l->Next;
    }    

    return n;
}
#endif

void RemapMChunkyColors(struct MChunky *m, uint8_t *pens)
{
    struct ALine *l = m->FirstLine;
    struct ABlock *b;

    while(l) {
        b = l->FirstBlock;

        while(b) {
            if(b->Buffer) {
// tolti i "register" per favorire il debugging
                int i;
                uint8_t *c = b->Buffer;

                for(i = b->Length; i > 0; i--) {
                    *c = pens[*c];
                    c++;
                }
            }
            b = b->Next;
        }
        l = l->Next;
    }
}


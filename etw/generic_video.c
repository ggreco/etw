#include "eat.h"
 
// Main bitmap declaration

uint8_t * main_bitmap=NULL;
extern int bitmap_width, bitmap_height;


// 96 chars table for font remapping
uint8_t * char_map[CHAR_ARRAY_LEN];
// Actual font...
uint8_t * font_bm=NULL;
int font_width, font_height;
int font_span;

/* Creation of reference index for scaling */

/* I use fixed point math for speed */

void MakeRef(uint8_t *ref, int source_size, int dest_size)
{

    int i, t, r, m;
/* 
   x = x0/y0 * y 

   line passing from the origin

   based on rothstein code

*/
    m=(source_size<<8)/dest_size;

    memset(ref, 0, source_size);

    for(i=0;i<dest_size;i++)
    {
        t=i+i+1;

        r=(m*t)>>9;

        if (r < source_size)
            ref[r]++;
    }
}

void write_char(uint8_t * bm, char c, uint8_t color)
{
    uint8_t *ch = char_map[c-' '];
    int i, j;

    for(i=0;i<font_height;i++)
    {
        for(j=0;j<font_width;j++)
        {
            if(ch[j])
                bm[j]=color;
        }

#ifdef __CODEGUARD__
        if(i < (font_height -1))
        {
#endif

            ch+=font_span;
            bm+=bitmap_width;

#ifdef __CODEGUARD__
        }
#endif
    }
}

int drawtext(char *buffer, int length, int x, int y, int color)
{
    uint8_t * dest;
    int t=0;

// baseline calculation.

    y-=font_height;
    y++;

    dest=main_bitmap+x+y*bitmap_width;

    while(length)
    {
#ifdef SUPER_DEBUG
        if(*buffer<32||*buffer>=128)
        {
            D(bug("Errore nella stringa %s, contiene <32!\n", buffer-(t/font_width)));
            return t;
        }
#endif
        write_char(dest, *buffer, (uint8_t)color);

        dest+=font_width;
        t+=font_width;
        length--;
        buffer++;
    }
    
    return t;
}

void setfont(struct myfont *f)
{
    int i;

    font_width=f->width;
    font_height=f->height;
    font_span=f->width*CHAR_ARRAY_LEN;

    font_bm=f->bm;

    for(i=0;i<CHAR_ARRAY_LEN;i++)
        char_map[i]=f->bm+(f->width*i);
}

struct myfont *openfont(char *filename)
{
    FILE *f;
    struct myfont *t=NULL;
    char c;

    if ((f=fopen(filename, "rb"))) {
        if ((t=malloc(sizeof(struct myfont))))    {
            fread(&c, sizeof(char), 1, f);
            t->width=(int)c;
            fread(&c, sizeof(char), 1, f);
            t->height=(int)c;

            if ((t->bm=malloc(t->width*t->height*CHAR_ARRAY_LEN))) {
                fread(t->bm, t->width*t->height*CHAR_ARRAY_LEN, 1, f);
                D(bug("Carico font <%s, %ldx%ld>...\n", filename, t->width, t->height));
            }
            else {
                free(t);
                t=NULL;
            }
        }
        fclose(f);
    }

    return t;
}

void closefont(struct myfont *f)
{    
    free(f->bm);
    free(f);
}

void rectfill(uint8_t * b, int x1, int y1, int x2, int y2, uint8_t color, int width)
{
    b+=x1+y1*width;

    x2-=x1;
    x2++;

    y2-=y1;

    while(y2>=0)
    {
        memset(b, color, x2);
      /* AC: For debug */
#ifdef __CODEGUARD__
        if(y2 != 0)
#endif      
            b+=width;
        y2--;
    }
}

#if 0

void rectfill_pattern(uint8_t * b, int x1, int y1, int x2, int y2, uint8_t color, int width)
{
    register int i, k=1;

    b+=x1+y1*width;

    x2-=x1;
    y2-=y1;

    while(y2>=0)
    {
        i=x2;

        i-=k;

        while(i>0)
        {
            b[i]=color;
            i-=2;
        }

        k^=1;

        b+=width;
        y2--;
    }
}
#else

void rectfill_pattern(uint8_t * b, int x1, int y1, int x2, int y2, uint8_t color, int width)
{
    register int i; // , k=1; not used

    b+=x1+y1*width;

    x2-=x1;
    y2-=y1;

    while(y2>=0) {
        i=x2;

        while(i>0) {
            b[i]+=224;  // 224 is the base for half shaded colors
            i--;
        }

        b+=width;
        y2--;
    }
}

#endif

void bitmapScale(struct MyScaleArgs *s)
{
    uint8_t xref[800], yref[600], *src, *dest;

    if(s->SrcWidth>sizeof(xref) || s->SrcHeight>sizeof(yref)) {
        D(bug("Error, src of bitmapScale too big!\n"));
        return;
    }

    MakeRef(xref, s->SrcWidth, s->DestWidth);
    MakeRef(yref, s->SrcHeight, s->DestHeight);

    src=s->Src+s->SrcX+s->SrcY*s->SrcSpan;
    dest=s->Dest+s->DestX+s->DestY*s->DestSpan;

// Note: xref and yref are read from the last to the first element but it's not a problem

    {
        register int i, j, k;
        register uint8_t *sline, *dline;

        i = s->SrcHeight;

        while (i--) {
            while (yref[i]--) {

                sline = src; dline = dest; 
                j = s->SrcWidth;

                while (j--) {
                    k = xref[j];

                    while (k--)
                        *dline++ = *sline;

                    sline++;
                }
                dest+=s->DestSpan;
            }
            src+=s->SrcSpan;
        }
    }
}

void draw(long pen, int x1, int y1, int x2, int y2)
{
    uint8_t * b;
    int t;

    if(x1>x2) {
        t=x1;
        x1=x2;
        x2=t;
    }
    
    if(y1>y2) {
        t=y1;
        y1=y2;
        y2=t;
    }

    b=main_bitmap+x1+y1*bitmap_width;

    if(x1==x2) {
        y2-=y1;

        while(y2>=0) {
            *b=pen;
            b+=bitmap_width;
            y2--;
        }
    }
    else
        memset(b, pen, x2-x1+1);
}

// midpoint algorithm, from foley, van dam

void midpoint_1(long pen, int x0, int y0, int x1, int y1)
{
    uint8_t * b=main_bitmap+x0+y0*bitmap_width;
    int dx=x1-x0;
    int dy=y1-y0;
    int d=2*dy-dx;
    int incrE=2*dy;
    int incrNE=2*(dy-dx);

    *b=pen;

    x1-=x0;
    
    while(x1)
    {
        x1--;

        if(d<=0)
        {
            d+=incrE;
            b++;
        }
        else
        {
            d+=incrNE;
            b++;
            b+=bitmap_width;
        }
        *b=pen;
    }
}

// Uguale a quello classico ma decremento la y perche' ho invertito y1 e y0

void midpoint_3(long pen, int x0, int y0, int x1, int y1)
{
    uint8_t * b=main_bitmap+x0+y0*bitmap_width;
    int dx=x1-x0;
    int dy=y1-y0;
    int d=2*dy-dx;
    int incrE=2*dy;
    int incrNE=2*(dy-dx);

    *b=pen;

    x1-=x0;
    
    while(x1)
    {
        x1--;

        if(d<=0)
        {
            d+=incrE;
            b++;
        }
        else
        {
            d+=incrNE;
            b++;
            b-=bitmap_width;
        }
        *b=pen;
    }
}

// Questo inverte x e y, quindi gli incrementi vanno invertiti

void midpoint_2(long pen, int x0, int y0, int x1, int y1)
{
    uint8_t * b=main_bitmap+y0+x0*bitmap_width;
    int dx=x1-x0;
    int dy=y1-y0;
    int d=2*dy-dx;
    int incrE=2*dy;
    int incrNE=2*(dy-dx);

    *b=pen;

    x1-=x0;
    
    while(x1) {
        x1--;

        if(d<=0) {
            d+=incrE;
            b+=bitmap_width;
        }
        else {
            d+=incrNE;
            b++;
            b+=bitmap_width;
        }
        *b=pen;
    }
}

// Questo inverte x e y, quindi gli incrementi vanno invertiti, in piu' fa il giochetto del 3o

void midpoint_4(long pen, int x0, int y0, int x1, int y1)
{
    uint8_t * b=main_bitmap+y0+x0*bitmap_width;
    int dx=x1-x0;
    int dy=y1-y0;
    int d=2*dy-dx;
    int incrE=2*dy;
    int incrNE=2*(dy-dx);

    *b=pen;

    x1-=x0;
    
    while(x1) {
        x1--;

        if(d<=0) {
            d+=incrE;
            b-=bitmap_width;
        }
        else {
            d+=incrNE;
            b++;
            b-=bitmap_width;
        }
        *b=pen;
    }
}

void freedraw(long pen, int x1, int y1, int x2, int y2)
{
    int t;

// Gestisco i prolungamenti.

    if(x1>x2) {
        t=x1;
        x1=x2;
        x2=t;
        t=y1;
        y1=y2;
        y2=t;
    }

    if(y1>y2) {
        if((x2-x1)>(y1-y2))
            midpoint_3(pen, x1, y1, x2, 2*y1-y2); // ok
        else            
            midpoint_4(pen, y1, x1, 2*y1-y2, x2); // ok
    }
    else {
        if((x2-x1)>(y2-y1))
            midpoint_1(pen, x1, y1, x2, y2); // ok
        else
            midpoint_2(pen, y1, x1, y2, x2); // ok
    }
}

void freepolydraw(long pen, int points, int *index)
{
    while(points>0)    {
        if(index[0]==index[2]||index[1]==index[3])
            draw(pen, index[0], index[1], index[2], index[3]);
        else        
            freedraw(pen, index[0], index[1], index[2], index[3]);

        index+=2;
        points--;
    }
}

void polydraw(long pen, int xs, int ys, int points, int *index)
{
    draw(pen, xs, ys, index[0], index[1]);

    points--;

    while(points>0)    {
        draw(pen, index[0], index[1], index[2], index[3]);
        index+=2;
        points--;
    }
}


void bitmapFastScale(struct MyFastScaleArgs *s)
{
    register uint8_t *xref = s->XRef, *yref = s->YRef;
    register int i=0, j, k, t;
    register uint8_t * sline, * dline, * src=s->Src, * dest=s->Dest;

    i = s->SrcHeight;

    while (i--) {
        t=yref[i];

        while (t--) {
            sline=src;
            dline = dest;

            j = s->SrcWidth;

            while (j--) {
                k = xref[j];

                while (k--)
                    *dline++ = *sline;

                sline++;
            }
            dest+=s->DestSpan;
        }
        src+=s->SrcSpan;
    }
}


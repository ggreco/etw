// #include "etw_locale.h"
#include "menu.h"

struct myfont *bigfont,*smallfont,*titlefont;

WORD restore_x,restore_y,restore_w=0,restore_h=0;

BOOL InitMenuFonts(void)
{
    if(!(bigfont=openfont(BIG_FONT))) /* eat2 */
    {
        D(bug("Unable to open the ETW font!\n"));
        return FALSE;
    }
    if(!(smallfont=openfont(SMALL_FONT))) /* eat */
    {
        D(bug("Unable to open the ETW font!\n"));
        closefont(bigfont);
        return FALSE;
    }
    if(!(titlefont=openfont(TITLE_FONT))) /* eatinfo */
    {
        D(bug("Unable to open the ETW font!\n"));
        closefont(smallfont);
        closefont(bigfont);
        return FALSE;
    }

    D(bug("Font aperti correttamente\n"));

    return TRUE;
}

void FreeMenuFonts(void)
{
    if(bigfont)
    {
        closefont(bigfont);
        bigfont=NULL;
    }

    if(smallfont)
    {
        closefont(smallfont);
        smallfont=NULL;
    }

    if(titlefont)
    {
        closefont(titlefont);
        titlefont=NULL;
    }
}

void PrintShadow(int x, int y, char *t, int l, struct myfont *tf)
{
    setfont(tf);

    TextShadow(x,y+tf->height,t,l);

    restore_x=x;
    restore_y=y;
    restore_w=l*tf->width+1;
    restore_h=tf->height+1;
}

void MyRestoreBack(void)
{
    if(!restore_w||!restore_h)
        return;

     bltchunkybitmap(back,restore_x,restore_y,main_bitmap,
        restore_x,restore_y,
        restore_w,restore_h,bitmap_width,bitmap_width);
}

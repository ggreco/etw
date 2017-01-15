// #include "etw_locale.h"
#include "menu.h"

struct myfont *bigfont,*smallfont,*titlefont;

static int16_t restore_x = 0,restore_y = 0, restore_w=0, restore_h=0;

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

    restore_x = x - 1;
    restore_y = y - 1;
    restore_w = l*tf->width + 2;
    restore_h = tf->height + 2;
}

void MyRestoreBack(void)
{
    int x,y;

    if(!restore_w||!restore_h)
        return;
    x = max(restore_x, 0);
    y = max(restore_y, 0);

    bltchunkybitmap(back, x, y, main_bitmap, x, y,
                     min(restore_w, bitmap_width - x - 1),
                     min(restore_h, bitmap_height - y - 1),
                     bitmap_width, bitmap_width);
}

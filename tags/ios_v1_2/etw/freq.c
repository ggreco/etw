#include "os_defs.h"

static char szFileName[300];

#if defined(WINCE)

#include "mytypes.h"
#include "freq.h"

void SHFullScreen(void) {}


// TODO
BOOL FileRequest(struct MyFileRequest *fr)
{
    return FALSE;
}

#elif defined(WIN)

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highdirent.h"
#include "freq.h"

BOOL FileRequest(struct MyFileRequest *fr)
{
    BOOL res;
    OPENFILENAME f;
    char temp[200];
    char dirbuf[256];
    char olddirbuf[256];
    
    *szFileName = 0;

    strcpy(olddirbuf,".");

    ZeroMemory(&f, sizeof(OPENFILENAME));
    f.lStructSize = sizeof(OPENFILENAME);
    f.hwndOwner = NULL;            // da settare magari con la finestra attiva
    f.hInstance = NULL;
    f.lpstrFile = szFileName;
    f.nMaxFile = sizeof(szFileName) - 1;

    if (!fr->Save)
        f.Flags = OFN_FILEMUSTEXIST;

    if (fr->Filter) {
        int i, l;
        strcpy(temp, fr->Filter);

        l = strlen(temp);
        temp[l + 1] = 0;

        for (i = 0; i < l; i++)
            if (temp[i] == '|')
                temp[i] = 0;

        f.lpstrFilter = temp;
    }

    f.lpstrTitle = fr->Title;

    *dirbuf = 0;

    if (GetCurrentDirectory(sizeof(dirbuf), dirbuf)) {
        int i, l;

        strcpy(olddirbuf,dirbuf);
        if (fr->Dir) {
            l = strlen(dirbuf) - 1;

            if (dirbuf[l] != '/' && dirbuf[l] != '\\')
                strcat(dirbuf, "/");

            strcat(dirbuf, fr->Dir);
        }

        f.lpstrInitialDir = dirbuf;

        l = strlen(dirbuf);

        for (i = 0; i < l; i++)
            if (dirbuf[i] == '/')
                dirbuf[i] = '\\';
    }

    fr->Title = NULL;
    fr->Dir = NULL;
    fr->Filter = NULL;
    fr->File = szFileName;

    /* AC: La GetOpenFileName, cambia la current dir del processo e non va bene */
    /* La devo reimpostare */
    if (!fr->Save)
        res = GetOpenFileName(&f);
    else {
        fr->Save = FALSE;
        res = GetSaveFileName(&f);
    }

    /* AC: Prima di ritornare, reimposta la current DIR */
    SetCurrentDirectory(olddirbuf);
    return res;
}

#elif defined(LINUX) || defined(SOLARIS_X86)

#include "mytypes.h"
#include "freq.h"
#include "mydebug.h"
#include <string.h>
#include <gtk/gtk.h>
#include <unistd.h>

BOOL FileRequest(struct MyFileRequest *fr)
{
    char buffer[200];
    BOOL ok = FALSE;
    GtkWidget *dialog = gtk_file_chooser_dialog_new (fr->Title, NULL,
				      fr->Save ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      fr->Save ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

    fr->File = szFileName;
    if (fr->Dir)
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), fr->Dir);

    D(bug("Open freq (title:%s, dir:%s)...\n", fr->Title ? fr->Title : "NONE", fr->Dir ? fr->Dir : "NONE"));

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        strcpy(fr->File, filename);
        g_free (filename);
        ok = TRUE;
    }

    gtk_widget_destroy (dialog);

    while (gtk_events_pending())
        gtk_main_iteration();

    D(bug("Closed filereq...(%d)\n", ok));
    return ok;
}

#elif defined(MACOSX)

#include "mytypes.h"
#include "freq.h"

BOOL FileRequest(struct MyFileRequest *fr)
{
    extern int MacRequester(struct MyFileRequest *);
    *szFileName = 0;
    fr->File = szFileName;    
    return MacRequester(fr);
}

#elif defined(AMIGA) || defined(AROS)

#include "freq.h"
#include <libraries/asl.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <string.h>

BOOL FileRequest(struct MyFileRequest *fr)
{
    struct FileRequester *f;
    
    if ((f = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest, TAG_DONE))) {
        char  *use_filter = NULL;
        
        if (fr->Filter) {
            use_filter = strrchr(fr->Filter, '|');
    
            if (use_filter) {
                use_filter[0] = '#';
                use_filter[1] = '?';   
            }
        }
        
        if (AslRequestTags(f, 
                    fr->File ? ASLFR_InitialFile : TAG_IGNORE, (uint32_t) fr->File,
                    fr->Dir ? ASLFR_InitialDrawer : TAG_IGNORE, (uint32_t) fr->Dir,
                    ASLFR_DoSaveMode, (BOOL) fr->Save,
                    ASLFR_TitleText, (uint32_t) fr->Title,
                    ASLFR_RejectIcons, TRUE,
                    use_filter ? ASLFR_InitialPattern : TAG_IGNORE, (uint32_t) use_filter,
                    TAG_DONE
                    )) {

            fr->File = szFileName;

            if (f->fr_Drawer)
                strcpy(szFileName, f->fr_Drawer);
            else
                *szFileName = 0;
            
            AddPart(szFileName, f->fr_File, sizeof(szFileName));

            FreeAslRequest(f);

            return TRUE;
        }

        FreeAslRequest(f);
    }
    return FALSE;
}
#else

#include "mytypes.h"
#include "freq.h"

BOOL FileRequest(struct MyFileRequest *fr)
{
    fr->File = szFileName;
    snprintf(szFileName, sizeof(szFileName), "%ssavegame", TEMP_DIR);
    return TRUE;
}
#endif                            /* WIN */

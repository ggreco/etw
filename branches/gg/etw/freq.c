static char szFileName[300];

#if defined(WIN)

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highdirent.h"
#include "freq.h"

BOOL AslRequest(struct MyFileRequest *fr)
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
	f.hwndOwner = NULL;			// da settare magari con la finestra attiva
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

#elif defined(LINUX)

#include "mytypes.h"
#include "freq.h"
#include "mydebug.h"
#include <string.h>
#include <gtk/gtk.h>

typedef struct fdt {
	GtkWidget *fs;
	struct MyFileRequest *ofn;
	int success;
} fsdatas;

void fw_cancel(GtkWidget * w, fsdatas * fs)
{
	fs->success = 0;
	gtk_widget_destroy(fs->fs);
	gtk_main_quit();
}

void fw_ok(GtkWidget * w, fsdatas * fs)
{
	strcpy(fs->ofn->File,
		   gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs->fs)));
	D(bug("Selected: %s\n", fs->ofn->File));
	gtk_widget_destroy(fs->fs);
	fs->success = 1;
	gtk_main_quit();
}

BOOL AslRequest(struct MyFileRequest *fr)
{
	char buffer[200];
	fsdatas fs;
	fs.success = 0;
	fs.ofn = fr;
	fs.fs = gtk_file_selection_new(fr->Title);

	fr->File = szFileName;

	gtk_signal_connect(GTK_OBJECT
					   (GTK_FILE_SELECTION(fs.fs)->cancel_button),
					   "clicked", (GtkSignalFunc) fw_cancel, &fs);
	gtk_signal_connect(GTK_OBJECT(fs.fs), "destroy",
					   (GtkSignalFunc) fw_cancel, &fs);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs.fs)->ok_button),
					   "clicked", (GtkSignalFunc) fw_ok, &fs);

/*	if(!fr->Save)
	    f.Flags = OFN_FILEMUSTEXIST;
 */

	if (fr->Dir) {
		if (getcwd(buffer, sizeof(buffer))) {
			strcat(buffer, "/");
			strcat(buffer, fr->Dir);

			if (buffer[strlen(buffer) - 1] != '/')
				strcat(buffer, "/");

			gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs.fs),
											buffer);
		}
	}

	if (fr->Filter) {
		char *temp = strrchr(fr->Filter, '|');

		if (temp)
			gtk_file_selection_complete(GTK_FILE_SELECTION(fs.fs),
										temp + 1);
	}
//      f.lpstrTitle = fr->Title;


	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(fs.fs));
	gtk_widget_show(fs.fs);

	D(bug("Open freq...\n"));
	gtk_main();
	D(bug("Closed filereq...(%d)\n", fs.success));
	return fs.success;
}

#elif defined(MACOSX)

#include "mytypes.h"
#include "freq.h"

BOOL AslRequest(struct MyFileRequest *fr)
{
	extern int MacRequester(struct MyFileRequest *);
	*szFileName = 0;
	fr->File = szFileName;	
	return MacRequester(fr);
}

#else

#include "mytypes.h"
#include "freq.h"

BOOL AslRequest(struct MyFileRequest *fr)
{
	return FALSE;
}
#endif							/* WIN */
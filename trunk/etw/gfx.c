#include "eat.h"

/* Modulo grafico general purpose,
	contiene funzioni di caricamento/gestione di oggetti statici
	e mobili
 */

void AddNode(struct MyList *l, APTR ptr, BYTE type);

int ClipX = 0, ClipY = 0;
int screen_depth;

/* Switch gestiti dal sistema, in seguito potrebbero essere inseriti
	come tags della chiamata InitAnimSystem
 */

BOOL double_buffering = FALSE, public_screen = FALSE;
BOOL use_remapping = TRUE, use_window = FALSE;
BOOL save_back = TRUE, use_scaling = FALSE, use_clipping = FALSE;
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
	register AnimObj *obj;
	register WORD cf;

	for (obj = (AnimObj *) DrawList.lh_Head; obj->node.mln_Succ;
		 obj = (AnimObj *) obj->node.mln_Succ) {
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
			LONG xs, ys, xd = obj->x_pos, yd = obj->y_pos, w =
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

/* Aggiunge un AnimObj alla drawlist, ne inizializza anche posizione e immagine
	a seconda dei parametri
 */

void AddAnimObj(AnimObj * obj, WORD x, WORD y, WORD frame)
{
	MyAddTail(&DrawList, (struct MyNode *) obj);

// C'erano anche delle condizioni su double_buffering, non capisco bene perche'...

	if (save_back) {
		obj->x_back = x;
		obj->y_back = y;
	}

	obj->x_pos = x;
	obj->y_pos = y;
	obj->current_frame = frame;
	obj->bottom = y + obj->Heights[frame];
	obj->moved = FALSE;

// Come sopra

	if (save_back)
		bltchunkybitmap(main_bitmap, obj->x_back, obj->y_back, obj->bg,
						0, 0, obj->max_width, obj->max_height,
						bitmap_width, obj->max_width);
}

/* Sposta un AnimObj, nel caso il movimento sia verticale controlla anche
	se le priorita' sono cambiate ed aggiorna di conseguenza la lista 
 */

void SortDrawList(void)
{
	register AnimObj *o, *best;
	register WORD best_bottom;

// Non uso NewList per la velocita'!

	TempList.lh_TailPred = (struct MyNode *) &TempList;
	TempList.lh_Head = (struct MyNode *) &TempList.lh_Tail;
	TempList.lh_Tail = NULL;

	while (DrawList.lh_TailPred != (struct MyNode *) &DrawList) {
		best = NULL;
		best_bottom = 30000;

		for (o = (AnimObj *) DrawList.lh_Head; o->node.mln_Succ;
			 o = (AnimObj *) o->node.mln_Succ) {
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
				best = (AnimObj *) DrawList.lh_Head;

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




/* Cancella tutti gli AnimObj */


void ClearAnimObj(void)
{
	register AnimObj *obj;

	/* Leggo la lista al contrario per cancellare correttamente tutto */

	for (obj = (AnimObj *) DrawList.lh_TailPred; obj->node.mln_Pred;
		 obj = (AnimObj *) obj->node.mln_Pred) {
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

GfxObj *LoadGfxObject(char *name, LONG * pens, bitmap dest)
{
	GfxObj *obj;
	FILE *fh;
	int i;
	unsigned short temp;
	UBYTE *planes[8];
	BOOL ok = TRUE;

	D(bug("Loading %s...", name));

	if ((obj = calloc(1, sizeof(struct GfxObject)))) {
		if ((fh = fopen(name, "rb"))) {
			char buffer[4];

			fread(buffer, 4, 1, fh);

			if (strncmp(buffer, "GGFX" /*-*/ , 4)) {
				fclose(fh);
				D(bug("File is not a GfxObj!\n"));
				free(obj);
				return NULL;
			}

			fread(&temp, sizeof(WORD), 1, fh);
			SWAP_WORD(temp);
			obj->width = temp;
			fread(&temp, sizeof(WORD), 1, fh);
			SWAP_WORD(temp);
			obj->height = temp;
			fread(&temp, sizeof(WORD), 1, fh);
			SWAP_WORD(temp);
			obj->realdepth = temp;

			if (use_remapping && !pens) {
				if ((obj->Palette = malloc((1 << obj->realdepth) * 3))) {
					fread(obj->Palette, sizeof(char) * 3,
						  (1 << obj->realdepth), fh);

					if ((obj->Pens =
						malloc((1 << obj->realdepth) * sizeof(long)))) {
						for (i = 0; i < (1 << obj->realdepth); i++) {
							obj->Pens[i] = obtain_pen(obj->Palette[i * 3],
													  obj->Palette[i * 3 +
																   1],
													  obj->Palette[i * 3 +
																   2]);
						}
					} else {
						use_remapping = FALSE;
						D(bug
						  ("Remapping disabilitato for low memory.\n"));
					}
				} else {
					use_remapping = FALSE;
					D(bug
					  ("Remapping disabilitato per problemi di memoria.\n"));
				}
			} else {
				obj->Pens = pens;
				fseek(fh, (1 << obj->realdepth) * 3, SEEK_CUR);
			}

			if (dest) {
				obj->bmap = dest;
			} else {
				if ((obj->bmap = malloc(obj->width * obj->height))) {
					for (i = 0; i < obj->realdepth; i++) {
						if (!
							(planes[i] =
							 malloc(BITRASSIZE(obj->width, obj->height))))
							ok = FALSE;
					}
				} else {
					D(bug("Non c'e' memoria per la bitmap!\n"));
					ok = FALSE;
				}
			}


			if (ok) {
				for (i = 0; i < obj->realdepth; i++)
					fread(planes[i], BITRASSIZE(obj->width, obj->height),
						  1, fh);

				do_p2c(planes, obj->bmap, obj->width, obj->height,
					   obj->realdepth, obj->Pens);

				fclose(fh);

				D(bug
				  ("GfxObject %ld x %ld x %ld\n", obj->width, obj->height,
				   obj->realdepth));

				AddNode(&GfxList, obj, TYPE_GFXOBJ);

				if (pens)
					obj->Pens = NULL;

				return obj;
			} else {
				D(bug("Non c'e' memoria per la bitmap temporanea...\n"));
			}

		} else {
			D(bug("File not found...\n"));
		}

		free(obj);
	} else {
		D(bug("Fine memoria.\n"));
	}

	return NULL;
}

AnimObj *LoadAnimObject(char *name, LONG * pens)
{
	AnimObj *obj;
	FILE *fh, *fo = NULL;
	BOOL convert = FALSE;

	if ((obj = calloc(1, sizeof(struct AnimObject)))) {
// Routine che cambia dir/nome.obj in newgfx/nome.objc e vede se esiste gia'
// l'oggetto convertito.

		{
			char bb[100], *c;

			c = name + strlen(name) - 1;

			while (*(c - 1) != '/')
				c--;

			sprintf(bb, NEWGFX_DIR "%s", c);

			bb[strlen(bb) - 1] = 'c';

			if (!(fh = fopen(bb, "rb"))) {
				D(bug("chunky animobj not found...\n"));

				if ((fh = fopen(name, "rb"))) {
					convert = TRUE;

					if (!(fo = fopen(bb, "wb")))
						D(bug("*** Unable to write to %s\n", bb));
				}
			}
		}

		if (fh) {
			char buffer[4];
			LONG i;

			D(bug("Loading %s...", name));


			fread(buffer, 4, 1, fh);

			if (strncmp(buffer, "GOBJ" /*-*/ , 4)
				&& strncmp(buffer, "GOBC" /*-*/ , 4)) {
				fclose(fh);
				D(bug("File is not an AnimObj!\n"));
				free(obj);
				return NULL;
			}

			if (fo)
				fwrite("GOBC", 4, 1, fo);

			fread(&obj->num_frames, sizeof(WORD), 1, fh);
			if (fo)
				fwrite(&obj->num_frames, sizeof(WORD), 1, fo);
			SWAP_WORD(obj->num_frames);

			fread(&obj->max_width, sizeof(WORD), 1, fh);
			if (fo)
				fwrite(&obj->max_width, sizeof(WORD), 1, fo);
			SWAP_WORD(obj->max_width);

			fread(&obj->max_height, sizeof(WORD), 1, fh);
			if (fo)
				fwrite(&obj->max_height, sizeof(WORD), 1, fo);
			SWAP_WORD(obj->max_height);

			fread(&obj->RealDepth, sizeof(WORD), 1, fh);
			if (fo)
				fwrite(&obj->RealDepth, sizeof(WORD), 1, fo);
			SWAP_WORD(obj->RealDepth);

			obj->bg = NULL;

			if (save_back) {
				if (!(obj->bg = malloc(obj->max_width * obj->max_height))) {
					D(bug("Non riesco ad allocare il saveback...\n"));
					free(obj);
					fclose(fh);
					return NULL;
				}
			}

			if (use_scaling) {
				if (!(obj->sb = malloc(obj->max_width * obj->max_height))) {
					if (obj->bg)
						free(obj->bg);

					D(bug("Non riesco ad allocare lo scale buffer..\n"));
					free(obj);
					fclose(fh);
					return NULL;
				}
			}

			if (use_remapping && !pens) {
				if ((obj->Palette = malloc((1 << obj->RealDepth) * 3))) {
					fread(obj->Palette, sizeof(char) * 3,
						  (1 << obj->RealDepth), fh);
					if (fo)
						fwrite(obj->Palette, sizeof(char) * 3,
							   (1 << obj->RealDepth), fo);

					if ((obj->Pens =
						malloc((1 << obj->RealDepth) * sizeof(LONG)))) {
						for (i = 0; i < (1 << obj->RealDepth); i++) {
							obj->Pens[i] =
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
					fseek(fh, (1 << obj->RealDepth) * 3, SEEK_CUR);
				else {
					char *c = malloc((1 << obj->RealDepth) * 3);

					if (c) {
						fread(c, sizeof(char) * 3, (1 << obj->RealDepth),
							  fh);
						if (fo)
							fwrite(c, sizeof(char) * 3,
								   (1 << obj->RealDepth), fo);
						free(c);
					}
				}
			}

			if (use_remapping && pens) {
				obj->Flags |= AOBJ_SHAREPENS;
				obj->Pens = pens;
			}

			if ((obj->Frames = calloc(obj->num_frames, sizeof(APTR)))) {
				if ((obj->Widths = malloc(obj->num_frames * sizeof(LONG)))) {
					if ((obj->Heights =
						malloc(obj->num_frames * sizeof(LONG)))) {
						BOOL ok = TRUE;
						unsigned short tempw;

						D(bug
						  ("Carico i frames (%ld), mh: %ld mw: %ld rd: %ld\n",
						   obj->num_frames, obj->max_width,
						   obj->max_height, obj->RealDepth));

						for (i = 0; i < obj->num_frames; i++) {
							fread(&tempw, sizeof(WORD), 1, fh);
							if (fo)
								fwrite(&tempw, sizeof(WORD), 1, fo);
							SWAP_WORD(tempw);
							obj->Widths[i] = tempw;

							fread(&tempw, sizeof(WORD), 1, fh);
							if (fo)
								fwrite(&tempw, sizeof(WORD), 1, fo);
							SWAP_WORD(tempw);
							obj->Heights[i] = tempw;

							if (convert) {
								if (!
									(obj->Frames[i] =
									 convert_mchunky(fh, fo,
													 obj->Widths[i],
													 obj->Heights[i],
													 obj->RealDepth,
													 obj->Pens))) {
									ok = FALSE;
									D(bug
									  ("Non c'e' memoria per le bitmap!\n"));
									break;
								}
							} else {
								if (!
									(obj->Frames[i] =
									 load_mchunky(fh, obj->Heights[i],
												  obj->Pens))) {
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
						   obj->RealDepth, obj->num_frames));

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
		} else {
			D(bug("Unable to open file...\n"));
		}

		free(obj);
	}

	return NULL;
}

void FreeGfxObj(GfxObj * obj)
{
	struct MyNode *n;

	D(bug("FreeGfxObj - width: %ld\n", obj->width));

	if ((n = InAList(&GfxList, obj))) {
		MyRemove(n);
		free(n);
	}

	if (obj->Palette)
		free(obj->Palette);

	if (obj->Pens) {
		int i;

		for (i = 0; i < (1 << obj->realdepth); i++)
			release_pen(obj->Pens[i]);

		free(obj->Pens);
	}

	if (obj->bmap)
		free(obj->bmap);

	free(obj);
}

void FreeAnimObj(AnimObj * obj)
{
	int i;
	struct MyNode *n;

	D(bug
	  ("FreeAnimObj - frames:%ld flags:%ld\n", obj->num_frames,
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

	for (i = 0; i < obj->num_frames; i++)
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

	if (obj->Frames)
		free(obj->Frames);

	if (obj->Pens && ((obj->Flags & AOBJ_SHAREPENS) == 0)) {
		for (i = 0; i < (1 << obj->RealDepth); i++)
			release_pen(obj->Pens[i]);

		free(obj->Pens);
	}

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
			D(bug("Freeing a GfxObj.\n"));
			FreeGfxObj((GfxObj *) n->ln_Name);
			break;
		case TYPE_ANIMOBJ:
			D(bug("Freeing a AnimObj.\n"));
			FreeAnimObj((AnimObj *) n->ln_Name);
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

void RemapAnimObjColor(AnimObj * o, UBYTE source_color, UBYTE dest_color)
{
	int k;
	unsigned char pens[256];

	for (k = 0; k < 256; k++) {
		pens[k] = k;
	}

	pens[source_color] = dest_color;

	for (k = 0; k < o->num_frames; k++) {
		RemapMChunkyColors(o->Frames[k], pens);
	}
}

void RemapMColor(struct MChunky *c, UBYTE source_color, UBYTE dest_color)
{
	register int k;
	unsigned char pens[256];

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
	ULONG palette[256 * 3 + 2];
	char buffer[8];
	BOOL rc = FALSE;

	if ((fh = fopen(filename, "rb"))) {
		long cmap_len;
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
						fread(&cmap_len, sizeof(long), 1, fh);
						SWAP_LONG(cmap_len);
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
							palette[j + 1] = cc << 24;
						}

						palette[0] = c << 16;
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

AnimObj *CloneAnimObj(AnimObj * obj)
{
	AnimObj *o;

	if ((o = malloc(sizeof(struct AnimObject)))) {
		memcpy(o, obj, sizeof(struct AnimObject));
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

AnimObj *CopyAnimObj(AnimObj * obj)
{
	AnimObj *o;

	if ((o = malloc(sizeof(struct AnimObject)))) {
		BOOL ok = TRUE;
		register int i;

		memcpy(o, obj, sizeof(struct AnimObject));
		o->node.mln_Succ = o->node.mln_Pred = NULL;

		o->Flags = (AOBJ_COPIED | AOBJ_SHAREPENS);
		o->Pens = NULL;
		o->Palette = NULL;

		if (!
			(o->Frames =
			 malloc(o->num_frames * sizeof(struct MChunky *)))) {
			free(o);
			return NULL;
		}

		if (save_back)
			if (!(o->bg = malloc(o->max_width * o->max_height))) {
				free(o->Frames);
				free(o);
				return NULL;
			}

		for (i = 0; i < o->num_frames; i++) {
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

LONG RemapIFFPalette(char *filename, LONG * Pens)
{
	FILE *fh;
	char buffer[8];
	unsigned char r, g, b;
	LONG cmap_len = 0;

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
						fread(&cmap_len, sizeof(long), 1, fh);
						SWAP_LONG(cmap_len);

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
	FILE *fh;
	LONG i;
	ULONG palette[256 * 3 + 2];
	unsigned short temp, depth;

	if ((fh = fopen(name, "rb"))) {
		fread(&i, sizeof(LONG), 1, fh);
		fread(&temp, sizeof(WORD), 1, fh);
		fread(&temp, sizeof(WORD), 1, fh);
		fread(&temp, sizeof(WORD), 1, fh);	// Questa e' realdepth
		SWAP_WORD(temp);

		depth = min(screen_depth, temp);

		depth = (1 << depth);

		for (i = 0; i < depth * 3; i++) {

			unsigned char c;


			fread(&c, sizeof(char), 1, fh);
			palette[i + 1] = c << 24;
		}

		palette[0] = depth << 16;
		palette[depth * 3 + 1] = 0;
		os_load_palette(palette);

		D(bug("Loading %ld colors from GfxObj %s\n", depth, name));

		for (i = 0; i < depth; i++)
			lock_pen(i);

		fclose(fh);
	}
}

void FreeIFFPalette(void)
{
	long depth, i;

	depth = (1 << screen_depth);

	D(bug("Freeed %ld colors...\n", depth));

	for (i = 0; i < depth; i++)
		release_pen(i);
}

void RemapColor(bitmap b, UBYTE old, UBYTE new, int size)
{
	while (size) {
		if (*b == old)
			*b = new;

		b++;
		size--;
	}
}

void RemapColors(bitmap b, long *pens, int size)
{
	while (size) {
		*b = (unsigned char) pens[*b];
		b++;
		size--;
	}
}


// ScreenSwap spostata in os_video.c perche' troppo os_dependent!

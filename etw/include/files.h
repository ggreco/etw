#ifndef FILES_H

#define FILES_H

// use on boolean to handle them as bytes
#define WRITE_BOOL(var, file) { unsigned char c; c = var; fwrite(&c, sizeof(c), 1, file); }
#define READ_BOOL(var, file) { unsigned char c; fread(&c, sizeof(c), 1, file); var = c; }

// use on datas that need swapping
#define WRITE_LONG(var, file) { ULONG c; c = var; SWAP_LONG(c); fwrite(&c, sizeof(c), 1, file); }
#define WRITE_WORD(var, file) { UWORD c; c = var; SWAP_WORD(c); fwrite(&c, sizeof(c), 1, file); }
#define READ_WORD(var, file) { UWORD c;  fread(&c, sizeof(c), 1, file); SWAP_WORD(c); var = c; }
#define READ_LONG(var, file) { ULONG c;  fread(&c, sizeof(c), 1, file); SWAP_LONG(c); var = c; }

// these can be used for datas that doesn't need swapping
#define WRITE_DATA(var, file) fwrite(&var, sizeof(var), 1, file)
#define READ_DATA(var, file) fread(&var, sizeof(var), 1, file)

#endif

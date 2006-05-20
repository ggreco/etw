#ifndef FILES_H
#define FILES_H

static inline void fwrite_u8(uint8_t var, FILE *f)
{
    fwrite(&var, sizeof(uint8_t), 1, f);
}

static inline uint8_t fread_u8(FILE *f)
{
    uint8_t c;
    fread(&c, sizeof(uint8_t), 1, f);
    return c;
}

static inline void fwrite_u16(uint16_t var, FILE *f)
{
    var = SDL_Swap16(var);
    fwrite(&var, sizeof(uint16_t), 1, f);
}

static inline uint16_t fread_u16(FILE *f)
{
    uint16_t c;
    fread(&c, sizeof(uint16_t), 1, f);
    return SDL_Swap16(c);
}

static inline void fwrite_u32(uint32_t var, FILE *f)
{
    var = SDL_Swap32(var);
    fwrite(&var, sizeof(uint32_t), 1, f);
}

static inline uint32_t fread_u32(FILE *f)
{
    uint32_t c;
    fread(&c, sizeof(uint32_t), 1, f);
    return SDL_Swap32(c);
}

/* These can be used for data that doesn't need byteswap, eg. strings */
static inline void fwrite_data(void *addr, unsigned int size, FILE *f)
{
    fwrite(addr, size, 1, f);
}

static inline void fread_data(void *addr, unsigned int size, FILE *f)
{
    fread(addr, size, 1, f);
}

#endif

#if !defined(HIGHDIRENT_H)

#define HIGHDIRENT_H

#if defined(WIN)

#include <io.h>

#define MAXNAMLEN	512	/* maximum filename length  */
#define DIRBUF		1048	/* buffer size for fs-indep. dirs  */

#define _DIR

struct dirent				/* data from readdir() */
	{
	char		d_name[257];	/* name of file */
	};


typedef struct {
	long Handle;
	char DirName[257];
	struct dirent DirentBuf;
	struct _finddata_t FindBuf;

} DIR;            /* stream data from opendir()  */

typedef struct dirent	dirent_t;

extern DIR *opendir(char *DirName);
extern dirent_t *readdir(DIR *Directory);
extern void closedir(DIR *);

/*

extern DIR *eopendir(const char *);
extern DIR	*opendir(const char *);
extern struct dirent	*readdir(DIR *);
extern void	rewinddir(DIR *);
extern long	telldir(DIR *);
extern void	seekdir(DIR *, long);
#define rewinddir(p)	seekdir(p, 0L)

extern struct dirent	*readdir_r(DIR *, struct dirent *);

*/

#elif defined(__SASC)

#include <sys/dir.h>

#else

#include <sys/types.h>
#include <sys/dir.h>

#endif

#endif /* HIGHDIRENT_H */

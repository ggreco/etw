#ifndef MYFREQ



#define MYFREQ



struct MyFileRequest

{

	char *Title;

	char *Dir;

	char *Filter;

	char *File;
#ifdef MACOSX
	/*: Il tipo BOOL in MacOS X esiste gi� ed � un char, purtroppo */
	int Save;
#else
	BOOL Save;
#endif
};



#endif

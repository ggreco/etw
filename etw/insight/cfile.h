#ifndef CFILE_H

#define CFILE_H

// piccola classe per gestire i file con le API C x renderli + amichevoli con le exception,
// la classe e' crossplatform win32/osx/linux e supporta i nomi UTF-8 anche su windows

#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef WIN32
// it's a warning on any not visual studio compiler!
#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif
#include <winsock2.h>
#include <windows.h>
#define PATH_CHARS "/\\"
#define DEFAULT_PATH_CHAR "\\"
#define DEFAULT_ROOT_CHAR ""
#define DEFAULT_PATH_CHAR_AS_CHAR '\\'

inline FILE * myopen(std::string src, const char *m) {
    // first we change path component "/" to "\\"
    for (auto &i: src) if (i == '/') i = '\\';
#ifndef _MSC_VER
    wchar_t wide[src.size() + 1], wm[8];
#else
    wchar_t *wide = (wchar_t *)_alloca(sizeof(wchar_t) * (src.size() + 1)), wm[8];
#endif
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, wide, (int)src.size() + 1);
    MultiByteToWideChar(CP_UTF8, 0, m, -1, wm, 8);
    return ::_wfopen(wide, wm);
}
inline std::wstring widename(std::string src) {
    for (auto &i: src) if (i == '/') i = '\\';
#ifndef _MSC_VER
    wchar_t wide[src.size() + 1];
#else
    wchar_t *wide = (wchar_t *)_alloca(sizeof(wchar_t) * (src.size() + 1));
#endif
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, wide, (int)src.size() + 1);
    return wide;
}
typedef _WDIR MDIR;
typedef _wdirent dent;
typedef struct _stat64 sstat;
// su windows readdir e' rientrante
inline int mreaddir(_WDIR *d, _wdirent *ent, _wdirent **res) {
    if ((*res = _wreaddir(d)))
        return 0;
    else
        return -1;
}
inline std::string utf8name(const std::wstring &str)
{
#ifndef _MSC_VER
    char dest[str.length() * 3 + 1];
#else
    char *dest = (char *)_alloca(sizeof(char) * (str.length() * 3 + 1));
#endif
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, dest, (int)str.length() * 3 + 1, NULL, NULL);
    return dest;
}
inline const char *mgetcwd(char * x, size_t l) {
    wchar_t *b = (wchar_t*)x;
    wchar_t *r = _wgetcwd(b, (int)(l/sizeof(wchar_t)));
    if (r != nullptr) {
        std::string rutf8 = utf8name(r);
        strncpy(x, rutf8.c_str(), l);
        return x;
    }
    return "";
}

#define makedir(x,y) ::_wmkdir(widename(x).c_str())
#define openmdir(x) ::_wopendir(widename(x).c_str())
#define mrename(x,y) ::_wrename(widename(x).c_str(),widename(y).c_str())
#define closemdir ::_wclosedir
#define mstat(x,y) ::_wstat64(widename(x).c_str(), y)
#define mfstat ::_fstat64
#define mremove(x) ::_wremove(widename(x).c_str())
#define mrmdir(x) ::_wrmdir(widename(x).c_str())
#define mtell ::_ftelli64
#define mseek ::_fseeki64
#define mcopy(s,d) CopyFileW(widename(s).c_str(), widename(d).c_str(), FALSE)
#ifndef _MSC_VER
#define mfscanf __mingw_fscanf
#define msscanf __mingw_sscanf
#else
#define mfscanf fscanf
#define msscanf sscanf
#endif
#define mchmod(x,y) _wchmod(widename(x).c_str(), y)
#define msymlink_file(x,y) CreateSymbolicLinkW(widename(x).c_str(), widename(y).c_str(), 0)
#define msymlink_dir(x,y) CreateSymbolicLinkW(widename(x).c_str(), widename(y).c_str(), 1)
#else
#include <unistd.h>

#ifdef __APPLE__
#include <copyfile.h>
#define mcopy(s,d) (copyfile(s.c_str(), d.c_str(), NULL, COPYFILE_DATA|COPYFILE_UNLINK) == 0)
#define mreaddir readdir_r
#else
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

inline bool mcopy(const std::string &src, const std::string &dst) {
    int source = ::open(src.c_str(), O_RDONLY | O_LARGEFILE, 0);
    if (source < 0)
        return false;
    int dest = ::open(dst.c_str(), O_WRONLY | O_CREAT | O_LARGEFILE, 0644);
    if (dest < 0) {
        ::close(source);
        return false;
    }

    // struct required, rationale: function stat() exists also
    struct stat stat_source;
    ::fstat(source, &stat_source);

    const int buffersize = 1024 * 1024;
    char *buffer = (char *)malloc(buffersize);
    long long rc = 0;
    while (rc < stat_source.st_size) {
        size_t len = ::read(source, buffer, buffersize);
        if (len < 0)
            break;

        size_t res = ::write(dest, buffer, len);
        if (res < 0)
            break;

        rc += res;
    }
    ::free(buffer);
    ::close(source);
    ::close(dest);

    return rc == stat_source.st_size;
}

// e pure su linux!
inline int mreaddir(DIR *d, dirent *ent, dirent **res) {
    if ((*res = readdir(d)))
        return 0;
    else
        return -1;
}
#endif
#define PATH_CHARS "/"
#define DEFAULT_PATH_CHAR "/"
#define DEFAULT_ROOT_CHAR "/"
#define DEFAULT_PATH_CHAR_AS_CHAR '/'

typedef DIR MDIR;
typedef dirent dent;
typedef struct stat sstat;
#define mfscanf ::fscanf
#define msscanf ::sscanf
#define mchmod ::chmod
#define makedir(x,y) ::mkdir(x.c_str(), y)
#define openmdir(x) ::opendir(x.c_str())
#define closemdir ::closedir
#define utf8name(x) x
#define myopen(s, m) ::fopen(s.c_str(), m)
#define mrename(x,y) ::rename(x.c_str(),y.c_str())
#define mstat(x,y) ::stat(x.c_str(), y)
#define mremove(x) ::remove(x.c_str())
#define mrmdir(x) ::rmdir(x.c_str())
#define mtell ::ftell
#define mfstat ::fstat
#define mseek ::fseek
#define msymlink_file(x,y) (::symlink(x.c_str(), y.c_str()) == 0)
#define msymlink_dir(x,y) (::symlink(x.c_str(), y.c_str()) == 0)
#define mgetcwd ::getcwd
#endif

/** This class can be used to handle files in a cross-platform way with faster performances that standard C++ ifstreams */
class CFile
{
    FILE *file_;

    public:
        /** Opening modes for files */
        enum Mode {Read, Write, Append};
        /** Seek types */
        enum SeekPos {Start = SEEK_SET, Current = SEEK_CUR, End = SEEK_END};
        /** Create a CFile object without opening a file */
        CFile() : file_(NULL) {}
        /** Create a CFIle object opening a file with a particular mode, mode defaults to CFile::Read */
        CFile(const std::string &name, Mode mode = Read) : file_(NULL) { open(name, mode); }
        ~CFile() { close(); }

        /** Returns if the file has been opened correctly. */
        bool good() const { return file_ != NULL; }

        /** Change the permissions of a file. */
        static int chmod(const std::string &src, int mode) { return mchmod(src.c_str(), mode); }

        /** Opens the file with the specified mode, defaults to CFile::Read */
        bool open(const std::string &name, Mode mode = Read) {
            if (file_) close();
            const char *m;

            if (mode == Write)
                m = "wb";
            else if (mode == Append)
                m = "a";
            else
                m = "rb";

            return (file_ = myopen(name, m)) != NULL;
        }
        /** Close the file */
        void close() { if (file_) ::fclose(file_); file_ = NULL; }

        /** Returns the low level FILE pointer associated with the file */
        FILE *file() { return file_; }

        /** Read an integer as binary from a file, this method is not endian safe. */
        bool read(int &dest) {
            int d;

            if (file_ && mfscanf(file_, "%d", &d) == 1) {
                dest = d;
                return true;
            }
            return false;
        }
        /** Read a long long as binary from a file, this method is not endian safe. */
        bool read(long long &dest) {
            long long d;

            if (file_ && mfscanf(file_, "%lld", &d) == 1) {
                dest = d;
                return true;
            }
            return false;
        }
        /** Read the contents of the file to a string, returns true if no error occurred */
        bool read(std::string &dest) {
            if (!file_)
                return false;

            dest.clear();

            char buffer[4096];
            while (!feof(file_)) {
                size_t len = fread(buffer, 1, sizeof(buffer), file_);
                if (len > 0)
                    dest.append(buffer, len);
                else if (!feof(file_))
                    return false;
            }
            return true;
        }
        /** Read binary data from the file, returns the number of bytes read. */
        size_t read(void *dest, size_t len) {
            if (file_)
                return ::fread(dest, 1, len, file_);
            else
                return 0;
        }
        /** Write a buffer to the file, returns true if all the bytes were written. */
        size_t write(const void *source, size_t len) {
            if (file_)
                return ::fwrite(source, 1, len, file_);
            else
                return 0;
        }
        /** Write a string to the file, returns true if the string was written successfully. */
        bool write(const std::string &source) {
            if (file_)
                return ::fwrite(&source[0], 1, source.length(), file_) == source.length();
            else
                return false;
        }
        // this does not work on windows
        int64_t getsize() {
            int64_t len = -1;
            if (file_) {
                int64_t pos = mtell(file_);
                mseek(file_, 0, SEEK_END);
                len = mtell(file_);
                mseek(file_, pos, SEEK_SET);
            }
            return len;
        }
        /** Seeks in the file from the specified position (default to CFile::Current) */
        int64_t seek(size_t bytes, SeekPos position = Current) {
            if (file_)
                return mseek(file_, bytes, position);
            else
                return -1;
        }
        /** Write a line to a file, a \n is added to the end of the input string */
        bool writeln(const std::string &line) {
            if (!file_) return false;

            if (fwrite(&line[0], 1, line.length(), file_) != line.length())
                return false;

            fputc('\n', file_);

            return true;
        }
        /** Write a single character to the file */
        bool put(int c) {
            if (!file_) return false;
            return fputc(c, file_) != EOF;
        }
        /** Returns the modification time of a file */
        static time_t timestamp(const std::string &name) {
            sstat sb;
            if (mstat(name, &sb) == 0) return sb.st_mtime;
            return 0;
        }
        /** Returns the size of a file without opening it */
        static long long size(const std::string &name) {
            sstat sb;
            if (mstat(name, &sb) == 0) return sb.st_size;
            return -1;
        }
        /** Returns the size of the current file */
        long long size() const {
            sstat sb;
            if (file_ && mfstat(::fileno(file_), &sb) == 0) return sb.st_size;
            return -1;
        }

        /** Returns true if we have reached the end of the file */
        bool eof() const { if (file_) return feof(file_); else return true; }

        /** Write vprintf like format string to a file */
        int vwritef(const char *format, va_list va) {
            if (!file_)
                return 0;

            return ::vfprintf(file_, format, va);
        }
        /** Write printf like format string to file */
        int writef(const char *format, ...) {
            if (!file_)
                return 0;

            va_list va;
            va_start(va, format);
            int rc = ::vfprintf(file_, format, va);
            va_end(va);

            return rc;
        }
        /** Get a line from the file inside a string, fails if the line is longer than 8192 bytes */
        bool getline(std::string &line) {
            if (file_) {
                char buffer[8192];

                if (::fgets((char *)buffer, (int)sizeof(buffer), file_) != NULL) {
                    line = buffer;
                    return true;
                }
            }
            return false;
        }
        /** Get a line from the file, in the given buffer, fails if the line is longer than len */
        bool getline(void *buffer, size_t len) {
            if (file_)
                return ::fgets((char *)buffer, (int)len, file_) != NULL;
            else
                return false;
        }
        /** Returns the current reading/writing index in the file */
        long long position() const {
            if (file_)
                return mtell(file_);
            else
                return -1;
        }
        /** Link a file */
        static bool link(const std::string &src, const std::string &dest) {
            return msymlink_file(src, dest);
        }
        /** Move a file, works between different filesystems */
        static bool move(const std::string &src, const std::string &dest_file_or_path);
        /** Rename a file, fails if the destination path is on a different filesystem (use CFile::move if you need to rename across filesystems) */
        static bool rename(const std::string &src, const std::string &dest) {
            return mrename(src, dest) == 0;
        }
        /** Copy a file, returns true if success, use a temporary file to be atomic */
        static bool copy(const std::string &src, const std::string &dest) {
            std::string temp_dest = dest + ".tmp";
            if (!mcopy(src, temp_dest)) {
                remove(temp_dest);
                return false;
            }
            if (rename(temp_dest, dest))
                return true;
            else {
                CFile::remove(temp_dest);
                return false;
            }
        }
        /** Remove a file, returns true if success */
        static bool remove(const std::string &src) { return mremove(src) == 0;  }
        /** Check if a path is a mount point or not */
        static bool mount_point(const std::string &name) {
#ifndef WIN32
            sstat sb, sbp;

            if (name == "/")
                return true;
            if (mstat(name, &sb) == 0 && mstat((name + DEFAULT_PATH_CHAR ".."), &sbp) == 0)
                return (sb.st_dev != sbp.st_dev);
#endif
            return false;
        }
        /** Returns the filename with the extension, given the full path */
        static std::string filenameWithExtension(const std::string& fullPath) {
            const std::string::size_type pos = fullPath.find_last_of(PATH_CHARS);
            if (pos != std::string::npos)
                return fullPath.substr(pos + 1);
            else
                return fullPath;
        }
        /** Returns the filename without the extension, given the full path */
        static std::string filenameWithoutExtension(const std::string& fullPath) {
            std::string filename = filenameWithExtension(fullPath);
            const std::string::size_type pos2 = filename.find_last_of('.');

            if (pos2 != std::string::npos && filename.length() - pos2 > 1)
                return filename.substr(0, pos2);

            return filename;
        }
        /** Returns the directory of a file */
        static std::string directory(const std::string& fullPath) {
            const std::string::size_type pos = fullPath.find_last_of(PATH_CHARS);
            if (pos != std::string::npos)
                return fullPath.substr(0, pos);
            else
                return fullPath;
        }
        /** Returns the extension of a file (with the '.' included, e.g. ".mp4") */
        static std::string extension(const std::string& fullPath) {
            // we should skip as extension paths with dot inside and files starting with dot (like .vimrc)
            const std::string::size_type path_pos = fullPath.find_last_of(PATH_CHARS);
            const std::string::size_type pos = fullPath.find_last_of('.');

            if (pos != std::string::npos && fullPath.length() - pos > 1 && pos > (path_pos + 1) && pos > 0)
                return fullPath.substr(pos); // does not have the +1 to return with the '.' (e.g. ".mp4")

            return "";
        }
        /** Returns the filename without extension and the extension, given a filename with extension or the full file path */
        static void splitFilename(const std::string& fullPath, std::string& cleanName, std::string& extension) {
            const std::string filename = CFile::filenameWithExtension(fullPath);

            const std::string::size_type pos = filename.find_last_of('.');

            if (pos != std::string::npos && filename.length() - pos > 1)
            {
                cleanName = filename.substr(0, pos);
                extension = filename.substr(pos); // does not have the +1 to return with the '.' (e.g. ".mp4")
            }
            else
            {
                cleanName = filename;
                extension = "";
            }
        }
        /** Returns TRUE if the filename had the the given extension */
        static bool extensionMatch(const std::string& filename, const std::string& extension) {
            return (filename.size() >= extension.size()) && equal(extension.rbegin(), extension.rend(), filename.rbegin());
        }

        /** Returns the filepath with the new extension */
        static std::string changeExtension(const std::string& filepath, const std::string& newExtension) {
            const std::string::size_type pos = filepath.find_last_of('.');

            if (pos != std::string::npos && filepath.length() - pos > 1) {
                return filepath.substr(0, pos) + newExtension;
            } else
                return filepath + newExtension;
        }
};

class CDir
{
    MDIR *dir_;

    static int do_mkdir(const std::string &path, int mode) {
        CDir d(path);

        if (d.good())
            return 0;

        if (makedir(path, mode) != 0)
            return -1;

        return 0;
    }
public:
    /** Create a directory object without opening any real directory */
    CDir() : dir_(NULL) {}
    /** Create a CDir object and open the given path, use good() to check if the operation succeeds */
    CDir(const std::string &path) { open(path); }
    ~CDir() { close(); }

    /** check if the directory is available */
    bool good() const { return dir_ != NULL; }
    /** open a directory */
    bool open(const std::string &path) {
        return (dir_ = openmdir(path)) != NULL;
    }
    /** close a directory */
    void close() {
        if (!dir_) return;
        closemdir(dir_);
        dir_ = nullptr;
    }
    /** Get a file (or directory) from the directory, if the directory does not contain other files it returns false and the input parameter is not changed */
    bool get(std::string &name) {
        if (!dir_)
            return false;

        dent *e = nullptr;
        char buffer[sizeof(dent) + 256];
        if (( ::mreaddir(dir_, (dent *) buffer, &e) == 0) && e) {
            // skippo i file "." e ".."
            if (e->d_name[0] == '.' &&
                (e->d_name[1] == 0 || (e->d_name[1] == '.' && e->d_name[2] == 0))
                )
                return get(name);

            name = utf8name(e->d_name);
            return true;
        }
        return false;
    }
    /** Returns the current directory */
    static std::string current() {
        char buffer[4096];
        if (!mgetcwd(buffer, sizeof(buffer)))
            return "";
        return buffer;
    }
    /** Remove a directory, if empty or if recursive is true, returns true if the call succeeds */
    static bool remove(const std::string &path, bool recursive = false) {
        if (recursive) {
            CDir d(path);
            if (d.good()) {
                std::string content;
                while (d.get(content)) {
                    if (!remove(path + DEFAULT_PATH_CHAR + content, true))
                        return false;
                }
            }
        }
        if (mrmdir(path) == 0) return true;
        return CFile::remove(path);
    }
    /** Create a new directory */
    static bool create(const std::string &path, int mode = 0755) {
            std::string::size_type sp, pp = 0;
            int status = 0;

            while (status == 0 && (sp = path.find_first_of(PATH_CHARS, pp)) != std::string::npos) {
                if (sp != pp)
                    status = do_mkdir(path.substr(0, sp), mode);

                pp = sp + 1;
            }
            if (status == 0)
                status = do_mkdir(path, mode);

            return status == 0;
    }
    /** Create a link to a directory */
    static bool link(const std::string &src, const std::string &dest) {
        return msymlink_dir(src, dest);
    }
    /** Returns the folder name, given the full path */
    static std::string folderName(const std::string& folderPath) {
        const std::string::size_type pos = folderPath.find_last_of(PATH_CHARS);
        if (pos != std::string::npos)
            return folderPath.substr(pos + 1);
        else
            return folderPath;
    }
    static bool empty(const std::string &path) {
        CDir d(path);
        if (!d.good())
            return true;
        std::string file;
        size_t counter = 0;
        while (d.get(file)) ++counter;
        return counter == 0;
    }
};

inline bool CFile::move(const std::string &src, const std::string &dest_file_or_path) {
    std::string dest;
    if (CDir(dest_file_or_path).good())
        dest = dest_file_or_path + DEFAULT_PATH_CHAR + filenameWithExtension(src);
    else
        dest = dest_file_or_path;

    if (!rename(src, dest)) {
        if (copy(src, dest)) {
            return remove(src);
        }
        return false;
    }
    return true;
}
#endif

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <string>
#include <vector>
#include "sqlite3.h"
#include <stdlib.h>

enum DbTypes {DB_INTEGER, DB_FLOAT, DB_STRING, DB_DATE};

struct Traits {
    virtual void Convert(const char *) = 0;
};

template <typename T> struct DbTraits;

// // Specialization for date
// template <>
// struct DbTraits<time_t> : public Traits
// {
//     time_t &to;
//     DbTraits(time_t &t) : to(t) {}
//     enum { TypeId = DB_DATE };
//     void Convert(const char * from)
//     {
//         struct tm mytm;
//         if (!strptime(from, "%Y-%m-%d %T", &mytm))
//             to = 0;
//         else
//             to = mktime(&mytm);
//     }
// };

// Specialization for int
template <>
struct DbTraits<int> : public Traits
{
    int &to;
    DbTraits(int &t) : to(t) {}
    enum { TypeId = DB_INTEGER };
    void Convert(const char * from)
    {
        if (from != NULL)
            to = atoi(from);
        else
            to = 0;
    }
};

template <>
struct DbTraits<long long> : public Traits
{
    long long &to;
    DbTraits(long long &t) : to(t) {}
    enum { TypeId = DB_INTEGER };
    void Convert(const char * from)
    {
        if (from != NULL)
            to = atoll(from);
        else
            to = 0;
    }
};
// Specialization for string
template <>
struct DbTraits<std::string> : public Traits
{
    std::string &to;
    DbTraits(std::string &t) : to(t) {}
    enum { TypeId = DB_STRING };
    void Convert(const char *from)
    {
        if (from != NULL)
            to = from;
        else
            to.clear();
    }
};

// Specialization for float
template <>
struct DbTraits<float>  : public Traits
{
    float to;
    DbTraits(float &t) : to(t) {}
    enum { TypeId = DB_FLOAT };
    void Convert(const char * from)
    {
        if (from != NULL)
            to = atof(from);
        else
            to = 0.0f;
    }
};

class AbstractCbk
{
    public:
        virtual ~AbstractCbk() {}

        virtual void notify(int, char **, char **) const = 0;

        static int real_callback(AbstractCbk *cbk, int cnt, char **col, char **title) {
            cbk->notify(cnt, col, title);
            return 0; // not zero, causa abort
        }
};

template <typename T, typename D>
class CbkEvent : public AbstractCbk
{
public:
    CbkEvent( T* obj, void (T::*fnc)(int, char **, char **))
        : myObj(obj), myFnc0(fnc) {}
    CbkEvent( T* obj, void (T::*fnc)(int, char **, char **, D), D data)
        : myObj(obj), myFnc0(NULL), myFnc1(fnc), userData(data) {}

private:
    T*  myObj;
    void (T::*myFnc0)(int, char **, char **);
    void (T::*myFnc1)(int, char **, char **, D);
    D userData;
    virtual void notify(int cnt, char **col, char **title) const {
        if (myFnc0)
            return (myObj->*myFnc0)(cnt, col, title);
        else
            return (myObj->*myFnc1)(cnt, col, title, userData);
    }
};

class SqliteDb
{
    typedef int (*SqliteCbk)(void*, int, char**, char**);
    std::string name_;
    int mode_;
    sqlite3 *db_;
    std::string error_;

    static int field_callback(std::vector<Traits *> *arg, int cnt, char **col, char **title) {
        int num = std::min<int>(cnt, arg->size());

        for (int i = 0; i < num; ++i)
            (*arg)[i]->Convert(col[i]);

        return 0;
    }

    bool perform_call(const std::string &query, SqliteCbk cbk = NULL, void *data = NULL) {
        char *err = NULL;
        int rc = sqlite3_exec(db_, query.c_str(), (SqliteCbk)cbk, data, &err);

        if (rc != SQLITE_OK)
            error_ = err;

        if (err)
            sqlite3_free(err);

        return rc == SQLITE_OK;
    }

    template <typename T>
    bool get_fields_internal(const std::string &query, std::vector<Traits *> &params, T &t) {
        DbTraits<T> type(t);
        params.push_back(&type);
        return perform_call(query, (SqliteCbk)field_callback, &params);
    }
    template <typename T, typename... Args>
    bool get_fields_internal(const std::string &query, std::vector<Traits *> &params, T &t, Args &... args) {
        DbTraits<T> type(t);
        params.push_back(&type);
        return get_fields_internal(query, params, args...);
    }
public:
    SqliteDb(const std::string &name = "") : name_(name), mode_(0), db_(NULL) {}
    ~SqliteDb() { close(); }

    bool begin() { return perform_call("BEGIN IMMEDIATE"); }
    bool commit() { return perform_call("COMMIT"); }
    bool rollback() { return perform_call("ROLLBACK"); }
    std::string error() const { return error_; }

    // NOTA: da usare unicamente dopo una segnalazione di errore!
    bool locked() const { return error_.find("locked") != std::string::npos; }

    bool open(const std::string &name = "", bool rw = false) {
        if (!name.empty())
            name_ = name;

        if (name_.empty())
            return false;

        if (rw) {
            mode_ = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        } else {
            mode_ = SQLITE_OPEN_READONLY;
        }

        sqlite3_enable_shared_cache(true);
        return sqlite3_open_v2(name.c_str(), &db_, mode_, NULL) == 0;
    }

    bool BusyTimeout(int secs) {
        if (db_) {
            sqlite3_busy_timeout(db_, (secs * 1000));
            return true;
        }
        return false;
    }
    bool update(const std::string &query) { return insert(query); }
    bool insert(const std::string &query) { return perform_call(query); }
    bool command(const std::string &cmd) { return insert(cmd); }

    template <typename... Args>
    bool get_fields(const std::string &query, Args &... args) {
        std::vector<Traits *> params;
        return get_fields_internal(query, params, args...);
    }

    template <typename T>
    bool query(const std::string &query, T *base, void (T::*fnc)(int, char **, char **)) {
        CbkEvent<T,void*> ev(base, fnc);
        return perform_call(query, (SqliteCbk)AbstractCbk::real_callback, &ev);
    }

    template <typename T, typename R>
    bool query(const std::string &query, T *base, void (T::*fnc)(int, char **, char **, R), R data) {
        CbkEvent<T,R> ev(base, fnc, data);
        return perform_call(query, (SqliteCbk)AbstractCbk::real_callback, &ev);
    }

    void close() {
        if (db_) {
            sqlite3_close(db_);
            db_ = NULL;
        }
    }
};

#endif

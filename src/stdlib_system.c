#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif /* ifdef _WIN32 */

// Returns the string describing the meaning of `errno` code (by calling `strerror`).
char* stdlib_strerror(size_t* len, bool winapi){

    if (winapi) {
#ifdef _WIN32
    LPSTR err = NULL;
    DWORD dw = GetLastError();

    FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dw,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPSTR) &lpMsgBuf,
    0,
    NULL);

    *len = strlen(err);
    return (char*) err;
#endif /* ifdef _WIN32 */
    }

    char* err = strerror(errno);
    *len = strlen(err);
    return err;
}

// Wrapper to the platform's `mkdir`(make directory) call.
// Uses `mkdir` on unix, `_mkdir` on windows.
// Returns 0 if successful, otherwise returns the `errno`.
int stdlib_make_directory(const char* path){
    int code;
#ifdef _WIN32
    code = _mkdir(path);
#else
    // Default mode 0777
    code = mkdir(path, 0777);
#endif /* ifdef _WIN32 */
    
    return (!code) ? 0 : errno;
}

// Wrapper to the platform's `rmdir`(remove directory) call.
// Uses `rmdir` on unix, `_rmdir` on windows.
// Returns 0 if successful, otherwise returns the `errno`.
int stdlib_remove_directory(const char* path){
    int code;
#ifdef _WIN32
    code = _rmdir(path);
#else
    code = rmdir(path);
#endif /* ifdef _WIN32 */

    return (!code) ? 0 : errno;
}

// Wrapper to the platform's `stat`(status of path) call.
// Uses `lstat` on unix, `GetFileAttributesA` on windows.
// Returns the `type` of the path, negative values indicate an error.
int stdlib_exists(const char* path, int* stat){
    const int type_unknown = 0;
    const int type_regular_file = 1;
    const int type_directory = 2;
    const int type_symlink = 3;

    int type = type_unknown;
    *stat = 0;

#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);

    if (attrs == INVALID_FILE_ATTRIBUTES) {
        *stat = (int) GetLastError();
        return type_unknown;
    }

    if (attrs & FILE_ATTRIBUTE_NORMAL) type = type_regular_file;
    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) type = type_symlink;
    if (attrs & FILE_ATTRIBUTE_DIRECTORY) type = type_directory;
#else
    struct stat buf = {0};
    int status;
    status = lstat(path, &buf);

    if (status == -1) {
        *stat = errno;
        return type_unknown;
    }

    switch (buf.st_mode & S_IFMT) {
        case S_IFREG: type = type_regular_file; break;
        case S_IFDIR: type = type_directory;    break;
        case S_IFLNK: type = type_symlink;      break;
        default:      type = type_unknown;      break;
    }
#endif /* ifdef _WIN32 */
    return type;
}

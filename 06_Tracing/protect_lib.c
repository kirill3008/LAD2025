#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>

typedef int (*remove_func_t)(const char *pathname);
typedef int (*unlink_func_t)(const char *pathname);

int remove(const char *pathname) {
    remove_func_t original_remove = (remove_func_t)dlsym(RTLD_NEXT, "remove");
    
    if (pathname && strstr(pathname, "PROTECT") != NULL) {
        fprintf(stderr, "[PROTECT] Prevented removal of protected file: %s\n", pathname);
        errno = EPERM;
        return -1;
    }
    
    return original_remove(pathname);
}

int unlink(const char *pathname) {
    unlink_func_t original_unlink = (unlink_func_t)dlsym(RTLD_NEXT, "unlink");
    
    if (pathname && strstr(pathname, "PROTECT") != NULL) {
        fprintf(stderr, "[PROTECT] Prevented unlinking of protected file: %s\n", pathname);
        errno = EPERM;
        return -1;
    }
    
    return original_unlink(pathname);
}

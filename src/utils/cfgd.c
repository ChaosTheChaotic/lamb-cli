#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <limits.h>
#include <stdio.h>
#include "include/cfgd.h"

const char* get_user_config_dir() {
    char path[PATH_MAX];
    const char *home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    snprintf(path, sizeof(path), "%s/.config/yourapp", home);
    char *retpath = (char *)malloc(strlen(path));
    return retpath;
}

const char* get_password_file() {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/password.enc", get_user_config_dir());
    char *retpath = (char *)malloc(strlen(path));
    return retpath;
}

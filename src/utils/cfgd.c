#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <limits.h>
#include <stdio.h>
#include "cfgd.h"

const char* get_user_config_dir() {
    char path[PATH_MAX];
    const char *home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    snprintf(path, sizeof(path), "%s/.config/yourapp", home);
    const char *retpath = strdup(path);
    return retpath;
}

const char* get_password_file() {
    char path[PATH_MAX];
    const char *ucfgd = get_user_config_dir();
    snprintf(path, sizeof(path), "%s/password.enc", ucfgd);
    free((char *)ucfgd);
    ucfgd = NULL;
    char *retpath = strdup(path);
    return retpath;
}

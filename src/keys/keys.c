#include "keys.h"
#include <linux/keyctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int store_pwd(const char *restrict pwd) {
  if (!pwd)
    return -1;

  int kid = syscall(SYS_add_key, "user", "lamb_session_password", pwd,
                    strlen(pwd), KEY_SPEC_USER_SESSION_KEYRING);
  return kid;
}

char *get_pwd() {
  int kid = syscall(SYS_keyctl, KEYCTL_SEARCH, KEY_SPEC_USER_SESSION_KEYRING,
                    "user", "lamb_session_password", 0);
  if (kid < 0) {
    return NULL;
  }

  long klen = syscall(SYS_keyctl, KEYCTL_READ, kid, NULL, 0);
  if (klen < 0) {
    return NULL;
  }

  char *buf = (char *)malloc(klen + 1);
  if (!buf) {
    return NULL;
  }

  long ret = syscall(SYS_keyctl, KEYCTL_READ, kid, buf, klen);
  if (ret < 0) {
    free(buf);
    return NULL;
  }

  buf[klen] = '\0';
  return buf;
}

// Just in case we need it
int clear_pwd(void) {
  int key_id = syscall(SYS_keyctl, KEYCTL_SEARCH, KEY_SPEC_SESSION_KEYRING,
                       "user", "session_password", 0);
  if (key_id < 0) {
    return -1;
  }

  return syscall(SYS_keyctl, KEYCTL_UNLINK, key_id, KEY_SPEC_SESSION_KEYRING);
}

int prompt_pwd(const char *restrict prompt, char *restrict *restrict pbuf) {
  const char *restrict pwd = getpass(prompt);
  *pbuf = strdup(pwd);
  if (!*pbuf) {
    fprintf(stderr, "Failed to malloc for pbuf, exiting");
    exit(1);
  }
  return store_pwd(pwd);
}

#include "pwds.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

int genrndkey(unsigned char *restrict key, size_t len) {
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) return -1;

  if (fread(key, len, 1, fp) != 1) {
    fclose(fp);
    return -1;
  }
  fclose(fp);
  return 0;
}

int getpwds(char *restrict pwd) {
  char *uencp = getpass("No password file found\nEnter the password you would like to use: ");
  memcpy(pwd, uencp, strlen(uencp));
  unsigned char key[32];
  if (genrndkey(key, sizeof(key)) == -1) {
    fprintf(stderr, "Failed to get random number from /dev/urandom\nFalling back to the rand function");
    srand(time(NULL));
    for (size_t i = 0; i < sizeof(key); i++) {
      key[i] = (unsigned char)rand(); // Generate pseudo random bytes
    }
  }
  unsigned char *cptxt = NULL;
  unsigned int len = 0;
  if (epwd(uencp, (const char *)key, &cptxt, &len) != 0) {
    fprintf(stderr, "Failed to encrypt password, not writing to file");
    return -1;
  }

  if (wencp(cptxt, len) != 0) {
    fprintf(stderr, "Failed to write encrypted password to file");
    return -1;
  }
  return 0;
}

#include <errno.h>
#include "cfgd.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "pwds.h"

int wencp(const unsigned char *restrict encd, unsigned int encl) {
  const char *restrict cfgd = get_user_config_dir();
  if (mkdir(cfgd, 0700)) { // If non-zero exit
    if (!(errno == EEXIST)) { // If the error is not that the directory already exists
      return -1;
    }
  }
  const char *pwdd = get_password_file();
  FILE *fp = fopen(pwdd, "wb");
  if (!fp) {
    free((char *)pwdd);
    pwdd = NULL;
    return -1;
  }

  // Write the encrypted data length then the encrypted data to the file
  fwrite(&encl, sizeof(unsigned int), 1, fp); // This is so we know how much data to read back
  fwrite(encd, encl, 1, fp);

  fclose(fp);
  chmod(pwdd, 0600);
  free((char *)pwdd);
  pwdd = NULL;
  return 0;
}

int rencp(unsigned char **restrict encd, unsigned int *restrict encl) {
  const char *fpath = get_password_file();
  FILE *fp = fopen(fpath, "rb");
  if (!fp) {
    free((char *)fpath);
    fpath = NULL;
    return -1;
  }
  
  // Read the length
  if (fread(encl, sizeof(unsigned int), 1, fp) != 1) {
    fclose(fp);
    free((char *)fpath);
    fpath = NULL;
    return -1;
  }
  
  // Allocate buffer for encrypted data
  *encd = (unsigned char *)malloc(*encl);
  if (!*encd) {
    fclose(fp);
    free((char *)fpath);
    fpath = NULL;
    return -1;
  }
  
  // Read the actual encrypted data
  if (fread(*encd, *encl, 1, fp) != 1) {
    free(*encd);
    *encd = NULL;
    fclose(fp);
    free((char *)fpath);
    fpath = NULL;
    return -1;
  }
  
  fclose(fp);
  free((char *)fpath);
  fpath = NULL;
  return 0;
}

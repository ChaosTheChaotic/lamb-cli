#ifndef PWDS_H
#define PWDS_H

#include <stdio.h>

int epwd(const char *restrict plaintext, const char *restrict key, unsigned char *restrict *restrict cptxt, unsigned int *restrict cptxt_len);
int dpwd(const unsigned char *restrict cptxt, unsigned int cptxt_len, const char *restrict key, char *restrict *restrict plaintext);

int wencp(const unsigned char *restrict encd, unsigned int encl);
int rencp(unsigned char **restrict encd, unsigned int *restrict encl);

// Attempts to generate a random key from /dev/urandom if this fails it returns -1
// Use the rand() function instead with srand(time(NULL))
int genrndkey(unsigned char *restrict key, size_t len);
// Gets the password from the user, writes it to pwd and attempts to write to dir (returns if it fails on any step after writing to pwd)
int getpwds(char *restrict pwd);

#endif

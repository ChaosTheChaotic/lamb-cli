#ifndef PWDS_H
#define PWDS_H

int epwd(const char *restrict plaintext, const char *restrict key, unsigned char *restrict *restrict cptxt, unsigned int *restrict cptxt_len);
int dpwd(const unsigned char *restrict cptxt, unsigned int cptxt_len, const char *restrict key, char *restrict *restrict plaintext);

int wencp(const unsigned char *restrict encd, unsigned int encl);
int rencp(unsigned char **restrict encd, unsigned int *restrict encl);

#endif

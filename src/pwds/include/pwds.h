#ifndef PWDS_H
#define PWDS_H

int epwd(const char *restrict plaintext, const char *restrict key, unsigned char *restrict *restrict cptxt, int *restrict cptxt_len);

int dpwd(const unsigned char *restrict cptxt, int cptxt_len, const char *restrict key, char *restrict *restrict plaintext);

#endif

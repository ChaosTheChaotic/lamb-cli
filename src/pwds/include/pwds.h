#ifndef PWDS_H
#define PWDS_H

#include <stdio.h>

/**
 * Encryts a password using evp
 *
 * @param plaintext the plain text form of the password
 * @param key an encryption key to be used to encrypt the data
 * @param cptxt a buffer into which the encrypted data will be written to
 * @param ctpxt_len the length of the encrypted data which gets written in
*/
int epwd(const char *restrict plaintext, const char *restrict key, unsigned char *restrict *restrict cptxt, unsigned int *restrict cptxt_len);

/**
 * Decrypts a password
 *
 * @param cptxt the encrypted data which needs to get decrypted
 * @param cptxt_len the length of the encrypted data
 * @param key the key which was used to encrypt the password
 * @param plaintext a buffer into which the plain text of the password will be written into
*/
int dpwd(const unsigned char *restrict cptxt, unsigned int cptxt_len, const char *restrict key, char *restrict *restrict plaintext);

/**
 * Write encrypted passwords
 *
 * @param encd encrypted data to be written into the file
 * @param encl the length of the encrypted data
 * @return 0 on success else -1
*/
int wencp(const unsigned char *restrict encd, unsigned int encl);

/**
 * Read encrypted passwords form the dedicated password file
 *
 * @param encd encrypted data into which the data will be read into
 * @param encl the length of the encrypted data which gets written into it
 * @return 0 on success else -1
*/
int rencp(unsigned char **restrict encd, unsigned int *restrict encl);

// Attempts to generate a random key from /dev/urandom if this fails it returns -1
// Use the rand() function instead with srand(time(NULL))
int genrndkey(unsigned char *restrict key, size_t len);
// Gets the password from the user, writes it to pwd and attempts to write to dir (returns if it fails on any step after writing to pwd)
int getpwds(char *restrict pwd);

#endif

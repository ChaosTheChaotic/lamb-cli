#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "pwds.h"

/**
 * Encryts a password using evp
 *
 * @param plaintext the plain text form of the password
 * @param key an encryption key to be used to encrypt the data
 * @param cptxt a buffer into which the encrypted data will be written to
 * @param ctpxt_len the length of the encrypted data which gets written in
*/
int epwd(
  const char *restrict plaintext,
  const char *restrict key,
  unsigned char *restrict *restrict cptxt,
  unsigned int *restrict cptxt_len
) {
  EVP_CIPHER_CTX *ctx;
  int len;
  int ptl = strlen(plaintext);
  unsigned char iv[EVP_MAX_IV_LENGTH];

  if (!RAND_bytes(iv, EVP_MAX_IV_LENGTH)) {
    return -1;
  }

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return -1;

  if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned  char *restrict)key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }

  *cptxt = (unsigned char *restrict)malloc(ptl + EVP_MAX_IV_LENGTH + EVP_MAX_BLOCK_LENGTH);
  if (!*cptxt) {
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }

  memcpy(*cptxt, iv, EVP_MAX_IV_LENGTH);

  if (EVP_EncryptUpdate(ctx, *cptxt + EVP_MAX_IV_LENGTH, &len, (unsigned char *restrict)plaintext, ptl) != 1) {
      free(*cptxt);
      *cptxt = NULL;
      EVP_CIPHER_CTX_free(ctx);
      return -1;
  }
  *cptxt_len = len + EVP_MAX_IV_LENGTH;
  if (EVP_EncryptFinal_ex(ctx, *cptxt + *cptxt_len, &len) != 1) {
      free(*cptxt);
      *cptxt = NULL;
      EVP_CIPHER_CTX_free(ctx);
      return -1;
  }
  *cptxt_len += len;
  
  EVP_CIPHER_CTX_free(ctx);
  return 0;
}

/**
 * Decrypts a password
 *
 * @param cptxt the encrypted data which needs to get decrypted
 * @param cptxt_len the length of the encrypted data
 * @param key the key which was used to encrypt the password
 * @param plaintext a buffer into which the plain text of the password will be written into
*/
int dpwd(
  const unsigned char *restrict cptxt,
  unsigned int cptxt_len,
  const char *restrict key,
  char *restrict *restrict plaintext
) {
  EVP_CIPHER_CTX *ctx = NULL;
  int len;
  int plaintext_len;

  // Check if ciphertext is long enough to contain IV
  if (cptxt_len <= EVP_MAX_IV_LENGTH) {
    return -1;
  }

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return -1;

  // Extract IV from beginning of ciphertext
  const unsigned char *iv = cptxt;
  const unsigned char *encrypted_data = cptxt + EVP_MAX_IV_LENGTH;
  int encrypted_data_len = cptxt_len - EVP_MAX_IV_LENGTH;

  if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                        (unsigned char *restrict)key, iv) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }

  // Allocate buffer for decrypted text (same size as encrypted data + padding)
  *plaintext = (char *restrict)malloc(encrypted_data_len + 1);
  if (!*plaintext) {
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }

  if (EVP_DecryptUpdate(ctx, (unsigned char *restrict)*plaintext, &len, 
                       encrypted_data, encrypted_data_len) != 1) {
    free(*plaintext);
    *plaintext = NULL;
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }
  plaintext_len = len;

  // Finalize decryption
  if (EVP_DecryptFinal_ex(ctx, (unsigned char *restrict)*plaintext + plaintext_len, &len) != 1) {
    free(*plaintext);
    *plaintext = NULL;
    EVP_CIPHER_CTX_free(ctx);
    return -1;
  }
  plaintext_len += len;

  (*plaintext)[plaintext_len] = '\0';

  EVP_CIPHER_CTX_free(ctx);
  return 0;
}

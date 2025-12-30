/*
gcc -o OISafeDecryptor  OISafeDecryptor.c  -lssl -lcrypto && ./OISafeDecryptor  TestTest b2e7e20476b362f6 ac911ad4d87d6ce56bbad16c3631792c08f07c497ea2ddd15f471d64672f6ab4c088b10571ea89f73274abeb8a21575fec57bc7295e86bce730ef0265a1446cc48fd82089ef2e8bb746f877e7367f59f 5c433a6aef892c34446aec1cedf85f14376ecba2a9fcd04de66a5dd146e17b68  a9932a9dbe4a67b3b109f57cd7e24830ee0bb0d44f98facb6174ef70b3ecb3e6 e94f19f69bedce1b24882c3a8fed003b           

Master: 5b3a182a0efb37e278e5c010156a019757810b72b60a44ef783615d605c34d22
Field: NotesTestTest24

Field: DescriptionTestTest42
Field: Business                                                                                                                                   
*/

#include <ctype.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const EVP_MD *md;
  const EVP_CIPHER *cipher;
  int iterations;
  int use_pkcs12;
} Cfg;

void derive_keys(const char *pwd, const unsigned char *salt, int sl, const Cfg *c, unsigned char *k,
                 unsigned char *iv) {
  int kl = EVP_CIPHER_key_length(c->cipher), vl = EVP_CIPHER_iv_length(c->cipher);
  if (c->use_pkcs12) {
    PKCS12_key_gen_utf8(pwd, strlen(pwd), (unsigned char *)salt, sl, 1, c->iterations, kl, k, c->md);
    PKCS12_key_gen_utf8(pwd, strlen(pwd), (unsigned char *)salt, sl, 2, c->iterations, vl, iv, c->md);
  } else
    EVP_BytesToKey(c->cipher, c->md, salt, (unsigned char *)pwd, strlen(pwd), c->iterations, k, iv);
}

int decrypt_buffer(const unsigned char *k, const unsigned char *iv, const unsigned char *in, int inl,
                   const EVP_CIPHER *ci, unsigned char *out, int *ol) {
  EVP_CIPHER_CTX *c = EVP_CIPHER_CTX_new();
  int t, f, r = 0;
  if (c && EVP_DecryptInit_ex(c, ci, NULL, k, iv) && EVP_DecryptUpdate(c, out, &t, in, inl) &&
      EVP_DecryptFinal_ex(c, out + t, &f)) {
    *ol = t + f;
    r = 1;
  }
  EVP_CIPHER_CTX_free(c);
  return r;
}

unsigned char *get_master(const char *p, const char *s, const char *c, int *ol) {
  long sl, cl;
  unsigned char *sa = OPENSSL_hexstr2buf(s, &sl), *me = OPENSSL_hexstr2buf(c, &cl), *out = NULL;
  if (sa && me) {
    out = OPENSSL_malloc(cl + 16);
    Cfg cf[] = {{EVP_md5(), EVP_aes_128_cbc(), 1, 0},
                {EVP_md5(), EVP_aes_128_cbc(), 20, 0},
                {EVP_sha1(), EVP_aes_256_cbc(), 1, 1},
                {EVP_sha1(), EVP_aes_256_cbc(), 20, 1}};
    unsigned char k[32], iv[16];
    for (int i = 0; i < 4; i++) {
      derive_keys(p, sa, (int)sl, &cf[i], k, iv);
      if (decrypt_buffer(k, iv, me, (int)cl, cf[i].cipher, out, ol)) {
        OPENSSL_cleanse(k, 32);
        OPENSSL_cleanse(iv, 16);
        OPENSSL_free(sa);
        OPENSSL_free(me);
        return out;
      }
    }
    OPENSSL_free(out);
    out = NULL;
  }
  if (sa) OPENSSL_free(sa);
  if (me) OPENSSL_free(me);
  return out;
}

unsigned char *get_field(const unsigned char *mk, int ml, const char *s, const char *f, int *ol) {
  long sl, fl;
  unsigned char *sa = OPENSSL_hexstr2buf(s, &sl), *fe = OPENSSL_hexstr2buf(f, &fl), *out = NULL;
  if (sa && fe) {
    char pwd[ml + 1];
    memcpy(pwd, mk, ml);
    pwd[ml] = 0;
    Cfg cfg = {EVP_md5(), EVP_aes_128_cbc(), 1, 0};
    unsigned char k[32], iv[16];
    out = OPENSSL_malloc(fl + 16);
    derive_keys(pwd, sa, (int)sl, &cfg, k, iv);
    if (!decrypt_buffer(k, iv, fe, (int)fl, cfg.cipher, out, ol)) {
      OPENSSL_free(out);
      out = NULL;
    }
    OPENSSL_cleanse(pwd, ml + 1);
    OPENSSL_cleanse(k, 32);
    OPENSSL_cleanse(iv, 16);
  }
  if (sa) OPENSSL_free(sa);
  if (fe) OPENSSL_free(fe);
  return out;
}

int main(int argc, char **argv) {
  if (argc < 4) return fprintf(stderr, "Usage: %s <Pass> <Salt> <Master> [Field...]\n", argv[0]);
  int ml;
  unsigned char *mk = get_master(argv[1], argv[2], argv[3], &ml);
  if (!mk) return fprintf(stderr, "Error: Master decryption failed\n");

  printf("Master: ");
  for (int i = 0; i < ml; i++) printf("%c", tolower(mk[i]));
  printf("\n");

  for (int i = 4; i < argc; i++) {
    int fl;
    unsigned char *fd = get_field(mk, ml, argv[2], argv[i], &fl);
    if (fd) {
      printf("Field: %.*s\n", fl, fd);
      OPENSSL_free(fd);
    } else
      fprintf(stderr, "Warning: Failed to decrypt field\n");
  }
  OPENSSL_cleanse(mk, ml);
  OPENSSL_free(mk);
  return 0;
}

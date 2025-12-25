/*
 * OI Safe Decryptor - Compact Version
 * Compile: gcc -o oisafe OISafeDecryptor.c -lcrypto
 * Usage: ./oisafe <password> <saltHex> <masterHex> [<fieldHex> ...]

gcc -o OISafeDecryptor  OISafeDecryptor.c  -lssl -lcrypto && ./OISafeDecryptor  TestTest b2e7e20476b362f6 ac911ad4d87d6ce56bbad16c3631792c08f07c497ea2ddd15f471d64672f6ab4c088b10571ea89f73274abeb8a21575fec57bc7295e86bce730ef0265a1446cc48fd82089ef2e8bb746f877e7367f59f 5c433a6aef892c34446aec1cedf85f14376ecba2a9fcd04de66a5dd146e17b68    
                                                                                                                                                     
Master: 5b3a182a0efb37e278e5c010156a019757810b72b60a44ef783615d605c34d22
Field: NotesTestTest24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define ALG_MD5 0
#define ALG_SHA1 1

static const int MASTER_ITERS[] = {20, 1};
static const int MASTER_ALGS[] = {ALG_MD5, ALG_SHA1};

unsigned char* hex2bytes(const char *h, size_t *len) {
    *len = strlen(h) / 2;
    unsigned char *b = malloc(*len);
    for (size_t i = 0; i < *len; i++) sscanf(h + 2*i, "%2hhx", &b[i]);
    return b;
}

unsigned char* pkcs12_kdf(const EVP_MD *md, const unsigned char *pw, size_t plen, 
                          const unsigned char *salt, size_t slen, int iter, int id, int klen) {
    int u = EVP_MD_size(md), v = EVP_MD_block_size(md);
    unsigned char *D = malloc(v), *S = malloc(v * ((slen+v-1)/v)), *P = malloc(v * ((plen+v-1)/v));
    memset(D, id, v);
    size_t st = v * ((slen+v-1)/v), pt = v * ((plen+v-1)/v);
    for (size_t i = 0; i < st; i++) S[i] = salt[i % slen];
    for (size_t i = 0; i < pt; i++) P[i] = pw[i % plen];
    unsigned char *I = malloc(st + pt);
    memcpy(I, S, st); memcpy(I + st, P, pt);
    unsigned char *key = malloc(klen), A[64];
    size_t pos = 0;
    
    for (int c = 0; pos < klen; c++) {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, md, NULL);
        EVP_DigestUpdate(ctx, D, v);
        EVP_DigestUpdate(ctx, I, st + pt);
        unsigned int alen;
        EVP_DigestFinal_ex(ctx, A, &alen);
        EVP_MD_CTX_free(ctx);
        
        for (int j = 1; j < iter; j++) {
            ctx = EVP_MD_CTX_new();
            EVP_DigestInit_ex(ctx, md, NULL);
            EVP_DigestUpdate(ctx, A, u);
            EVP_DigestFinal_ex(ctx, A, &alen);
            EVP_MD_CTX_free(ctx);
        }
        
        size_t n = (pos + u > klen) ? klen - pos : u;
        memcpy(key + pos, A, n);
        pos += u;
        
        if (pos < klen) {
            unsigned char B[64] = {0};
            for (int i = 0; i < v; i++) B[i] = A[i % u];
            for (int i = v-1; i >= 0 && ++B[i] == 0; i--);
            for (int k = 0; k < (st+pt)/v; k++) {
                unsigned int carry = 0;
                for (int j = v-1; j >= 0; j--) {
                    unsigned int sum = I[k*v + j] + B[j] + carry;
                    I[k*v + j] = sum & 0xff;
                    carry = sum >> 8;
                }
            }
        }
    }
    free(D); free(S); free(P); free(I);
    return key;
}

int decrypt(int alg, int iter, const char *pass, const unsigned char *salt, size_t slen, 
            const unsigned char *ct, size_t ctlen, unsigned char **out, size_t *olen) {
    int klen = (alg == ALG_SHA1) ? 32 : 16, ivlen = 16;
    const EVP_MD *md = (alg == ALG_SHA1) ? EVP_sha1() : EVP_md5();
    const EVP_CIPHER *cipher = (alg == ALG_SHA1) ? EVP_aes_256_cbc() : EVP_aes_128_cbc();
    unsigned char *keyiv = malloc(klen + ivlen);
    
    if (alg == ALG_SHA1) {
        size_t plen = strlen(pass);
        unsigned char *pb = calloc(2 * (plen + 1), 1);
        for (size_t i = 0; i < plen; i++) pb[2*i + 1] = pass[i];
        unsigned char *key = pkcs12_kdf(md, pb, 2*(plen+1), salt, slen, iter, 1, klen);
        unsigned char *iv = pkcs12_kdf(md, pb, 2*(plen+1), salt, slen, iter, 2, ivlen);
        memcpy(keyiv, key, klen); memcpy(keyiv + klen, iv, ivlen);
        free(pb); free(key); free(iv);
    } else {
        EVP_BytesToKey(cipher, md, salt, (unsigned char*)pass, strlen(pass), iter, keyiv, keyiv + klen);
    }
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, keyiv, keyiv + klen)) goto err;
    
    *out = malloc(ctlen + 16);
    int len = 0, tmp;
    if (!EVP_DecryptUpdate(ctx, *out, &tmp, ct, ctlen)) goto err;
    len += tmp;
    if (!EVP_DecryptFinal_ex(ctx, *out + len, &tmp)) goto err;
    *olen = len + tmp;
    
    EVP_CIPHER_CTX_free(ctx); free(keyiv);
    return 0;
err:
    EVP_CIPHER_CTX_free(ctx); free(keyiv); free(*out);
    return -1;
}

int main(int argc, char **argv) {
    if (argc < 4) return fprintf(stderr, "Usage: %s <pass> <salt> <master> [fields...]\n", argv[0]), 1;
    
    size_t slen, mlen;
    unsigned char *salt = hex2bytes(argv[2], &slen);
    unsigned char *encm = hex2bytes(argv[3], &mlen);
    unsigned char *mkey = NULL;
    size_t mklen;
    
    for (int a = 0; a < 2; a++)
        for (int i = 0; i < 2; i++)
            if (decrypt(MASTER_ALGS[a], MASTER_ITERS[i], argv[1], salt, slen, encm, mlen, &mkey, &mklen) == 0) {
                printf("Master: %.*s\n", (int)mklen, mkey);
                goto decrypt_fields;
            }
    
    return fprintf(stderr, "Failed to decrypt master key\n"), 1;
    
decrypt_fields:
    for (int f = 4; f < argc; f++) {
        size_t elen, dlen;
        unsigned char *enc = hex2bytes(argv[f], &elen);
        unsigned char *dec = NULL;
        char mstr[mklen + 1];
        memcpy(mstr, mkey, mklen); mstr[mklen] = 0;
        
        if (decrypt(ALG_MD5, 1, mstr, salt, slen, enc, elen, &dec, &dlen) == 0)
            printf("Field: %.*s\n", (int)dlen, dec);
        else
            printf("Field: Failed\n");
        
        free(enc); free(dec);
    }
    
    free(salt); free(encm); free(mkey);
    return 0;
}

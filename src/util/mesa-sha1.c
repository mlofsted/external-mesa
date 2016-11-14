/* Copyright © 2007 Carl Worth
 * Copyright © 2009 Jeremy Huddleston, Julien Cristau, and Matthieu Herrb
 * Copyright © 2009-2010 Mikhail Gusarov
 * Copyright © 2012 Yaakov Selkowitz and Keith Packard
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "mesa-sha1.h"

#ifdef HAVE_SHA1

#if defined(HAVE_SHA1_IN_LIBMD)  /* Use libmd for SHA1 */ \
	|| defined(HAVE_SHA1_IN_LIBC)   /* Use libc for SHA1 */

#include <sha1.h>
#include <string.h>

/*
 * Android does not expose sha1 (?) so let's just include it here
 * (bionic/libc/upstream-netbsd/common/lib/libc/hash/sha1/sha1.c)
 */
typedef union {
    uint8_t c[64];
    uint32_t l[16];
} CHAR64LONG16;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#if BYTE_ORDER == LITTLE_ENDIAN
# define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#else
# define blk0(i) block->l[i]
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

void SHA1Transform(uint32_t state[5], const uint8_t buffer[64])
{
    uint32_t a, b, c, d, e;
    CHAR64LONG16 *block;

#ifdef SHA1HANDSOFF
    CHAR64LONG16 workspace;
#endif

#ifdef SHA1HANDSOFF
    block = &workspace;
    (void)memcpy(block, buffer, 64);
#else
    block = (CHAR64LONG16 *)(void *)buffer;
#endif

    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

#ifdef SPARC64_GCC_WORKAROUND
    do_R01(&a, &b, &c, &d, &e, block);
    do_R2(&a, &b, &c, &d, &e, block);
    do_R3(&a, &b, &c, &d, &e, block);
    do_R4(&a, &b, &c, &d, &e, block);
#else
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
#endif

    /* Add the working vars back into context.state[] */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    /* Wipe variables */
    a = b = c = d = e = 0;
}

void SHA1Update(SHA1_CTX *context, const uint8_t *data, unsigned int len)
{
    unsigned int i, j;

    j = context->count[0];
    if ((context->count[0] += len << 3) < j)
        context->count[1] += (len>>29)+1;
    j = (j >> 3) & 63;
    if ((j + len) > 63) {
        (void)memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context->state, context->buffer);
        for ( ; i + 63 < len; i += 64)
            SHA1Transform(context->state, &data[i]);
        j = 0;
    } else {
        i = 0;
    }
    (void)memcpy(&context->buffer[j], &data[i], len - i);
}

void SHA1Final(uint8_t digest[20], SHA1_CTX *context)
{
    unsigned int i;
    uint8_t finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((context->count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  /* Endian independent */
    }
    SHA1Update(context, (const uint8_t *)"\200", 1);
    while ((context->count[0] & 504) != 448)
        SHA1Update(context, (const uint8_t *)"\0", 1);
    SHA1Update(context, finalcount, 8);  /* Should cause a SHA1Transform() */

    if (digest) {
        for (i = 0; i < 20; i++)
            digest[i] = (uint8_t)
                ((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
}

void SHA1Init(SHA1_CTX *context)
{
    /* SHA1 initialization constants */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}


struct mesa_sha1 *
_mesa_sha1_init(void)
{
   SHA1_CTX *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;

   SHA1Init(ctx);
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   SHA1_CTX *sha1_ctx = (SHA1_CTX *) ctx;

   SHA1Update(sha1_ctx, data, size);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   SHA1_CTX *sha1_ctx = (SHA1_CTX *) ctx;

   SHA1Final(result, sha1_ctx);
   free(sha1_ctx);
   return 1;
}

#elif defined(HAVE_SHA1_IN_COMMONCRYPTO)        /* Use CommonCrypto for SHA1 */

#include <CommonCrypto/CommonDigest.h>

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   CC_SHA1_CTX *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;

   CC_SHA1_Init(ctx);
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   CC_SHA1_CTX *sha1_ctx = (CC_SHA1_CTX *) ctx;

   CC_SHA1_Update(sha1_ctx, data, size);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   CC_SHA1_CTX *sha1_ctx = (CC_SHA1_CTX *) ctx;

   CC_SHA1_Final(result, sha1_ctx);
   free(sha1_ctx);
   return 1;
}

#elif defined(HAVE_SHA1_IN_CRYPTOAPI)        /* Use CryptoAPI for SHA1 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

static HCRYPTPROV hProv;

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   HCRYPTHASH *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;

   CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
   CryptCreateHash(hProv, CALG_SHA1, 0, 0, ctx);
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   HCRYPTHASH *hHash = (HCRYPTHASH *) ctx;

   CryptHashData(*hHash, data, size, 0);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   HCRYPTHASH *hHash = (HCRYPTHASH *) ctx;
   DWORD len = 20;

   CryptGetHashParam(*hHash, HP_HASHVAL, result, &len, 0);
   CryptDestroyHash(*hHash);
   CryptReleaseContext(hProv, 0);
   free(ctx);
   return 1;
}

#elif defined(HAVE_SHA1_IN_LIBNETTLE)   /* Use libnettle for SHA1 */

#include <nettle/sha.h>

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   struct sha1_ctx *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;
   sha1_init(ctx);
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   sha1_update((struct sha1_ctx *) ctx, size, data);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   sha1_digest((struct sha1_ctx *) ctx, 20, result);
   free(ctx);
   return 1;
}

#elif defined(HAVE_SHA1_IN_LIBGCRYPT)   /* Use libgcrypt for SHA1 */

#include <gcrypt.h>
#include "c11/threads.h"

static void _mesa_libgcrypt_init(void)
{
   if (!gcry_check_version(NULL))
      return;

   gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
   gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   static once_flag flag = ONCE_FLAG_INIT;
   gcry_md_hd_t h;
   gcry_error_t err;

   call_once(&flag, _mesa_libgcrypt_init);

   err = gcry_md_open(&h, GCRY_MD_SHA1, 0);
   if (err)
      return NULL;
   return (struct mesa_sha1 *) h;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   gcry_md_hd_t h = (gcry_md_hd_t) ctx;

   gcry_md_write(h, data, size);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   gcry_md_hd_t h = (gcry_md_hd_t) ctx;

   memcpy(result, gcry_md_read(h, GCRY_MD_SHA1), 20);
   gcry_md_close(h);
   return 1;
}

#elif defined(HAVE_SHA1_IN_LIBSHA1)     /* Use libsha1 */

#include <libsha1.h>

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   sha1_ctx *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;
   sha1_begin(ctx);
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   sha1_hash(data, size, (sha1_ctx *) ctx);
   return 1;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   sha1_end(result, (sha1_ctx *) ctx);
   free(ctx);
   return 1;
}

#else                           /* Use OpenSSL's libcrypto */

#include <stddef.h>             /* buggy openssl/sha.h wants size_t */
#include <openssl/sha.h>

struct mesa_sha1 *
_mesa_sha1_init(void)
{
   int ret;
   SHA_CTX *ctx = malloc(sizeof(*ctx));

   if (!ctx)
      return NULL;
   ret = SHA1_Init(ctx);
   if (!ret) {
      free(ctx);
      return NULL;
   }
   return (struct mesa_sha1 *) ctx;
}

int
_mesa_sha1_update(struct mesa_sha1 *ctx, const void *data, int size)
{
   int ret;
   SHA_CTX *sha_ctx = (SHA_CTX *) ctx;

   ret = SHA1_Update(sha_ctx, data, size);
   if (!ret)
      free(sha_ctx);
   return ret;
}

int
_mesa_sha1_final(struct mesa_sha1 *ctx, unsigned char result[20])
{
   int ret;
   SHA_CTX *sha_ctx = (SHA_CTX *) ctx;

   ret = SHA1_Final(result, (SHA_CTX *) sha_ctx);
   free(sha_ctx);
   return ret;
}

#endif

void
_mesa_sha1_compute(const void *data, size_t size, unsigned char result[20])
{
   struct mesa_sha1 *ctx;

   ctx = _mesa_sha1_init();
   _mesa_sha1_update(ctx, data, size);
   _mesa_sha1_final(ctx, result);
}

char *
_mesa_sha1_format(char *buf, const unsigned char *sha1)
{
   static const char hex_digits[] = "0123456789abcdef";
   int i;

   for (i = 0; i < 40; i += 2) {
      buf[i] = hex_digits[sha1[i >> 1] >> 4];
      buf[i + 1] = hex_digits[sha1[i >> 1] & 0x0f];
   }
   buf[i] = '\0';

   return buf;
}

#endif

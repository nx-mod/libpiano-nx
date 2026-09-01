/* See include/gcrypt.h for why this exists. */
#include <stdlib.h>
#include <string.h>

#include <mbedtls/blowfish.h>

#include "gcrypt.h"

struct gcry_cipher_handle {
	mbedtls_blowfish_context ctx;
};

gcry_error_t gcry_cipher_open (gcry_cipher_hd_t *hd, int algo, int mode,
		unsigned int flags) {
	(void) algo; /* only GCRY_CIPHER_BLOWFISH is ever requested */
	(void) mode; /* only GCRY_CIPHER_MODE_ECB is ever requested */
	(void) flags;

	struct gcry_cipher_handle *h = calloc (1, sizeof (*h));
	if (h == NULL) {
		return 1;
	}
	mbedtls_blowfish_init (&h->ctx);
	*hd = h;
	return GPG_ERR_NO_ERROR;
}

gcry_error_t gcry_cipher_setkey (gcry_cipher_hd_t hd, const void *key,
		size_t keylen) {
	if (mbedtls_blowfish_setkey (&hd->ctx, (const unsigned char *) key,
			(unsigned int) (keylen * 8)) != 0) {
		return 1;
	}
	return GPG_ERR_NO_ERROR;
}

/* libpiano only ever calls these in-place (in == NULL, out holds the data,
 * outsize/inlen give its length), always a multiple of the 8-byte
 * blowfish block size -- see PianoEncryptString/PianoDecryptString. */
static gcry_error_t crypt_ecb (gcry_cipher_hd_t hd, void *out, size_t outsize,
		const void *in, size_t inlen, int mbedtls_mode) {
	unsigned char *buf = (unsigned char *) out;
	size_t len = (in == NULL) ? outsize : inlen;

	if (in != NULL && in != out) {
		memcpy (out, in, len);
	}

	if (len % MBEDTLS_BLOWFISH_BLOCKSIZE != 0) {
		return 1;
	}

	for (size_t off = 0; off < len; off += MBEDTLS_BLOWFISH_BLOCKSIZE) {
		unsigned char block[MBEDTLS_BLOWFISH_BLOCKSIZE];
		memcpy (block, buf + off, MBEDTLS_BLOWFISH_BLOCKSIZE);
		if (mbedtls_blowfish_crypt_ecb (&hd->ctx, mbedtls_mode, block,
				buf + off) != 0) {
			return 1;
		}
	}

	return GPG_ERR_NO_ERROR;
}

gcry_error_t gcry_cipher_encrypt (gcry_cipher_hd_t hd, void *out,
		size_t outsize, const void *in, size_t inlen) {
	return crypt_ecb (hd, out, outsize, in, inlen, MBEDTLS_BLOWFISH_ENCRYPT);
}

gcry_error_t gcry_cipher_decrypt (gcry_cipher_hd_t hd, void *out,
		size_t outsize, const void *in, size_t inlen) {
	return crypt_ecb (hd, out, outsize, in, inlen, MBEDTLS_BLOWFISH_DECRYPT);
}

void gcry_cipher_close (gcry_cipher_hd_t hd) {
	if (hd == NULL) {
		return;
	}
	mbedtls_blowfish_free (&hd->ctx);
	free (hd);
}

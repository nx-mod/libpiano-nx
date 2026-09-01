/* Minimal libgcrypt-compatible shim for the Switch port.
 *
 * libpiano only ever uses gcrypt for one thing: Blowfish/ECB encrypt and
 * decrypt of the Pandora partner keys (see piano.c PianoInit and
 * crypt.c). There is no devkitPro portlib for libgcrypt itself (it needs
 * libgpg-error, secure memory via mlock, and a build system that assumes a
 * hosted OS), so rather than port all of libgcrypt for one cipher mode,
 * this header/impl reimplements just the subset of the API libpiano calls,
 * backed by mbedtls_blowfish_* (switch-mbedtls, already a portlib and
 * still ships Blowfish support as of the installed 2.28.x branch).
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int gcry_error_t;
typedef struct gcry_cipher_handle *gcry_cipher_hd_t;

#define GPG_ERR_NO_ERROR 0

#define GCRY_CIPHER_BLOWFISH   1
#define GCRY_CIPHER_MODE_ECB   1

gcry_error_t gcry_cipher_open (gcry_cipher_hd_t *hd, int algo, int mode,
		unsigned int flags);
gcry_error_t gcry_cipher_setkey (gcry_cipher_hd_t hd, const void *key,
		size_t keylen);
gcry_error_t gcry_cipher_encrypt (gcry_cipher_hd_t hd, void *out,
		size_t outsize, const void *in, size_t inlen);
gcry_error_t gcry_cipher_decrypt (gcry_cipher_hd_t hd, void *out,
		size_t outsize, const void *in, size_t inlen);
void gcry_cipher_close (gcry_cipher_hd_t hd);

/* pianobar's main.c calls these three at startup purely for gcrypt's own
 * library-init/secure-memory setup, neither of which applies to this
 * shim -- no-ops. */
#define GCRYCTL_DISABLE_SECMEM          0
#define GCRYCTL_INITIALIZATION_FINISHED 1

const char *gcry_check_version (const char *req_version);
gcry_error_t gcry_control (int cmd, ...);

#ifdef __cplusplus
}
#endif

#include <string.h>
#include <openssl/sha.h>

char *
eke_crypt_get(const char *str)
{
    char buf[SHA_DIGEST_LENGTH + 1];

    SHA1(str, strlen(str), buf);
    buf[SHA_DIGEST_LENGTH] = '\0';
    return strdup(buf);
}


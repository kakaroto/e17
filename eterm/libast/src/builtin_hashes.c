/*
 * Copyright (C) 1997-2004, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

/*
 * Bob Jenkins' hash algorithm as published in December 1996.  Public
 * domain.  See http://burtleburtle.net/bob/hash/
 */

/**
 * Hashes a variable-length key into a 32-bit unsigned integer value.
 *
 * This function hashes a bitstream of a given length into a 32-bit
 * hash value suitable for use in hash tables.  Note that this
 * function should NOT be used for cryptography.  About 6n+36
 * instructions for an n-byte key.
 *
 * For a hash value of w bits, the returned value should be
 * bitwise-AND'd with JENKINS_HASH_MASK(w), and the hash table should
 * have JENKINS_HASH_SIZE(w) buckets.
 *
 * @param key    Pointer to bitstream holding key.
 * @param length Number of bytes in bitstream.
 * @param seed   The last hash value returned, or an arbitrary seed
 *               value.
 * @return       A 32-bit hash value.
 *
 */
spif_uint32_t
jenkins_hash(register spif_uint8_t *key, register spif_uint32_t length, register spif_uint32_t seed)
{
    register spif_uint32_t a, b, c, len;

    len = length;
    a = b = 0xf721b64d;  /* This can be any 32-bit value. */
    c = seed;

    /* The loop below handles most of the key (all but the last
       length % 12 bytes). */
    while (len >= 12) {
        a += (key[0] + (SPIF_CAST(uint32) key[1] << 8) + (SPIF_CAST(uint32) key[2] << 16) + (SPIF_CAST(uint32) key[3] << 24));
        b += (key[4] + (SPIF_CAST(uint32) key[5] << 8) + (SPIF_CAST(uint32) key[6] << 16) + (SPIF_CAST(uint32) key[7] << 24));
        c += (key[8] + (SPIF_CAST(uint32) key[9] << 8) + (SPIF_CAST(uint32) key[10] << 16) + (SPIF_CAST(uint32) key[11] << 24));
        JENKINS_MIX(a, b, c);
        key += 12;
        len -= 12;
    }

    /* The switch below handles the last length % 12 (0 through 11)
       bytes.  All cases drop through to the next case. */
    c += length;
    switch (len) {
        case 11:  c += (SPIF_CAST(uint32) key[10] << 24);
        case 10:  c += (SPIF_CAST(uint32) key[9] << 16);
        case 9:   c += (SPIF_CAST(uint32) key[8] << 8);
        case 8:   b += (SPIF_CAST(uint32) key[7] << 24);
        case 7:   b += (SPIF_CAST(uint32) key[6] << 16);
        case 6:   b += (SPIF_CAST(uint32) key[5] << 8);
        case 5:   b += key[4];
        case 4:   a += (SPIF_CAST(uint32) key[3] << 24);
        case 3:   a += (SPIF_CAST(uint32) key[2] << 16);
        case 2:   a += (SPIF_CAST(uint32) key[1] << 8);
        case 1:   a += key[0];
        /* case 0: nothing left to add */
    }
    JENKINS_MIX(a, b, c);

    return c;
}

/**
 * Hashes a variable-length key into a 32-bit unsigned integer value.
 *
 * This function hashes a series of 32-bit unsigned integers of a
 * given length into a 32-bit hash value suitable for use in hash
 * tables.  This hash is basically identical to jenkins_hash(), except
 * that the key length must be a whole number of 32-bit dword's, and
 * the length is given in spif_uint32_t's, not bytes.  It is much
 * faster than jenkins_hash(), so if padding keys to 32 bit chunks is
 * inexpensive, this function is probably preferable.
 *
 * @param key    Pointer to bitstream holding key.
 * @param length Number of 32-bit integers in bitstream.
 * @param seed   The last hash value returned, or an arbitrary seed
 *               value.
 * @return       A 32-bit hash value.
 *
 */
spif_uint32_t
jenkins_hash32(register spif_uint32_t *key, register spif_uint32_t length, register spif_uint32_t seed)
{
    register spif_uint32_t a, b, c, len;

    len = length;
    a = b = 0xf721b64d;  /* This can be any 32-bit value. */
    c = seed;

    /* The loop below handles most of the key (all but the last
       length % 3 uint32's). */
    while (len >= 3) {
        a += key[0];
        b += key[1];
        c += key[2];
        JENKINS_MIX(a, b, c);
        key += 3;
        len -= 3;
    }

    /* The switch below handles the last length % 3 (0 through 2)
       uint32's.  All cases drop through to the next case. */
    c += length;
    switch (len) {
        case 2:   b += key[1];
        case 1:   a += key[0];
        /* case 0: nothing left to add */
    }
    JENKINS_MIX(a, b, c);

    return c;
}

#if !(WORDS_BIGENDIAN)
/**
 * Hashes a variable-length key into a 32-bit unsigned integer value.
 *
 * This function hashes a bitstream of a given length into a 32-bit
 * hash value suitable for use in hash tables.  This hash is basically
 * identical to jenkins_hash(), except that it only works on
 * little-endian machines (e.g., Intel x86 and VAX).  It is faster
 * than jenkins_hash(), so it should be preferred on little-endian
 * systems.
 *
 * @param key    Pointer to bitstream holding key.
 * @param length Number of bytes in bitstream.
 * @param seed   The last hash value returned, or an arbitrary seed
 *               value.
 * @return       A 32-bit hash value.
 *
 */
spif_uint32_t
jenkins_hashLE(register spif_uint8_t *key, register spif_uint32_t length, register spif_uint32_t seed)
{
    register spif_uint32_t a, b, c, len;

    len = length;
    a = b = 0xf721b64d;  /* This can be any 32-bit value. */
    c = seed;

    /* The loop below handles most of the key (all but the last
       length % 12 bytes). */
    if ((SPIF_CAST(uint32) key) & 3) {
        /* Not 32-bit aligned.  Use old method. */
        while (len >= 12) {
            a += (key[0] + (SPIF_CAST(uint32) key[1] << 8) + (SPIF_CAST(uint32) key[2] << 16) + (SPIF_CAST(uint32) key[3] << 24));
            b += (key[4] + (SPIF_CAST(uint32) key[5] << 8) + (SPIF_CAST(uint32) key[6] << 16) + (SPIF_CAST(uint32) key[7] << 24));
            c += (key[8] + (SPIF_CAST(uint32) key[9] << 8) + (SPIF_CAST(uint32) key[10] << 16) + (SPIF_CAST(uint32) key[11] << 24));
            JENKINS_MIX(a, b, c);
            key += 12;
            len -= 12;
        }
    } else {
        /* 32-bit aligned.  Use speedier method. */
        while (len >= 12) {
            /* These three lines are the only ones which differ from
               jenkins_hash(). */
            a += *key;
            b += *(SPIF_CAST_PTR(uint32) (key + 4));
            c += *(SPIF_CAST_PTR(uint32) (key + 8));
            JENKINS_MIX(a, b, c);
            key += 12;
            len -= 12;
        }
    }

    /* The switch below handles the last length % 12 (0 through 11)
       bytes.  All cases drop through to the next case. */
    c += length;
    switch (len) {
        case 11:  c += (SPIF_CAST(uint32) key[10] << 24);
        case 10:  c += (SPIF_CAST(uint32) key[9] << 16);
        case 9:   c += (SPIF_CAST(uint32) key[8] << 8);
        case 8:   b += (SPIF_CAST(uint32) key[7] << 24);
        case 7:   b += (SPIF_CAST(uint32) key[6] << 16);
        case 6:   b += (SPIF_CAST(uint32) key[5] << 8);
        case 5:   b += key[4];
        case 4:   a += (SPIF_CAST(uint32) key[3] << 24);
        case 3:   a += (SPIF_CAST(uint32) key[2] << 16);
        case 2:   a += (SPIF_CAST(uint32) key[1] << 8);
        case 1:   a += key[0];
        /* case 0: nothing left to add */
    }
    JENKINS_MIX(a, b, c);

    return c;
}
#endif

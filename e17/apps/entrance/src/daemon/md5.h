#ifndef _MD5_H_
#define _MD5_H_

#include <sys/types.h>

#define MD5_HASHBYTES 16

typedef struct _Entranced_MD5_Context {
	u_int32_t buf[4];
	u_int32_t bits[2];
	unsigned char in[64];
} Entranced_MD5_Context;

extern void   entranced_md5_init(Entranced_MD5_Context **context);
extern void   entranced_md5_update(Entranced_MD5_Context *context,unsigned char const *buf,unsigned len);
extern void   entranced_md5_final(unsigned char digest[MD5_HASHBYTES], Entranced_MD5_Context *context);
extern void   entranced_md5_transform(u_int32_t buf[4], u_int32_t const in[16]);

#endif

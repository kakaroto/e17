#include <config.h>

#ifdef DO_MMX_ASM

/*\ 
|*| MMX assembly blending routines for Imlib2
|*| Written by Willem Monsuwe <willem@stack.nl>
|*|
|*| Special (hairy) constructs are only commented on first use.
\*/

/*\ All functions have the same calling convention:
|*|  __imlib_mmx_<op>_rgba_to_rgb[A](void *src, int sw, void *dst, int dw,
|*|				     int w, int h, ImlibColorModifier *cm)
\*/

#define src	8(%ebp)
#define sw	12(%ebp)
#define dst	16(%ebp)
#define dw	20(%ebp)
#define w	24(%ebp)
#define h	28(%ebp)
#define cm	32(%ebp)

.global __imlib_mmx_blend_rgba_to_rgb
	.type __imlib_mmx_blend_rgba_to_rgb,@function
.global __imlib_mmx_blend_rgba_to_rgba
	.type __imlib_mmx_blend_rgba_to_rgba,@function
.global __imlib_mmx_copy_rgba_to_rgb
	.type __imlib_mmx_copy_rgba_to_rgb,@function
.global __imlib_mmx_copy_rgba_to_rgba
	.type __imlib_mmx_copy_rgba_to_rgba,@function
.global __imlib_mmx_copy_rgb_to_rgba
	.type __imlib_mmx_copy_rgb_to_rgba,@function
.global __imlib_mmx_add_blend_rgba_to_rgb
	.type __imlib_mmx_add_blend_rgba_to_rgb,@function
.global __imlib_mmx_add_blend_rgba_to_rgba
	.type __imlib_mmx_add_blend_rgba_to_rgba,@function
.global __imlib_mmx_add_copy_rgba_to_rgb
	.type __imlib_mmx_add_copy_rgba_to_rgb,@function
.global __imlib_mmx_add_copy_rgba_to_rgba
	.type __imlib_mmx_add_copy_rgba_to_rgba,@function
.global __imlib_mmx_subtract_blend_rgba_to_rgb
	.type __imlib_mmx_subtract_blend_rgba_to_rgb,@function
.global __imlib_mmx_subtract_blend_rgba_to_rgba
	.type __imlib_mmx_subtract_blend_rgba_to_rgba,@function
.global __imlib_mmx_subtract_copy_rgba_to_rgb
	.type __imlib_mmx_subtract_copy_rgba_to_rgb,@function
.global __imlib_mmx_subtract_copy_rgba_to_rgba
	.type __imlib_mmx_subtract_copy_rgba_to_rgba,@function
.global __imlib_mmx_reshade_blend_rgba_to_rgb
	.type __imlib_mmx_reshade_blend_rgba_to_rgb,@function
.global __imlib_mmx_reshade_blend_rgba_to_rgba
	.type __imlib_mmx_reshade_blend_rgba_to_rgba,@function
.global __imlib_mmx_reshade_copy_rgba_to_rgb
	.type __imlib_mmx_reshade_copy_rgba_to_rgb,@function
.global __imlib_mmx_reshade_copy_rgba_to_rgba
	.type __imlib_mmx_reshade_copy_rgba_to_rgba,@function

.bss
.text
.align 8
/*\ Some useful masks \*/
m0X000000: .byte   0,   0,   0,   0,   0,   0, 255,   0
m10000000: .byte   0,   0,   0,   0,   0,   0,   0,   1
m00XXXXXX: .byte 255, 255, 255, 255, 255, 255,   0,   0
mVX000000: .byte   0,   0,   0,   0,   0,   0, 255, 127
mV0000000: .byte   0,   0,   0,   0,   0,   0,   0, 128
m0XXX0XXX: .byte 255, 255, 255,   0, 255, 255, 255,   0
mX000X000: .byte   0,   0,   0, 255,   0,   0,   0, 255
m10001000: .byte   0,   0,   0,   1,   0,   0,   0,   1
m000V0V0V: .byte 127,   0, 127,   0, 127,   0,   0,   0
mI0000000: .byte   0,   0,   0,   0,   0,   0,   0,  64
m0VVV0VVV: .byte 127, 127, 127,   0, 127, 127, 127,   0


/*\ MMX register use:
|*| %mm1 = Source value
|*| %mm2 = Destination value
|*| %mm3 = Alpha value
|*| %mm4 = 0
|*| %mm5-%mm6 = masks
\*/

/*\ Common code \*/
/*\ Set MMX mode, save registers, load common parameters \*/
#define ENTER			\
	emms;			\
	pushl %ebp;		\
	movl  %esp, %ebp;	\
	pushl %ebx;		\
	pushl %ecx;		\
	pushl %edx;		\
	pushl %edi;		\
	pushl %esi;		\
	movl  src,  %esi;	\
	movl  dst,  %edi;	\
	movl  w,    %ebx;	\
	movl  h,    %edx

/*\ Unset MMX mode, reset registers, return \*/
#define LEAVE			\
	emms;			\
	popl %esi;		\
	popl %edi;		\
	popl %edx;		\
	popl %ecx;		\
	popl %ebx;		\
	movl %ebp, %esp;	\
	popl %ebp;		\
	ret


__imlib_mmx_blend_rgba_to_rgb:
	ENTER

	pxor %mm4, %mm4

	decl %edx
	js .end_blend_rgb
	decl %ebx
	js .end_blend_rgb
.for_loop_y_blend_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_blend_rgb:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes
	|*|  which are treated as four words.
	|*| Result ranges from [0, 0x7fff), and is mapped to
	|*|  point value in [0.0, 1.0) by using the high word
	|*|  of the 16->32 multiplications.
	|*| (Because we want the unsigned value we shift one bit,
	|*|  and also shift the other factor to compensate.)
	|*| Magic to get the fourth byte: lhh
	\*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	/*\ Make the alpha value that gets multiplied to the
	|*|  alpha channels 0, so the resulting alpha value is
	|*|  the destination alpha value.
	\*/
	psrlq $16, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (a * (s - d)) \*/
	psubw %mm2, %mm1
	psllw $1, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_blend_rgb

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_blend_rgb

.end_blend_rgb:
	LEAVE


__imlib_mmx_blend_rgba_to_rgba:
	ENTER

	pxor %mm4, %mm4
	movq m10000000, %mm5
	movq m00XXXXXX, %mm6

	decl %edx
	js .end_blend_rgba
	decl %ebx
	js .end_blend_rgba
.for_loop_y_blend_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_blend_rgba:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and target, a = src + (255 - dest) \*/
	movq %mm2, %mm3
	pxor %mm6, %mm3
	paddusb %mm1, %mm3
	/*\ Unpack/copy to eight bytes \*/
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (a * (s - d)) \*/
	psubw %mm2, %mm1
	/*\ Make the alpha channel that gets multiplied to the
	|*|  alpha value 256, so the resulting alpha value is
	|*|  the sum of the source and destination alpha value.
	\*/
	pand %mm6, %mm1
	por %mm5, %mm1

	psllw $1, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_blend_rgba

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_blend_rgba

.end_blend_rgba:
	LEAVE


__imlib_mmx_copy_rgba_to_rgb:
	ENTER

	movq m0XXX0XXX, %mm5
	movq mX000X000, %mm6

	decl %edx
	js .end_copy_rgb
	decl %ebx
	js .end_copy_rgb
.for_loop_y_copy_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_copy_rgb

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source, get alpha of destination \*/
	pand %mm5, %mm1
	pand %mm6, %mm2

	/*\ d = d | s, and save \*/
	por %mm1, %mm2
	movd %mm2, (%edi, %ecx, 8)
	jz .skip_x_copy_rgb

.for_loop_x_copy_rgb:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source \*/
	pand %mm5, %mm1
	pand %mm6, %mm2

	/*\ d = d | s, and save \*/
	por %mm1, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_copy_rgb
.skip_x_copy_rgb:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_copy_rgb

.end_copy_rgb:
	LEAVE


__imlib_mmx_copy_rgba_to_rgba:
	ENTER

	decl %edx
	js .end_copy_rgba
	decl %ebx
	js .end_copy_rgba
.for_loop_y_copy_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_copy_rgba

	/*\ Load source, save destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd %mm1, (%edi, %ecx, 8)

	jz .skip_x_copy_rgba

.for_loop_x_copy_rgba:
	/*\ Load source, save destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq %mm1, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_copy_rgba
.skip_x_copy_rgba:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_copy_rgba

.end_copy_rgba:
	LEAVE


__imlib_mmx_copy_rgb_to_rgba:
	ENTER

	movq mX000X000, %mm5

	decl %edx
	js .end_copy_rgb_rgba
	decl %ebx
	js .end_copy_rgb_rgba
.for_loop_y_copy_rgb_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_copy_rgb_rgba

	/*\ Load source, make alpha 0xff, save destination \*/
	movd (%esi, %ecx, 8), %mm1
	por %mm5, %mm1
	movd %mm1, (%edi, %ecx, 8)

	jz .skip_x_copy_rgb_rgba

.for_loop_x_copy_rgb_rgba:
	/*\ Load source, save destination \*/
	movq (%esi, %ecx, 8), %mm1
	por %mm5, %mm1
	movq %mm1, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_copy_rgb_rgba
.skip_x_copy_rgb_rgba:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_copy_rgb_rgba

.end_copy_rgb_rgba:
	LEAVE


__imlib_mmx_add_blend_rgba_to_rgb:
	ENTER

	pxor %mm4, %mm4

	decl %edx
	js .end_add_blend_rgb
	decl %ebx
	js .end_add_blend_rgb
.for_loop_y_add_blend_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_add_blend_rgb:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	psrlq $16, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (a * s) \*/
	psllw $1, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_add_blend_rgb

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_add_blend_rgb

.end_add_blend_rgb:
	LEAVE


__imlib_mmx_add_blend_rgba_to_rgba:
	ENTER

	pxor %mm4, %mm4
	movq mVX000000, %mm5

	decl %edx
	js .end_add_blend_rgba
	decl %ebx
	js .end_add_blend_rgba
.for_loop_y_add_blend_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_add_blend_rgba:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	/*\ Make the alpha value that gets multiplied to the
	|*|  alpha channels 0x7fff, so the resulting alpha value is
	|*|  the sum of the source and destination alpha values.
	\*/
	por %mm5, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (a * s) \*/
	psllw $1, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_add_blend_rgba

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_add_blend_rgba

.end_add_blend_rgba:
	LEAVE


__imlib_mmx_add_copy_rgba_to_rgb:
	ENTER

	movq m0XXX0XXX, %mm5

	decl %edx
	js .end_add_copy_rgb
	decl %ebx
	js .end_add_copy_rgb
.for_loop_y_add_copy_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_add_copy_rgb

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source \*/
	pand %mm5, %mm1

	/*\ d = d + s, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	movd %mm2, (%edi, %ecx, 8)
	jz .skip_x_add_copy_rgb

.for_loop_x_add_copy_rgb:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source \*/
	pand %mm5, %mm1

	/*\ d = d + s, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_add_copy_rgb
.skip_x_add_copy_rgb:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_add_copy_rgb

.end_add_copy_rgb:
	LEAVE


__imlib_mmx_add_copy_rgba_to_rgba:
	ENTER

	decl %edx
	js .end_add_copy_rgba
	decl %ebx
	js .end_add_copy_rgba
.for_loop_y_add_copy_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_add_copy_rgba

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ d = d + s, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	movd %mm2, (%edi, %ecx, 8)
	jz .skip_x_add_copy_rgba

.for_loop_x_add_copy_rgba:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	/*\ d = d + s, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_add_copy_rgba
.skip_x_add_copy_rgba:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_add_copy_rgba

.end_add_copy_rgba:
	LEAVE


__imlib_mmx_subtract_blend_rgba_to_rgb:
	ENTER

	pxor %mm4, %mm4

	decl %edx
	js .end_subtract_blend_rgb
	decl %ebx
	js .end_subtract_blend_rgb
.for_loop_y_subtract_blend_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_subtract_blend_rgb:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	psrlq $16, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d - (a * s) \*/
	psllw $1, %mm1
	pmulhw %mm3, %mm1
	psubw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_subtract_blend_rgb

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_subtract_blend_rgb

.end_subtract_blend_rgb:
	LEAVE


__imlib_mmx_subtract_blend_rgba_to_rgba:
	ENTER

	pxor %mm4, %mm4
	movq mV0000000, %mm5

	decl %edx
	js .end_subtract_blend_rgba
	decl %ebx
	js .end_subtract_blend_rgba
.for_loop_y_subtract_blend_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_subtract_blend_rgba:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	/*\ Make alpha value that gets multiplied with alpha channel
	|*| 0x8000, (-1.0), so that the alpha result is s + d
	\*/
	psrlq $16, %mm3
	por %mm5, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d - (a * s) \*/
	psllw $1, %mm1
	pmulhw %mm3, %mm1
	psubw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_subtract_blend_rgba

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_subtract_blend_rgba

.end_subtract_blend_rgba:
	LEAVE


__imlib_mmx_subtract_copy_rgba_to_rgb:
	ENTER

	movq m0XXX0XXX, %mm5

	decl %edx
	js .end_subtract_copy_rgb
	decl %ebx
	js .end_subtract_copy_rgb
.for_loop_y_subtract_copy_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_subtract_copy_rgb

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source \*/
	pand %mm5, %mm1

	/*\ d = d - s, unsigned saturation, and save \*/
	psubusb %mm1, %mm2
	movd %mm2, (%edi, %ecx, 8)
	jz .skip_x_subtract_copy_rgb

.for_loop_x_subtract_copy_rgb:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	/*\ Clear alpha channel of source \*/
	pand %mm5, %mm1

	/*\ d = d - s, unsigned saturation, and save \*/
	psubusb %mm1, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_subtract_copy_rgb
.skip_x_subtract_copy_rgb:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_subtract_copy_rgb

.end_subtract_copy_rgb:
	LEAVE


__imlib_mmx_subtract_copy_rgba_to_rgba:
	ENTER

	movq mX000X000, %mm5

	decl %edx
	js .end_subtract_copy_rgba
	decl %ebx
	js .end_subtract_copy_rgba
.for_loop_y_subtract_copy_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_subtract_copy_rgba

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ Negate destination alphas \*/
	pxor %mm5, %mm2

	/*\ d = d - s, unsigned saturation, and save \*/
	psubusb %mm1, %mm2

	/*\ Negate result alphas \*/
	pxor %mm5, %mm2
	movd %mm2, (%edi, %ecx, 8)
	jz .skip_x_subtract_copy_rgba

.for_loop_x_subtract_copy_rgba:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2
	pxor %mm5, %mm2

	/*\ d = d - s, unsigned saturation, and save \*/
	psubusb %mm1, %mm2
	pxor %mm5, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_subtract_copy_rgba
.skip_x_subtract_copy_rgba:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_subtract_copy_rgba

.end_subtract_copy_rgba:
	LEAVE


__imlib_mmx_reshade_blend_rgba_to_rgb:
	ENTER

	pxor %mm4, %mm4
	movq m000V0V0V, %mm6

	decl %edx
	js .end_reshade_blend_rgb
	decl %ebx
	js .end_reshade_blend_rgb
.for_loop_y_reshade_blend_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_reshade_blend_rgb:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	psrlq $16, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (2 * a * (s - 127)) \*/
	psubw %mm6, %mm1
	psllw $2, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_reshade_blend_rgb

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_reshade_blend_rgb

.end_reshade_blend_rgb:
	LEAVE


__imlib_mmx_reshade_blend_rgba_to_rgba:
	ENTER

	pxor %mm4, %mm4
	movq mI0000000, %mm5
	movq m000V0V0V, %mm6

	decl %edx
	js .end_reshade_blend_rgba
	decl %ebx
	js .end_reshade_blend_rgba
.for_loop_y_reshade_blend_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

.for_loop_x_reshade_blend_rgba:
	/*\ Load source and destination \*/
	movd (%esi, %ecx, 4), %mm1
	movd (%edi, %ecx, 4), %mm2

	/*\ Get alpha from source and unpack/copy to eight bytes \*/
	movq %mm1, %mm3
	punpcklbw %mm3, %mm3
	punpckhwd %mm3, %mm3
	punpckhdq %mm3, %mm3
	psrlw $1, %mm3

	/*\ Make the alpha value that gets multiplied to the
	|*|  alpha channels 0x4000, so the resulting alpha value is
	|*|  the sum of the source and destination alpha values.
	\*/
	psrlq $16, %mm3
	por %mm5, %mm3

	/*\ Unpack source and destination, bytes to words \*/
	punpcklbw %mm4, %mm1
	punpcklbw %mm4, %mm2

	/*\ d = d + (2 * a * (s - 127)) \*/
	psubw %mm6, %mm1
	psllw $2, %mm1
	pmulhw %mm3, %mm1
	paddw %mm1, %mm2

	/*\ Pack into lower 4 bytes and save \*/
	packuswb %mm4, %mm2
	movd %mm2, (%edi, %ecx, 4)
	
	decl %ecx
	jns .for_loop_x_reshade_blend_rgba

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_reshade_blend_rgba

.end_reshade_blend_rgba:
	LEAVE


__imlib_mmx_reshade_copy_rgba_to_rgb:
	ENTER

	pxor %mm4, %mm4
	movq m0XXX0XXX, %mm5
	movq m0VVV0VVV, %mm6

	decl %edx
	js .end_reshade_copy_rgb
	decl %ebx
	js .end_reshade_copy_rgb
.for_loop_y_reshade_copy_rgb:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_reshade_copy_rgb

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ To take advantage of saturation and be able to do 8 bytes
	|*|  at a time, we divide reshading into two separate steps:
	|*|  adding values above 128, and subtracting values below 128
	|*| These values go into %mm1 and %mm3 respectively
	|*| - %mm1 becomes (2 * (s - 127))
	|*| - %mm3 becomes (2 * (255 - (127 + s))) = (2 * (128 - s))
	\*/
	movq %mm1, %mm3
	psubusb %mm6, %mm1
	paddusb %mm1, %mm1
	paddusb %mm6, %mm3
	pxor %mm5, %mm3
	paddusb %mm3, %mm3

	/*\ Clear alpha channel of s1 and s2 \*/
	pand %mm5, %mm1
	pand %mm5, %mm3

	/*\ d = d + s1 - s2, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	psubusb %mm3, %mm2
	movd %mm2, (%edi, %ecx, 8)

	/*\ MMX ops don't set flags \*/
	jz .skip_x_reshade_copy_rgb

.for_loop_x_reshade_copy_rgb:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	movq %mm1, %mm3
	psubusb %mm6, %mm1
	paddusb %mm1, %mm1
	paddusb %mm6, %mm3
	pxor %mm5, %mm3
	paddusb %mm3, %mm3

	/*\ Clear alpha channel of s1 and s2 \*/
	pand %mm5, %mm1
	pand %mm5, %mm3

	/*\ d = d + s1 - s2, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	psubusb %mm3, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_reshade_copy_rgb
.skip_x_reshade_copy_rgb:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_reshade_copy_rgb

.end_reshade_copy_rgb:
	LEAVE


__imlib_mmx_reshade_copy_rgba_to_rgba:
	ENTER

	pxor %mm4, %mm4
	movq m0XXX0XXX, %mm5
	movq m0VVV0VVV, %mm6

	decl %edx
	js .end_reshade_copy_rgba
	decl %ebx
	js .end_reshade_copy_rgba
.for_loop_y_reshade_copy_rgba:
	/*\ Load width \*/
	movl %ebx, %ecx

	/*\ Check for oddness \*/
	sarl $1, %ecx
	jc .for_loop_x_reshade_copy_rgba

	/*\ Load source and destination \*/
	movd (%esi, %ecx, 8), %mm1
	movd (%edi, %ecx, 8), %mm2

	/*\ This time, the alpha channels have to be added.
	|*| For that, the alpha channel of %mm1 should remain
	|*|  the same.  This is done by subtracting 0 from the
	|*|  alpha channel, and then doing the *2 via a separate
	|*|  register, clearing its alpha channel first.
	\*/
	movq %mm1, %mm3
	psubusb %mm6, %mm1
	movq %mm1, %mm0
	pand %mm5, %mm0
	paddusb %mm0, %mm1
	paddusb %mm6, %mm3
	pxor %mm5, %mm3
	paddusb %mm3, %mm3

	/*\ Clear alpha channel of s2 \*/
	pand %mm5, %mm3

	/*\ d = d + s1 - s2, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	psubusb %mm3, %mm2
	movd %mm2, (%edi, %ecx, 8)

	/*\ MMX ops don't set flags \*/
	jz .skip_x_reshade_copy_rgba

.for_loop_x_reshade_copy_rgba:
	/*\ Load source and destination \*/
	movq (%esi, %ecx, 8), %mm1
	movq (%edi, %ecx, 8), %mm2

	movq %mm1, %mm3
	psubusb %mm6, %mm1
	movq %mm1, %mm0
	pand %mm5, %mm0
	paddusb %mm0, %mm1
	paddusb %mm6, %mm3
	pxor %mm5, %mm3
	paddusb %mm3, %mm3

	/*\ Clear alpha channel of s2 \*/
	pand %mm5, %mm3

	/*\ d = d + s1 - s2, unsigned saturation, and save \*/
	paddusb %mm1, %mm2
	psubusb %mm3, %mm2
	movq %mm2, (%edi, %ecx, 8)
	
	decl %ecx
	jns .for_loop_x_reshade_copy_rgba
.skip_x_reshade_copy_rgba:

	/*\ Add row widths \*/
	movl sw, %ecx
	leal (%esi, %ecx, 4), %esi
	movl dw, %ecx
	leal (%edi, %ecx, 4), %edi
	decl %edx
	jns .for_loop_y_reshade_copy_rgba

.end_reshade_copy_rgba:
	LEAVE

#endif

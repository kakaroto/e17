#include <config.h>

#ifdef DO_MMX_ASM

/*
* AlphaBlending using MMX
* 
* __imlib_asm_blend_rgba_to_rgb: blend with alpha values
* for every pixel
*
* Copyright (C) 2000 Panagiotis Issaris <panagiotis.issaris@advalvas.be>
* This little snippet of code is released under the GPL.
* See http://www.gnu.org/ for information about GPL.
*/

.global __imlib_toggle_mmx
            .type __imlib_toggle_mmx,@function
.global __imlib_asm_blend_rgba_to_rgb
            .type __imlib_asm_blend_rgba_to_rgb,@function

.bss
.text
.align 4

/*****************************************************************************
 __imlib_toggle_mmx(void)
******************************************************************************/

__imlib_toggle_mmx:
  emms
ret



/*****************************************************************************
 __imlib_asm_blend_rgba_to_rgb(void *src, void *dst, int w, void *mmx_data)
******************************************************************************/

#define components   (%ebx)
#define mask_red    8(%ebx)
#define mask_green 16(%ebx)
#define mask_blue  24(%ebx)
#define mask_alpha 32(%ebx)
#define mask       40(%ebx)

__imlib_asm_blend_rgba_to_rgb:
  pushl     %ebp
  movl      %esp,        %ebp
		
/* Save all registers to the stack */
  pushl     %ebx
  pushl     %ecx
  pushl     %edx
  pushl     %edi
  pushl     %esi
  
  movl       8(%ebp),    %esi           /* esi = src; */
  movl      12(%ebp),    %eax           /* eax = dst; */
  movl      16(%ebp),    %edx           /* edx = w */
  movl      20(%ebp),    %ebx           /* ebx = mmx_data */

  imul      $4,          %edx           /* w *= 4; */
  
  movl      $0,          %ecx           /* int i = 0; */
  pxor      %mm1,        %mm1           /* mm1 = 0; */
.for_loop__imlib_asm_blend_rgba_to_rgb:

  movd      (%esi,%ecx), %mm2           /* mm2 = src[i]; */
  movd      (%eax,%ecx), %mm3           /* mm3 = dst[i]; */
  
  movq      %mm2,        %mm4           /* mm4 = mm2; */
  psrlq     $24,         %mm4           /* mm4 >>= 24; (mm4 = oooooooA) */

  punpcklbw %mm1,        %mm2           /* mm2 = mm2(oAoRoGoB) */
  punpcklbw %mm1,        %mm3           /* mm3 = mm3(oAoRoGoB) */
  psubw     %mm3,        %mm2           /* mm2 -= mm3; */

  punpcklwd %mm4,        %mm4           /* mm4 = mm4(oooooAoA) */
  punpckldq %mm4,        %mm4           /* mm4 = mm4(oAoAoAoA) */
  pmullw    %mm4,        %mm2           /* mm2 *= mm4; */

  psraw     $8,          %mm2           /* mm2 >>= 8; */
  paddw     %mm3,        %mm2           /* mm2 += mm3; */

  pand      components,  %mm2           /* mm2 &= components */
  packuswb  %mm1,        %mm2           /* mm2 = mm2(ooooARGB) */
#if 1 /* need this to be correct - but normally can be left out */
  por       mask_alpha,  %mm2           /* mm2 |= mask_alpha */
#endif

  movd      %mm2,        (%eax,%ecx)    /* dst[i] = mm3; */
  add       $4,          %ecx           /* i += 4; */

  cmp       %edx,        %ecx           /* if (i != w) */
  jne       .for_loop__imlib_asm_blend_rgba_to_rgb  /* goto .for_loop__imlib_asm_blend_rgba_to_rgb: */
  
/* Restore all registers from the stack */
  popl      %esi
  popl      %edi
  popl      %edx
  popl      %ecx
  popl      %ebx
	
  movl      %ebp,        %esp
  popl      %ebp
ret

#endif

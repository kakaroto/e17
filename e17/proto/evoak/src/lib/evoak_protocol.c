#include "config.h"

#include <Eet.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Job.h>
#include <Ecore_Con.h>
#include <Ecore_Ipc.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Embryo.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <fnmatch.h>
#include <locale.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "evoak_protocol.h"

/* macros to make the encoders and decoders much more compact and faster to */
/* write, and have less bugs. encoded/decoded data is simply the binary */
/* data in memory flattened out (and byteswapped to little endian if needed) */
/* and then the referse done, filling in struct members etc. */
#ifdef WORDS_BIGENDIAN
#define SWAP2NET64(x) _swap_64(x)
#define SWAP2CPU64(x) _swap_64(x)
#define SWAP2NET32(x) _swap_32(x)
#define SWAP2CPU32(x) _swap_32(x)
#define SWAP2NET16(x) _swap_16(x)
#define SWAP2CPU16(x) _swap_16(x)
#define SWAP2NET8(x) (x)
#define SWAP2CPU8(x) (x)
#else
#define SWAP2NET64(x) (x)
#define SWAP2CPU64(x) (x)
#define SWAP2NET32(x) (x)
#define SWAP2CPU32(x) (x)
#define SWAP2NET16(x) (x)
#define SWAP2CPU16(x) (x)
#define SWAP2NET8(x) (x)
#define SWAP2CPU8(x) (x)
#endif

/* 1, 2, 4 and 8 byte datatypes */
#define GET64(v) {p->v = SWAP2CPU64(*(long long *)(ptr)); ptr += 8;}
#define GET32(v) {p->v = SWAP2CPU32(*(int *)(ptr)); ptr += 4;}
#define GET16(v) {p->v = SWAP2CPU16(*(short *)(ptr)); ptr += 2;}
#define GET8(v)  {p->v = SWAP2CPU8(*(char *)(ptr)); ptr += 1;}
#define PUT64(v) {*(long long *)(ptr) = SWAP2NET64(p->v); ptr += 8;}
#define PUT32(v) {*(int *)(ptr) = SWAP2NET32(p->v); ptr += 4;}
#define PUT16(v) {*(short *)(ptr) = SWAP2NET16(p->v); ptr += 2;}
#define PUT8(v)  {*(char *)(ptr) = SWAP2NET8(p->v); ptr += 1;}
#define PAD8()   ptr += 1
#define PAD16()  ptr += 2
#define PAD32()  ptr += 4
#define PAD64()  ptr += 8

/* strings */
#define CHEKS()  if (*((unsigned char *)d + s - 1) != 0) return 0;
#define GETS(v)  {if (ptr < ((unsigned char *)d + s)) {p->v = ptr; ptr += strlen(p->v) + 1;}}
#define PUTS(v, l) {strcpy(ptr, p->v); ptr += l + 1;}

/* handy to calculate what sized block we need to alloc */
#define SLEN(l, v) ((l = strlen(p->v)) + 1)

/* saves typing function headers */
#define DECPROTO(x) static int x(void *d, int s, void *pp)
#define ENCPROTO(x) static void *x(void *pp, int *s)

/* decoder setup - saves typing. requires data packet of exact size, or fail */
#define DECHEAD_EXACT(typ, x) \
	     typ *p; \
	     unsigned char *ptr; \
             p = (typ *)pp; \
             if (!d) return 0; \
             if (s != (x)) return 0; \
	     ptr = d;
/* decoder setup - saves typing. requires data packet of a minimum size */
#define DECHEAD_MIN(typ, x) \
	     typ *p; \
	     unsigned char *ptr; \
             p = (typ *)pp; \
             if (!d) return 0; \
             if (s < (x)) return 0; \
	     ptr = d;
/* footer for the hell of it */
#define DECFOOT() return 1
/* header for encoder - gives native strct type and size of flattened packet */
#define ENCHEAD(typ, sz) \
             typ *p; \
             unsigned char *d, *ptr; \
             int len; \
             p = (typ *)pp; \
             *s = 0; \
             len = sz; \
             d = malloc(len); \
             if (!d) return NULL; \
             *s = len; \
             ptr = d;
/* footer for the hell of it */
#define ENCFOOT() return d

/* byte swappers - for dealing with big vs little endian machines */
static unsigned short
_swap_16(unsigned short v)
{
   unsigned char *s, t;
   
   s = (unsigned char *)(&v);
   t = s[0]; s[0] = s[1]; s[1] = t;
   return v;
}

static unsigned int
_swap_32(unsigned int v)
{
   unsigned char *s, t;
   
   s = (unsigned char *)(&v);
   t = s[0]; s[0] = s[3]; s[3] = t;
   t = s[1]; s[1] = s[2]; s[2] = t;
   return v;
}

static unsigned long long
_swap_64(unsigned long long v)
{
   unsigned char *s, t;
   
   s = (unsigned char *)(&v);
   t = s[0]; s[0] = s[7]; s[7] = t;
   t = s[1]; s[1] = s[6]; s[6] = t;
   t = s[2]; s[2] = s[5]; s[5] = t;
   t = s[3]; s[3] = s[4]; s[4] = t;
   return v;
}

/* anything to set up the protocol handlers et.c */
void
_evoak_protocol_init(void)
{
}

void
_evoak_protocol_shutdown(void)
{
}


/****************************************************************************/
DECPROTO(_pr_connect_dec) {
   DECHEAD_MIN(Evoak_PR_Connect,
	       (4 + 4) + 
	       1 +
	       1 +
	       1);
   CHEKS();
   GET32(proto_min);
   GET32(proto_max);
   GETS(client_name);
   GETS(client_class);
   GETS(auth_key);
   DECFOOT();
}

ENCPROTO(_pr_connect_enc) {
   int l1, l2, l3;
   ENCHEAD(Evoak_PR_Connect,
	   (4 + 4) + 
	   SLEN(l1, client_name) +
	   SLEN(l2, client_class) +
	   SLEN(l3, auth_key));
   PUT32(proto_min);
   PUT32(proto_max);
   PUTS(client_name, l1);
   PUTS(client_class, l2);
   PUTS(auth_key, l3);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_canvas_info_dec) {
   DECHEAD_EXACT(Evoak_PR_Canvas_Info, 4 * 3);
   GET32(w);
   GET32(h);
   GET32(rot);
   DECFOOT();
}

ENCPROTO(_pr_canvas_info_enc) {
   ENCHEAD(Evoak_PR_Canvas_Info, 4 * 3);
   PUT32(w);
   PUT32(h);
   PUT32(rot);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_add_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Add, 1);
   GET8(type);
   DECFOOT();
}

ENCPROTO(_pr_object_add_enc) {
   ENCHEAD(Evoak_PR_Object_Add, 1);
   PUT8(type);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_move_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Move, 4 * 2);
   GET32(x);
   GET32(y);
   DECFOOT();
}

ENCPROTO(_pr_object_move_enc) {
   ENCHEAD(Evoak_PR_Object_Move, 4 * 2);
   PUT32(x);
   PUT32(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_resize_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Resize, 4 * 2);
   GET32(w);
   GET32(h);
   DECFOOT();
}

ENCPROTO(_pr_object_resize_enc) {
   ENCHEAD(Evoak_PR_Object_Resize, 4 * 2);
   PUT32(w);
   PUT32(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_clip_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Clip_Set, 4);
   GET32(clipper_id);
   DECFOOT();
}

ENCPROTO(_pr_object_clip_set_enc) {
   ENCHEAD(Evoak_PR_Object_Clip_Set, 4);
   PUT32(clipper_id);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_color_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Color_Set, 4);
   GET8(r);
   GET8(g);
   GET8(b);
   GET8(a);
   DECFOOT();
}

ENCPROTO(_pr_object_color_set_enc) {
   ENCHEAD(Evoak_PR_Object_Color_Set, 4);
   PUT8(r);
   PUT8(g);
   PUT8(b);
   PUT8(a);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_layer_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Layer_Set, 4);
   GET32(layer);
   DECFOOT();
}

ENCPROTO(_pr_object_layer_set_enc) {
   ENCHEAD(Evoak_PR_Object_Layer_Set, 4);
   PUT32(layer);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_stack_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Stack, 4 + 1);
   GET32(relative_id);
   GET8(relative);
   DECFOOT();
}

ENCPROTO(_pr_object_stack_enc) {
   ENCHEAD(Evoak_PR_Object_Stack, 4 + 1);
   PUT32(relative_id);
   PUT8(relative);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_prop_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Prop_Set, 2);
   GET8(property);
   GET8(value);
   DECFOOT();
}

ENCPROTO(_pr_object_event_prop_set_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Prop_Set, 2);
   PUT8(property);
   PUT8(value);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_mask_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Mask_Set, 4 * 2);
   GET32(mask1);
   GET32(mask2);
   DECFOOT();
}

ENCPROTO(_pr_object_event_mask_set_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Mask_Set, 4 * 2);
   PUT32(mask1);
   PUT32(mask2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_focus_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Focus_Set, 1);
   GET8(onoff);
   DECFOOT();
}

ENCPROTO(_pr_object_focus_set_enc) {
   ENCHEAD(Evoak_PR_Object_Focus_Set, 1);
   PUT8(onoff);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_file_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Image_File_Set, 1 + 1);
   GETS(file);
   GETS(key);
   DECFOOT();
}

ENCPROTO(_pr_object_image_file_set_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Image_File_Set,
	   SLEN(l1, file) +
	   SLEN(l2, key));
   PUTS(file, l1);
   PUTS(key, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_fill_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Fill_Set, 4 * 4);
   GET32(x);
   GET32(y);
   GET32(w);
   GET32(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_fill_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Fill_Set, 4 * 4);
   PUT32(x);
   PUT32(y);
   PUT32(w);
   PUT32(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_border_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Border_Set, 4 * 2);
   GET16(l);
   GET16(r);
   GET16(t);
   GET16(b);
   DECFOOT();
}

ENCPROTO(_pr_object_image_border_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Border_Set, 4 * 2);
   PUT16(l);
   PUT16(r);
   PUT16(t);
   PUT16(b);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_smooth_scale_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Smooth_Scale_Set, 1);
   GET8(onoff);
   DECFOOT();
}

ENCPROTO(_pr_object_image_smooth_scale_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Smooth_Scale_Set, 1);
   PUT8(onoff);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_size_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Size_Set, 2 + 2);
   GET16(w);
   GET16(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_size_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Size_Set, 2 + 2);
   PUT16(w);
   PUT16(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_alpha_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Alpha_Set, 1);
   GET8(onoff);
   DECFOOT();
}

ENCPROTO(_pr_object_image_alpha_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Alpha_Set, 1);
   PUT8(onoff);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_mouse_move_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Mouse_Move, 4 * 7);
   GET32(x);
   GET32(y);
   GET32(px);
   GET32(py);
   GET32(bmask);
   GET32(modmask);
   GET32(lockmask);
   DECFOOT();
}

ENCPROTO(_pr_object_event_mouse_move_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Mouse_Move, 4 * 7);
   PUT32(x);
   PUT32(y);
   PUT32(px);
   PUT32(py);
   PUT32(bmask);
   PUT32(modmask);
   PUT32(lockmask);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_mouse_downup_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Mouse_Downup, (4 * 5) + 1);
   GET32(x);
   GET32(y);
   GET32(b);
   GET32(modmask);
   GET32(lockmask);
   GET8(downup);
   DECFOOT();
}

ENCPROTO(_pr_object_event_mouse_downup_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Mouse_Downup, (4 * 5) + 1);
   PUT32(x);
   PUT32(y);
   PUT32(b);
   PUT32(modmask);
   PUT32(lockmask);
   PUT8(downup);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_mouse_inout_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Mouse_Downup, (4 * 5) + 1);
   GET32(x);
   GET32(y);
   GET32(b);
   GET32(modmask);
   GET32(lockmask);
   GET8(downup);
   DECFOOT();
}

ENCPROTO(_pr_object_event_mouse_inout_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Mouse_Downup, (4 * 5) + 1);
   PUT32(x);
   PUT32(y);
   PUT32(b);
   PUT32(modmask);
   PUT32(lockmask);
   PUT8(downup);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_mouse_wheel_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Mouse_Wheel, 4 * 6);
   GET32(dir);
   GET32(z);
   GET32(x);
   GET32(y);
   GET32(modmask);
   GET32(lockmask);
   DECFOOT();
}

ENCPROTO(_pr_object_event_mouse_wheel_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Mouse_Wheel, 4 * 6);
   PUT32(dir);
   PUT32(z);
   PUT32(x);
   PUT32(y);
   PUT32(modmask);
   PUT32(lockmask);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_key_downup_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Event_Key_Downup, (4 * 2) + 1 + 1 + 1);
   GET32(modmask);
   GET32(lockmask);
   GET8(downup);
   GETS(keyname);
   GETS(string);
   DECFOOT();
}

ENCPROTO(_pr_object_event_key_downup_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Event_Key_Downup, 
	   (4 * 2) + 1 + 
	   SLEN(l1, keyname) + 
	   SLEN(l2, string));
   PUT32(modmask);
   PUT32(lockmask);
   PUT8(downup);
   PUTS(keyname, l1);
   PUTS(string, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_line_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Line_Set, 4 * 4);
   GET32(x1);
   GET32(y1);
   GET32(x2);
   GET32(y2);
   DECFOOT();
}

ENCPROTO(_pr_object_line_set_enc) {
   ENCHEAD(Evoak_PR_Object_Line_Set, 4 * 4);
   PUT32(x1);
   PUT32(y1);
   PUT32(x2);
   PUT32(y2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_grad_angle_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Grad_Angle_Set, 4);
   GET32(ang100);
   DECFOOT();
}

ENCPROTO(_pr_object_grad_angle_set_enc) {
   ENCHEAD(Evoak_PR_Object_Grad_Angle_Set, 4);
   PUT32(ang100);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_grad_color_add_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Grad_Color_Add, 4 + 4);
   GET32(dist);
   GET8(r);
   GET8(g);
   GET8(b);
   GET8(a);
   DECFOOT();
}

ENCPROTO(_pr_object_grad_color_add_enc) {
   ENCHEAD(Evoak_PR_Object_Grad_Color_Add, 4 + 4);
   PUT32(dist);
   PUT8(r);
   PUT8(g);
   PUT8(b);
   PUT8(a);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_poly_point_add_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Poly_Point_Add, 4 * 2);
   GET32(x);
   GET32(y);
   DECFOOT();
}

ENCPROTO(_pr_object_poly_point_add_enc) {
   ENCHEAD(Evoak_PR_Object_Poly_Point_Add, 4 * 2);
   PUT32(x);
   PUT32(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_text_source_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Text_Source_Set, 1);
   GETS(source);
   DECFOOT();
}

ENCPROTO(_pr_object_text_source_set_enc) {
   int l1;
   ENCHEAD(Evoak_PR_Object_Text_Source_Set, 
	   SLEN(l1, source));
   PUTS(source, l1);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_text_font_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Text_Font_Set, 4 + 1);
   GET32(size);
   GETS(font);
   DECFOOT();
}

ENCPROTO(_pr_object_text_font_set_enc) {
   int l1;
   ENCHEAD(Evoak_PR_Object_Text_Font_Set, 
	   4 +
	   SLEN(l1, font));
   PUT32(size);
   PUTS(font, l1);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_text_text_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Text_Text_Set, 1);
   GETS(text);
   DECFOOT();
}

ENCPROTO(_pr_object_text_text_set_enc) {
   int l1;
   ENCHEAD(Evoak_PR_Object_Text_Text_Set, 
	   SLEN(l1, text));
   PUTS(text, l1);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_move8_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Move8, 1 + 1);
   GET8(x);
   GET8(y);
   DECFOOT();
}

ENCPROTO(_pr_object_move8_enc) {
   ENCHEAD(Evoak_PR_Object_Move8, 1 + 1);
   PUT8(x);
   PUT8(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_move16_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Move16, 2 + 2);
   GET16(x);
   GET16(y);
   DECFOOT();
}

ENCPROTO(_pr_object_move16_enc) {
   ENCHEAD(Evoak_PR_Object_Move16, 2 + 2);
   PUT16(x);
   PUT16(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_move_rel8_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Move_Rel8, 1 + 1);
   GET8(x);
   GET8(y);
   DECFOOT();
}

ENCPROTO(_pr_object_move_rel8_enc) {
   ENCHEAD(Evoak_PR_Object_Move_Rel8, 1 + 1);
   PUT8(x);
   PUT8(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_move_rel16_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Move_Rel16, 2 + 2);
   GET16(x);
   GET16(y);
   DECFOOT();
}

ENCPROTO(_pr_object_move_rel16_enc) {
   ENCHEAD(Evoak_PR_Object_Move_Rel16, 2 + 2);
   PUT16(x);
   PUT16(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_resize8_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Resize8, 1 + 1);
   GET8(w);
   GET8(h);
   DECFOOT();
}

ENCPROTO(_pr_object_resize8_enc) {
   ENCHEAD(Evoak_PR_Object_Resize8, 1 + 1);
   PUT8(w);
   PUT8(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_resize16_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Resize16, 2 + 2);
   GET16(w);
   GET16(h);
   DECFOOT();
}

ENCPROTO(_pr_object_resize16_enc) {
   ENCHEAD(Evoak_PR_Object_Resize16, 2 + 2);
   PUT16(w);
   PUT16(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_resize_rel8_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Resize_Rel8, 1 + 1);
   GET8(w);
   GET8(h);
   DECFOOT();
}

ENCPROTO(_pr_object_resize_rel8_enc) {
   ENCHEAD(Evoak_PR_Object_Resize_Rel8, 1 + 1);
   PUT8(w);
   PUT8(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_resize_rel16_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Resize_Rel16, 2 + 2);
   GET16(w);
   GET16(h);
   DECFOOT();
}

ENCPROTO(_pr_object_resize_rel16_enc) {
   ENCHEAD(Evoak_PR_Object_Resize_Rel16, 2 + 2);
   PUT16(w);
   PUT16(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_fill8_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Fill8_Set, 1 + 1 + 1 + 1);
   GET8(x);
   GET8(y);
   GET8(w);
   GET8(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_fill8_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Fill8_Set, 1 + 1 + 1 + 1);
   PUT8(x);
   PUT8(y);
   PUT8(w);
   PUT8(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_fill16_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Fill16_Set, 2 + 2 + 2 + 2);
   GET16(x);
   GET16(y);
   GET16(w);
   GET16(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_fill16_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Fill16_Set, 2 + 2 + 2 + 2);
   PUT16(x);
   PUT16(y);
   PUT16(w);
   PUT16(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_fill_size8_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Fill_Size8_Set, 1 + 1);
   GET8(w);
   GET8(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_fill_size8_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Fill_Size8_Set, 1 + 1);
   PUT8(w);
   PUT8(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_image_fill_size16_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Image_Fill_Size16_Set, 2 + 2);
   GET16(w);
   GET16(h);
   DECFOOT();
}

ENCPROTO(_pr_object_image_fill_size16_set_enc) {
   ENCHEAD(Evoak_PR_Object_Image_Fill_Size16_Set, 2 + 2);
   PUT16(w);
   PUT16(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_object_move_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Object_Move, 4 * 2);
   GET32(x);
   GET32(y);
   DECFOOT();
}

ENCPROTO(_pr_object_event_object_move_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Object_Move, 4 * 2);
   PUT32(x);
   PUT32(y);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_object_resize_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Object_Resize, 4 * 2);
   GET32(w);
   GET32(h);
   DECFOOT();
}

ENCPROTO(_pr_object_event_object_resize_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Object_Resize, 4 * 2);
   PUT32(w);
   PUT32(h);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_object_restack_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Object_Restack, 4 + 1);
   GET32(relative_id);
   GET8(abovebelow);
   DECFOOT();
}

ENCPROTO(_pr_object_event_object_restack_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Object_Restack, 4 + 1);
   PUT32(relative_id);
   PUT8(abovebelow);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_event_object_layer_set_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Event_Object_Layer_Set, 4);
   GET32(l);
   DECFOOT();
}

ENCPROTO(_pr_object_event_object_layer_set_enc) {
   ENCHEAD(Evoak_PR_Object_Event_Object_Layer_Set, 4);
   PUT32(l);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_file_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_File_Set, 1 + 1);
   GETS(file);
   GETS(group);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_file_set_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_File_Set,
	   SLEN(l1, file) +
	   SLEN(l2, group));
   PUTS(file, l1);
   PUTS(group, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_swallow_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Swallow, 4 + 1);
   GET32(swallow_id);
   GETS(part);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_swallow_enc) {
   int l1;
   ENCHEAD(Evoak_PR_Object_Edje_Swallow,
	   4 +
	   SLEN(l1, part));
   GET32(swallow_id);
   PUTS(part, l1);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_unswallow_dec) {
   DECHEAD_EXACT(Evoak_PR_Object_Edje_Unswallow, 4);
   GET32(swallow_id);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_unswallow_enc) {
   ENCHEAD(Evoak_PR_Object_Edje_Unswallow, 4);
   PUT32(swallow_id);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_text_set_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Text_Set, 1 + 1);
   GETS(part);
   GETS(text);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_text_set_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Text_Set,
	   SLEN(l1, part) +
	   SLEN(l2, text));
   PUTS(part, l1);
   PUTS(text, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_text_changed_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Text_Changed, 1 + 1);
   GETS(part);
   GETS(text);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_text_changed_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Text_Changed,
	   SLEN(l1, part) +
	   SLEN(l2, text));
   PUTS(part, l1);
   PUTS(text, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_signal_listen_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Signal_Listen, 4 + 1 + 1);
   GET32(callback_id);
   GETS(emission);
   GETS(source);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_signal_listen_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Signal_Listen,
	   4 + 
	   SLEN(l1, emission) +
	   SLEN(l2, source));
   PUT32(callback_id);
   PUTS(emission, l1);
   PUTS(source, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_signal_unlisten_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Signal_Unlisten, 4 + 1 + 1);
   GET32(callback_id);
   GETS(emission);
   GETS(source);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_signal_unlisten_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Signal_Unlisten,
	   4 + 
	   SLEN(l1, emission) +
	   SLEN(l2, source));
   PUT32(callback_id);
   PUTS(emission, l1);
   PUTS(source, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_signal_emit_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Signal_Emit, 1 + 1);
   GETS(emission);
   GETS(source);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_signal_emit_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Signal_Emit,
	   SLEN(l1, emission) +
	   SLEN(l2, source));
   PUTS(emission, l1);
   PUTS(source, l2);
   ENCFOOT();
}

/****************************************************************************/
DECPROTO(_pr_object_edje_signal_dec) {
   DECHEAD_MIN(Evoak_PR_Object_Edje_Signal, 4 + 1 + 1);
   GET32(callback_id);
   GETS(emission);
   GETS(source);
   DECFOOT();
}

ENCPROTO(_pr_object_edje_signal_enc) {
   int l1, l2;
   ENCHEAD(Evoak_PR_Object_Edje_Signal,
	   4 +
	   SLEN(l1, emission) +
	   SLEN(l2, source));
   PUT32(callback_id);
   PUTS(emission, l1);
   PUTS(source, l2);
   ENCFOOT();
}

/****************************************************************************/
Evoak_Protocol _evoak_proto[] =
{
     {NULL, NULL}, /* 0 */
     {_pr_connect_dec, _pr_connect_enc}, /* 1 */
     {_pr_canvas_info_dec, _pr_canvas_info_enc}, /* 2 */
     {NULL, NULL}, /* 3 */
     {NULL, NULL}, /* 4 */
     {_pr_object_add_dec, _pr_object_add_enc}, /* 5 */
     {NULL, NULL}, /* 6 */
     {_pr_object_move_dec, _pr_object_move_enc}, /* 7 */
     {_pr_object_resize_dec, _pr_object_resize_enc}, /* 8 */
     {NULL, NULL}, /* 9 */
     {NULL, NULL}, /* 10 */
     {_pr_object_clip_set_dec, _pr_object_clip_set_enc}, /* 11 */
     {_pr_object_color_set_dec, _pr_object_color_set_enc}, /* 12 */
     {_pr_object_layer_set_dec, _pr_object_layer_set_enc}, /* 13 */
     {_pr_object_stack_dec, _pr_object_stack_enc}, /* 14 */
     {_pr_object_event_prop_set_dec, _pr_object_event_prop_set_enc}, /* 15 */
     {_pr_object_event_mask_set_dec, _pr_object_event_mask_set_enc}, /* 16 */
     {_pr_object_focus_set_dec, _pr_object_focus_set_enc}, /* 17 */
     {_pr_object_image_file_set_dec, _pr_object_image_file_set_enc}, /* 18 */
     {_pr_object_image_fill_set_dec, _pr_object_image_fill_set_enc}, /* 19 */
     {NULL, NULL}, /* 20 */
     {NULL, NULL}, /* 21 */
     {_pr_object_image_border_set_dec, _pr_object_image_border_set_enc}, /* 22 */
     {_pr_object_image_smooth_scale_set_dec, _pr_object_image_smooth_scale_set_enc}, /* 23 */
     {_pr_object_image_size_set_dec, _pr_object_image_size_set_enc}, /* 24 */
     {_pr_object_image_alpha_set_dec, _pr_object_image_alpha_set_enc}, /* 25 */
     {_pr_object_event_mouse_move_dec, _pr_object_event_mouse_move_enc}, /* 26 */
     {_pr_object_event_mouse_downup_dec, _pr_object_event_mouse_downup_enc}, /* 27 */
     {_pr_object_event_mouse_inout_dec, _pr_object_event_mouse_inout_enc}, /* 28 */
     {_pr_object_event_mouse_wheel_dec, _pr_object_event_mouse_wheel_enc}, /* 29 */
     {_pr_object_event_key_downup_dec, _pr_object_event_key_downup_enc}, /* 30 */
     {_pr_object_line_set_dec, _pr_object_line_set_enc}, /* 31 */
     {_pr_object_grad_angle_set_dec, _pr_object_grad_angle_set_enc}, /* 32 */
     {_pr_object_grad_color_add_dec, _pr_object_grad_color_add_enc}, /* 33 */
     {NULL, NULL}, /* 34 */
     {_pr_object_poly_point_add_dec, _pr_object_poly_point_add_enc}, /* 35 */
     {NULL, NULL}, /* 36 */
     {_pr_object_text_source_set_dec, _pr_object_text_source_set_enc}, /* 37 */
     {_pr_object_text_font_set_dec, _pr_object_text_font_set_enc}, /* 38 */
     {_pr_object_text_text_set_dec, _pr_object_text_text_set_enc}, /* 39 */
     {_pr_object_move8_dec, _pr_object_move8_enc}, /* 40 */
     {_pr_object_move16_dec, _pr_object_move16_enc}, /* 41 */
     {_pr_object_move_rel8_dec, _pr_object_move_rel8_enc}, /* 42 */
     {_pr_object_move_rel16_dec, _pr_object_move_rel16_enc}, /* 43 */
     {_pr_object_resize8_dec, _pr_object_resize8_enc}, /* 44 */
     {_pr_object_resize16_dec, _pr_object_resize16_enc}, /* 45 */
     {_pr_object_resize_rel8_dec, _pr_object_resize_rel8_enc}, /* 46 */
     {_pr_object_resize_rel16_dec, _pr_object_resize_rel16_enc}, /* 47 */
     {_pr_object_image_fill8_set_dec, _pr_object_image_fill8_set_enc}, /* 48 */
     {_pr_object_image_fill16_set_dec, _pr_object_image_fill16_set_enc}, /* 49 */
     {_pr_object_image_fill_size8_set_dec, _pr_object_image_fill_size8_set_enc}, /* 50 */
     {_pr_object_image_fill_size16_set_dec, _pr_object_image_fill_size16_set_enc}, /* 51 */
     {NULL, NULL}, /* 52 */
     {_pr_object_event_object_move_dec, _pr_object_event_object_move_enc}, /* 53 */
     {_pr_object_event_object_resize_dec, _pr_object_event_object_resize_enc}, /* 54 */
     {_pr_object_event_object_restack_dec, _pr_object_event_object_restack_enc}, /* 55 */
     {_pr_object_event_object_layer_set_dec, _pr_object_event_object_layer_set_enc}, /* 56 */
     {NULL, NULL}, /* 57 */
     {NULL, NULL}, /* 58 */
     {_pr_object_edje_file_set_dec, _pr_object_edje_file_set_enc}, /* 59 */
     {_pr_object_edje_swallow_dec, _pr_object_edje_swallow_enc}, /* 60 */
     {_pr_object_edje_unswallow_dec, _pr_object_edje_unswallow_enc}, /* 61 */
     {_pr_object_edje_text_set_dec, _pr_object_edje_text_set_enc}, /* 62 */
     {_pr_object_edje_text_changed_dec, _pr_object_edje_text_changed_enc}, /* 63 */
     {_pr_object_edje_signal_listen_dec, _pr_object_edje_signal_listen_enc}, /* 64 */
     {_pr_object_edje_signal_unlisten_dec, _pr_object_edje_signal_unlisten_enc}, /* 65 */
     {_pr_object_edje_signal_emit_dec, _pr_object_edje_signal_emit_enc}, /* 66 */
     {_pr_object_edje_signal_dec, _pr_object_edje_signal_enc}, /* 67 */
};
	       


#define ARGB_JOIN(a, r, g, b) \
 (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))

#define RGB_JOIN(r, g, b) \
 (((r) << 16) + ((g) << 8) + (b))

#define MUL4_256(a, r, g, b, c) \
 ( (((((c) >> 8) & 0xff0000) * (a)) & 0xff000000) + \
   (((((c) & 0xff0000) * (r)) >> 8) & 0xff0000) + \
   (((((c) & 0xff00) * (g)) >> 8) & 0xff00) + \
   ((((c) & 0xff) * (b)) >> 8) )

#define MUL3_256(r, g, b, c) \
 ( (((((c) & 0xff0000) * (r)) >> 8) & 0xff0000) + \
   (((((c) & 0xff00) * (g)) >> 8) & 0xff00) + \
   ((((c) & 0xff) * (b)) >> 8) )

#define MUL_256(a, c) \
 ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) + \
   (((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) )

#define MUL4_SYM(x, y) \
 ( ((((((x) >> 8) & 0xff0000) * (((y) >> 24) & 0xff)) + 0xff0000) & 0xff000000) + \
   ((((((x) & 0xff0000) * (((y) >> 16) & 0xff)) + 0xff0000) >> 8) & 0xff0000) + \
   ((((((x) & 0xff00) * (((y) >> 8) & 0xff)) + 0xff00) >> 8) & 0xff00) + \
   (((((x) & 0xff) * ((y) & 0xff)) + 0xff) >> 8) )

#define MUL3_SYM(x, y) \
 ( ((((((x) & 0xff0000) * (((y) >> 16) & 0xff)) + 0xff0000) >> 8) & 0xff0000) + \
   ((((((x) & 0xff00) * (((y) >> 8) & 0xff)) + 0xff00) >> 8) & 0xff00) + \
   (((((x) & 0xff) * ((y) & 0xff)) + 0xff) >> 8) )

#define MUL_SYM(a, x) \
 ( (((((x) >> 8) & 0x00ff00ff) * (a) + 0xff00ff) & 0xff00ff00) + \
   (((((x) & 0x00ff00ff) * (a) + 0xff00ff) >> 8) & 0x00ff00ff) )

#define INTERP_256(a, c0, c1) \
 ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a)) \
   + ((c1) & 0xff00ff00)) & 0xff00ff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )


# include "./op_copy/op_copy_pixel_.c"
# include "./op_copy/op_copy_color_.c"
# include "./op_copy/op_copy_pixel_color_.c"
# include "./op_copy/op_copy_pixel_mask_.c"
# include "./op_copy/op_copy_mask_color_.c"
# include "./op_copy/op_copy_pixel_mask_color_.c"

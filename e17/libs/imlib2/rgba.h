#ifndef __RGBA
#define __RGBA 1

void    __imlib_RGBA_init(void);
void    __imlib_RGBA_to_RGB565_fast(DATA32 *src , int src_jump,
			    DATA16 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB565_dither(DATA32 *src , int src_jump,
			      DATA16 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB555_fast(DATA32 *src , int src_jump,
			    DATA16 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB555_dither(DATA32 *src , int src_jump,
			      DATA16 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB332_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB332_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB232_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB232_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB222_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB222_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB221_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB221_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB121_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB121_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB111_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB111_dither(DATA32 *src , int src_jump,
			      DATA8 *dest, int dest_jump,
			      int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB1_fast(DATA32 *src , int src_jump,
			  DATA8 *dest, int dest_jump,
			  int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB1_dither(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_A1_fast(DATA32 *src , int src_jump,
			DATA8 *dest, int dest_jump,
			int width, int height, int dx, int dy);
void    __imlib_RGBA_to_A1_dither(DATA32 *src , int src_jump,
			  DATA8 *dest, int dest_jump,
			  int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB8888_fast(DATA32 *src , int src_jump,
			    DATA32 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
void    __imlib_RGBA_to_RGB888_fast(DATA32 *src , int src_jump,
			    DATA8 *dest, int dest_jump,
			    int width, int height, int dx, int dy);
#endif


#define MOV_A2R(a, mma) \
	movd_m2r(a, mma); \
	punpcklwd_r2r(mma, mma); \
	punpckldq_r2r(mma, mma);

#define MOV_P2R(c, mmc, mmz) \
	movd_m2r(c, mmc); \
	punpcklbw_r2r(mmz, mmc);

#define MOV_R2P(mmc, c, mmz) \
	packuswb_r2r(mmz, mmc); \
	movd_r2m(mmc, c);

#define MUL4_256_R2R(mmx, mmy) \
	pmullw_r2r(mmx, mmy); \
	psrlw_i2r(8, mmy);

#define MUL4_SYM_R2R(mmx, mmy, mm255) \
	pmullw_r2r(mmx, mmy); \
	paddw_r2r(mm255, mmy); \
	psrlw_i2r(8, mmy);

#define MOV_RA2R(mmx, mma) \
	movq_r2r(mmx, mma); \
	punpckhwd_r2r(mma, mma); \
	punpckhdq_r2r(mma, mma);

#define INTERP_256_R2R(mma, mmx, mmy) \
	psubusw_r2r(mmy, mmx); \
	MUL4_256_R2R(mma, mmx) \
	paddw_r2r(mmx, mmy);


volatile const DATA32 PIX_FULL = 0xffffffff;
volatile const DATA32 PIX_HALF = 0x80808080;

# include "./op_copy/op_copy_pixel_i386.c"
# include "./op_copy/op_copy_color_i386.c"
# include "./op_copy/op_copy_pixel_color_i386.c"
# include "./op_copy/op_copy_pixel_mask_i386.c"
# include "./op_copy/op_copy_mask_color_i386.c"
# include "./op_copy/op_copy_pixel_mask_color_i386.c"


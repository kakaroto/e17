#include "epplet.h"
#include "esdcloak.h"

#define VARIANCE 40
#define VARTREND 16
#define HSPREAD  10
#define VSPREAD  160
#define RESIDUAL 75
#define MAX      255
double              prev_val = 0.0;
int                *flame = NULL;
int                *vspread, *hspread, *residual;
unsigned char       rm[255], gm[255], bm[255];
int                 rr, gg, bb;
extern RGB_buf      buf;
int                 load_val = 35;
int                 load_r = 0;
int                 load_l = 0;

static void         aa_line(int x1, int y1, int x2, int y2, unsigned char b,
			    unsigned char rr, unsigned char gg,
			    unsigned char bb);
static void         set_col_pixel(int x, int y, unsigned char c,
				  unsigned char rrr, unsigned char ggg,
				  unsigned char bbb);
/* static void set_pixel (int x, int y, unsigned char c); */
static void         aa_pixel(double wx, double wy, unsigned char c,
			     unsigned char rrr, unsigned char ggg,
			     unsigned char bbb);
static void         color_buf(unsigned char rr, unsigned char gg,
			      unsigned char bb);
static void         fade_buf(int percent);
static void         scroll_buf(void);

/* Raster's flame colors */
/* Look, even US spelling of colours ;-) */
int                 colors[] = {
   120, 0, 169,
   217, 152, 249,
   255, 255, 255,

   30, 90, 90,
   50, 255, 255,
   255, 255, 255,

   255, 0, 0,
   255, 255, 0,
   255, 255, 255,

   0, 255, 0,
   255, 255, 0,
   255, 255, 255,

   0, 0, 255,
   255, 0, 255,
   255, 255, 255,

   0, 0, 200,
   40, 80, 255,
   100, 200, 255,

   80, 90, 140,
   140, 150, 180,
   255, 230, 200,

   20, 40, 180,
   255, 160, 0,
   255, 255, 100
};

void
set_flame_col(int j)
{
   flame_col(colors[j], colors[j + 1], colors[j + 2], colors[j + 3],
	     colors[j + 4], colors[j + 5], colors[j + 6], colors[j + 7],
	     colors[j + 8]);
   rr = colors[j];
   gg = colors[j + 1];
   bb = colors[j + 2];
   if (rr == 0)
      rr = 1;
   if (gg == 0)
      gg = 1;
   if (bb == 0)
      bb = 1;
}

void
flame_col(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3,
	  int b3)
{
   int                 i;

   for (i = 0; i < 25; i++)
     {
	rm[i] = (r1 * i) / 25;
	gm[i] = (g1 * i) / 25;
	bm[i] = (b1 * i) / 25;
     }
   for (i = 0; i < 25; i++)
     {
	rm[25 + i] = ((r2 * i) + (r1 * (25 - i))) / 25;
	gm[25 + i] = ((g2 * i) + (g1 * (25 - i))) / 25;
	bm[25 + i] = ((b2 * i) + (b1 * (25 - i))) / 25;
     }
   for (i = 0; i < 25; i++)
     {
	rm[50 + i] = ((r3 * i) + (r2 * (25 - i))) / 25;
	gm[50 + i] = ((g3 * i) + (g2 * (25 - i))) / 25;
	bm[50 + i] = ((b3 * i) + (b2 * (25 - i))) / 25;
     }
   for (i = 75; i < 255; i++)
     {
	rm[i] = rm[74];
	gm[i] = gm[74];
	bm[i] = bm[74];
     }
}

/* I use this to black-out the rgb-buf */
/* There must be a faster way, but I'm not used to this stuff yet */
void
blank_buf(void)
{
   int                 x, y;
   unsigned char      *rgb, *rptr;

   for (y = 0; y < 40; y++)
     {
	rgb = Epplet_get_rgb_pointer(buf);
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 40; x++)
	  {
	     rptr[0] = rptr[1] = rptr[2] = 0;
	     rptr += 4;
	  }
     }
}

/* Same as blank_buf, but not black ;-) */
void
color_buf(unsigned char rr, unsigned char gg, unsigned char bb)
{
   int                 x, y;
   unsigned char      *rgb, *rptr;

   for (y = 0; y < 40; y++)
     {
	rgb = Epplet_get_rgb_pointer(buf);
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 40; x++)
	  {
	     rptr[0] = rr;
	     rptr[1] = gg;
	     rptr[2] = bb;
	     rptr += 4;
	  }
     }
}

/* Used to fade out the buf a bit before each draw */
/* Helps the radar look right */
void
fade_buf(int percentage)
{
   int                 x, y;
   unsigned char      *rgb, *rptr;

   for (y = 0; y < 40; y++)
     {
	rgb = Epplet_get_rgb_pointer(buf);
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 40; x++)
	  {
	     rptr[0] = rptr[0] * percentage / 100;
	     rptr[1] = rptr[1] * percentage / 100;
	     rptr[2] = rptr[2] * percentage / 100;
	     rptr += 4;
	  }
     }
}

/* Scrolls buf along one to the left */
static void
scroll_buf(void)
{
   int                 x, y;
   unsigned char      *rgb, *rptr;

   for (y = 0; y < 40; y++)
     {
	rgb = Epplet_get_rgb_pointer(buf);
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 39; x++)
	  {
	     rptr[0] = rptr[4];
	     rptr[1] = rptr[5];
	     rptr[2] = rptr[6];
	     rptr += 4;
	  }
	rptr[0] = 0;
	rptr[1] = 0;
	rptr[2] = 0;
     }
}

/* RasterFire :) */
void
draw_flame(void)
{
   unsigned char      *rgb, *rptr;
   int                 x, y, *ptr, val1, val2, val3, j;

   if (!flame)
     {
	vspread = malloc(40 * sizeof(int));
	hspread = malloc(40 * sizeof(int));
	residual = malloc(40 * sizeof(int));
	flame = malloc(sizeof(int) * 40 * 40);
	memset(flame, 0, sizeof(int) * 40 * 40);
     }

   ptr = flame + (39 * 40);
   for (x = 0; x < 40; x++)
     {
	vspread[x] = VSPREAD + (load_val / 50);
	hspread[x] = HSPREAD + (load_val / 50);
	residual[x] = RESIDUAL + (load_val / 50);
	ptr[x] = (rand() % (load_val + 155));
	if (ptr[x] > MAX)
	   ptr[x] = 0;
	else if (ptr[x] < 0)
	   ptr[x] = 0;
     }

   for (x = (40 / (4)) - 1; x <= (40 / (4)) + 1; x++)
     {
	j = (load_l * 40) / 100;
	ptr = flame + ((40 - j) * (40)) + (x);
	for (y = 0; y < j; y++)
	  {
	     ptr[0] += ((y * 64 * load_l) / (j * 100));
	     ptr += 40;
	  }
     }

   for (x = (40 / (4)) - 1 + (1 * (40 / 2));
	x <= (40 / (4)) + 1 + (1 * (40 / 2)); x++)
     {
	j = (load_r * 40) / 100;
	ptr = flame + ((40 - j) * (40)) + (x);
	for (y = 0; y < j; y++)
	  {
	     ptr[0] += ((y * 64 * load_r) / (j * 100));
	     ptr += 40;
	  }
     }

   for (y = 39; y >= 2; y--)
     {
	ptr = flame + (y * 40);
	for (x = 1; x < 39; x++)
	  {
	     val1 = (ptr[x] * vspread[x]) >> 8;
	     val2 = (ptr[x] * hspread[x]) >> 8;
	     val3 = (ptr[x] * residual[x]) >> 8;

	     ptr[x - 1] += val2;
	     if (ptr[x - 1] > MAX)
		ptr[x - 1] = MAX;
	     ptr[x - 40] += val1;
	     if (ptr[x - 40] > MAX)
		ptr[x - 40] = MAX;
	     ptr[x + 1] += val2;
	     if (ptr[x + 1] > MAX)
		ptr[x + 1] = MAX;
	     ptr[x] = val3;
	  }
     }
   for (x = 0; x < 40; x++)
      flame[x] = 0;
   for (x = 0; x < 40; x++)
      flame[40 + x] /= 2;
   for (y = 0; y < 40; y++)
      flame[y * 40] = 0;
   for (y = 0; y < 40; y++)
      flame[(y * 40) + 39] = 0;
   for (y = 0; y < 40; y++)
      flame[(y * 40) + 38] /= 2;
   rgb = Epplet_get_rgb_pointer(buf);
   for (y = 0; y < 40; y++)
     {
	ptr = flame + (y * 40) + 1;
	rptr = rgb + (y * 40 * 4);
	for (x = 0; x < 40; x++)
	  {
	     val1 = ptr[x] & 0xff;
	     rptr[0] = rm[val1];
	     rptr[1] = gm[val1];
	     rptr[2] = bm[val1];
	     rptr += 4;
	  }
     }
}

/* Scanner type thing */
void
draw_scanner(void)
{
   static int          y = 0, y2 = 0;
   static int          setup = 0;

   if (!setup)
     {
	blank_buf();
	setup = 1;
     }

   y = 37 - (37 * load_l / 100) + 1;
   y2 = 37 - (37 * load_r / 100) + 1;

   fade_buf(82);
   aa_line(0, y, 38, y2, 255, 255, 0, 0);
}

/* Lame, but easy :) */
void
draw_aa_triangle(void)
{
   int                 x1, y1;
   int                 x2, y2;
   int                 x3, y3;
   static double       x = 0.0;
   int                 radius = 0;

   radius = 55 * load_val / 100;

   x += 0.05;

   x1 = 19 + radius * cos(x) / 3;
   y1 = 19 + radius * sin(x) / 3;
   x2 = 19 + radius * cos(x + (2 * M_PI / 3)) / 3;
   y2 = 19 + radius * sin(x + (2 * M_PI / 3)) / 3;
   x3 = 19 + radius * cos(x + (4 * M_PI / 3)) / 3;
   y3 = 19 + radius * sin(x + (4 * M_PI / 3)) / 3;

   fade_buf(80);
   aa_line(x1, y1, x2, y2, 255, 0, 0, 255);
   aa_line(x2, y2, x3, y3, 255, 0, 0, 255);
   aa_line(x3, y3, x1, y1, 255, 0, 0, 255);
}

/* Two triangles. */
void
draw_aa_star(void)
{
   int                 x1, y1;
   int                 x2, y2;
   int                 x3, y3;
   int                 x4, y4;
   int                 x5, y5;
   int                 x6, y6;
   static double       x = 0.0;
   int                 radius = 0;

   radius = 55 * load_val / 100;

   x += 0.05;

   x1 = 19 + radius * cos(x) / 3;
   y1 = 19 + radius * sin(x) / 3;
   x2 = 19 + radius * cos(x + (2 * M_PI / 3)) / 3;
   y2 = 19 + radius * sin(x + (2 * M_PI / 3)) / 3;
   x3 = 19 + radius * cos(x + (4 * M_PI / 3)) / 3;
   y3 = 19 + radius * sin(x + (4 * M_PI / 3)) / 3;

   x4 = 19 + radius * cos(x + (M_PI / 3)) / 3;
   y4 = 19 + radius * sin(x + (M_PI / 3)) / 3;
   x5 = 19 + radius * cos(x + (3 * M_PI / 3)) / 3;
   y5 = 19 + radius * sin(x + (3 * M_PI / 3)) / 3;
   x6 = 19 + radius * cos(x + (5 * M_PI / 3)) / 3;
   y6 = 19 + radius * sin(x + (5 * M_PI / 3)) / 3;

   fade_buf(80);

   aa_line(x1, y1, x2, y2, 255, 255, 255, 0);
   aa_line(x2, y2, x3, y3, 255, 255, 255, 0);
   aa_line(x3, y3, x1, y1, 255, 255, 255, 0);

   aa_line(x4, y4, x5, y5, 255, 255, 255, 0);
   aa_line(x5, y5, x6, y6, 255, 255, 255, 0);
   aa_line(x6, y6, x4, y4, 255, 255, 255, 0);
}

void
draw_colorwarp(void)
{
   static int          setup = 0;
   static int          rr = 0;
   static int          gg = 0;
   static int          bb = 0;
   static int          d_rr = 2;
   static int          d_gg = -3;
   static int          d_bb = 1;

   if (!setup)
     {
	/* setup */
	rr = rand() % 255;
	gg = rand() % 255;
	bb = rand() % 255;
	setup = 1;
     }

   rr += d_rr;
   gg += d_gg;
   bb += d_bb;

   if (rr <= 0)
     {
	d_rr = -d_rr;
	rr = 0;
     }
   if (rr >= 255)
     {
	d_rr = -d_rr;
	rr = 255;
     }
   if (gg <= 0)
     {
	d_gg = -d_gg;
	gg = 0;
     }
   if (gg >= 255)
     {
	d_gg = -d_gg;
	gg = 255;
     }
   if (bb <= 0)
     {
	d_bb = -d_bb;
	bb = 0;
     }
   if (bb >= 255)
     {
	d_bb = -d_bb;
	bb = 255;
     }

   color_buf(rr, gg, bb);
}

/* Bouncing ball */
void
draw_ball(void)
{
   static int          setup = 0;
   static double       ball_x, ball_y;
   static double       d_x, d_y;

   if (!setup)
     {
	ball_x = rand() % 39;
	ball_y = rand() % 39;
	while (abs(d_x) < 0.5)
	   d_x = ((double)rand() / RAND_MAX * 4) - 2.0;
	while (abs(d_y) < 0.5)
	   d_y = ((double)rand() / RAND_MAX * 4) - 2.0;
	blank_buf();
	setup = 1;
     }

   ball_x += d_x;
   ball_y += d_y;

   if (ball_x < 1)
     {
	ball_x = 1;
	d_x = -d_x;
     }
   else if (ball_x > 37)
     {
	ball_x = 37;
	d_x = -d_x;
     }

   if (ball_y < 1)
     {
	ball_y = 1;
	d_y = -d_y;
     }
   else if (ball_y > 37)
     {
	ball_y = 37;
	d_y = -d_y;
     }

   fade_buf(92);

   set_col_pixel(ball_x, ball_y, 255, 255, 0, 0);
   set_col_pixel(ball_x + 1, ball_y, 255, 255, 0, 0);
   set_col_pixel(ball_x, ball_y + 1, 255, 255, 0, 0);
   set_col_pixel(ball_x + 1, ball_y + 1, 155, 255, 0, 0);

   set_col_pixel(ball_x - 1, ball_y, 255, 255, 0, 0);
   set_col_pixel(ball_x, ball_y - 1, 255, 255, 0, 0);
   set_col_pixel(ball_x - 1, ball_y - 1, 155, 255, 0, 0);
   set_col_pixel(ball_x - 1, ball_y + 1, 155, 255, 0, 0);
   set_col_pixel(ball_x + 1, ball_y - 1, 155, 255, 0, 0);
}

#define NUM_ATOMS 8
static double       atom_x[NUM_ATOMS];
static double       atom_y[NUM_ATOMS];
static double       atom_d_x[NUM_ATOMS];
static double       atom_d_y[NUM_ATOMS];

/* Bouncing atoms */
void
draw_atoms(void)
{
   static int          setup = 0;
   int                 i;

   if (!setup)
     {
	for (i = 0; i < NUM_ATOMS; i++)
	  {
	     atom_x[i] = rand() % 39;
	     atom_y[i] = rand() % 39;
	     while (fabs(atom_d_x[i]) < 0.3)
		atom_d_x[i] = ((double)rand() / RAND_MAX * 3) - 1.5;
	     while (fabs(atom_d_y[i]) < 0.3)
		atom_d_y[i] = ((double)rand() / RAND_MAX * 3) - 1.5;
	     blank_buf();
	     setup = 1;
	  }
     }
   else
      fade_buf(87);

   for (i = 0; i < NUM_ATOMS; i++)
     {
	atom_x[i] += atom_d_x[i];
	atom_y[i] += atom_d_y[i];

	if (atom_x[i] < 0)
	  {
	     atom_x[i] = 0;
	     atom_d_x[i] = -atom_d_x[i];
	  }
	else if (atom_x[i] > 38)
	  {
	     atom_x[i] = 38;
	     atom_d_x[i] = -atom_d_x[i];
	  }

	if (atom_y[i] < 0)
	  {
	     atom_y[i] = 0;
	     atom_d_y[i] = -atom_d_y[i];
	  }
	else if (atom_y[i] > 38)
	  {
	     atom_y[i] = 38;
	     atom_d_y[i] = -atom_d_y[i];
	  }
	aa_pixel(atom_x[i], atom_y[i], 255, 255, 255, 0);
     }
}

void
draw_history(void)
{
   static int          setup = 0;

   if (!setup)
     {
	blank_buf();
	setup = 1;
     }
   else
      scroll_buf();

   set_col_pixel(39, 39 - ((load_val * 36) / 100), 255, 115, 255, 165);
}

void
draw_history_bar(void)
{
   static int          setup = 0;

   if (!setup)
     {
	blank_buf();
	setup = 1;
     }
   else
      scroll_buf();

   aa_line(39, 39 - ((load_val * 36) / 100), 39, 39, 255, 80, 255, 125);
}

/* ************************* */

/* ************************* */
/* Utility drawing functions */
/* ************************* */

/* Maybe I'll optimise this later ;-) */
static void
aa_line(int x1, int y1, int x2, int y2, unsigned char b,
	unsigned char rr, unsigned char gg, unsigned char bb)
{
   double              grad, line_width, line_height, xgap, ygap, xend, yend,
      yf, xf, brightness1, brightness2, db, xm, ym;
   int                 ix1, ix2, iy1, iy2, i;
   int                 temp;

   unsigned char       c1, c2;

   line_width = (x2 - x1);
   line_height = (y2 - y1);

   if (abs(line_width) > abs(line_height))
     {
	if (x1 > x2)
	  {
	     temp = x1;
	     x1 = x2;
	     x2 = temp;
	     temp = y1;
	     y1 = y2;
	     y2 = temp;
	     line_width = (x2 - x1);
	     line_height = (y2 - y1);
	  }

	/* This is currently broken. It is supposed to account
	 * for lines that don't span more than one pixel */
	if (abs(line_width) < 0.1)
	  {
	     x2 = x1 + 0.5;
	     x1 -= 0.5;
	     grad = 0;
	  }
	else
	  {
	     grad = line_height / line_width;
	     if (line_width < 1)
	       {
		  xm = (x1 + x2) / 2;
		  ym = (y1 + y2) / 2;

		  x1 = xm - 0.5;
		  x2 = xm + 0.5;
		  y1 = ym - (grad / 2);
		  y2 = ym + (grad / 2);

		  line_width = 1;
		  line_height = grad;
	       }
	  }

	xend = (int)x1 + 0.5;
	yend = y1 + grad * (xend - x1);

	xgap = (1 - modf(x1 + 0.5, &db));
	ix1 = (int)xend;
	iy1 = (int)yend;

	brightness1 = (1 - modf(yend, &db)) * xgap;
	brightness2 = modf(yend, &db) * xgap;

	c1 = (unsigned char)(brightness1 * b);
	c2 = (unsigned char)(brightness2 * b);

	set_col_pixel(ix1, iy1, c1, rr, gg, bb);
	set_col_pixel(ix1, iy1 + 1, c2, rr, gg, bb);

	yf = yend + grad;

	xend = (int)(x2 + .5);
	yend = y2 + grad * (xend - x2);

	xgap = 1 - modf(x2 - .5, &db);

	ix2 = (int)xend;
	iy2 = (int)yend;

	brightness1 = (1 - modf(yend, &db)) * xgap;
	brightness2 = modf(yend, &db) * xgap;

	c1 = (unsigned char)(brightness1 * b);
	c2 = (unsigned char)(brightness2 * b);

	set_col_pixel(ix2, iy2, c1, rr, gg, bb);
	set_col_pixel(ix2, iy2 + 1, c2, rr, gg, bb);

	for (i = ix1 + 1; i < ix2; i++)
	  {
	     brightness1 = (1 - modf(yf, &db));
	     brightness2 = modf(yf, &db);

	     c1 = (unsigned char)(brightness1 * b);
	     c2 = (unsigned char)(brightness2 * b);

	     set_col_pixel(i, (int)yf, c1, rr, gg, bb);
	     set_col_pixel(i, (int)yf + 1, c2, rr, gg, bb);

	     yf = yf + grad;
	  }
     }
   else
     {
	if (y2 < y1)
	  {
	     temp = x1;
	     x1 = x2;
	     x2 = temp;
	     temp = y1;
	     y1 = y2;
	     y2 = temp;
	     line_width = (x2 - x1);
	     line_height = (y2 - y1);
	  }

	/* This is currently broken */
	if (abs(line_height) < 0.1)
	  {
	     y2 = y1 + 0.5;
	     y1 -= 0.5;
	     grad = 0;
	  }
	else
	  {
	     grad = line_width / line_height;
	     if (line_height < 1)
	       {
		  xm = (x1 + x2) / 2;
		  ym = (y1 + y2) / 2;

		  x1 = xm - (grad / 2);
		  x2 = xm + (grad / 2);
		  y1 = ym - 0.5;
		  y2 = ym + 0.5;

		  line_height = 1;
		  line_width = grad;
	       }
	  }

	yend = (int)(y1 + 0.5);
	xend = x1 + grad * (yend - y1);

	ygap = (1 - modf(y1 + 0.5, &db));
	ix1 = (int)xend;
	iy1 = (int)yend;

	brightness1 = (1 - modf(xend, &db)) * ygap;
	brightness2 = modf(xend, &db) * ygap;

	c1 = (unsigned char)(brightness1 * b);
	c2 = (unsigned char)(brightness2 * b);

	set_col_pixel(ix1, iy1, c1, rr, gg, bb);
	set_col_pixel(ix1 + 1, iy1, c2, rr, gg, bb);

	xf = xend + grad;

	yend = (int)(y2 + .5);
	xend = x2 + grad * (yend - y2);

	ygap = 1 - modf(y2 - .5, &db);

	ix2 = (int)xend;
	iy2 = (int)yend;

	brightness1 = (1 - modf(xend, &db)) * ygap;
	brightness2 = modf(xend, &db) * ygap;

	c1 = (unsigned char)(brightness1 * b);
	c2 = (unsigned char)(brightness2 * b);

	set_col_pixel(ix2, iy2, c1, rr, gg, bb);
	set_col_pixel(ix2 + 1, iy2, c2, rr, gg, bb);

	for (i = iy1 + 1; i < iy2; i++)
	  {
	     brightness1 = (1 - modf(xf, &db));
	     brightness2 = modf(xf, &db);

	     c1 = (unsigned char)(brightness1 * b);
	     c2 = (unsigned char)(brightness2 * b);

	     set_col_pixel((int)xf, i, c1, rr, gg, bb);
	     set_col_pixel((int)(xf + 1), i, c2, rr, gg, bb);

	     xf += grad;
	  }
     }
}

/* Draw an anti-aliased pixel */
static void
aa_pixel(double wx, double wy, unsigned char c, unsigned char rrr,
	 unsigned char ggg, unsigned char bbb)
{
   int                 x, y;
   double              fx, fy;
   int                 btl, btr, bbl, bbr;

   x = (int)wx;
   y = (int)wy;
   fx = (double)wx - x;
   fy = (double)wy - y;

   btl = ((1.0 - fx) * (1.0 - fy)) * c;
   btr = ((fx) * (1.0 - fy)) * c;
   bbl = ((1.0 - fx) * (fy)) * c;
   bbr = (fx * fy) * c;

   set_col_pixel(x, y, btl, rrr, ggg, bbb);
   set_col_pixel(x + 1, y, btr, rrr, ggg, bbb);
   set_col_pixel(x, y + 1, bbl, rrr, ggg, bbb);
   set_col_pixel(x + 1, y + 1, bbr, rrr, ggg, bbb);
}

/* Set a pixel, takes a brightness and a colour value */
static void
set_col_pixel(int x, int y, unsigned char c, unsigned char rrr,
	      unsigned char ggg, unsigned char bbb)
{
   char               *ptr;

   if ((((int)c) == 0) || (x < 0) || (y < 0) || (x > 39) || (y > 39))
      return;

   ptr = Epplet_get_rgb_pointer(buf);
   ptr += (imlib_image_get_width() * 4 * (y)) + (4 * x);
   ptr[0] = ((double)rrr / 255 * (double)c);
   ptr[1] = ((double)ggg / 255 * (double)c);
   ptr[2] = ((double)bbb / 255 * (double)c);
}

/* Set a pixel, grayscale, takes a brightness value */
/*
static void
set_pixel (int x, int y, unsigned char c)
{
  char *ptr;

  if ((((int) c) == 0) || (x < 0) || (y < 0) || (x > 39) || (y > 39))
    return;

  ptr = (buf)->im->rgb_data + ((buf)->im->rgb_width * 3 * (y)) + (3 * x);
  ptr[0] = ptr[1] = ptr[2] = c;
}
*/

/* Do *not* flame me for doing it this way... */
/* I know !!! */
/* ;-) */
void
draw_text(void)
{
/* 19 x 132 */
   char                enlighten[19][172] = {
      "....+@@@@@@@@@@.............+@+..@#.............#@..................................................................................",
      "....@$$$$$$$$$$.............@$@..$%.............%$..........&@................................................................&@....",
      "....@$*@@@@@@@@.............@$@..@#.............%$.........@$@...............................................................@$@....",
      "....@$%.....................@$@.................%$.........@$@...............................................................@$@....",
      "....@$%..........+@++=%=#...@$@..@#....&%%#.@#..%$.#%%&...#=$=@....@%%=+....@#.&%%@....@#.&%=+.+=%=+.....#=%%#....#@.#%%&+..#=$=@...",
      "....@$%..........@$=$$$$$@..@$@..$%...*$$$$%$%..%$%$$$$-+.%$$$$..+-$$$$$@...$%-$$$$%...$%-$$$-#-$$$-+...%$$$$$%...%$=$$$$-+.%$$$$...",
      "....@$*@@@@@@@+..@$$&..&$*..@$@..$%..=$%+.&$$%..%$-#.+%$&.#=$=@..-$&..#*$#..$$%+.#-$+..$$%+.%$$%+.%$=..=$%+.+=$=..%$-#.+%$&.#=$=@...",
      "....@$$$$$$$$$@..@$%....*$..@$@..$%..$*....&$%..%$@...+$%..@$@..@$&....+$%..$$....&$@..$$....$-....$%..$*.....*$..%$@...+$%..@$@....",
      "....@$*@@@@@@@+..@$@....%$..@$@..$%.@$&....+$%..%$.....$%..@$@..=$@@@@@@*-..$%....@$@..$%....$%....$%.#$=@@@@@=$#.%$.....$%..@$@....",
      "....@$%..........@$@....%$..@$@..$%.@$@.....$%..%$.....$%..@$@..%$$$$$$$$$..$%....@$@..$%....$%....$%.@$$$$$$$$$@.%$.....$%..@$@....",
      "....@$%..........@$@....%$..@$@..$%.@$@.....$%..%$.....$%..@$@..%$@@@@@@@@..$%....@$@..$%....$%....$%.@$=@@@@@@@+.%$.....$%..@$@....",
      "....@$%..........@$@....%$..@$@..$%.+$=....@$%..%$.....$%..@$@..=$#.....#...$%....@$@..$%....$%....$%.#$=.....++..%$.....$%..@$@....",
      "....@$%..........@$@....%$..@$@..$%..*$@..+-$%..%$.....$%..@$@..#$*....+$*..$%....@$@..$%....$%....$%..-$+....*$+.%$.....$%..@$@....",
      "....@$-%%%%%%%%#.@$@....%$..@$@..$%..+$$*%$$$%..%$.....$%..@$%@..%$-&@=-$#..$%....@$@..$%....$%....$%..@$$=@&*$=..%$.....$%..@$%@...",
      "....@$$$$$$$$$$@.@$@....%$..@$@..$%...+%$$*+$%..%$.....$%...-$$#..=$$$$-@...$%....@$@..$%....$%....$%...@-$$$$=...%$.....$%...-$$#..",
      ".....................................@.....+$=...............@@+....@@#...................................#@@..................@@+..",
      ".....................................$-....%$@......................................................................................",
      ".....................................=$-%%-$%.......................................................................................",
      "......................................&*$$-=........................................................................................"
   };

   static int          setup = 0;
   static int          i = 0, j = 0, y = 0;
   unsigned char       b = 0;

   if (!setup)
     {
	blank_buf();
	setup = 1;
	y = (rand() % 14) + 4;
     }
   else
     {
	scroll_buf();
     }

   if (j > 132)
     {
	/* Do nothing this time */
	;
     }
   else
     {

	for (i = y; i < (y + 19); i++)
	  {
	     switch (enlighten[i - y][j])
	       {
	       case '.':
		  {
		     b = 0;
		     break;
		  }
	       case '@':
		  {
		     b = 85;
		     break;
		  }
	       case '$':
		  {
		     b = 255;
		     break;
		  }
	       case '-':
		  {
		     b = 224;
		     break;
		  }
	       case '=':
		  {
		     b = 141;
		     break;
		  }
	       case '#':
		  {
		     b = 56;
		     break;
		  }
	       case '+':
		  {
		     b = 28;
		     break;
		  }
	       case '*':
		  {
		     b = 98;
		     break;
		  }
	       case '%':
		  {
		     b = 70;
		     break;
		  }
	       case '&':
		  {
		     b = 113;
		     break;
		  }
	       }
	     set_col_pixel(39, i, b, 136, 221, 221);
	  }
     }

   ++j;
   if (j > 172)
     {
	j = 0;
	y = (rand() % 14) + 4;
     }
}

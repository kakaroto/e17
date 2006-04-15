/* Just blank the epplet, kind of like the blank-screen screensaver */
void                blank_buf(void);

/* Draw a flame on the epplet, a la Raster's E-Cpu.epplet*/
void                draw_flame(void);
void                set_flame_col(int j);
void                flame_col(int r1, int g1, int b1, int r2, int g2, int b2,
			      int r3, int g3, int b3);
void                aa_line(int x1, int y1, int x2, int y2, unsigned char b,
			    unsigned char rr, unsigned char gg,
			    unsigned char bb);
void                set_col_pixel(int x, int y, unsigned char c,
				  unsigned char rrr, unsigned char ggg,
				  unsigned char bbb);
void                aa_pixel(double wx, double wy, unsigned char c,
			     unsigned char rrr, unsigned char ggg,
			     unsigned char bbb);
void                color_buf(unsigned char rr, unsigned char gg,
			      unsigned char bb);
void                fade_buf(int percent);
void                scroll_buf(void);

/* Radar thing by me */
void                draw_radar(void);
void                draw_aa_radar(void);

/* Triangle thingummy */
void                draw_aa_triangle(void);

/* Star thingummys */
void                draw_aa_star(void);
void                draw_starfield(void);
void                draw_aa_starfield(void);
void                draw_rotator(void);
void                draw_scanner(void);
void                draw_colorwarp(void);
void                draw_ball(void);
void                draw_atoms(void);
void                draw_text(void);
void                draw_sine(void);
void                draw_funky_rotator(void);

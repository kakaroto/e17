/* Just blank the epplet, kind of like the blank-screen screensaver */
void                blank_buf(void);

/* Draw a flame on the epplet, a la Raster's E-Cpu.epplet*/
void                draw_flame(void);
void                set_flame_col(int j);
void                flame_col(int r1, int g1, int b1, int r2, int g2, int b2,
			      int r3, int g3, int b3);
/* Triangle thingummy */
void                draw_aa_triangle(void);

/* Star thingummys */
void                draw_aa_star(void);
void                draw_scanner(void);
void                draw_colorwarp(void);
void                draw_ball(void);
void                draw_atoms(void);
void                draw_text(void);
void                draw_history(void);
void                draw_history_bar(void);

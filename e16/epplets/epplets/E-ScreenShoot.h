Epplet_gadget btn_shoot, btn_conf, btn_close, btn_help, btn_ctimer;
Epplet_gadget tog_win, btn_col, btn_stimer;
Epplet_gadget sldr_qual;
Epplet_gadget p, col_p, ctimer_p, stimer_p;
Window win;
RGB_buf buf;
Epplet_gadget da;
int cloaked = 0;
extern int load_val;
extern int colors[];
static void cb_in (void *data, Window w);
int invquality = 0;

int cloak_anims[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};
int cloak_delays[] = {
  1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};
int shot_delays[] = {
  0, 1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};

struct
{
  int quality;
  int win;
  int beep;
  int cloak_anim;
  int frame;
  int do_cloak;
  int run_script;
  double delay;
  double cloak_delay;
  double draw_interval;
  char *dir;
  char *file_prefix;
  char *file_stamp;
  char *file_type;
  char *script;
}
opt;

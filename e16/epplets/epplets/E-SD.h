Epplet_gadget btn_conf, btn_close, btn_help, btn_ctimer;
Epplet_gadget p, ctimer_p;
Window win;
RGB_buf buf;
Epplet_gadget da;
int cloaked = 0;
extern int load_val;
extern int load_r;
extern int load_l;
extern int colors[];
static void cb_in (void *data, Window w);

int cloak_anims[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};
int cloak_delays[] = {
  1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};

struct {
    int fd;
    int vol_l;
    int vol_r;
    int vol_ave;
    int standby;
} esd;


Epplet_gadget lbar, rbar, standby;


struct
{
  int cloak_anim;
  int do_cloak;
  double cloak_delay;
  double draw_interval;
  char *dir;
}
opt;

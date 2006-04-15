Epplet_gadget       btn_conf, btn_close, btn_help, btn_www, btn_ftp, btn_wget;
Epplet_gadget       lbl_url, btn_file_url, btn_urllist, btn_conf;
Epplet_gadget       p, url_p;

char               *urllist[10] =
   { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
int                 num_urls = 0;

struct
{
   char               *str;
   unsigned int        len;
   unsigned int        pos;
} dtext;

struct
{
   int                 win;
   int                 save_urls;
   int                 check_url_file;
   int                 do_new_url_command;
   int                 always_show_file_urls;
   char               *new_url_command;
   char               *url_save_file;
   char               *url_file;
   char               *www_command;
   char               *ftp_command;
   char               *get_command;
}
opt;

Window              confwin = 0;
Epplet_gadget       txt_new_url_command;
Epplet_gadget       txt_url_save_file;
Epplet_gadget       txt_url_file;
Epplet_gadget       txt_www_command;
Epplet_gadget       txt_ftp_command;
Epplet_gadget       txt_get_command;

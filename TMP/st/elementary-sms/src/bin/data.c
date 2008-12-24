#include "common.h"

// this is a really simple data store for messages. it's a directory in ~/ with
// 1 message per file in text formats

typedef struct _Callback Callback;

struct _Callback
{
   void (*func) (void *data, Data_Message *msg);
   void  *data;
   Eina_Bool delete_me : 1;
};

static const char *messagedir = NULL;
static Eina_List *messages = NULL;
static const char *contactdir = NULL;
static Eina_List *contacts = NULL;
static int add_walking = 0;
static int add_deletes = 0;
static Eina_List *add_callbacks = NULL;
static int del_walking = 0;
static int del_deletes = 0;
static Eina_List *del_callbacks = NULL;

static Callback *
_callback_new(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb;
   
   cb = calloc(1, sizeof(Callback));
   if (!cb) return NULL;
   cb->func = func;
   cb->data = (void *)data;
   return cb;
}

static Eina_List *
_callback_del(Eina_List *list, int *walking, int *deletes, void (*func) (void *data, Data_Message *msg), const void *data)
{
   Eina_List *l;
   
   for (l = list; l; l = l->next)
     {
        Callback *cb = l->data;
        if ((cb->func == func) && (cb->data == data))
          {
             if (*walking == 0)
               {
                  free(cb);
                  list = eina_list_remove_list(list, l);
               }
             else
               {
                  cb->delete_me = 1;
                  (*deletes)++;
               }
             return list;
          }
     }
   return list;
}

static Eina_List *
_callback_call(Eina_List *list, int *walking, int *deletes, Data_Message *msg)
{
   Eina_List *l, *pl;
   
   (*walking)++;
   for (l = list; l; l = l->next)
     {
        Callback *cb = l->data;
        if (!cb->delete_me)
          cb->func(cb->data, msg);
     }
   (*walking)--;
   if ((*walking == 0) && (*deletes > 0))
     {
        for (l = list; l;)
          {
             Callback *cb = l->data;
             pl = l;
             l = l->next;
             if (cb->delete_me)
               {
                  free(cb);
                  list = eina_list_remove_list(list, pl);
                  (*deletes)--;
               }
          }
     }
   return list;
}

static void
_data_message_load_key_val(Data_Message *msg, const char *key, const char *val)
{
   if (!strcasecmp(key, "from"))
     msg->from_to = eina_stringshare_add(val);
   else if (!strcasecmp(key, "to"))
     msg->from_to = eina_stringshare_add(val);
   else if (!strcasecmp(key, "date"))
     {
        struct tm tm;
        if (strptime(val, "%a, %d %b %Y %H:%M:%S %z", &tm))
          msg->timestamp = mktime(&tm);
     }
   else if (!strcasecmp(key, "id"))
     msg->id = atoll(val);
   // FIXME: handle Content-Type
}

static char *
_buf_append(char *buf, const char *str, int *len, int *alloc)
{
   int len2;
   
   len2 = strlen(str);
   if ((*len + len2) >= *alloc)
     {
        char *buf2;
        
        buf2 = realloc(buf, *alloc + 512);
        if (!buf2) return NULL;
        buf = buf2;
        *alloc += 512;
     }
   strcpy(buf + *len, str);
   *len += len2;
   return buf;
}

static Data_Message *
_data_message_load(const char *file)
{
   Data_Message *msg;
   FILE *f;
   int headers = 1;
   char buf[16384];
   char *body = NULL;
   int body_len = 0, body_alloc = 0;
   
   msg = calloc(1, sizeof(Data_Message));
   if (!msg) return NULL;
   f = fopen(file, "rb");
   if (!f)
     {
        free(msg);
        return NULL;
     }
   msg->path = eina_stringshare_add(file);
   while (fgets(buf, sizeof(buf), f))
     {
        if (headers)
          {
             char *key1, *key2, *val1, *val2;
             char *p;
             
             key1 = key2 = val1 = val2 = NULL;
             p = buf;
             while (*p)
               {
                  if (!key1)
                    {
                       if ((!((*p == '\n') || (*p == '\r'))) &&
                           (!isblank(*p))) key1 = p;
                    }
                  else if (!key2)
                    {
                       if (*p == ':') key2 = p;
                    }
                  else if (!val1)
                    {
                       if (!isblank(*p)) val1 = p;
                    }
                  else if (!val2)
                    {
                       if ((*p == '\n') || (*p == '\r')) val2 = p;
                    }
                  p++;
               }
             if ((val1) && (!val2)) val2 = p;
             if (!key1) headers = 0;
             else if ((key1) && (key2) && (val1) && (val2))
               {
                  char *key, *val;
                  
                  key = malloc(key2 - key1 + 1);
                  if (key)
                    {
                       val = malloc(val2 - val1 + 1);
                       if (val)
                         {
                            strncpy(key, key1, key2 - key1);
                            key[key2 - key1] = 0;
                            strncpy(val, val1, val2 - val1);
                            val[val2 - val1] = 0;
                            _data_message_load_key_val(msg, key, val);
                            free(val);
                         }
                       free(key);
                    }
               }
          }
        else
          {
             int pos, size;
             
             pos = 0;
             size = strlen(buf);
             while (pos < size)
               {
                  int ch, ppos;
                  
                  ppos = pos;
                  ch = evas_common_font_utf8_get_next(buf, &pos);
                  if (ch == '\n')
                    body = _buf_append(body, "<br>", &body_len, &body_alloc);
                  else
                    {
                       int stlen = 0;
                       const char *escape;
                       char str[16];
                       
                       escape = evas_textblock_string_escape_get(buf + ppos, &stlen);
                       if (escape)
                         body = _buf_append(body, escape, &body_len, &body_alloc);
                       else
                         {
                            strncpy(str, buf + ppos, pos - ppos);
                            str[pos - ppos] = 0;
                            body = _buf_append(body, str, &body_len, &body_alloc);
                         }
                    }
               }
          }
     }
   if (body)
     {
        msg->body = eina_stringshare_add(body);
        free(body);
     }
   fclose(f);
   return msg;
}

static void
_data_message_dir_load(const char *dir, Data_Message_Flags extra_flags)
{
   Ecore_List *files;
   
   files = ecore_file_ls(dir);
   if (files)
     {
        char *file;
        
        ecore_list_first_goto(files);
        while ((file = ecore_list_current(files)))
          {
             char *ext;
             
             ext = strrchr(file, '.');
             if ((ext) && (!strcasecmp(ext, ".msg")))
               {
                  Data_Message *msg;
                  char buf[PATH_MAX];
                  
                  snprintf(buf, sizeof(buf), "%s/%s", dir, file);
                  msg = _data_message_load(buf);
                  if (msg)
                    {
                       msg->flags |= extra_flags;
                       messages = evas_list_prepend(messages, msg);
                    }
               }
             ecore_list_next(files);
          }
        ecore_list_destroy(files);
     }
}

static int
_vcard_parse_line(const char *line, char **key, char **val)
{
   char *p;
   
   p = strchr(line, ':');
   if (!p) return 0;
   *key = malloc(p - line + 1);
   if (!(*key)) return 0;
   *val = malloc(strlen(line) - (p - line));
   if (!(*val))
     {
        free(*key);
        return 0;
     }
   strncpy(*key, line, p -line);
   (*key)[p - line] = 0;
   strcpy(*val, p + 1);
   return 1;
}

static Eina_List *
_data_vcard_list_parse(const char *string, char delim)
{
   Eina_List *list = NULL;
   char *p, *pp, *str;
   
   for (p = (char *)string, pp = (char *)string;;)
     {
        if ((*p == delim) || (*p == 0))
          {
             str = malloc(p - pp + 1);
             if (str)
               {
                  strncpy(str, pp, p - pp);
                  str[p - pp] = 0;
                  list = eina_list_append(list, eina_stringshare_add(str));
                  free(str);
               }
             if (*p == 0) break;
             p++;
             pp = p;
          }
        else p++;
     }
   return list;
}

static int
_data_vcard_key_handle(Data_Contact *ctc, Eina_List *key_items, const char *val)
{
   const char *key;
   
   key = key_items->data;
   if (!strcasecmp(key, "version"))
     {
//        key_items->data = NULL;
        ctc->version = eina_stringshare_add(val);
        return 1;
     }
   else if (!strcasecmp(key, "n"))
     {
        Eina_List *list, *l;
        
        list = _data_vcard_list_parse(val, ';');
        if (list)
          {
             int n;
             
             for (n = 0, l = list; l; l = l->next, n++)
               {
                  if (n == 0)
                    ctc->name.lasts = _data_vcard_list_parse(l->data, ',');
                  else if (n == 1)
                    ctc->name.firsts = _data_vcard_list_parse(l->data, ',');
                  else if (n == 2)
                    ctc->name.others = _data_vcard_list_parse(l->data, ',');
                  else if (n == 3)
                    ctc->name.titles = _data_vcard_list_parse(l->data, ',');
                  else if (n == 4)
                    ctc->name.honorifics = _data_vcard_list_parse(l->data, ',');
                  eina_stringshare_del(l->data);
               }
             eina_list_free(list);
          }
        return 1;
     }
   else if (!strcasecmp(key, "fn"))
     {
        ctc->name.display = eina_stringshare_add(val);
        return 1;
     }
   else if (!strcasecmp(key, "nickname"))
     {
        ctc->name.nicks = _data_vcard_list_parse(val, ',');
        return 1;
     }
   else if (!strcasecmp(key, "tel"))
     {
        Data_Contact_Tel *tel;
        
        tel = calloc(1, sizeof(Data_Contact_Tel));
        if (tel)
          {
             if (key_items->next->data)
               {
                  Eina_List *list, *l;
                  
                  list = _data_vcard_list_parse(key_items->next->data, '=');
                  if (list)
                    {
                       l = list->next;
                       if (l)
                         {
                            Eina_List *list2;
                            
                            list2 = _data_vcard_list_parse(l->data, ',');
                            while (list2)
                              {
                                 if (!strcasecmp(list2->data, "home"))
                                   tel->flags |= DATA_CONTACT_TEL_HOME;
                                 else if (!strcasecmp(list2->data, "msg"))
                                   tel->flags |= DATA_CONTACT_TEL_MSG;
                                 else if (!strcasecmp(list2->data, "work"))
                                   tel->flags |= DATA_CONTACT_TEL_WORK;
                                 else if (!strcasecmp(list2->data, "pref"))
                                   tel->flags |= DATA_CONTACT_TEL_PREF;
                                 else if (!strcasecmp(list2->data, "voice"))
                                   tel->flags |= DATA_CONTACT_TEL_VOICE;
                                 else if (!strcasecmp(list2->data, "fax"))
                                   tel->flags |= DATA_CONTACT_TEL_FAX;
                                 else if (!strcasecmp(list2->data, "cell"))
                                   tel->flags |= DATA_CONTACT_TEL_CELL;
                                 else if (!strcasecmp(list2->data, "video"))
                                   tel->flags |= DATA_CONTACT_TEL_VIDEO;
                                 else if (!strcasecmp(list2->data, "pager"))
                                   tel->flags |= DATA_CONTACT_TEL_PAGER;
                                 else if (!strcasecmp(list2->data, "bbs"))
                                   tel->flags |= DATA_CONTACT_TEL_BBS;
                                 else if (!strcasecmp(list2->data, "modem"))
                                   tel->flags |= DATA_CONTACT_TEL_MODEM;
                                 else if (!strcasecmp(list2->data, "car"))
                                   tel->flags |= DATA_CONTACT_TEL_CAR;
                                 else if (!strcasecmp(list2->data, "isdn"))
                                   tel->flags |= DATA_CONTACT_TEL_ISDN;
                                 else if (!strcasecmp(list2->data, "pcs"))
                                   tel->flags |= DATA_CONTACT_TEL_PCS;
                                 eina_stringshare_del(list2->data);
                                 list2 = eina_list_remove_list(list2, list2);
                              }
                         }
                       while (list)
                         {
                            eina_stringshare_del(list->data);
                            list = eina_list_remove_list(list, list);
                         }
                    }
                  tel->number = eina_stringshare_add(val);
               }
             else
               {
                  tel->flags = DATA_CONTACT_TEL_VOICE;
                  tel->number = eina_stringshare_add(val);
               }
             ctc->tel.numbers = eina_list_append(ctc->tel.numbers, tel);
          }
        return 1;
     }
   else if (!strcasecmp(key, "photo"))
     {
        int ok;
        
        ok = 0;
        if (key_items->next->data)
          {
             Eina_List *list, *l;
             
             list = _data_vcard_list_parse(key_items->next->data, '=');
             if (list)
               {
                  l = list->next;
                  if (l)
                    {
                       if (!strcasecmp(l->data, "uri"))
                         {
                            if (!strncasecmp(val, "file:", 5))
                              {
                                 ctc->photo_file = eina_stringshare_add(val + 5);
                                 ok = 1;
                              }
                         }
                    }
               }
          }
        return ok;
     }
   return 0;
}

static Data_Contact *
_data_contact_load(const char *file)
{
   Data_Contact *ctc;
   FILE *f;
   char buf[16384];
   int incard = 0;
   
   ctc = calloc(1, sizeof(Data_Contact));
   if (!ctc) return NULL;
   f = fopen(file, "rb");
   if (!f)
     {
        free(ctc);
        return NULL;
     }
   ctc->path = eina_stringshare_add(file);
   while (fgets(buf, sizeof(buf), f))
     {
        int len;
        
        len = strlen(buf);
        if (len > 0) buf[len - 1] = 0;
        if (!strcasecmp(buf, "BEGIN:VCARD")) incard = 1;
        else if (!strcasecmp(buf, "END:VCARD")) incard = 0;
        else if (incard)
          {
             char *key = NULL, *val = NULL;
             
             if (_vcard_parse_line(buf, &key, &val))
               {
                  Eina_List *items;
                  
                  items = _data_vcard_list_parse(key, ';');
                  if (items)
                    {
                       int used;
                       
                       used = _data_vcard_key_handle(ctc, items, val);
                       if (used == 0)
                         ctc->extra_lines = eina_list_append(ctc->extra_lines, eina_stringshare_add(buf));
                       else if (used == 1)
                         {
                            while (items)
                              {
                                 if (items->data)
                                   eina_stringshare_del(items->data);
                                 items = eina_list_remove_list(items, items);
                              }
                         }
                    }
                  free(key);
                  free(val);
               }
          }
     }
   fclose(f);
   return ctc;
}

static int
_data_contact_save(Data_Contact *ctc, const char *file)
{
   FILE *f;
   Eina_List *l;
   
   f = fopen(file, "wb");
   if (!f) return 0;
   fprintf(f, "BEGIN:VCARD\n");
   if (ctc->version)
     {
        fprintf(f, "VERSION:%s\n", ctc->version);
     }
   fprintf(f, "N:");
   for (l = ctc->name.lasts; l; l = l->next)
     {
        fprintf(f, "%s", l->data);
        if (l->next) fprintf(f, ",");
     }
   fprintf(f, ";");
   for (l = ctc->name.firsts; l; l = l->next)
     {
        fprintf(f, "%s", l->data);
        if (l->next) fprintf(f, ",");
     }
   fprintf(f, ";");
   for (l = ctc->name.others; l; l = l->next)
     {
        fprintf(f, "%s", l->data);
        if (l->next) fprintf(f, ",");
     }
   fprintf(f, ";");
   for (l = ctc->name.titles; l; l = l->next)
     {
        fprintf(f, "%s", l->data);
        if (l->next) fprintf(f, ",");
     }
   fprintf(f, ";");
   for (l = ctc->name.honorifics; l; l = l->next)
     {
        fprintf(f, "%s", l->data);
        if (l->next) fprintf(f, ",");
     }
   fprintf(f, "\n");
   if (ctc->name.display)
     {
        fprintf(f, "FN:%s\n", ctc->name.display);
     }
   if (ctc->name.nicks)
     {
        fprintf(f, "NICKNAME:");
        for (l = ctc->name.nicks; l; l = l->next)
          {
             fprintf(f, "%s", l->data);
             if (l->next) fprintf(f, ",");
          }
        fprintf(f, "\n");
     }
   for (l = ctc->tel.numbers; l; l = l->next)
     {
        Data_Contact_Tel *tel = l->data;
        int wrote = 0;
        fprintf(f, "TEL;TYPE=");
        if (tel->flags & DATA_CONTACT_TEL_HOME)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "HOME");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_MSG)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "MSG");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_WORK)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "WORK");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_PREF)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "PREF");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_VOICE)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "VOICE");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_FAX)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "FAX");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_CELL)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "CELL");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_VIDEO)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "VIDEO");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_PAGER)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "PAGER");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_BBS)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "BBS");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_MODEM)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "MODEM");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_CAR)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "CAR");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_ISDN)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "ISDN");
             wrote = 1;
          }
        if (tel->flags & DATA_CONTACT_TEL_PCS)
          { 
             if (wrote) fprintf(f, ",");
             fprintf(f, "PCS");
             wrote = 1;
          }
        fprintf(f, ":%s\n", tel->number);
     }
   if (ctc->photo_file)
     {
        fprintf(f, "PHOTO;VALUE=uri:file:%s\n", ctc->photo_file);
     }
   for (l = ctc->extra_lines; l; l = l->next)
     {
        fprintf(f, "%s\n", l->data);
     }
   fprintf(f, "END:VCARD\n");
   fclose(f);
   return 1;
}

static void
_data_contact_dir_load(const char *dir)
{
   Ecore_List *files;
   
   files = ecore_file_ls(dir);
   if (files)
     {
        char *file;
        
        ecore_list_first_goto(files);
        while ((file = ecore_list_current(files)))
          {
             char *ext;
             
             ext = strrchr(file, '.');
             if ((ext) && (!strcasecmp(ext, ".vcf")))
               {
                  Data_Contact *ctc;
                  char buf[PATH_MAX];
                  
                  snprintf(buf, sizeof(buf), "%s/%s", dir, file);
                  ctc = _data_contact_load(buf);
                  if (ctc)
                    contacts = evas_list_prepend(contacts, ctc);
               }
             ecore_list_next(files);
          }
        ecore_list_destroy(files);
     }
}

static char *
_del_tel_canonicalise(const char *tel)
{
   char *t = NULL, *p;
   int t_len = 0, t_alloc = 0;
   
   // FIXME: need to canonicalise based on current location (country/telco)
   // and local numbers vs full intl number svs intl dialcode numbers
   // that caller-id may report. eg canonicalising when in +61 (australia)
   //  02 1234 5678 -> +61212345678
   // when in the '02" dial prefix (state of nsw):
   //  1234 5678 -> +61212345678
   // or if in another state (victoria is 03):
   //  1234 5678 -> +61312345678
   // and 0011 is one of several inlt dial prefixes:
   //  0011 61 2 1234 5678 -> +61212345678
   // 
   // same with all countries. canonicalising depends on:
   // 1. the current country, telco and locality/prefix.
   // 2. on what's alread there in whats listed, but in the end the number
   // should uniquely refer to a fully qualified international dial line
   // for that number. some regions may not allow this to work and so when
   // dialled it needs to be simplified again.
   for (p = (char *)tel; *p; p++)
     {
        switch (*p)
          {
          case '0':
             t = _buf_append(t, "0", &t_len, &t_alloc);
             break;
          case '1':
             t = _buf_append(t, "1", &t_len, &t_alloc);
             break;
          case '2':
          case 'a':
          case 'b':
          case 'c':
          case 'A':
          case 'B':
          case 'C':
             t = _buf_append(t, "2", &t_len, &t_alloc);
             break;
          case '3':
          case 'd':
          case 'e':
          case 'f':
          case 'D':
          case 'E':
          case 'F':
             t = _buf_append(t, "3", &t_len, &t_alloc);
             break;
          case '4':
          case 'g':
          case 'h':
          case 'i':
          case 'G':
          case 'H':
          case 'I':
             t = _buf_append(t, "4", &t_len, &t_alloc);
             break;
          case '5':
          case 'j':
          case 'k':
          case 'l':
          case 'J':
          case 'K':
          case 'L':
             t = _buf_append(t, "5", &t_len, &t_alloc);
             break;
          case '6':
          case 'm':
          case 'n':
          case 'o':
          case 'M':
          case 'N':
          case 'O':
             t = _buf_append(t, "6", &t_len, &t_alloc);
             break;
          case '7':
          case 'p':
          case 'q':
          case 'r':
          case 's':
          case 'P':
          case 'Q':
          case 'R':
          case 'S':
             t = _buf_append(t, "7", &t_len, &t_alloc);
             break;
          case '8':
          case 't':
          case 'u':
          case 'v':
          case 'T':
          case 'U':
          case 'V':
             t = _buf_append(t, "8", &t_len, &t_alloc);
             break;
          case '9':
          case 'w':
          case 'x':
          case 'y':
          case 'z':
          case 'W':
          case 'X':
          case 'Y':
          case 'Z':
             t = _buf_append(t, "9", &t_len, &t_alloc);
             break;
          case '*':
             t = _buf_append(t, "*", &t_len, &t_alloc);
             break;
          case '#':
             t = _buf_append(t, "#", &t_len, &t_alloc);
             break;
          default:
             break;
          }
     }
   if (!t) t = strdup("");
   return t;
}

static int
_data_tel_match(const char *tel1, const char *tel2)
{
   char *t1, *t2;
   int match = 0;
   
   t1 = _del_tel_canonicalise(tel1);
   t2 = _del_tel_canonicalise(tel2);
   if ((t1) && (t2)) match = !strcmp(t1, t2);
   if (t1) free(t1);
   if (t2) free(t2);
   return match;
}

void
data_init(void)
{
   char buf[PATH_MAX];
   char *home;
   
   home = getenv("HOME");
   if (!home) home = ".";
   snprintf(buf, sizeof(buf), "%s/Messages", home);
   messagedir = eina_stringshare_add(buf);
   ecore_file_mkpath(messagedir);
   snprintf(buf, sizeof(buf), "%s/Inbox", messagedir);
   ecore_file_mkpath(buf);
   _data_message_dir_load(buf, DATA_MESSAGE_NONE);
   snprintf(buf, sizeof(buf), "%s/Sent", messagedir);
   ecore_file_mkpath(buf);
   _data_message_dir_load(buf, DATA_MESSAGE_SENT);
   snprintf(buf, sizeof(buf), "%s/Trash", messagedir);
   ecore_file_mkpath(buf);
   _data_message_dir_load(buf, DATA_MESSAGE_TRASH);
   snprintf(buf, sizeof(buf), "%s/Contacts", home);
   contactdir = eina_stringshare_add(buf);
   _data_contact_dir_load(buf);
   ecore_file_mkpath(contactdir);
}

void
data_shutdown(void)
{
   eina_stringshare_del(messagedir);
   messagedir = NULL;
   eina_stringshare_del(contactdir);
   contactdir = NULL;
   // FIXME: free messages, and callbacks
}

void
data_message_sent_store(Data_Message *reply_to, const char *to, const char *message)
{
   // FIXE: alloc msg, add to list and store on disk
}

void
data_message_add_callback_add(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb = _callback_new(func, data);
   if (!cb) return;
   add_callbacks = eina_list_append(add_callbacks, cb);
}

void
data_message_add_callback_del(void (*func) (void *data, Data_Message *msg), const void *data)
{
   add_callbacks = _callback_del(add_callbacks, &add_walking, &add_deletes, func, data);
}

void
data_message_del_callback_add(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb = _callback_new(func, data);
   if (!cb) return;
   del_callbacks = eina_list_append(del_callbacks, cb);
}

void
data_message_del_callback_del(void (*func) (void *data, Data_Message *msg), const void *data)
{
   del_callbacks = _callback_del(del_callbacks, &del_walking, &del_deletes, func, data);
}

const Eina_List *
data_message_all_list(void)
{
   return messages;
}
           
void
data_message_trash(Data_Message *msg)
{
   _callback_call(del_callbacks, &del_walking, &del_deletes, msg);
   msg->flags |= DATA_MESSAGE_TRASH;
   _callback_call(add_callbacks, &add_walking, &add_deletes, msg);
}

void
data_message_del(Data_Message *msg)
{
   _callback_call(del_callbacks, &del_walking, &del_deletes, msg);
   // FIXME: delete file
   messages = eina_list_remove(messages, msg);
   eina_stringshare_del(msg->path);
   eina_stringshare_del(msg->from_to);
   eina_stringshare_del(msg->body);
   free(msg);
}

Data_Contact *
data_contact_by_tel_find(const char *number)
{
   Eina_List *l, *l2;
   
   for (l = contacts; l; l = l->next)
     {
        Data_Contact *ctc = l->data;
        for (l2 = ctc->tel.numbers; l2; l2 = l2->next)
          {
             Data_Contact_Tel *tel = l2->data;
             if (_data_tel_match(tel->number, number))
               return ctc;
          }
     }
   return NULL;
}

char *
data_contact_photo_file_get(Data_Contact *ctc)
{
   char buf[PATH_MAX];
   
   if (!ctc->photo_file) return NULL;
   if (ctc->photo_file[0] == '/')
     snprintf(buf, sizeof(buf), "%s", ctc->photo_file);
   else
     snprintf(buf, sizeof(buf), "%s/%s", contactdir, ctc->photo_file);
   return strdup(buf);
}

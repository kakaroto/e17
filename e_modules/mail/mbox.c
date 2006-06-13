#include <e.h>
#include "e_mod_main.h"
#include "mbox.h"

static Evas_List *mboxes;

static void _mail_mbox_check_mail(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path);

void 
_mail_mbox_add_mailbox(void *data, void *data2) 
{
   MboxClient *mb;
   Config_Box *cb;
   
   cb = data2;
   if (!cb) return;
   
   mb = E_NEW(MboxClient, 1);
   mb->config = cb;
   mb->data = data;
   mb->config->num_new = 0;
   mb->config->num_total = 0;
   mb->monitor = ecore_file_monitor_add(cb->new_path, _mail_mbox_check_mail, mb);
   
   mboxes = evas_list_append(mboxes, mb);
}

void
_mail_mbox_del_mailbox(void *data) 
{
   Config_Box *cb;
   Evas_List *l;
   
   cb = data;
   if (!cb) return;
   for (l = mboxes; l; l = l->next) 
     {
	MboxClient *mb;
	
	mb = l->data;
	if (!mb) continue;
	if (mb->config != cb) continue;
	if (mb->monitor)
	  ecore_file_monitor_del(mb->monitor);
	mboxes = evas_list_remove(mboxes, mb);
	free(mb);
	break;
     }
}

void
_mail_mbox_shutdown() 
{
   if (!mboxes) return;
   while (mboxes) 
     {
	MboxClient *mb;
	
	mb = mboxes->data;
	if (mb->monitor)
	  ecore_file_monitor_del(mb->monitor);
	mboxes = evas_list_remove_list(mboxes, mboxes);
	free(mb);
     }
}

/* PRIVATES */
static void 
_mail_mbox_check_mail(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path) 
{
   MboxClient *mb;
   Config_Box *cb;
   FILE *f;
   char buf[1024];
   int total = 0, unread = 0;
   int header;
   
   mb = data;
   if (!mb) return;
   
   cb = mb->config;
   if (!cb) return;
   if (!cb->new_path) return;
   
   if (!(f = fopen(cb->new_path, "r")))
     return;

   mb->config->num_new = 0;
   mb->config->num_total = 0;
   
   while (fgets(buf, sizeof(buf), f)) 
     {
	if (buf[0] == '\n')
	  header = 0;
	else if (!strncmp(buf, "From ", 5)) 
	  {
	     header = 1;
	     mb->config->num_total++;
	     mb->config->num_new++;
	  }
	else if ((header) && (!strncmp(buf, "Status: ", 7)) && (strchr(buf, 'R')))
	  mb->config->num_new--;
     }
   fclose(f);

   _mail_set_text(mb->data);
   if ((mb->config->num_new > 0) && (mb->config->use_exec) && (mb->config->exec))
     _mail_start_exe(mb->config);
}

#include "exhibit.h"
#include "exhibit_file.h"

pid_t pid = -1;
Evas_List *thumb_list;

int
_ex_thumb_exe_exit(void *data, int type, void *event)
{
   Ecore_Event_Exe_Exit *ev;
   Ex_Thumb              *thumb;
   char                 *ext;

   ev = event;
   if (ev->pid != pid) return 1;
   if (!thumb_list) return 1;

   thumb = thumb_list->data;
   thumb_list = evas_list_remove_list(thumb_list, thumb_list);

   ext = strrchr(thumb->name, '.');

   if (ext)
     {
	thumb->image = (char*)epsilon_thumb_file_get(thumb->ep);
	etk_tree_append(ETK_TREE(thumb->e->itree), thumb->e->icol, thumb->image, thumb->name, NULL);
	free(thumb->image);
	free(thumb->name);
	epsilon_free(thumb->ep);
	free(thumb);
     }

   pid = -1;
   _ex_thumb_generate();
   return 1;
}

void
_ex_thumb_generate()
{
   Ex_Thumb *thumb;

   if ((!thumb_list) || (pid != -1)) return;

   pid = fork();
   if (pid == 0)
     {
	/* reset signal handlers for the child */
	signal(SIGSEGV, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGBUS, SIG_DFL);

	thumb = thumb_list->data;
	if(_ex_file_is_ebg(thumb->name))
	  epsilon_key_set(thumb->ep, "desktop/background");
	if(epsilon_generate(thumb->ep))
	  {
	     thumb->image = (char*)epsilon_thumb_file_get(thumb->ep);
	  }
	exit(0);
     }
}

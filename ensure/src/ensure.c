#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/signalfd.h>


#include <Eina.h>
#include <Elementary.h>

#include "enobj.h"
#include "enasn.h"
#include "ensure.h"
#include "parser.h"
#include "display.h"

struct error {
	const char *msg;
};

struct asninfo {
	bool enabled;
	void *data;
	struct assurance *asn;
};

struct severityinfo {
	const char *name;
	const char *icon;
	Elm_Genlist_Item *item;
	Eina_List *asninfo;
} severity[] = {
	[ENSURE_CRITICAL] = {
		.name = "Critical",
	},
	[ENSURE_BUG] = {
		.name = "Bug",
	},
	[ENSURE_BADFORM] = {
		.name = "Bad Form",
	},
	[ENSURE_POLICY] = {
		.name = "Local Policy",
	},
	[ENSURE_PEDANTIC] = {
		.name = "Pedantry",
	}
};





Evas_Object *window_add(char **argv);

static void on_run(void *data, Evas_Object *button, void *event_info);
static void on_check(void *data, Evas_Object *button, void *event_info);
static void on_switch_config(void *data, Evas_Object *button, void *event_info);


static void dochild(char **args, int fd);

static int signal_init(void);
static Eina_Bool signalfd_child(void *data, Ecore_Fd_Handler *fd_handler);

static void config_add_classes(Evas_Object *gl);
static void cfg_sel(void *data, Evas_Object *obj, void *event);
static void cfg_exp(void *data, Evas_Object *obj, void *event);
static void generic_contract(void *data, Evas_Object *obj, void *event);
static void generic_exp_req(void *data, Evas_Object *obj, void *event);
static void generic_con_req(void *data, Evas_Object *obj, void *event);

static void enobj_select(void *data, Evas_Object *obj, void *event);
//static void enobj_expand(void *data, Evas_Object *obj, void *event);
static char *enobj_label_get(void *data, Evas_Object *, const char *);
static Evas_Object *enobj_icon_get(void *data, Evas_Object *, const char *);
static Eina_Bool enobj_state_get(void *data, Evas_Object *, const char *);


static char *cfg_label_get(void *, Evas_Object *, const char *part);
static Evas_Object *cfg_icon_get(void *, Evas_Object *, const char *part);
static Eina_Bool cfg_state_get(void *, Evas_Object *, const char *part);
static void cfg_del(void *, Evas_Object *obj);

static char *asn_label_get(void *data, Evas_Object *, const char *);
static Evas_Object *asn_icon_get(void *data, Evas_Object *, const char *);
static Eina_Bool asn_state_get(void *data, Evas_Object *, const char *);
static void asn_del(void *data, Evas_Object *obj);
static void asn_select(void *data, Evas_Object *obj, void *event);
static void asn_select_toggle(void *data, Evas_Object *obj, void *event);

static char *enwin_label_get(void *data, Evas_Object *, const char *);
//static Evas_Object *enwin_icon_get(const void *data, Evas_Object *, const char *);
static Eina_Bool enwin_state_get(void *data, Evas_Object *, const char *);
static void enwin_select(void *data, Evas_Object *obj, void *event);
static void enwin_del(void *data, Evas_Object *obj);



static Evas_Object *runbutton;
static Evas_Object *checkbutton;
static pid_t childid;
Evas_Object *objlist;
Evas_Object *configlist;
Evas_Object *box;
Evas_Object *mainwindow;
bool changedir = false;

#include "enedj.h"
int
elm_main(int argc, char **argv){

        if (argc < 2){
		printf("Usage: %s  [-c] <program>\n", argv[0]);
		printf("\t-c\tStart subprogram in it's path directory\n");

                exit(0);
        }

	/* FIXME: this isn't implemented  */
	if (streq(argv[1],"-c")){
		changedir = true;
		memmove(argv + 1, argv + 2, argc - 1);
		argv[argc] = 0;
		argc --;
	}

        mainwindow = window_add(argv);

	signal_init();

	enasn_load(NULL);

        elm_list_go(mainwindow);
        elm_run();
        elm_shutdown();
        return 0;
}
ELM_MAIN()



Evas_Object *
window_add(char **args){
        Evas_Object *win,*bg,*bx,*ctrls,*run,*check,*gl,*gl2,*config;

        win = elm_win_add(NULL, "Ensure", ELM_WIN_BASIC);
        elm_win_title_set(win, "Ensure");
        //evas_object_smart_callback_add(win,"delete,request", win_del,NULL);

        bg = elm_bg_add(win);
        elm_win_resize_object_add(win, bg);
        evas_object_size_hint_weight_set(bg,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
        evas_object_show(bg);

        bx = elm_box_add(win);
	box = bx;
        evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, bx);
        evas_object_show(bx);

	gl = elm_genlist_add(win);
	configlist = gl;
	elm_genlist_always_select_mode_set(gl, false);
	evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	config_add_classes(gl);
//	evas_object_show(gl);

	/* Add object list */
	gl2 = elm_genlist_add(win);
	objlist = gl2;
	elm_genlist_always_select_mode_set(gl2, true);
	evas_object_size_hint_align_set(gl2, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(gl2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(gl2);

	evas_object_smart_callback_add(gl2, "expand,request", generic_exp_req,
			gl2);
	evas_object_smart_callback_add(gl2, "contract,request",generic_con_req,
			gl2);
	evas_object_smart_callback_add(gl2, "expanded", enasn_display_bugs, gl2);
	evas_object_smart_callback_add(gl2, "contracted", generic_contract,gl2);
	elm_box_pack_end(bx, gl2);

	ctrls = elm_box_add(win);
	elm_box_horizontal_set(ctrls, true);
        evas_object_size_hint_weight_set(ctrls, EVAS_HINT_EXPAND, 0);
        evas_object_size_hint_align_set(ctrls, EVAS_HINT_FILL, EVAS_HINT_FILL);


	/* Buttons: Main event is the 'clicked' event */
	config = elm_button_add(ctrls);
	elm_button_label_set(config, "Config");
	elm_button_autorepeat_set(config, false);
	elm_box_pack_start(ctrls, config);
	evas_object_show(config);
	evas_object_smart_callback_add(config, "clicked", on_switch_config,
			NULL);

	run = elm_button_add(ctrls);
	elm_button_label_set(run, "Run");
	elm_button_autorepeat_set(run, false);
	elm_box_pack_end(ctrls,run);
	evas_object_show(run);
	evas_object_smart_callback_add(run, "clicked", on_run, args + 1);
	runbutton = run;

	check = elm_button_add(ctrls);
	elm_button_label_set(check, "Check");
	elm_button_autorepeat_set(check, false);
	elm_object_disabled_set(check, true);
	elm_box_pack_end(ctrls,check);
	evas_object_show(check);
	evas_object_smart_callback_add(check, "clicked", on_check, NULL);
	checkbutton = check;

	elm_box_pack_end(bx, ctrls);
	evas_object_show(ctrls);


	evas_object_resize(win, 320, 480);
	evas_object_show(win);

        /* Urh.. the list widget for now */
        return gl;

}

static const Elm_Genlist_Item_Class clc = {
	.item_style = "default",
	.func = {
		.label_get = cfg_label_get,
		.icon_get = cfg_icon_get,
		.state_get = cfg_state_get,
		.del = cfg_del,
	},
};
static const Elm_Genlist_Item_Class asnclass = {
	.item_style = "default",
	.func = {
		.label_get = asn_label_get,
		.icon_get = asn_icon_get,
		.state_get = asn_state_get,
		.del = asn_del
	},
};

static const Elm_Genlist_Item_Class objc = {
	.item_style = "default",
	.func = {
		.label_get = enobj_label_get,
		.icon_get = enobj_icon_get,
		.state_get = enobj_state_get,
		.del = asn_del
	}
};

static const Elm_Genlist_Item_Class windowclass = {
	.item_style = "default",
	.func = {
		.label_get = enwin_label_get,
		.state_get = enwin_state_get,
		.del = enwin_del,
	},
};

static void
config_add_classes(Evas_Object *gl){
	int i;
	for (i = 0 ; i < ENSURE_N_SEVERITIES ; i ++){
		severity[i].item = elm_genlist_item_append(gl,&clc,
				severity + i,
				NULL, /* No parent */
				ELM_GENLIST_ITEM_SUBITEMS,
				cfg_sel,
				severity + i /* data */);

	}

	evas_object_smart_callback_add(gl, "expand,request", generic_exp_req, gl);
	evas_object_smart_callback_add(gl, "contract,request", generic_con_req, gl);
	evas_object_smart_callback_add(gl, "expanded", cfg_exp, gl);
	evas_object_smart_callback_add(gl, "contracted", generic_contract, gl);

}

static void
cfg_sel(void *data, Evas_Object *obj ensure_unused, void *event ensure_unused){
	const struct severityinfo *info = data;

	printf("Item selected! %s\n",info->name);
}
static void
cfg_exp(void *data ensure_unused, Evas_Object *obj ensure_unused, void *event){
	Elm_Genlist_Item *parent = event;
	Evas_Object *gl = elm_genlist_item_genlist_get(parent);
	struct severityinfo *sev;
	struct asninfo *ai;
	Eina_List *l;


	sev = (struct severityinfo *)elm_genlist_item_data_get(parent);

	EINA_LIST_FOREACH(sev->asninfo, l, ai){
		elm_genlist_item_append(gl, &asnclass,
				ai, parent,
				ELM_GENLIST_ITEM_NONE,
				asn_select, ai);
	}

}
static void
generic_contract(void *data ensure_unused, Evas_Object *obj ensure_unused, void *event){
	Elm_Genlist_Item *it = event;
	elm_genlist_item_subitems_clear(it);
}
static void
generic_exp_req(void *data ensure_unused, Evas_Object *obj ensure_unused, void *event){
	Elm_Genlist_Item *it = event;
	elm_genlist_item_expanded_set(it, 1);
}
static void
generic_con_req(void *data ensure_unused, Evas_Object *obj ensure_unused, void *event){
	Elm_Genlist_Item *it = event;
	elm_genlist_item_expanded_set(it, 0);
}
static void
cfg_del(void *data ensure_unused, Evas_Object *obj ensure_unused){

}


static char *
cfg_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	const struct severityinfo *info;

	info = data;
	return strdup(info->name);
}
static Evas_Object *
cfg_icon_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return NULL;
}
static Eina_Bool
cfg_state_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return false;
}

/** Handlers for the subitems in the assurance list */
static char *
asn_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	struct asninfo *info;

	info = data;
	return strdup(info->asn->summary);
}

static Evas_Object *
asn_icon_get(void *data, Evas_Object *obj, const char *part){
	struct asninfo *ai = data;
	Evas_Object *ck;

	if (strcmp(part, "elm.swallow.end") == 0){
		ck = elm_check_add(obj);
		evas_object_show(ck);
		elm_check_state_set(ck, ai->enabled);
		evas_object_smart_callback_add(ck, "changed",
				asn_select_toggle, ai);
		return ck;
	}

	return NULL;
}
static Eina_Bool
asn_state_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	struct asninfo *ai;

	ai = data;
	return ai->enabled;
}
static void
asn_del(void *data ensure_unused, Evas_Object *obj ensure_unused){
	struct enwin *enwin = data;

	enwin->genitem = NULL;
}

static void
asn_select(void *data, Evas_Object *obj ensure_unused, void *event ensure_unused){
	struct asninfo *info = data;

	info->enabled = !info->enabled;
}

static void
asn_select_toggle(void *data, Evas_Object *obj ensure_unused, void *event ensure_unused){
	struct asninfo *ai = data;

	ai->enabled = elm_check_state_get(obj);
}


static char *
enwin_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	const struct enwin *enwin;
	const char *fmt = "Untitled Window '%p'";
	char *buf;
	int len;

	enwin = data;

	if (enwin->name && strlen(enwin->name) > 1){
		return strdup(enwin->name);
	}

	len = snprintf(NULL,0,fmt,enwin->id);
	if (len < 1) return NULL;
	len ++;
	buf = malloc(len);
	if (!buf) return NULL;
	/* remmeber to update both snprintfs if you change it */
	snprintf(buf,len,fmt,enwin->id);
	return buf;
}
/*
static Evas_Object *
enwin_icon_get(const void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return NULL;
}*/
static Eina_Bool
enwin_state_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *state ensure_unused){
	return false;
}
static void
enwin_select(void *data, Evas_Object *obj, void *event){
	/* FIXME: Do something or delete this */
	printf("Select... ignoring\n");
}
static void enwin_del(void *data, Evas_Object *obj){
	/* FIXME: Do something or delete this */
}




int
ensure_enobj_err_list_add(struct enobj *enobj){

	/* Does the window have an item */
	if (!enobj->enwin->genitem){
		enobj->enwin->genitem = elm_genlist_item_append(objlist,
				&windowclass, enobj->enwin, NULL,
				ELM_GENLIST_ITEM_SUBITEMS, enwin_select,
				enobj->enwin);
	}

	enobj->genitem = elm_genlist_item_append(objlist, &objc,
			enobj, enobj->enwin->genitem, ELM_GENLIST_ITEM_SUBITEMS,
			enobj_select, enobj);
	return 0;
}
static void
enobj_select(void *data ensure_unused, Evas_Object *obj ensure_unused,
		void *itemv){
	elm_genlist_item_expanded_set(itemv, true);
}
static char *
enobj_label_get(void *data, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	const struct enobj *enobj = data;
	char buf[200];

	if (enobj->name){
		snprintf(buf,sizeof(buf), "%s (%lx) - %s",
				enobj->name, enobj->id, enobj->type);
	} else {
		snprintf(buf,sizeof(buf), "%lx - %s",
				enobj->id,enobj->type);
	}
	return strdup(buf);
}
static Evas_Object *
enobj_icon_get(void *enobjv, Evas_Object *obj,
		const char *part){
	Evas_Object *bt;
	if (strcmp(part, "elm.swallow.end") == 0){
		bt = elm_button_add(obj);
		elm_button_label_set(bt, "View");
		elm_button_autorepeat_set(bt, false);
		evas_object_show(bt);
		evas_object_smart_callback_add(bt, "clicked",
				display_enobj_cb, enobjv);
		return bt;
	}


	return NULL;
}
static Eina_Bool
enobj_state_get(void *data ensure_unused, Evas_Object *obj ensure_unused,
		const char *part ensure_unused){
	return false;
}



/**
 * Initialise our signal handler
 */
static int
signal_init(void){
	int fd;
	sigset_t sigchld;

	sigemptyset(&sigchld);
	sigaddset(&sigchld, SIGCHLD);

	fd = signalfd(-1, &sigchld, SFD_CLOEXEC | SFD_NONBLOCK);
	ecore_main_fd_handler_add(fd,ECORE_FD_READ|ECORE_FD_ERROR,
			signalfd_child, NULL,
			NULL, NULL);

	return 0;
}

static Eina_Bool
signalfd_child(void *data ensure_unused, Ecore_Fd_Handler *fdh){
	int fd;
	struct signalfd_siginfo siginfo;
	printf("Child exited\n");

	fd = ecore_main_fd_handler_fd_get(fdh);

	read(fd, &siginfo, sizeof(struct signalfd_siginfo));

	elm_object_disabled_set(runbutton, false);
	elm_object_disabled_set(checkbutton, true);

	return 1;
}




static void
on_switch_config(void *data ensure_unused, Evas_Object *button, void *event_info ensure_unused){
	bool state;

	state = evas_object_visible_get(objlist);
	if (state){
		/* Front currently: Change to back */
		elm_button_label_set(button, "Report");
		evas_object_hide(objlist);
		evas_object_show(configlist);
		elm_box_unpack(box, objlist);
		elm_box_pack_start(box, configlist);
	} else {
		elm_button_label_set(button, "Config");
		evas_object_show(objlist);
		evas_object_hide(configlist);
		elm_box_unpack(box, configlist);
		elm_box_pack_start(box, objlist);
	}

}


/**
 * The user clicked the 'check' button: Start checking the application.
 */
static void
on_check(void *data ensure_unused, Evas_Object *button ensure_unused,
		void *event_info ensure_unused){

	/* clear the list of items */
	enobj_clear();

	printf("Sending kill to %d\n",childid);
	if (kill(childid, SIGUSR2))
		perror("kill(child, SIGUSR2))");
	printf("Done\n");

}





/**
 * The user clicked the 'run' button: Start running the application.
 */
static void
on_run(void *data, Evas_Object *button ensure_unused, void *event_info ensure_unused){
	pid_t pid;
	char **args = data;
	int pipefd[2];
	struct ensure *ensure;

	elm_object_disabled_set(runbutton, true);
	elm_object_disabled_set(checkbutton, false);

	printf("Running %s\n", args[0]);

	if (pipe(pipefd)){
		fprintf(stderr,"Unable to create pipe\n");
		return;
	}

	/* Watch the fd */
	ensure = calloc(1,sizeof(struct ensure));
	ecore_main_fd_handler_add(pipefd[0], ECORE_FD_READ, child_data,
					ensure, NULL, NULL);

	/* I'm sure someone will complain I'm doing this myself... but anyway
	 */
	switch ((pid = fork())){
	case -1:
		perror("fork()");
		exit(7);
		break;
	case 0:
		close(pipefd[0]);
		dochild(args,pipefd[1]);
		exit(7);
	default:
		/* Parent */
		close(pipefd[1]);
		childid = pid;
		break;
	}

	return;
}

/**
 * Run the child process
 */
static void
dochild(char **args, int fd){
	char buf[4];

//	setlinebuf(fd);
	setenv("LD_PRELOAD", LIBENSURE_DIR "/libensure.so",1);
	snprintf(buf, sizeof(buf), "%d",fd);
	setenv("ENSURE_FD", buf, 1);
	execvp(args[0],args);
	perror("execvp");
	exit(7);
}



int
ensure_assurance_add(struct assurance *enasn){
	struct asninfo *info;

	assert((int)enasn->severity >= 0);
	assert(enasn->severity < ENSURE_N_SEVERITIES);
	if ((int)enasn->severity < 0 && enasn->severity >= ENSURE_N_SEVERITIES){
		return -1;
	}

	info = calloc(1,sizeof(struct asninfo));
	info->enabled = true;
	info->asn = enasn;
	severity[enasn->severity].asninfo = eina_list_append(
			severity[enasn->severity].asninfo, info);

	return 0;
}


static Eina_Bool
check_obj(const Eina_Hash *hash ensure_unused, const void *key ensure_unused,
		void *data, void *ensure){
	struct enobj *enobj = data;
	Eina_List *l;
	struct asninfo *ai;
	int i;
	assert(enobj->magic == ENOBJMAGIC);

	for (i = 0 ; i < ENSURE_N_SEVERITIES ; i ++){
		EINA_LIST_FOREACH(severity[i].asninfo, l, ai){
			if (ai->enabled && ai->asn->object)
				ai->asn->object(ensure, enobj, ai->data);
		}
	}

	return 1;
}


int
enasn_check(struct ensure *ensure){
	int i;
	Eina_List *l;
	struct asninfo *ai;

	for (i = 0 ; i < ENSURE_N_SEVERITIES ; i ++){
		EINA_LIST_FOREACH(severity[i].asninfo, l, ai){
			if (ai->enabled && ai->asn->init)
				ai->data = ai->asn->init(ensure);
		}
	}

	eina_hash_foreach(objdb, check_obj, ensure);

	for (i = 0 ; i < ENSURE_N_SEVERITIES ; i ++){
		EINA_LIST_FOREACH(severity[i].asninfo, l, ai){
			if (ai->enabled && ai->asn->fini)
				ai->asn->fini(ensure, ai->data);
			ai->data = NULL;
		}
	}

	return 1;
}

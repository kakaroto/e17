#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Ipc.h>
#include "spawner.h"

/* Globals */
/* Entranced_Spawner_Display *d; */
static Ecore_Event_Handler *_e_handler = NULL;
static Ecore_Event_Handler *_d_handler = NULL;
static Ecore_Event_Handler *_sigusr1_handler = NULL;
static Ecore_Event_Filter *_e_filter = NULL;

static struct sigaction _entrance_x_sa, _entrance_d_sa;

static unsigned char is_respawning = 0;
static unsigned char exev = 0;
static unsigned char x_ready = 0;
Ecore_Timer *respawn_timer = NULL;

/**
 * Write the entranced pid to the defined pidfile.
 * @param pid The spawner's process ID, which is the pid after the fork if there was one
 * @return 0 if the operation was successful, 1 otherwise.
 */
int Entranced_Write_Pidfile (pid_t pid) {
    FILE *fp;
    int size, result = 1;
    char buf[PATH_MAX];

    size = snprintf(buf, PATH_MAX, "%d", pid);
    if ((fp = fopen(PIDFILE, "w+"))) {
        fwrite(buf, sizeof(char), size, fp);
        fclose(fp);
        result = 0;
    }

    return result;
}

/**
 * Make entranced a daemon by fork-and-exit. This is the default behavior.
 */
void Entranced_Fork_And_Exit(void) {
    pid_t entranced_pid;

    switch (entranced_pid = fork()) {
        case 0:
            break;
        default:
            if (Entranced_Write_Pidfile(entranced_pid)) {
                syslog(LOG_CRIT, "%d is the pid, but I couldn't write to %s.",
                        entranced_pid, PIDFILE);
                kill(entranced_pid, SIGKILL);
                exit(1);
            }
            exit(0);
    }
}

/**
 * Create a new display context.
 * @return A pointer to an Entranced_Spawner_Display handle for the new context
 */
Entranced_Spawner_Display *Entranced_Spawner_Display_New(void) {
    Entranced_Spawner_Display *d;

    d = malloc(sizeof(Entranced_Spawner_Display));
    memset(d, 0, sizeof(Entranced_Spawner_Display));
    /* TODO: Config-ize these parameters */
    d->xprog = strdup(X_SERVER);
    d->attempts = 5;
    d->status = NOT_RUNNING;
    d->e_exe = NULL;
    d->x_exe = NULL;
    d->display = NULL;
    return d;
}

/**
 * Launch a new X server
 * @param d The spawner display context that will handle this server
 */
void Entranced_Spawn_X(Entranced_Spawner_Display *d) {
    int i = 0;

    if (d == NULL)
        return;

    d->status = NOT_RUNNING;
    while ((i < d->attempts) && (d->status != RUNNING)) {
        if ((d->pid.x = Entranced_Start_Server_Once(d)) > 0)
            break;
        ++i;
    }
}

/**
 * Single attempt to start the X Server.
 * @param d The spawner display context that will handle this server
 * @return The status of the display context after the launch attempt
 */
pid_t Entranced_Start_Server_Once(Entranced_Spawner_Display *d) {
    double start_time;
    /* Ecore_Exe *x_exe; */
    pid_t xpid;

    d->status = LAUNCHING;
    /* Initialize signal handler for SIGUSR1 */
    _entrance_x_sa.sa_handler = SIG_IGN;
    _entrance_x_sa.sa_flags = SA_RESTART;
    sigemptyset(&_entrance_x_sa.sa_mask);
    _entrance_x_sa.sa_flags = 0;

    x_ready = 0;
    
    /* x_exe = ecore_exe_run(d->xprog, d); */
    switch(xpid = fork()) {
        case -1:
            syslog(LOG_WARNING, "fork() to start X server failed.");
            return -1;
        case 0:
            sigaction(SIGUSR1, &_entrance_x_sa, NULL);
            execl("/bin/sh", "/bin/sh", "-c", X_SERVER, NULL);
            syslog(LOG_WARNING, "Could not execute X server.");
            exit(1);
        default:
            d->name = strdup(getenv("DISPLAY"));
        	start_time = ecore_time_get();
	
            while (!x_ready) {
                double current_time;
                usleep(100000);
                current_time = ecore_time_get();
                if ((start_time - current_time) > 5.0)
                    break;
            }

            if (!x_ready)
                d->status = NOT_RUNNING;
            else
                d->status = RUNNING;

            return xpid;
    }
}

/**
 * Start a new Entrance session
 * @param d The spawner display context that this session will use
 */
void Entranced_Spawn_Entrance(Entranced_Spawner_Display *d) {
    char entrance_cmd[PATH_MAX];
    
    snprintf(entrance_cmd, PATH_MAX, "%s %s", ENTRANCE, d->name);
    d->e_exe = ecore_exe_run(entrance_cmd, d);
}

int Entranced_Respawn_Reset(void *data) {
	_DEBUG("Respawn timer reset.\n");
    is_respawning = 0;
	respawn_timer = NULL;
    return 0;
}

int Entranced_X_Restart(Entranced_Spawner_Display *d) {

    /* Attempt to restart X server */
    d->status = NOT_RUNNING;
        
    syslog(LOG_INFO, "Attempting to restart X server.");
    Entranced_Spawn_X(d);
    if (d->status != RUNNING) {
        syslog(LOG_CRIT, "Failed to restart the X server. Aborting.");
        return 0;
    } else
        syslog(LOG_INFO, "Successfully restarted the X server.");
    return 1;
}

/* Event Filters */
void *Entranced_Filter_Start(void *data) {
	return &exev;
}

int Entranced_Filter_Loop(void *data, void *loop_data, int type, void *event) {

	/* Filter out redundant exit events */
	if(type == ECORE_EVENT_EXE_EXIT) {
		if(exev)
			return 0;
		else
			exev = 1;
	}

	return 1;
}
	
void Entranced_Filter_End(void *data, void *loop_data) {
	exev = 0;
}

/* Event handlers */

/*int _Entranced_SIGUSR(void *data, int type, void *event) {*/
static void _Entranced_SIGUSR(int sig) {
/*    Ecore_Event_Signal_User *e = (Ecore_Event_Signal_User *) event; */

    _DEBUG("SIGUSR event triggered.\n");

    /* X sends SIGUSR1 to let us know it is ready */
/*    if (e->number == 1)*/
        x_ready = 1;
/*    return 1; */
}

int Entranced_Exe_Exited(void *data, int type, void *event) {
    Ecore_Event_Exe_Exit *e = (Ecore_Event_Exe_Exit *) event;
    Entranced_Spawner_Display *d = (Entranced_Spawner_Display *) data;
    
	_DEBUG("Ecore_Event_Exe_Exit triggered.\n");

    if(is_respawning) {
		_DEBUG("Event ignored.\n");
        return 1;
	} else {
		_DEBUG("Processing Event.\n");
	}
    
    is_respawning = 1;
    respawn_timer = ecore_timer_add(15.0, Entranced_Respawn_Reset, d);
	
    if (e->exe == d->e_exe) {
        /* Session exited or crashed */
        if (e->exited)
            syslog(LOG_INFO, "The session has ended normally.");
        else if (e->signalled)
            syslog(LOG_INFO, "The session was terminated with signal %d.", e->exit_signal);

        kill(d->pid.x, SIGHUP);
        sleep(3);
        if (waitpid(d->pid.x, NULL, WNOHANG) > 0) {
            syslog(LOG_INFO, "The X Server apparently died as well.");
            if(!Entranced_X_Restart(d))
                exit(1);
        }
    
    } else {
        /* X terminated for some reason */
        if (e->exited)
            syslog(LOG_INFO, "The X Server terminated for some reason.");
        else if (e->signalled)
            syslog(LOG_INFO, "The X server was terminated with signal %d.", e->exit_signal);

        sleep(2);
        kill(d->pid.x, SIGKILL);
        if(!Entranced_X_Restart(d))
            exit(1);

    }

    Entranced_Spawn_Entrance(d);

    return 1;
}

int Entranced_Signal_Exit(void *data, int type, void *event) {
	_DEBUG("Ecore_Signal_Exit_Triggered\n");
    syslog(LOG_INFO, "Caught exit signal.");
    syslog(LOG_INFO, "Display and display manager are shutting down.");
    ecore_main_loop_quit();
    return 0;
}

void Entranced_AtExit(void) {
	_DEBUG("Entranced exits.\n");
}

/*
 * Main function
 */
int main (int argc, char **argv) {
    int c;
    int nodaemon = 0;           /* TODO: Config-ize this variable */
    Entranced_Spawner_Display *d;
    struct option d_opt[] = {
        {"nodaemon", 0, 0, 1},
        {"help", 0, 0, 2},
        {0, 0, 0, 0}
    };
    pid_t entranced_pid = getpid();

    /* Initialize Ecore */
    ecore_init();
    ecore_app_args_set(argc, (const char **) argv);

    putenv("DISPLAY");          /* Not sure why this is here :) */
    openlog("entranced", LOG_NOWAIT, LOG_DAEMON);

    /* Parse command-line options */
    while (1) {
        c = getopt_long_only(argc, argv, "d:", d_opt, NULL);
        if (c == -1)
            break;
        switch (c) {
            case 'd':
                setenv("DISPLAY", optarg, 1);
                break;
            case 1:
                nodaemon = 1;
                break;
            case 2:
                /* This should probably in a separate usage function, but bleh */
                printf("Entranced - Launcher for the Entrance Display Manager\n");
                printf("Usage: %s [OPTION] ...\n\n", argv[0]);
                printf
                    ("--------------------------------------------------------------------------\n");
                printf("  -d DISPLAY         Connect to an existing X server\n");
                printf("  -help              Display this help message\n");
                printf
                    ("  -nodaemon          Don't fork to background (useful for init scripts)\n");
                printf
                    ("==========================================================================\n\n");
                printf
                    ("Note: if you're launching Entrance from within an existing X session, don't\n");
                printf
                    ("try to use entranced or you may get unexpected results. Instead, launch\n");
                printf("entrance directly by typing \"entrance\".\n\n");
                exit(0);

        }
    }

    /* TODO: Config-ize this */
    if (!getenv("DISPLAY"))
        setenv("DISPLAY", X_DISP, 1);

    if (nodaemon) {
        if (Entranced_Write_Pidfile(entranced_pid)) {
            syslog(LOG_CRIT, "%d is the pid, but I couldn't write to %s.",
                    entranced_pid, PIDFILE);
            exit(1);
        }
    } else
        Entranced_Fork_And_Exit();

    /* Check to make sure entrance binary is executable */
    if (access(ENTRANCE, X_OK)) {
        syslog(LOG_CRIT, "Fatal Error: Unable to launch entrance binary. Aborting.");
        exit(1);
    }

    /* Daemonize */
    if (!nodaemon) {
        close(0);
        close(1);
        close(2);
    }

    /* Set up a spawner context */
    d = Entranced_Spawner_Display_New();

    /* Event filter */
    _e_filter = ecore_event_filter_add(Entranced_Filter_Start, Entranced_Filter_Loop,
                                      Entranced_Filter_End, NULL);

    /* Set up event handlers */
    _e_handler = ecore_event_handler_add(ECORE_EVENT_EXE_EXIT, Entranced_Exe_Exited, d);
    _d_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, Entranced_Signal_Exit, NULL);
/*    _sigusr1_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_USER, _Entranced_SIGUSR, NULL); */

    /* Manually add signal handler for SIGUSR1 */
    _entrance_d_sa.sa_handler = _Entranced_SIGUSR;
    _entrance_x_sa.sa_flags = SA_RESTART;
    sigemptyset(&_entrance_d_sa.sa_mask);
    _entrance_x_sa.sa_flags = 0;
    sigaction(SIGUSR1, &_entrance_d_sa, NULL);
   
    /* Launch X Server */
    syslog(LOG_INFO, "Starting X server.");
    Entranced_Spawn_X(d);

    if (d->status == NOT_RUNNING) {
        free(d);
        syslog(LOG_CRIT, "Could not start X server.");
        exit(1);
    }

    /* Run Entrance */
    syslog(LOG_INFO, "Starting Entrance.");
    Entranced_Spawn_Entrance(d);

    /* Main program loop */
	_DEBUG("Entering main loop.\n");
    ecore_main_loop_begin();
    ecore_main_loop_quit();
    
    /* Shut down */
	_DEBUG("Exited main loop! Shutting down...\n");
    ecore_exe_terminate(d->e_exe);
    kill(d->pid.x, SIGTERM);
    sleep(5);
    /* Die harder */
    ecore_exe_kill(d->e_exe);
    kill(d->pid.x, SIGKILL);
        
    closelog();
    ecore_shutdown();
    exit(0);
}


#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Ipc.h>
#include "spawner.h"

/* Globals */
/* Entranced_Spawner_Display *d; */
Ecore_Event_Handler *e_handler = NULL;
Ecore_Event_Handler *d_handler = NULL;

static unsigned char is_respawning = 0;
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
static void Entranced_Spawn_X(Entranced_Spawner_Display *d) {
    int i = 0;

    if (d == NULL)
        return;

    d->status = NOT_RUNNING;
    while ((i < d->attempts) && (d->status != RUNNING)) {
        if ((d->x_exe = Entranced_Start_Server_Once(d)))
            break;
        ++i;
    }
}

/**
 * Single attempt to start the X Server.
 * @param d The spawner display context that will handle this server
 * @return The status of the display context after the launch attempt
 */
Ecore_Exe * Entranced_Start_Server_Once(Entranced_Spawner_Display *d) {
    double start_time;
    Ecore_Exe *x_exe;
/*    pid_t x_pid; */

    d->status = LAUNCHING;
    /* switch (x_pid = fork()) {
        case 0:
            execl("/bin/sh", "/bin/sh", "-c", d->xprog, d->xprog, NULL);
            start_time = ecore_time_get();
            break;
        case -1:
            syslog(LOG_CRIT, "Could not fork() to spawn X process.");
            perror("Entranced");
            exit(0);
            break;
        default:
            d->pid.x = x_pid;
            break;
    } */

    x_exe = ecore_exe_run(d->xprog, d);

    d->name = strdup(getenv("DISPLAY"));
	start_time = ecore_time_get();
	
    while (!(ecore_x_init(d->name))) {
        double current_time;
        usleep(100000);
        current_time = ecore_time_get();
        if ((start_time - current_time) > 5.0)
            break;
    }

    d->display = ecore_x_display_get();

    if (d->display == NULL) {
        d->status = NOT_RUNNING;
        if(x_exe != NULL) {
            ecore_exe_free(x_exe);
            x_exe = NULL;
        }
    } else
        d->status = RUNNING;

    return x_exe;
}

/**
 * Kill all X client windows. This is useful after the death of the session.
 */
void Entranced_X_Killall() {
    int i, n;
    Ecore_X_Window *roots;

    roots = ecore_x_window_root_list(&n);

    for (i = 0; i < n; ++i)
        ecore_x_killall(roots[i]);
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
    is_respawning = 0;
    return 0;
}

/* Event handlers */
int Entranced_Exe_Exited(void *data, int type, void *event) {
    Ecore_Event_Exe_Exit *e = (Ecore_Event_Exe_Exit *) event;
    Entranced_Spawner_Display *d = (Entranced_Spawner_Display *) data;
    
	printf("Ecore_Event_Exe_Exit triggered.\n");

    if(is_respawning)
        return;
    
    is_respawning = 1;
    respawn_timer = ecore_timer_add(3.0, Entranced_Respawn_Reset, NULL);
	
    if (e->exe == d->e_exe) {
        /* Session exited or crashed */
        if (e->exited)
            syslog(LOG_INFO, "The session has ended normally.");
        else if (e->signalled)
            syslog(LOG_INFO, "The session was terminated with signal %d.", e->exit_signal);
    
    } else {
        /* X terminated for some reason */
        if (e->exited)
            syslog(LOG_INFO, "The X Server terminated for some reason.");
        else if (e->signalled)
            syslog(LOG_INFO, "The X server was terminated with signal %d.", e->exit_signal);

    }

    /* Die harder */
    ecore_exe_terminate(d->e_exe);
    ecore_exe_terminate(d->x_exe);
    sleep(1);
    ecore_exe_kill(d->x_exe);
    sleep(1);

    ecore_exe_free(d->e_exe);
    ecore_exe_free(d->x_exe);
    
    d->status = NOT_RUNNING;
	
	/* Wait 4 seconds */
	sleep(4);

    /* Attempt to restart X server */
    Entranced_Spawn_X(d);
    if (d->status != RUNNING) {
        syslog(LOG_CRIT, "Failed to restart the X server. Aborting.");
        exit(1);
    }

    /* Launch Entrance */
    Entranced_Spawn_Entrance(d);
    
    return 1;
}

int Entranced_Signal_Exit(void *data, int type, void *event) {
	printf("Ecore_Signal_Exit_Triggered\n");
    syslog(LOG_INFO, "Display and display manager are shutting down.");
    ecore_main_loop_quit();
    return 0;
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

    /* Set up event handlers */
    e_handler = ecore_event_handler_add(ECORE_EVENT_EXE_EXIT, Entranced_Exe_Exited, d);
    d_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, Entranced_Signal_Exit, NULL);

    /* Main program loop */
	printf("Entering main loop.\n");
    ecore_main_loop_begin();

    
    /* Shut down */
	printf("Exited main loop! Shutting down...\n");
    ecore_exe_terminate(d->e_exe);
    ecore_exe_terminate(d->x_exe);
    sleep(5);
    /* Die harder */
    ecore_exe_kill(d->e_exe);
    ecore_exe_kill(d->x_exe);
        
    closelog();
    ecore_shutdown();
    exit(0);
}


#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "elsa.h"


char *_mcookie;
char **env;
char *_user;
static pid_t _session_pid;
static int _elsa_session_userid_set(struct passwd *pwd);

long
elsa_session_seed_get()
{
    struct timespec ts;
    long pid = getpid();
    long tm = time(NULL);
    if (clock_gettime(CLOCK_MONOTONIC, &ts))
       ts.tv_sec = ts.tv_nsec = 0;
    return pid + tm + (ts.tv_sec ^ ts.tv_nsec);
}

static int
_elsa_session_cookie_add(const char *mcookie, const char *display,
                         const char *xauth_cmd, const char *auth_file)
{
    char buf[4096];
    FILE *cmd;
    if (!xauth_cmd || !auth_file) return 1;
    snprintf(buf, sizeof(buf), "%s -f %s -q", xauth_cmd, auth_file);
    fprintf(stderr, PACKAGE": write auth %s\n", buf);

    cmd = popen(strdup(buf), "w");
    if (!cmd) return 1;
    fprintf(cmd, "remove %s\n", display);
    fprintf(cmd, "add %s . %s\n", display, mcookie);
    fprintf(cmd, "exit\n");
    pclose(cmd);
    return 0;
}

static int
_elsa_session_userid_set(struct passwd *pwd)
{
   if (!pwd)
     {
        fprintf(stderr, PACKAGE": no passwd !\n");
        return 1;
     }
   if (initgroups(pwd->pw_name, pwd->pw_gid) != 0)
     {
        fprintf(stderr, PACKAGE": can't init group\n");
        return 1;
     }
   if (setgid(pwd->pw_gid) != 0)
     {
        fprintf(stderr, PACKAGE": can't set gid\n");
        return 1;
     }
   if (setuid(pwd->pw_uid) != 0) 
     {
        fprintf(stderr, PACKAGE": can't set uid\n");
        return 1;
     }

   fprintf(stderr, PACKAGE": name -> %s, gid -> %d, uid -> %d\n", pwd->pw_name, pwd->pw_gid, pwd->pw_uid);
   return 0;
}

Eina_Bool
elsa_session_init(struct passwd *pwd)
{
   char buf[4096];
   char **tmp;
   fprintf(stderr, "%s: Session Init\n", PACKAGE);
   env = elsa_pam_env_list_get();
   elsa_pam_end();
   for (tmp = env; *tmp; ++tmp)
      fprintf(stderr, "%s: env %s\n", PACKAGE, *tmp);
   if(_elsa_session_userid_set(pwd)) return EINA_FALSE;
   if (pwd->pw_shell[0] == '\0')
     {
        setusershell();
        strcpy(pwd->pw_shell, getusershell());
        endusershell();
     }
   snprintf(buf, sizeof(buf), "%s/.Xauthority", pwd->pw_dir);
   _elsa_session_cookie_add(_mcookie, ":0",
                            elsa_config->command.xauth_path, buf);
   return EINA_TRUE;
}

void
elsa_session_run(struct passwd *pwd)
{
#ifdef HAVE_PAM
   char buf[4096];
   pid_t pid;
   pid = fork();
   if (pid == 0)
     {
        if (!elsa_session_init(pwd))
          {
             fprintf(stderr, "Elsa: couldn't open session\n");
             exit(1);
          }
        fprintf(stderr, "%s: Session Run\n", PACKAGE);
        snprintf(buf, sizeof(buf),
                 "%s %s ",
                 elsa_config->command.session_start,
                 pwd->pw_name);
        system(buf);
        chdir(pwd->pw_dir);
        fprintf(stderr, "je suis une session de %s\n", pwd->pw_name);

        snprintf(buf, sizeof(buf), "%s/.elsa_session.log", pwd->pw_dir);
        remove(buf);
        snprintf(buf, sizeof(buf),
                 "%s > %s/.elsa_session.log",
                 elsa_config->command.session_login,
                 pwd->pw_dir);
        execle(pwd->pw_shell, pwd->pw_shell, "-c",
               buf, NULL, env);
        fprintf(stderr, "Enlightenment n'est pas lance\n");
     }
   else 
     {
        elsa_session_pid_set(pid);
        _user = strdup(pwd->pw_name);
        elsa_gui_shutdown();
     }
#endif
}

void
elsa_session_pid_set(pid_t pid)
{
   fprintf(stderr, "%s: session pid %d\n", PACKAGE, pid);
   _session_pid = pid;
}

pid_t
elsa_session_pid_get()
{
   return _session_pid;
}

void
elsa_session_shutdown()
{
   char buf[4096];
   fprintf(stderr, PACKAGE": Session Shutdown\n");
   if (_user)
     {
        snprintf(buf, sizeof(buf),
                 "%s %s ",
                 elsa_config->command.session_stop,
                 _user);
        free(_user);
        system(buf);
     }
}

void
elsa_session_auth(const char *file)
{
   /* this is the mit cookie */

   uint16_t word;
   uint8_t hi, lo;
   int i;
   char buf[4096];

   _mcookie = calloc(32, sizeof(char));

   const char *dig = "0123456789abcdef";
   srand(elsa_session_seed_get());
   for (i=0; i<32; i+=4)
     {
        word = rand() & 0xffff;
        lo = word & 0xff;
        hi = word >> 8;
        _mcookie[i] = dig[lo & 0x0f];
        _mcookie[i+1] = dig[lo >> 4];
        _mcookie[i+2] = dig[hi & 0x0f];
        _mcookie[i+3] = dig[hi >> 4];
     }
   remove(file);
   snprintf(buf, sizeof(buf), "XAUTHORITY=%s", file);
   putenv(buf);
   fprintf(stderr, "Elsa: cookie %s \n", _mcookie);
   _elsa_session_cookie_add(_mcookie, ":0", "/usr/bin/xauth", file);
}

Eina_Bool
elsa_session_login(void *data)
{
#ifdef HAVE_PAM
   struct passwd *pwd;
   char buf[4096];
   char *term;
   int status;
   status = elsa_pam_authenticate();
   if (status)
     {
        elsa_gui_auth_error();
        return ECORE_CALLBACK_CANCEL;
     }
   if (!elsa_pam_open_session())
     {
        pwd = getpwnam(elsa_pam_item_get(ELSA_PAM_ITEM_USER));
        endpwent();
        term = getenv("TERM");
        if (term) elsa_pam_env_set("TERM", term);
        elsa_pam_env_set("HOME", pwd->pw_dir);
        elsa_pam_env_set("SHELL", pwd->pw_shell);
        elsa_pam_env_set("USER", pwd->pw_name);
        elsa_pam_env_set("LOGNAME", pwd->pw_name);
        elsa_pam_env_set("PATH", "./:/bin:/usr/bin:/usr/local/bin");
        elsa_pam_env_set("DISPLAY", ":0.0");
        elsa_pam_env_set("MAIL", "");
        snprintf(buf, sizeof(buf), "%s/.Xauthority", pwd->pw_dir);
        elsa_pam_env_set("XAUTHORITY", buf);
        elsa_gui_auth_valid(pwd);
     }
     return ECORE_CALLBACK_CANCEL;
#endif
}


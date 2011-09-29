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
#include <Ecore_File.h>
#include <Efreet.h>
#include "elsa.h"

static char *_mcookie;
static char **env;
static char *_login = NULL;
static unsigned char _logged = 0;
//static Eina_List *_user_list = NULL;
static pid_t _session_pid;
static Eina_List *_xsessions = NULL;
static int _elsa_session_userid_set(struct passwd *pwd);

static void _elsa_session_run(struct passwd *pwd, const char *cmd, const char *cookie);

static void _elsa_session_scan_desktops_file(const char *path);
static void _elsa_session_scan_desktops(const char *dir);
static void _elsa_session_init_desktops();
static const char *_elsa_session_find_command(const char *path, const char *session);

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
    char buf[PATH_MAX];
    FILE *cmd;

    if (!xauth_cmd || !auth_file) return 1;
    snprintf(buf, sizeof(buf), "%s -f %s -q", xauth_cmd, auth_file);
    fprintf(stderr, PACKAGE": write auth '%s'\n", buf);
    cmd = popen(buf, "w");
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

/*   fprintf(stderr, PACKAGE": name -> %s, gid -> %d, uid -> %d\n",
           pwd->pw_name, pwd->pw_gid, pwd->pw_uid); */
   return 0;
}

static Eina_Bool
_elsa_session_begin(struct passwd *pwd, const char *cookie)
{
   char *term;
   fprintf(stderr, PACKAGE": Session Init\n");
   if (pwd->pw_shell[0] == '\0')
     {
        setusershell();
        strcpy(pwd->pw_shell, getusershell());
        endusershell();
     }
   term = getenv("TERM");
   if (term) elsa_pam_env_set("TERM", term);
   elsa_pam_env_set("HOME", pwd->pw_dir);
   elsa_pam_env_set("SHELL", pwd->pw_shell);
   elsa_pam_env_set("USER", pwd->pw_name);
   elsa_pam_env_set("LOGNAME", pwd->pw_name);
   elsa_pam_env_set("PATH", elsa_config->session_path);
   elsa_pam_env_set("DISPLAY", ":0.0");
   elsa_pam_env_set("MAIL", "");
   elsa_pam_env_set("XAUTHORITY", cookie);
   return EINA_TRUE;
}

static void
_elsa_session_run(struct passwd *pwd, const char *cmd, const char *cookie)
{
#ifdef HAVE_PAM
   //char **tmp;
   char buf[PATH_MAX];
   pid_t pid;
   pid = fork();
   if (pid == 0)
     {

        fprintf(stderr, PACKAGE": Session Run\n");
        env = elsa_pam_env_list_get();
        elsa_pam_end();
        snprintf(buf, sizeof(buf),
                 "%s %s ",
                 elsa_config->command.session_start,
                 pwd->pw_name);
        if (-1 == system(buf))
          fprintf(stderr, PACKAGE": Error on session start command %s\n", buf);
        if(_elsa_session_userid_set(pwd)) return;
        _elsa_session_cookie_add(_mcookie, ":0",
                                 elsa_config->command.xauth_path, cookie);
        if (chdir(pwd->pw_dir))
          {
             fprintf(stderr, PACKAGE": change directory for user fail\n");
             return;
          }
//        fprintf(stderr, PACKAGE": Open %s`s session\n", pwd->pw_name);
        snprintf(buf, sizeof(buf), "%s/.elsa_session.log", pwd->pw_dir);
        remove(buf);
#ifdef HAVE_CONSOLEKIT
        snprintf(buf, sizeof(buf), PACKAGE_BIN_DIR"/elsa_ck_launch %s > %s/.elsa_session.log 2>&1",
                 cmd, pwd->pw_dir);
#else
        snprintf(buf, sizeof(buf), "%s > %s/.elsa_session.log 2>&1",
                 cmd, pwd->pw_dir);
#endif
        execle(pwd->pw_shell, pwd->pw_shell, "-c", buf, NULL, env);
        fprintf(stderr, PACKAGE": The Xsessions are not launched :(\n");
     }
#endif
}

void
elsa_session_end(const char *user)
{
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf),
            "%s %s ", elsa_config->command.session_stop, user);
   if (-1 == system(buf))
     fprintf(stderr, PACKAGE": Error on session stop command %s", buf);
   elsa_pam_close_session();
   elsa_pam_end();
   elsa_pam_shutdown();
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

static const char *dig = "0123456789abcdef";

void
elsa_session_init(const char *file)
{
   uint16_t word;
   uint8_t hi, lo;
   int i;
   char buf[PATH_MAX];

   fprintf(stderr, PACKAGE": Session init '%s'\n", file);

   _mcookie = calloc(33, sizeof(char));
   _mcookie[0] = 'a';

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
   putenv(strdup(buf));
   //fprintf(stderr, PACKAGE": cookie %s \n", _mcookie);
   _elsa_session_cookie_add(_mcookie, ":0",
                            elsa_config->command.xauth_path, file);
   _elsa_session_init_desktops();
}

void
elsa_session_shutdown()
{
}

Eina_Bool
elsa_session_authenticate(const char *login, const char *passwd)
{
   return (!elsa_pam_auth_set(login, passwd)
           && !elsa_pam_authenticate());
}

Eina_Bool
elsa_session_login(const char *session, Eina_Bool push)
{
#ifdef HAVE_PAM
   struct passwd *pwd;
   const char *cmd;
   char buf[PATH_MAX];

   if (!elsa_pam_open_session())
     {
        pwd = getpwnam(elsa_pam_item_get(ELSA_PAM_ITEM_USER));
        endpwent();
        _logged = EINA_TRUE;
        if (!pwd) return ECORE_CALLBACK_CANCEL;
        snprintf(buf, sizeof(buf), "%s/.Xauthority", pwd->pw_dir);
        if (!_elsa_session_begin(pwd, buf))
          {
             fprintf(stderr, "Elsa: couldn't open session\n");
             exit(1);
          }
        if (push) elsa_history_push(pwd->pw_name, session);
        _login = strdup(pwd->pw_name);
        cmd = _elsa_session_find_command(pwd->pw_dir, session);
        fprintf(stderr, PACKAGE": launching %s for user %s\n", cmd, _login);
        _elsa_session_run(pwd, cmd, buf);
     }
#endif
   return ECORE_CALLBACK_CANCEL;
}

static const char *
_elsa_session_find_command(const char *path, const char *session)
{
   Eina_List *l;
   Elsa_Xsession *xsession;
   char buf[PATH_MAX];
   if (session)
     {
        EINA_LIST_FOREACH(_xsessions, l, xsession)
          {
             if (!strcmp(xsession->name, session))
               {
                  if (xsession->command)
                    return xsession->command;
               }
          }
     }
   snprintf(buf, sizeof(buf), "%s/%s", path, ".Xsession");
   if (ecore_file_can_exec(buf))
     return eina_stringshare_add(buf);
   return (elsa_config->command.session_login);
}

char *
elsa_session_login_get()
{
   return _login;
}

int
elsa_session_logged_get()
{
   return !!_logged;
}

Eina_List *
elsa_session_list_get()
{
   return _xsessions;
}

static void
_elsa_session_init_desktops()
{
   char buf[PATH_MAX];
   Eina_List *dirs;
   const char *path;
   Elsa_Xsession *xsession;
   Eina_List *l;

   xsession = calloc(1, sizeof(Elsa_Xsession));
   xsession->name = eina_stringshare_add("System");
   xsession->icon = eina_stringshare_add("elsa/system");
   _xsessions = eina_list_append(_xsessions, xsession);

   efreet_init();
   efreet_desktop_type_alias(EFREET_DESKTOP_TYPE_APPLICATION, "XSession");
   /* Maybee need to scan other directories ?
    * _elsa_server_scan_desktops("/etc/share/xsessions");
    */
   snprintf(buf, sizeof(buf), "%s/xsessions", efreet_data_home_get());
   _elsa_session_scan_desktops(buf);
   dirs = efreet_data_dirs_get();
   EINA_LIST_FOREACH(dirs, l, path)
     {
        snprintf(buf, sizeof(buf), "%s/xsessions", path);
        _elsa_session_scan_desktops(buf);
     }
   efreet_shutdown();
}

static void
_elsa_session_scan_desktops(const char *dir)
{
   Eina_List *files;
   char *filename;
   char path[PATH_MAX];

   fprintf(stderr, PACKAGE": scanning directory %s\n", dir);
   files = ecore_file_ls(dir);
   EINA_LIST_FREE(files, filename)
     {
        snprintf(path, sizeof(path), "%s/%s", dir, filename);
        _elsa_session_scan_desktops_file(path);
        free(filename);
     }
}

static void
_elsa_session_scan_desktops_file(const char *path)
{
   Efreet_Desktop *desktop;
   Eina_List *commands;
   Eina_List *l;
   Elsa_Xsession *xsession;
   char *command = NULL;

   desktop = efreet_desktop_get(path);
   if (!desktop) return;
   EINA_LIST_FOREACH(_xsessions, l, xsession)
     {
        if (!strcmp(xsession->name, desktop->name))
          {
             efreet_desktop_free(desktop);
             return;
          }
     }

   commands = efreet_desktop_command_local_get(desktop, NULL);
   if (commands)
     command = eina_list_data_get(commands);
   if (command && desktop->name)
     {
        xsession= calloc(1, sizeof(Elsa_Xsession));
        xsession->command = eina_stringshare_add(command);
        xsession->name = eina_stringshare_add(desktop->name);
        if (desktop->icon) xsession->icon = eina_stringshare_add(desktop->icon);
        _xsessions = eina_list_append(_xsessions, xsession);
        fprintf(stderr, PACKAGE": client find sessions %s\n", desktop->name);
     }
   efreet_desktop_free(desktop);
}


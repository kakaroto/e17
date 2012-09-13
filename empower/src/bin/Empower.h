#ifndef EMPOWER_H
#define EMPOWER_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <pwd.h>
#include "Eina.h"
#include "Ecore_X.h"
#include "Elementary.h"
#include "Efreet.h"
#include <locale.h>
#include "config.h"
#define _(x) gettext(x)

typedef struct _Empower_Identity      Empower_Identity;
typedef struct _Empower_Subject       Empower_Subject;
typedef struct _Empower_Auth_Info     Empower_Auth_Info;

typedef enum   _Empower_Identity_Kind Empower_Identity_Kind;
typedef enum   _Empower_Subject_Kind  Empower_Subject_Kind;
typedef enum   _Empower_Auth_State    Empower_Auth_State;

enum _Empower_Subject_Kind
{
  EMPOWER_SUBJECT_PROCESS=0,
  EMPOWER_SUBJECT_SESSION=1,
  EMPOWER_SUBJECT_BUS=2,
  EMPOWER_SUBJECT_COUNT
};

enum _Empower_Identity_Kind
{
  EMPOWER_IDENTITY_USER=0,
  EMPOWER_IDENTITY_GROUP=1,
  EMPOWER_IDENTITY_COUNT
};

struct _Empower_Identity
{
  Empower_Identity_Kind kind;
  union
  {
    struct {
      uint32_t uid;
      const char *name;
    } user;

    struct {
      uint32_t gid;
      const char *name;
    } group;
  } details;
};

struct _Empower_Subject
{
  Empower_Subject_Kind kind;
  union
  {
    struct {
      uint32_t pid;
      uint64_t start_time;
    } process;

    struct {
      const char *id;
    } session;

    struct {
      const char *name;
    } bus;
  } details;
};

struct _Empower_Auth_Info
{
  DBusMessage *msg;
  const char  *action;
  const char  *message;
  const char  *icon;
  const char  *cookie;
  char        *password;
  Eina_List   *identities;
  Eina_Hash   *details;
  struct
  {
    Ecore_Exe           *exe;
    Ecore_Event_Handler *stdout;
    Ecore_Event_Handler *stderr;
  } helper;
};

enum _Empower_Auth_State
{
  INVALID,
  REGISTERING,
  REGISTERED,
  UNREGISTERED
};

int empower_log_domain;
#define DBG(...)    EINA_LOG_DOM_DBG(empower_log_domain, __VA_ARGS__)
#define INF(...)    EINA_LOG_DOM_INFO(empower_log_domain, __VA_ARGS__)
#define WRN(...)    EINA_LOG_DOM_WARN(empower_log_domain, __VA_ARGS__)
#define ERR(...)    EINA_LOG_DOM_ERR(empower_log_domain, __VA_ARGS__)
#define CRI(...)    EINA_LOG_DOM_CRIT(empower_log_domain, __VA_ARGS__)

int empower_uid;
int empower_pid;

#endif

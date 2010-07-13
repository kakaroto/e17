#include <security/pam_appl.h>
#include <stdlib.h>
#include <string.h>
#include "elsa.h"

 /*
  *   ...
  *   pam_start(...);                Initializes the PAM library
  *   ...
  *   if ( ! pam_authenticate(...) ) Autenticates using modules
  *      error_exit();
  *   ...
  *   if ( ! pam-acct_mgmt(...) )    Checks for a valid, unexpired account and
  *                                   verifies access restrictions with "account" modules
  *       error_exit();
  *   ...
  *   pam_setcred(...)               Sets extra credentials, e.g. a Kerberos ticket
  *   ...
  *   pam_open_session(...);         Sets up the session with "session" modules
  *   do_stuff();
  *
  *   pam_close_session(...);        Tear-down session using the "session" modules
  *   pam_end(...);
  *   */

//static int  _elsa_pam_open_session();
static void _elsa_pam_close_session();
static int  _elsa_pam_authenticate();

static struct pam_conv _pam_conversation;
static pam_handle_t* _pam_handle;
static int last_result;



static int
_elsa_pam_conv(int num_msg, const struct pam_message **msg,
     struct pam_response **resp, void *appdata_ptr) {
     int i, result = PAM_SUCCESS;
     *resp = (struct pam_response *) calloc(num_msg, sizeof(struct pam_response));
     for (i = 0; i < num_msg; ++i){
         resp[i]->resp=0;
         resp[i]->resp_retcode=0;
         switch(msg[i]->msg_style){
            case PAM_PROMPT_ECHO_ON:
                 // We assume PAM is asking for the username
                 printf("echo on\n");

		 resp[i]->resp = elsa_gui_user_get();
//                 resp[i]->resp=strdup("root");
                 break;

            case PAM_PROMPT_ECHO_OFF:
                 printf("echo off\n");
                 resp[i]->resp = elsa_gui_password_get();
                 break;
            case PAM_ERROR_MSG:
                 printf("error msg\n");
            case PAM_TEXT_INFO:
                 printf("info %s\n", msg[i]->msg);
                 break;
            case PAM_SUCCESS:
                 printf("success :)\n");
                 break;
            default:
                 printf("default\n");

         }
         if (result != PAM_SUCCESS) break;
     }
     if (result != PAM_SUCCESS) {
         for (i = 0; i < num_msg; ++i){
             if (resp[i]->resp==0) continue;
             free(resp[i]->resp);
             resp[i]->resp=0;
         };
         free(*resp);
         *resp=0;
     }
     return result;
}


int
elsa_pam_open_session() {
   last_result = pam_setcred(_pam_handle, PAM_ESTABLISH_CRED);
   switch (last_result) {
      case PAM_CRED_ERR:
      case PAM_USER_UNKNOWN:
         fprintf(stderr, "PAM user unknow\n");
         elsa_gui_auth_error();
         return 1;
      case PAM_AUTH_ERR:
      case PAM_PERM_DENIED:
         fprintf(stderr, "PAM error on login password\n");
         return 1;
      default:
         fprintf(stderr, "PAM warning unknow error\n");
         return 1;
      case PAM_SUCCESS:
         break;
   }
   return 0;
}

void
elsa_pam_close_session() {
   fprintf(stderr, PACKAGE": PAM close session\n");
   last_result = pam_close_session(_pam_handle, 0);
   switch (last_result) {
      default:
         //case PAM_SESSION_ERROR:
         pam_setcred(_pam_handle, PAM_DELETE_CRED);
         elsa_pam_end();
      case PAM_SUCCESS:
         break;
   };
   last_result = pam_setcred(_pam_handle, PAM_DELETE_CRED);
   switch(last_result) {
      default:
      case PAM_CRED_ERR:
      case PAM_CRED_UNAVAIL:
      case PAM_CRED_EXPIRED:
      case PAM_USER_UNKNOWN:
         elsa_pam_end();
      case PAM_SUCCESS:
         break;
   };
   return;
}

int
elsa_pam_end() {
   int result;
   result = pam_end(_pam_handle, last_result);
   _pam_handle = NULL;
   return result;
}

int
elsa_pam_authenticate() {
   last_result = pam_authenticate(_pam_handle, 0);
   switch (last_result) {
      case PAM_ABORT:
      case PAM_AUTHINFO_UNAVAIL:
         fprintf(stderr, "PAM error !\n");
         elsa_pam_end();
         return 1;
      case PAM_USER_UNKNOWN:
         fprintf(stderr, "PAM user unknow error !\n");
         return 1;
      case PAM_MAXTRIES:
         fprintf(stderr, "PAM max tries error !\n");
         return 1;
      case PAM_CRED_INSUFFICIENT:
         fprintf(stderr, "PAM %s don't have sufficient credential to authenticate !\n",
                 PACKAGE);
         return 1;
      case PAM_AUTH_ERR:
         fprintf(stderr, "PAM authenticate error !\n");
         return 1;
      default:
         fprintf(stderr, "PAM warning unknow error\n");
         return 1;
      case PAM_SUCCESS:
         break;
   }
   return 0;
}

int
elsa_pam_init(const char *service, const char *display) {
   int status;

   if (!service && !*service) goto pam_error;
   if (!display && !*display) goto pam_error;

   _pam_handle = NULL;
   _pam_conversation.conv = _elsa_pam_conv;
   _pam_conversation.appdata_ptr = NULL;


   fprintf(stderr, PACKAGE": Pam init with name %s\n", service);
   if (_pam_handle) elsa_pam_end();
   status = pam_start(service, NULL, &_pam_conversation, &_pam_handle);

   if (status != 0) goto pam_error;
   status = elsa_pam_item_set(ELSA_PAM_ITEM_TTY, display);
   if (status != 0) goto pam_error;
   status = elsa_pam_item_set(ELSA_PAM_ITEM_RUSER, "root");
   if (status != 0) goto pam_error;
   status = elsa_pam_item_set(ELSA_PAM_ITEM_RHOST, "localhost");
   if (status != 0) goto pam_error;
   return 0;

pam_error:
   fprintf(stderr, PACKAGE": PAM error !!!\n");
   return 1;
}

int
elsa_pam_item_set(ELSA_PAM_ITEM_TYPE type, const void *value) {
   last_result = pam_set_item(_pam_handle, type, value);
   if (last_result == PAM_SUCCESS) {
      return 0;
   }
   fprintf(stderr, "PAM error: %d on %d", last_result, type);
   return 1;
}

const void *
elsa_pam_item_get(ELSA_PAM_ITEM_TYPE type) {
   const void *data;
   last_result = pam_get_item(_pam_handle, type, &data);
   switch (last_result) {
      default:
      case PAM_SYSTEM_ERR:
         elsa_pam_end();
         fprintf(stderr, "Elsa: error on pam item get\n");
      case PAM_PERM_DENIED: /* Here data was NULL */
      case PAM_SUCCESS:
         break;
   }
   return data;
}

int
elsa_pam_env_set(const char *env, const char *value) {
   char buf[1024];
   if (!env || !value) return 1;
   snprintf(buf, sizeof(buf), "%s=%s", env, value);
   last_result = pam_putenv(_pam_handle, buf);
   switch (last_result) {
      default:
      case PAM_PERM_DENIED:
      case PAM_ABORT:
      case PAM_BUF_ERR:
         elsa_pam_end();
         return 1;
      case PAM_SUCCESS:
         break;
   };
   return 0;

}

char **
elsa_pam_env_list_get() {
   return pam_getenvlist(_pam_handle);
}

void
elsa_pam_shutdown() {
   fprintf(stderr, "Pam shutdown\n");
}


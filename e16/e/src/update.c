#include "E.h"
#include "timestamp.h"
#include "update.h"

int
CompareNetVersion(int major, int minor, int patchlevel, char *date)
{

   /* This function compares the currently compiled version with the
    * version that exists on the server.  returns a 0 if current, a 1
    * if you need to upgrade
    */

#ifdef AUTOUPGRADE

   if (mode.autoupgrade)
     {
	if (mode.autoupgrade > 1)
	  {
	     if (major > atoi(ENLIGHTENMENT_MAJOR))
		return 1;
	     else if (minor > atoi(ENLIGHTENMENT_MINOR))
		return 1;
	     else if (patchlevel > atoi(ENLIGHTENMENT_MICRO))
		return 1;
	  }
	else
	  {
	     if (strcmp(date, E_CHECKOUT_DATE))
		return 1;
	  }
     }
#else
   major = 0;
   minor = 0;
   patchlevel = 0;
   date = NULL;
#endif
   return 0;
}

int
RetrieveUpdate(int major, int minor, int patchlevel, char *date)
{
   /* This will download the update off the server */
#ifdef AUTOUPGRADE

   if (mode.autoupgrade)
     {
	return 1;

     }
#endif
   major = 0;
   minor = 0;
   patchlevel = 0;
   date = NULL;
   return 0;
}

int
InstallUpdate(void)
{
   /* this will install the update from off the server */
#ifdef AUTOUPGRADE
   if (mode.autoupgrade)
     {

	return 1;
     }
#endif
   return 0;
}

void
SpawnNetworkTester(void)
{

   /* This function forks off a seperate process to test the network
    * connectivity.  tests to see if I've got one running first, though.
    */
#ifdef AUTOUPGRADE
   if (mode.alreadytestingnetwork)
      return;
   mode.alreadytestingnetwork = 1;

#endif
   return;
}

void
TestForUpdate(void)
{

   /* This function will check to see if there is an available upgrade on
    * the server.  Then it will notify the user, and potentially attempt to
    * upgrade itself.
    */

#ifdef AUTOUPGRADE
   if (mode.autoupgrade)
     {
	if (mode.activenetwork)
	  {
	     if (mode.autoupgrade > 1)
	       {

	       }
	     else
	       {

	       }
	  }
	else
	  {
	     SpawnNetworkTester();
	  }
     }
#endif
   return;
}

void
CheckForNewMOTD(int val, void *data)
{

   /* This function will check to see if there is a new MOTD on the server,
    * and if there is, display it to the user
    */

   val = 0;
   data = NULL;
#ifdef AUTOUPGRADE
   if (mode.motd)
     {
	if (mode.activenetwork)
	  {
	     if (GetNetFileDate(MOTDFILE) > mode.motddate)
	       {
		  char               *MOTD = GetNetText(MOTDFILE);

		  if (MOTD)
		    {
		       DIALOG_OK("Enlightenment's message of the day", MOTD);
		       Efree(MOTD);
		    }
	       }
	     DoIn("MOTD_CHECK", 3600.0, CheckForNewMOTD, 0, NULL);
	  }
	else
	  {
	     SpawnNetworkTester();
	  }
     }
#endif
   return;
}

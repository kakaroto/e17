
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "note.h"

extern MainConfig *main_config;

/**
 * @param msg: The error/warning message to display.
 * @param level: The level at which the error message should be displayed.
 *               0 = No Debugging Messages, 1 = Core Stages
 *               2 = Event/Callback and Core Stages,
 *               3 = Error Message, 4 = Warning Message,
 *               5 = Notice Message
 * @brief: Display a warning/error message to the console.
 * @note: Levels 3, 4 and 5 aren't arbituary to the debugging level because
 *        they are part of the application, not its workings.
 */
void
debug_msg_lvl(char *msg, int level)
{
	if (main_config == NULL)
		return;

	if (level == 3) {
		msgbox("Error!", msg);
		if (main_config->debug != 0)
			printf(DEBUG_ERROR_MSG, msg);
		return;
	} else if (level == 4) {
		msgbox("Warning!", msg);
		if (main_config->debug != 0)
			printf(DEBUG_WARNING_MSG, msg);
		return;
	} else if (level == 5) {
		msgbox("Notice!", msg);
		if (main_config->debug != 0)
			printf(DEBUG_NOTICE_MSG, msg);
		return;
	}

	if (main_config->debug >= level) {
		printf(DEBUG_MSG, msg);
	}
	return;
}

/**
 * @param msg: The error/warning message to display.
 * @brief: Display a debugging message to the console.
 */
void
debug_msg(char *msg)
{
	if (main_config == NULL)
		return;

	if (main_config->debug > 0)
		printf(DEBUG_MSG, msg);
	return;
}

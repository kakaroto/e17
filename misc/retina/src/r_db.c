/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

extern E_DB_File *db;

void
r_db_init()
{
	char db_file[255];
	sprintf(db_file, "%s/.retina.db", getenv("HOME"));
	
	db = e_db_open(db_file);
	e_db_close(db);
}

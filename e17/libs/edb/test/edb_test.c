#include <stdio.h>
#include <stdlib.h>
#include <Edb.h>

int
main(int argc, char **argv)
{
   E_DB_File *db;
   
   db = e_db_open("test.db");
   if (db)
     {
	int i;
	char data[8192], *data2;
	
	for (i = 0; i < 100000; i++)
	  {
	     int size;
	     
	     e_db_data_set(db, "data", data, 8192);
	     data2 = e_db_data_get(db, "data", &size);
	     if (data2) free(data2);
	     e_db_data_del(db, "data");
	  }
	e_db_close(db);
     }
   else
      printf("ERROR! cannot open ./test.db\n");
   e_db_flush();
   return 0;
   argc = 0;
   argv = NULL;
}

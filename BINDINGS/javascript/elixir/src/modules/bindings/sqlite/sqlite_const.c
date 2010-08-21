#include <sqlite3.h>

#include "Elixir.h"

static const struct {
   const char*  name;
   int          value;
} sqlite_const_properties[] = {
   { "SQLITE_IOERR_READ", SQLITE_IOERR_READ },
   { "SQLITE_IOERR_SHORT_READ", SQLITE_IOERR_SHORT_READ },
   { "SQLITE_IOERR_WRITE", SQLITE_IOERR_WRITE },
   { "SQLITE_IOERR_FSYNC", SQLITE_IOERR_FSYNC },
   { "SQLITE_IOERR_DIR_FSYNC", SQLITE_IOERR_DIR_FSYNC },
   { "SQLITE_IOERR_TRUNCATE", SQLITE_IOERR_TRUNCATE },
   { "SQLITE_IOERR_FSTAT", SQLITE_IOERR_FSTAT },
   { "SQLITE_IOERR_UNLOCK", SQLITE_IOERR_UNLOCK },
   { "SQLITE_IOERR_RDLOCK", SQLITE_IOERR_RDLOCK },
   { "SQLITE_IOERR_DELETE", SQLITE_IOERR_DELETE },
   { "SQLITE_IOERR_BLOCKED", SQLITE_IOERR_BLOCKED },
   { "SQLITE_IOERR_NOMEM", SQLITE_IOERR_NOMEM },
/*    { "SQLITE_IOERR_ACCESS", SQLITE_IOERR_ACCESS }, */
/*    { "SQLITE_IOERR_CHECKRESERVEDLOCK", SQLITE_IOERR_CHECKRESERVEDLOCK }, */
/*    { "SQLITE_IOERR_LOCK", SQLITE_IOERR_LOCK }, */

   { "SQLITE_OK", SQLITE_OK },                  /* Successful result */
   { "SQLITE_ERROR", SQLITE_ERROR },            /* SQL error or missing database */
   { "SQLITE_INTERNAL", SQLITE_INTERNAL },      /* An internal logic error in SQLite */
   { "SQLITE_PERM", SQLITE_PERM },              /* Access permission denied */
   { "SQLITE_ABORT", SQLITE_ABORT },            /* Callback routine requested an abort */
   { "SQLITE_BUSY", SQLITE_BUSY },              /* The database file is locked */
   { "SQLITE_LOCKED", SQLITE_LOCKED },          /* A table in the database is locked */
   { "SQLITE_NOMEM", SQLITE_NOMEM },            /* A malloc() failed */
   { "SQLITE_READONLY", SQLITE_READONLY },      /* Attempt to write a readonly database */
   { "SQLITE_INTERRUPT", SQLITE_INTERRUPT },    /* Operation terminated by sqlite_interrupt() */
   { "SQLITE_IOERR", SQLITE_IOERR },            /* Some kind of disk I/O error occurred */
   { "SQLITE_CORRUPT", SQLITE_CORRUPT },        /* The database disk image is malformed */
   { "SQLITE_NOTFOUND", SQLITE_NOTFOUND },      /* (Internal Only) Table or record not found */
   { "SQLITE_FULL", SQLITE_FULL },              /* Insertion failed because database is full */
   { "SQLITE_CANTOPEN", SQLITE_CANTOPEN },      /* Unable to open the database file */
   { "SQLITE_PROTOCOL", SQLITE_PROTOCOL },      /* Database lock protocol error */
   { "SQLITE_EMPTY", SQLITE_EMPTY },            /* (Internal Only) Database table is empty */
   { "SQLITE_SCHEMA", SQLITE_SCHEMA },          /* The database schema changed */
   { "SQLITE_TOOBIG", SQLITE_TOOBIG },          /* Too much data for one row of a table */
   { "SQLITE_CONSTRAINT", SQLITE_CONSTRAINT },  /* Abort due to constraint violation */
   { "SQLITE_MISMATCH", SQLITE_MISMATCH },      /* Data type mismatch */
   { "SQLITE_MISUSE", SQLITE_MISUSE },          /* Library used incorrectly */
   { "SQLITE_NOLFS", SQLITE_NOLFS },            /* Uses OS features not supported on host */
   { "SQLITE_AUTH", SQLITE_AUTH },              /* Authorization denied */
   { "SQLITE_FORMAT", SQLITE_FORMAT },		/* Auxiliary database format error */
   { "SQLITE_RANGE", SQLITE_RANGE },		/* 2nd parameter to sqlite3_bind out of range */
   { "SQLITE_NOTADB", SQLITE_NOTADB },		/* File opened that is not a database file */
   { "SQLITE_ROW", SQLITE_ROW },                /* sqlite_step() has another row ready */
   { "SQLITE_DONE", SQLITE_DONE },              /* sqlite_step() has finished executing */

   { "SQLITE_INTEGER", SQLITE_INTEGER },
   { "SQLITE_FLOAT", SQLITE_FLOAT },
   { "SQLITE_TEXT", SQLITE_TEXT },
   { "SQLITE_BLOB", SQLITE_BLOB },
   { "SQLITE_NULL", SQLITE_NULL },

   { "SQLITE_CREATE_INDEX", SQLITE_CREATE_INDEX },              /* Index Name      Table Name      */
   { "SQLITE_CREATE_TABLE", SQLITE_CREATE_TABLE },              /* Table Name      NULL            */
   { "SQLITE_CREATE_TEMP_INDEX", SQLITE_CREATE_TEMP_INDEX },    /* Index Name      Table Name      */
   { "SQLITE_CREATE_TEMP_TABLE", SQLITE_CREATE_TEMP_TABLE },    /* Table Name      NULL            */
   { "SQLITE_CREATE_TEMP_TRIGGER", SQLITE_CREATE_TEMP_TRIGGER },/* Trigger Name    Table Name      */
   { "SQLITE_CREATE_TEMP_VIEW", SQLITE_CREATE_TEMP_VIEW },      /* View Name       NULL            */
   { "SQLITE_CREATE_TRIGGER", SQLITE_CREATE_TRIGGER },          /* Trigger Name    Table Name      */
   { "SQLITE_CREATE_VIEW", SQLITE_CREATE_VIEW },                /* View Name       NULL            */
   { "SQLITE_DELETE", SQLITE_DELETE },                          /* Table Name      NULL            */
   { "SQLITE_DROP_INDEX", SQLITE_DROP_INDEX },                  /* Index Name      Table Name      */
   { "SQLITE_DROP_TABLE", SQLITE_DROP_TABLE },                  /* Table Name      NULL            */
   { "SQLITE_DROP_TEMP_INDEX", SQLITE_DROP_TEMP_INDEX },        /* Index Name      Table Name      */
   { "SQLITE_DROP_TEMP_TABLE", SQLITE_DROP_TEMP_TABLE },        /* Table Name      NULL            */
   { "SQLITE_DROP_TEMP_TRIGGER", SQLITE_DROP_TEMP_TRIGGER },    /* Trigger Name    Table Name      */
   { "SQLITE_DROP_TEMP_VIEW", SQLITE_DROP_TEMP_VIEW },          /* View Name       NULL            */
   { "SQLITE_DROP_TRIGGER", SQLITE_DROP_TRIGGER },              /* Trigger Name    Table Name      */
   { "SQLITE_DROP_VIEW", SQLITE_DROP_VIEW },                    /* View Name       NULL            */
   { "SQLITE_INSERT", SQLITE_INSERT },                          /* Table Name      NULL            */
   { "SQLITE_PRAGMA", SQLITE_PRAGMA },                          /* Pragma Name     1st arg or NULL */
   { "SQLITE_READ", SQLITE_READ },                              /* Table Name      Column Name     */
   { "SQLITE_SELECT", SQLITE_SELECT },                          /* NULL            NULL            */
   { "SQLITE_TRANSACTION", SQLITE_TRANSACTION },                /* NULL            NULL            */
   { "SQLITE_UPDATE", SQLITE_UPDATE },                          /* Table Name      Column Name     */
   { "SQLITE_ATTACH", SQLITE_ATTACH },                          /* Filename        NULL            */
   { "SQLITE_DETACH", SQLITE_DETACH },                          /* Database Name   NULL            */
   { "SQLITE_ALTER_TABLE", SQLITE_ALTER_TABLE },                /* Database Name   Table Name      */
   { "SQLITE_REINDEX", SQLITE_REINDEX },                        /* Index Name      NULL            */
   { "SQLITE_ANALYZE", SQLITE_ANALYZE },                        /* Table Name      NULL            */
   { "SQLITE_CREATE_VTABLE", SQLITE_CREATE_VTABLE },            /* Table Name      Module Name     */
   { "SQLITE_DROP_VTABLE", SQLITE_DROP_VTABLE },                /* Table Name      Module Name     */
   { "SQLITE_FUNCTION", SQLITE_FUNCTION },                      /* Function Name   NULL            */

   { "SQLITE_DENY", SQLITE_DENY },                              /* Abort the SQL statement with an error */
   { "SQLITE_IGNORE", SQLITE_IGNORE },                          /* Don't allow access, but don't generate an error */

   /* Will need a special case in code using this constant */
   { "SQLITE_STATIC", 0 },                      /* need to match ((void(*)(void *))0) */
   { "SQLITE_TRANSIENT", -1 },                  /* need to match ((void(*)(void *))-1) */

   { "SQLITE_OPEN_READONLY", SQLITE_OPEN_READONLY },
   { "SQLITE_OPEN_READWRITE", SQLITE_OPEN_READWRITE },
   { "SQLITE_OPEN_CREATE", SQLITE_OPEN_CREATE },
   { "SQLITE_OPEN_DELETEONCLOSE", SQLITE_OPEN_DELETEONCLOSE },
   { "SQLITE_OPEN_EXCLUSIVE", SQLITE_OPEN_EXCLUSIVE },
   { "SQLITE_OPEN_MAIN_DB", SQLITE_OPEN_MAIN_DB },
   { "SQLITE_OPEN_TEMP_DB", SQLITE_OPEN_TEMP_DB },
   { "SQLITE_OPEN_TRANSIENT_DB", SQLITE_OPEN_TRANSIENT_DB },
   { "SQLITE_OPEN_MAIN_JOURNAL", SQLITE_OPEN_MAIN_JOURNAL },
   { "SQLITE_OPEN_TEMP_JOURNAL", SQLITE_OPEN_TEMP_JOURNAL },
   { "SQLITE_OPEN_SUBJOURNAL", SQLITE_OPEN_SUBJOURNAL },
   { "SQLITE_OPEN_MASTER_JOURNAL", SQLITE_OPEN_MASTER_JOURNAL },
/*    { "SQLITE_OPEN_NOMUTEX", SQLITE_OPEN_NOMUTEX }, */
/*    { "SQLITE_OPEN_FULLMUTEX", SQLITE_OPEN_FULLMUTEX }, */

   { NULL, 0 }
};

Eina_Bool
load_sqlite_const_binding(JSContext *cx, JSObject *parent)
{
   unsigned int	i = 0;
   jsval property;

   while (sqlite_const_properties[i].name)
     {
	property = INT_TO_JSVAL(sqlite_const_properties[i].value);
	if (!JS_DefineProperty(cx, parent,
			       sqlite_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
	  return EINA_FALSE;
	++i;
     }
   return EINA_TRUE;
}

Eina_Bool
unload_sqlite_const_binding(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (sqlite_const_properties[i].name)
     JS_DeleteProperty(cx, parent, sqlite_const_properties[i++].name);

   return EINA_TRUE;
}


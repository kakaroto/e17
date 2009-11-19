// Sample SQLite3 javascript.
// More at: http://freshmeat.net/articles/view/1428/

var test = elx.load("sqlite");

elx.print("sqlite: ", test, "\n");

var NULL = null;

var db = sqlite3_open("test.db");
elx.print("db: ", db, "\n");

test = sqlite3_exec(db, "create table t1 (t1key INTEGER PRIMARY KEY,data TEXT,num double,timeEnter DATE);", NULL, NULL);
elx.print("create: ", test, "\n");

test = sqlite3_exec(db, "CREATE TRIGGER insert_t1_timeEnter AFTER  INSERT ON t1 \
 BEGIN \
      UPDATE t1 SET timeEnter = DATETIME('NOW')  WHERE rowid = new.rowid; \
 END;", NULL, NULL);

test = sqlite3_exec(db, "insert into t1 (data,num) values ('This is sample data',3);", NULL, NULL);
test = sqlite3_exec(db, "insert into t1 (data,num) values ('More sample data',6);", NULL, NULL);
test = sqlite3_exec(db, "insert into t1 (data,num) values ('And a little more',9);", NULL, NULL);

function exec_display(cdata, row)
{
   for (var i in row)
     elx.print("{ .", i, "= [", row[i], "]} ");
   elx.print("\n");
   return 0;
}

elx.print("****\n");
sqlite3_exec(db, "select * from t1 limit 2;", exec_display, NULL);
elx.print("****\n");
sqlite3_exec(db, "select * from t1 order by t1key limit 1 offset 2;", exec_display, NULL);
elx.print("****\n");
sqlite3_exec(db, "select * from sqlite_master;", exec_display, NULL);
elx.print("****\n");

sqlite3_close(db);


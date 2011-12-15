#include "server_code.h"

static void _callback(Esql_Res *res, void *data)
{
	Test_Data *d = data;
	Azy_Value *v;
	Azy_Content *content;

	printf("Message saved!\n");


	//disconnect from the mysql db
	esql_disconnect(d->e);

	//set the return value of the azy call
	//we return a copy of the message sent by the client
	v = Demo_Message_to_azy_value(d->msg);
   	content = azy_server_module_content_get(d->m);
   	azy_content_retval_set(content, v);
	//

        //resume the azy call instance
	azy_server_module_events_resume(d->m, EINA_TRUE);

	//free our data
	free(d);
}

void _connect(Esql *e, void *data)
{
	Test_Data *d = data;
	Esql_Query_Id id;

	//execute the query
	//the second argument is a data you can set and then retrieve it in _callabck()
	id = esql_query_args(e, d, "INSERT INTO messages values('%s');", d->msg->msg);
	if (!id) /**< queue up a simple query */
	{
		fprintf(stderr, "Could not create query!\n");
		ecore_main_loop_quit();
	}
	esql_query_callback_set(id, _callback);
}


/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
#include "Extrackt.h"

/* FIXME for now just load the etk fe in this way */
Extrackt_Frontend *etk_fe_get(void);

/* initialize extrackt */
Extrackt * 
extrackt_init(void)
{
   Extrackt *ex;
   /* TEMPORAL */
   Ex_Cddb_Hello *hello;
   /* EOTEMPORAL */
     
   ex = E_NEW(1, Extrackt);
  
   if(!ex)
     {
	ERROR("cant allocate memory");
	extrackt_shutdown(ex);
	exit(-1);
     }
   
   if(!ex_config_init())
     {
	ERROR(_("Cant initialize config subsystem"));
	extrackt_shutdown(ex);
	exit(-1);
     }     
	   
   if( pipe(ex->pfd) < 0 )
     {
	ERROR("cant create the pipe");
	extrackt_shutdown(ex);	
	exit(-2);
     }
   
   /* read the stored configuration */
   ex_config_read(ex);
   
   /* init the device */
   if(!ex_cddev_init(ex->config.cd->cdrom, ex))
     ERROR("cant init the cdrom device");
   ex->disc_id = 0;
   ex->cddb.pid = 0;
   ex->cddb.status = EX_STATUS_NOT_DONE;
   ex->rip.tracks = NULL;
   ex->rip.num_total = 0;
   ex->rip.num_done = 0;
   ex->rip.status = EX_STATUS_NOT_DONE;
   ex->encode.tracks = NULL;
   ex->encode.num_total = 0;
   ex->encode.num_done = 0;
   ex->encode.num_queue = 0;
   ex->encode.status = EX_STATUS_NOT_DONE;

      
   /* TEMP */
   /* for now take the first on the list as the encoder/ripper */
   ex->rip.ripper = ex->config.rippers->data;
   ex->encode.encoder = ex->config.encode->encoders->data;
   
   hello = E_NEW(1, Ex_Cddb_Hello);
   snprintf(hello->hello_program, sizeof(hello->hello_program), "Extrackt");
   snprintf(hello->hello_version, sizeof(hello->hello_version), "%s", "0.0.1");
   hello->proto_version=6;
   ex->cddb.hello = hello;
   /* END TEMP */
   
   ex_cddev_update(ex);
   /* stat the disc */
   /* read the toc */
   /* lookup the database */
#if 0   
    if(ex_cdddev_stat(ex))
    if(ex_cddev_read_toc(ex))
    ex_cddev_lookup(ex);
#endif
  /* FIXME for now just load the etk fe in this way */
   ex->fe = etk_fe_get();
   
   return ex;
}

void 
extrackt_shutdown(Extrackt *ex)
{
   if(!ex)
     return;
   
   ex_config_write(ex);
   ex_config_shutdown();
   if(ex->cddb.pid)
     kill(ex->cddb.pid,SIGKILL);
   /* for the rip pid is different, it might have another child
    * forked, so send it a term and it will kill also his child */
   ex_command_rip_abort(ex);
   ex_command_encode_abort(ex);
   free(ex);
}

void
extrackt_parse(Extrackt *ext, char **options)
{

}

int 
main(int argc, char **argv)
{	
   Extrackt *ex;
   
   /* init extrackt */
   ex = extrackt_init();
   
   /* parse the arguments */
   extrackt_parse(ex,argv);
   
   /* parse the front end arguments */
   /* check for the selected frontend */
   /* init the frontend */
   ex->fe->init(ex);
   
   /* run the frontend main loop */
   ex->fe->main(ex->fe->data);
   
   /* shutdown the frontend */
   ex->fe->shutdown(ex->fe->data);
   
   /* shutdown the app */
   extrackt_shutdown(ex);

   return 0;
}

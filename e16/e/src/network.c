
/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

#ifdef AUTOUPGRADE
#include <ghttp.h>

/* a little struct to make our lives easier by grouping all this
 * http stuff in the same place
 */

typedef struct conn_data_tag {
   int                 read_sock;
   int                 write_sock;
   ghttp_request      *req;
} conn_data;

#endif

char               *
GetNetText(char *URL)
{

#ifdef AUTOUPGRADE

   /* This function will retreive a text file off the net
    * and return a (char *) to the appropriate text.
    * Useful for retreiving config information and/or
    * text messages off the net - such as the MOTD or
    * the current version of E
    */

   conn_data           l_conn_data;
   ghttp_status        l_status;
   ghttp_current_status l_cur_stat;
   char               *Data;

   if (!URL)
      return NULL;
   l_conn_data.req = ghttp_request_new();
   if (ghttp_set_uri(l_conn_data.req, URL) < 0)
     {

        /* invalid URI */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return NULL;
     }

   if (ghttp_prepare(l_conn_data.req) < 0)
     {

        /* failed prep */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return NULL;
     }

   /* set up some defaults */

   ghttp_set_sync(l_conn_data.req, ghttp_async);
   ghttp_set_chunksize(l_conn_data.req, 100);

   if (ghttp_process(l_conn_data.req) < 0)
     {

        /* failed to process request */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return NULL;
     }

   while ((l_status = ghttp_process(l_conn_data.req)) != ghttp_done)
     {
        l_cur_stat = ghttp_get_status(l_conn_data.req);
        if (l_status == ghttp_error)
          {
             /* there was an error somewhere in here */
             if (l_conn_data.req)
                ghttp_request_destroy(l_conn_data.req);

             return NULL;
          }
     }

   Data = duplicate(ghttp_get_body(l_conn_data.req));

   if (l_conn_data.req)
      ghttp_request_destroy(l_conn_data.req);

   if (Data)
      return Data;
#else
   URL = NULL;
#endif

   return NULL;

}

time_t
GetNetFileDate(char *URL)
{

#ifdef AUTOUPGRADE

   /* This function will return the date of a file on the
    * network, for cache comparison purposes (do I
    * have to download this file?)
    */

   conn_data           l_conn_data;
   ghttp_status        l_status;
   ghttp_current_status l_cur_stat;
   time_t              ModifyDate = 0;

   if (!URL)
      return 0;
   l_conn_data.req = ghttp_request_new();
   if (ghttp_set_uri(l_conn_data.req, URL) < 0)
     {

        /* invalid URI */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   if (ghttp_prepare(l_conn_data.req) < 0)
     {

        /* failed prep */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   /* set up some defaults */

   ghttp_set_sync(l_conn_data.req, ghttp_async);
   ghttp_set_chunksize(l_conn_data.req, 100);
   ghttp_set_type(l_conn_data.req, ghttp_type_head);

   if (ghttp_process(l_conn_data.req) < 0)
     {

        /* failed to process request */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   while ((l_status = ghttp_process(l_conn_data.req)) != ghttp_done)
     {
        l_cur_stat = ghttp_get_status(l_conn_data.req);
        if (l_status == ghttp_error)
          {
             /* there was an error somewhere in here */
             if (l_conn_data.req)
                ghttp_request_destroy(l_conn_data.req);

             return 0;
          }
     }

   /* somehow right here we have to get the date out of the header.
    * FIXME!!!!!!!!!!!!!!!
    */

   /* ModifyDate = ghttp_parse_date(ghttp_get_header(l_conn_data.req, http_hdr_LastModified)); */
   if (l_conn_data.req)
      ghttp_request_destroy(l_conn_data.req);

   if (ModifyDate)
      return ModifyDate;
#else
   URL = NULL;
#endif

   return 0;

}

int
SaveNetFile(char *URL, char *pathtosave)
{

#ifdef AUTOUPGRADE

   /* this function will download a file and save it to a local
    * location from the network.  this is for things like
    * graphic files, etc, which we will need to be able to load
    * again at another date.  Also this will allow us to keep
    * a local file cached copy of things we get off the net.
    * Used with GetNetFileDate to coordinate caching system
    */

   conn_data           l_conn_data;
   ghttp_status        l_status;
   ghttp_current_status l_cur_stat;

   /*char               *Data; */

   if (!URL)
      return 0;
   if (!pathtosave)
      return 0;
   l_conn_data.req = ghttp_request_new();
   if (ghttp_set_uri(l_conn_data.req, URL) < 0)
     {

        /* invalid URI */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   if (ghttp_prepare(l_conn_data.req) < 0)
     {

        /* failed prep */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   /* set up some defaults */

   ghttp_set_sync(l_conn_data.req, ghttp_async);
   ghttp_set_chunksize(l_conn_data.req, 100);

   if (ghttp_process(l_conn_data.req) < 0)
     {

        /* failed to process request */

        if (l_conn_data.req)
           ghttp_request_destroy(l_conn_data.req);

        return 0;
     }

   while ((l_status = ghttp_process(l_conn_data.req)) != ghttp_done)
     {
        l_cur_stat = ghttp_get_status(l_conn_data.req);
        if (l_status == ghttp_error)
          {
             /* there was an error somewhere in here */
             if (l_conn_data.req)
                ghttp_request_destroy(l_conn_data.req);

             return 0;
          }
     }

   if (l_conn_data.req)
      ghttp_request_destroy(l_conn_data.req);

   return 1;

#else
   URL = NULL;
   pathtosave = NULL;
#endif

   return 0;

}

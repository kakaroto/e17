/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include "eupnp_suite.h"

#include "eupnp_device_info.h"
#include "eupnp_device_parser.h"

#ifdef INFO
#undef INFO
#define INFO(...) printf(__VA_ARGS__)

static long
get_file_size(FILE *f)
{
   long size;
   fseek(f, 0, SEEK_END);
   size = ftell(f);
   rewind(f);
   return size;
}

START_TEST(eupnp_xml_parse_complete)
{
   eupnp_init();

   INFO("Test: complete xml parsing");
   FILE *xml;
   long size;
   char *buffer;
   Eupnp_Device_Info *device_info;
   const char *usn = "uuid:5dab3131-e636-4a12-912e-9646be962f18::"\
		     "urn:schemas-upnp-org:device:MediaServer:1";

   fail_if(!(xml = fopen("src/tests/sample.xml", "r")));
   size = get_file_size(xml);
   buffer = malloc(sizeof(char)*(size + 1));
   fail_if((fread(buffer, size, 1, xml) != 1));
   buffer[size] = '\0';
   fclose(xml);

   device_info = eupnp_device_info_new(usn, NULL, NULL, NULL);

   fail_if(!device_info);
   fail_if(!eupnp_device_parse_xml_buffer(buffer, size, device_info));

   eupnp_device_info_dump(device_info);

   free(buffer);
   eupnp_device_info_unref(device_info);
   eupnp_shutdown();
}
END_TEST


#define BLOCK_READ 100

START_TEST(eupnp_xml_parse_partial)
{
   eupnp_init();
   INFO("Incremental XML parsing");

   Eupnp_Device_Info *device_info;
   FILE *xml;
   long fixed;
   long size;
   int walk = 0;
   char *buffer;
   const char *usn = "uuid:5dab3131-e636-4a12-912e-9646be962f18::"\
		     "urn:schemas-upnp-org:device:MediaServer:1";
   char *tmp;

   device_info = eupnp_device_info_new(usn, NULL, NULL, NULL);

   fail_if(!device_info);
   fail_if(!(xml = fopen("src/tests/sample.xml", "r")));
   fixed = size = get_file_size(xml);
   buffer = malloc(sizeof(char)*(size+1));
   tmp = malloc(sizeof(char)*(BLOCK_READ + 1));
   fail_if((fread(buffer, size, 1, xml) != 1));
   fclose(xml);
   buffer[size] = '\0';

   while (1)
     {
	size = strlen(buffer+walk);
	size = (size > BLOCK_READ) ? BLOCK_READ : size;
	strncpy(tmp, buffer+walk, size);
	tmp[size] = '\0';
	INFO("Feeding with %ld chars: %s", size, tmp);
	fail_if(!eupnp_device_parse_xml_buffer(tmp, size, device_info));
	walk += BLOCK_READ;

	if (walk > fixed)
	  {
		INFO("Stopping feed");
		break;
	  }
     }

   eupnp_device_info_dump(device_info);
   free(buffer);
   free(tmp);
   eupnp_device_info_unref(device_info);
   eupnp_shutdown();
}
END_TEST

void
eupnp_test_device_parser(TCase *t)
{
   tcase_add_test(t, eupnp_xml_parse_complete);
   tcase_add_test(t, eupnp_xml_parse_partial);
}

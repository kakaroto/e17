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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include "eupnp_suite.h"

static const Eupnp_Test_Case cases[] = {
   { "AV DIDL", eupnp_test_av_didl_parser},
   { NULL, NULL }
};

Suite *
eupnp_build_suite(void)
{
   TCase *t;
   Suite *s;
   int i;

   s = suite_create("Eupnp");

   for (i = 0; cases[i].name; ++i)
     {
	t = tcase_create(cases[i].name);
	cases[i].build(t);
	suite_add_tcase(s, t);
     }

   return s;
}

int
main(void)
{
   Suite *s;
   SRunner *sr;
   int failed_count;

   s = eupnp_build_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   failed_count = srunner_ntests_failed(sr);
   srunner_free(sr);

   return (failed_count == 0) ? 0 : 255;
}

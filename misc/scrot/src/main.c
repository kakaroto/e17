/* main.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "scrot.h"
#include "options.h"

int call_level = 0;

int
main(int argc, char **argv)
{
   Imlib_Image image;
   Imlib_Load_Error err;
   D_ENTER(4);

   init_parse_options(argc, argv);

   init_x_and_imlib();

   if(!opt.output_file)
      show_mini_usage();

   image = scrot_grab_shot();

   imlib_context_set_image(image);
   imlib_save_image_with_error_return(opt.output_file, &err);
   if(err)
      eprintf("Saving to file %s failed\n", opt.output_file);
   if(opt.exec)
      scrot_exec_app();
   
   D_RETURN(4,0);
}

Imlib_Image scrot_grab_shot(void)
{
   Imlib_Image im;
   D_ENTER(3);

   imlib_context_set_drawable(root);
   im = imlib_create_image_from_drawable(0, 0,0,scr->width, scr->height, 1);

   D_RETURN(3, im);
}

void scrot_exec_app(void)
{
   D_ENTER(3);

   execlp(opt.exec, opt.exec, opt.output_file, NULL);
   eprintf("exec of %s failed:", opt.exec);
   D_RETURN_(3);
}

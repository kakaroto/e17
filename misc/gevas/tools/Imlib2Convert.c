/*

<raster> use that program
<raster> it canconvert any imae to any other image format
<raster> so to import an image intoa  db:
<raster> imlib2_conv original_file.png db database_file.db:/key/in/database


*/

#include <X11/Xlib.h>
#include <Imlib2.h>
#include <string.h>
int main(int argc, char **argv)
{
   Imlib_Image im;
   im = imlib_load_image(argv[1]);
   if (im)
     {
        imlib_context_set_image(im);
        imlib_image_set_format(argv[2]);
        imlib_save_image(argv[3]);
     }
}

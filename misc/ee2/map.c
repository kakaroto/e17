#include "ee2.h"

/*---------------------------------------------------------------------------*/
/* GUI callback - mapping - the image must be square at this point - Nolan*/

void 
sphere_map_image(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  DATA32 *image_buffer, *map_buffer;
  int image_width, image_height;
  int x, y, mapx, mapy;
  mapped_pixels *calc_return;
	
  
  if (im) 
    {
      imlib_context_set_image(im);
      image_width = imlib_image_get_width();
      image_height = imlib_image_get_height();
      
      /* This condition is only temporary - ellipsoid mapping is next. */
      if (image_height == image_width)
	{
	  map_buffer = malloc(image_width * image_height * sizeof(DATA32));
	  image_buffer = imlib_image_get_data(); 
	  memcpy(map_buffer, image_buffer, 
		 image_width * image_height * sizeof(DATA32));
	  
	  for (y = 0; y < image_height; y++) 
	    for (x = 0; x < image_width; x++)
	      {
		calc_return = calc_mapping(image_width, image_height, x, y);
		mapx = calc_return->mappedx;
		mapy = calc_return->mappedy;
		if (x>=0 && y>=0)
		  {
		    *(image_buffer + ((y * image_height) + x)) = 
		      *(map_buffer + ((mapy * image_height) + mapx));
		  }
		else
		  *(image_buffer + ((y * image_height) + x)) = 0;
	      }
	  imlib_image_put_back_data(image_buffer);
	  DrawImage(im, image_height, image_width);
	}
    }
}
/*---------------------------------------------------------------------------*/
    
/*---------------------------------------------------------------------------*/
/* The start of a mapping calculation for ee2 - Nolan */

mapped_pixels 
*calc_mapping(int im_width, int im_height, int x, int y)
{
  double z, mapx, mapy, xarc, yarc, Pi;
  double xtrav, ytrav, xrem, yrem, radius;
  mapped_pixels *mapped_return;
	

  Pi = 3.1415926;
  radius = im_width / 2;
  mapped_return = malloc(sizeof(mapped_pixels));
  x -= radius;
  y -= radius;
  
  if ((sqrt(pow(x, 2) + pow(y, 2))) < radius)
    {
      z = fabs(sqrt(pow(radius, 2) - pow(x, 2) - pow(y, 2)));
      
      xarc = fabs(radius * (atan(z / x)));
      yarc = fabs(radius * (atan(z / y)));
      xtrav = im_width * (((radius * Pi) - xarc) / (radius * Pi));
      ytrav = im_height * (((radius * Pi) - yarc) / (radius * Pi));

      if ((xrem = modf(xtrav, &mapx)) > 0.5)
	mapx++;
      if ((yrem = modf(ytrav, &mapy)) > 0.5)
	mapy++;

      mapx = fabs(mapx - im_width);
      if (x > 0)
	mapx = ((im_width/2) + ((im_width/2) - mapx));
      mapy = fabs(mapy - im_height);
      if (y > 0)
	mapy = ((im_height/2) + ((im_height/2) - mapy));
     
      mapped_return->mappedx = mapx;
      mapped_return->mappedy = mapy;
    }
  else
    {
      x += radius;
      y += radius;
      mapped_return->mappedx = -1; 
      mapped_return->mappedy = -1;
    }
  return(mapped_return);
}
/*---------------------------------------------------------------------------*/

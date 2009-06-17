#!/usr/bin/perl
use Evas;
use Imlib2;


make_image();
make_evas();
render_and_save();



sub make_image
{
	$image = Imlib2::create_image(250,250); #create our image
	Imlib2::set_context($image);
	Imlib2::set_format("png");
}

sub make_evas
{

	$evas = Evas::new();
	Evas::set_output_method($evas,4); #set to image render method
	Evas::set_output_image($evas,$image); #output to the image we created
	Evas::set_output_size($evas,250,250); #set evas size
	Evas::set_output_viewport($evas,0.0,0.0,250.0,250.0); #evas viewport
	Evas::font_add_path($evas,"./fonts"); #lookup fonts in this dir
	$o3 = Evas::add_rectangle($evas); # add a rectangle
	Evas::set_color($evas,$o3,255,255,255,255); #fill it with white
	Evas::set_layer($evas,$o3,0); #set as lowest layer
	Evas::show($evas,$o3); #show it
	Evas::move($evas, $o3, 0.0, 0.0); #position it
	Evas::resize($evas, $o3, 250.0, 250.0); #resize it
	$o = Evas::add_image_from_file($evas,"./images/mush.png"); #add an image_layer
	Evas::set_layer($evas,$o,100); #layer it atop the rectangle
	Evas::show($evas,$o); #show it
	$o2 = Evas::add_text($evas,"notepad",20,"Raster Eats This!"); #add a text_layer
	Evas::set_layer($evas,$o2,200); #set its layer 
	Evas::set_color($evas,$o2,0,0,0,160); #color it
	Evas::move($evas,$o2,0.0,60.0); #position it
	Evas::show($evas,$o2); #show it
	$o3 = Evas::add_image_from_file($evas,"./images/cheese.png"); #yet another image
	Evas::set_layer($evas,$o3,300); #layer
	Evas::move($evas,$o3,0.0,85.0); #position it
	Evas::show($evas,$o3);#show it

	Evas::update_rect($evas,0,0,250,250); #and tell evas to update that area
}

sub render_and_save
{
	Evas::render($evas); #render the evas
	Imlib2::set_context($image); 
	print "saving to output.png\n";
	Imlib2::save_image("output.png"); #and we have our output
	print "done\n";
}	

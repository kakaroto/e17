#!/usr/bin/perl
use Evas;
use Imlib2;


$image = Imlib2::create_image(250,250);
Imlib2::set_context($image);
Imlib2::set_format("png");
$evas = Evas::new();
Evas::set_output_method($evas,4);
Evas::set_output_image($evas,$image);
Evas::set_output_size($evas,250,250);
Evas::set_output_viewport($evas,0.0,0.0,250.0,250.0);
Evas::font_add_path($evas,"./fonts");
$o3 = Evas::add_rectangle($evas);
Evas::set_color($evas,$o3,255,255,255,255);
Evas::set_layer($evas,$o3,0);
Evas::show($evas,$o3);
Evas::move($evas, $o3, 0.0, 0.0);
Evas::resize($evas, $o3, 250.0, 250.0);
$o = Evas::add_image_from_file($evas,"./images/mush.png");
Evas::set_layer($evas,$o,100);
Evas::show($evas,$o);
$o2 = Evas::add_text($evas,"notepad",20,"Raster Eats This!");
Evas::set_layer($evas,$o2,200);
Evas::set_color($evas,$o2,0,0,0,160);
Evas::move($evas,$o2,0.0,60.0);
Evas::show($evas,$o2);
$o3 = Evas::add_image_from_file($evas,"./images/cheese.png");
Evas::set_layer($evas,$o3,300);
Evas::move($evas,$o3,0.0,85.0);
Evas::show($evas,$o3);

Evas::update_rect($evas,0,0,250,250);




render();


sub render
{
	Evas::render($evas);
	Imlib2::set_context($image);
	print "saving to output.png\n";
	Imlib2::save_image("output.png");
	print "done\n";
}	

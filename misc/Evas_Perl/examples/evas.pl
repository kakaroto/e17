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
$o3 = Evas::add_rectangle($evas);
Evas::set_layer($evas,$o3,0);
Evas::set_color($evas,$o3,255,255,255,255);
Evas::show($evas,$o3);
Evas::move($evas, $o3, 0.0, 0.0);
Evas::resize($evas, $o2, 250.0, 250.0);
$o = Evas::add_image_from_file($evas,"./images/mush.png");
print "add\n";
Evas::set_layer($evas,$o,100);
Evas::show($evas,$o);
#$o2 = Evas::add_text($evas,"notepad",20,"blah");
#Evas::set_layer($evas,$o2,200);
#Evas::show($evas,$o2);
Evas::update_rect($evas,0,0,250,250);

render();


sub render
{
	print("render\n");
	Evas::render($evas);
	Imlib2::set_context($image);
	print "saving\n";
	Imlib2::save_image("output.png");
}	

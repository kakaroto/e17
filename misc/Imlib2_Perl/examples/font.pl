#!/usr/bin/perl
die "usage font.pl text\n" if !$ARGV[0];
use Imlib2;
$image= Imlib2::create_image(200,200);
Imlib2::set_context($image);
Imlib2::add_path_to_font_path("./fonts");
$font = Imlib2::load_font("notepad/50");
Imlib2::context_set_font($font);
Imlib2::context_set_angle(65.0);
#Imlib2::context_set_direction(4);
Imlib2::text_draw(2,2,$ARGV[0]);
Imlib2::free_font();
print("output saved to font.png\n");
Imlib2::save_image("font.png");
system("./pfeh.pl font.png&");

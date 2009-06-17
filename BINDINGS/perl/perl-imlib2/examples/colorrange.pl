use Imlib2;
die "Supply an output image file name please!\n" if !$ARGV[0];
$image = Imlib2::create_image(300,300);
Imlib2::set_context($image);

$colorrange = Imlib2::create_color_range();
Imlib2::context_set_color_range($colorrange);
Imlib2::context_set_color(255,255,255,255);
Imlib2::add_color_to_color_range(0);
Imlib2::context_set_color(255, 200, 10, 100);
Imlib2::add_color_to_color_range(10);
Imlib2::context_set_color(0, 0, 0, 0);
Imlib2::add_color_to_color_range(20);
Imlib2::fill_color_range_rectangle(0,0,300,300,-30.0);
Imlib2::save_image("$ARGV[0]");
system("./pfeh.pl $ARGV[0]");

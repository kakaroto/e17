use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
$lib = "-L/usr/local/lib -L/usr/lib -L/usr/X11R6/lib -lImlib2 -lXext -lX11 -ljpeg -lttf -lm -ldl";
$inc = "-I/usr/local/include -I/usr/X11R6/include -I/usr/include";
#chomp($inc);
#chomp($lib);
WriteMakefile(
    'NAME'	=> 'Imlib2',
    'VERSION_FROM' => 'Imlib2.pm', # finds $VERSION
    'LIBS'	=> $lib,   # e.g., '-lm'
    'DEFINE'	=> '',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> $inc,     # e.g., '-I/usr/include/other' 
);

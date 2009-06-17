use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'      => 'X11::Esh',
    'VERSION_FROM' => 'Esh.pm', # finds $VERSION
    'dist' => {COMPRESS => 'gzip -9', SUFFIX => 'gz'},
);

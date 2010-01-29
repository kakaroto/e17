use strict;
use warnings;

use 5.10.0;

use File::Copy;
use File::Spec::Functions;
use FindBin qw($Bin);

use ExtUtils::MakeMaker;

foreach (qw(evas elementary)) {
    die("$_ not found in PKG_CONFIG_PATH\n") unless (qx(pkg-config --cflags $_));
}

# TODO get rid of this ugly hack
foreach (qw(common.c common.h)) {
    foreach my $c (qw(Evas Ecore Elementary)) {
        printf("Copying %s to %s\n", catfile($Bin, $_), catfile($Bin, $c, $_));
        copy(catfile($Bin, $_), catfile($Bin, $c, $_)) || die("copy() failed: $!");
    }
}

WriteMakefile(
    'NAME'          => 'EFL',
    'AUTHOR'        => 'Adam Flott <adam@npjh.com>',
    'VERSION_FROM'  => 'lib/EFL.pm',
    'PREREQ_PM'     => {
        'Sub::Exporter' => 0,
        'Test::More'    => '0.83'
    },
    'DEFINE'        => ($ENV{'TEST_MAINTAINER'}) ? '-DDEBUG=1' : '',

    # These 3 lines prevent ExtUtils::MakeMaker from building EFL.so
    'OBJECT' => '',
    'XS'     => {},
    'C'      => [],
);

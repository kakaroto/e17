use strict;
use warnings;

use 5.10.0;

use Devel::Peek;
use Data::Dumper;

my %images = (
    'logo'       => "logo.png",
    'logo_small' => "logo_small.png"
);

my $package_data_dir = $ENV{'ELM_PACKAGE_DATA_DIR'} || "/opt/e17/share/elementary/images/";

foreach (keys(%images)) {
    $images{$_} = $package_data_dir . $images{$_};

    die("$images{$_} not found, aborting test suite.\n") unless (-e $images{$_});
}

use EFL qw(:all);

elm_init(@ARGV);

my ($win, $bg, $ic);

$win = elm_win_add(undef, "icon-transparent", ELM_WIN_BASIC);
elm_win_title_set($win, "Icon Transparent");
evas_object_smart_callback_add($win, "delete,request", sub { elm_exit(); exit(0); }, undef);
elm_win_alpha_set($win, 1);

$ic = elm_icon_add($win);
elm_icon_file_set($ic, $images{'logo'}, undef);
elm_icon_scale_set($ic, 0, 0);
elm_win_resize_object_add($win, $ic);
evas_object_show($ic);

evas_object_smart_callback_add($ic, "clicked", \&icon_clicked, \$win);

evas_object_show($win);

elm_run();

elm_exit();

sub icon_clicked {
    print(Dump($_), "---\n") for (@_);

    my ($data, $obj, $event_info) = @_;
    evas_object_hide($$data);

    printf(STDERR "clicked!\n");
}


use strict;
use warnings;

use 5.10.0;

use ExtUtils::MakeMaker;
use ExtUtils::PkgConfig;

my %elm_pkg_info = ExtUtils::PkgConfig->find('elementary');

WriteMakefile(
    'NAME'         => 'EFL::Elementary',
    'VERSION_FROM' => '../lib/EFL/Elementary.pm',
    'PREREQ_PM'    => {
        'Carp'          => 0,
        'XSLoader'      => 0,
        'Sub::Exporter' => 0,
    },
    'AUTHOR'        => 'Adam Flott <adam@npjh.com>',
    'LIBS'          => $elm_pkg_info{'libs'},
    'INC'           => '-Wall -I. ' . $elm_pkg_info{'cflags'},
    'OBJECT'        => 'Elementary.o common.o',
    'XS'            => {'Elementary.xs' => 'Elementary.c'},
    'DEFINE'        => ($ENV{'TEST_MAINTAINER'}) ? '-DDEBUG=1' : '',
);

if (eval { require ExtUtils::Constant; 1 }) {

    # If you edit these definitions to change the constants used by this module,
    # you will need to use the generated const-c.inc and const-xs.inc
    # files to replace their "fallback" counterparts before distributing your
    # changes.
    my @names = (
        {'name' => 'ELM_PI',                             'macro' => '1'},
        {'name' => 'ELM_FLIP_ROTATE_X_CENTER_AXIS',      'macro' => '1'},
        {'name' => 'ELM_FLIP_ROTATE_XZ_CENTER_AXIS',     'macro' => '1'},
        {'name' => 'ELM_FLIP_ROTATE_YZ_CENTER_AXIS',     'macro' => '1'},
        {'name' => 'ELM_FLIP_ROTATE_Y_CENTER_AXIS',      'macro' => '1'},
        {'name' => 'ELM_GENLIST_ITEM_NONE',              'macro' => '1'},
        {'name' => 'ELM_GENLIST_ITEM_SUBITEMS',          'macro' => '1'},
        {'name' => 'ELM_HOVER_AXIS_BOTH',                'macro' => '1'},
        {'name' => 'ELM_HOVER_AXIS_HORIZONTAL',          'macro' => '1'},
        {'name' => 'ELM_HOVER_AXIS_NONE',                'macro' => '1'},
        {'name' => 'ELM_HOVER_AXIS_VERTICAL',            'macro' => '1'},
        {'name' => 'ELM_ICON_FILE',                      'macro' => '1'},
        {'name' => 'ELM_ICON_NONE',                      'macro' => '1'},
        {'name' => 'ELM_ICON_STANDARD',                  'macro' => '1'},
        {'name' => 'ELM_IMAGE_FLIP_HORIZONTAL',          'macro' => '1'},
        {'name' => 'ELM_IMAGE_FLIP_TRANSPOSE',           'macro' => '1'},
        {'name' => 'ELM_IMAGE_FLIP_TRANSVERSE',          'macro' => '1'},
        {'name' => 'ELM_IMAGE_FLIP_VERTICAL',            'macro' => '1'},
        {'name' => 'ELM_IMAGE_ORIENT_NONE',              'macro' => '1'},
        {'name' => 'ELM_IMAGE_ROTATE_180_CW',            'macro' => '1'},
        {'name' => 'ELM_IMAGE_ROTATE_90_CCW',            'macro' => '1'},
        {'name' => 'ELM_IMAGE_ROTATE_90_CW',             'macro' => '1'},
        {'name' => 'ELM_LIST_COMPRESS',                  'macro' => '1'},
        {'name' => 'ELM_LIST_LIMIT',                     'macro' => '1'},
        {'name' => 'ELM_LIST_SCROLL',                    'macro' => '1'},
        {'name' => 'ELM_MAP_ZOOM_MODE_AUTO_FILL',        'macro' => '1'},
        {'name' => 'ELM_MAP_ZOOM_MODE_AUTO_FIT',         'macro' => '1'},
        {'name' => 'ELM_MAP_ZOOM_MODE_MANUAL',           'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_BOTTOM',           'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_BOTTOM_LEFT',      'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_BOTTOM_RIGHT',     'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_LEFT',             'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_RIGHT',            'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_TOP',              'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_TOP_LEFT',         'macro' => '1'},
        {'name' => 'ELM_NOTIFY_ORIENT_TOP_RIGHT',        'macro' => '1'},
        {'name' => 'ELM_PANEL_ORIENT_BOTTOM',            'macro' => '1'},
        {'name' => 'ELM_PANEL_ORIENT_LEFT',              'macro' => '1'},
        {'name' => 'ELM_PANEL_ORIENT_RIGHT',             'macro' => '1'},
        {'name' => 'ELM_PANEL_ORIENT_TOP',               'macro' => '1'},
        {'name' => 'ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL',   'macro' => '1'},
        {'name' => 'ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT',    'macro' => '1'},
        {'name' => 'ELM_PHOTOCAM_ZOOM_MODE_MANUAL',      'macro' => '1'},
        {'name' => 'ELM_POLICY_LAST',                    'macro' => '1'},
        {'name' => 'ELM_POLICY_QUIT',                    'macro' => '1'},
        {'name' => 'ELM_POLICY_QUIT_LAST_WINDOW_CLOSED', 'macro' => '1'},
        {'name' => 'ELM_POLICY_QUIT_NONE',               'macro' => '1'},
        {'name' => 'ELM_SCROLLER_POLICY_AUTO',           'macro' => '1'},
        {'name' => 'ELM_SCROLLER_POLICY_OFF',            'macro' => '1'},
        {'name' => 'ELM_SCROLLER_POLICY_ON',             'macro' => '1'},
        {'name' => 'ELM_TEXT_FORMAT_MARKUP_UTF8',        'macro' => '1'},
        {'name' => 'ELM_TEXT_FORMAT_PLAIN_UTF8',         'macro' => '1'},
        {'name' => 'ELM_WIN_BASIC',                      'macro' => '1'},
        {'name' => 'ELM_WIN_DESKTOP',                    'macro' => '1'},
        {'name' => 'ELM_WIN_DIALOG_BASIC',               'macro' => '1'},
        {'name' => 'ELM_WIN_DOCK',                       'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_ALPHA',             'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_HEX',               'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_NUMERIC',           'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_OFF',               'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_ON',                'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_PASSWORD',          'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_PHONE_NUMBER',      'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_PIN',               'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_TERMINAL',          'macro' => '1'},
        {'name' => 'ELM_WIN_KEYBOARD_UNKNOWN',           'macro' => '1'},
        {'name' => 'ELM_WIN_MENU',                       'macro' => '1'},
        {'name' => 'ELM_WIN_SPLASH',                     'macro' => '1'},
        {'name' => 'ELM_WIN_TOOLBAR',                    'macro' => '1'},
        {'name' => 'ELM_WIN_UTILITY',                    'macro' => '1'},
        {'name' => 'EVAS_HINT_EXPAND',                   'macro' => '1'},
        {'name' => 'EVAS_ASPECT_CONTROL_VERTICAL',       'macro' => '1'},
        {'name' => 'EVAS_ASPECT_CONTROL_HORIZONTAL',     'macro' => '1'},
        {'name' => 'EVAS_HINT_FILL',                     'macro' => '1'},
    );

    ExtUtils::Constant::WriteConstants(
        'NAME'         => 'EFL::Elementary',
        'NAMES'        => \@names,
        'DEFAULT_TYPE' => 'IV',
        'C_FILE'       => 'elementary-const-c.inc',
        'XS_FILE'      => 'elementary-const-xs.inc',
    );
}
else {
    use File::Copy;
    use File::Spec;
    foreach my $file ('elementary-const-c.inc', 'elementary-const-xs.inc') {
        my $fallback = File::Spec->catfile('fallback', $file);
        copy($fallback, $file) or die "Can't copy $fallback to $file: $!";
    }
}

use strict;
use warnings;

use 5.10.0;

use ExtUtils::MakeMaker;
use ExtUtils::PkgConfig;

my %evas_pkg_info = ExtUtils::PkgConfig->find('evas');

WriteMakefile(
    'NAME'         => 'EFL::Evas',
    'VERSION_FROM' => '../lib/EFL/Evas.pm',
    'PREREQ_PM'    => {
        'Carp'          => 0,
        'XSLoader'      => 0,
        'Sub::Exporter' => 0,
    },
    'AUTHOR'        => 'Adam Flott <adam@npjh.com>',
    'LIBS'          => $evas_pkg_info{'libs'},
    'INC'           => '-Wall -I. ' . $evas_pkg_info{'cflags'},
    'OBJECT'        => 'Evas.o common.o',
    'XS'            => {'Evas.xs' => 'Evas.c'},
    'DEFINE'        => ($ENV{'TEST_MAINTAINER'}) ? '-DDEBUG=1' : '',
);

if (eval { require ExtUtils::Constant; 1 }) {

    # If you edit these definitions to change the constants used by this module,
    # you will need to use the generated const-c.inc and const-xs.inc
    # files to replace their "fallback" counterparts before distributing your
    # changes.
    my @names = (
        qw(
          EVAS_ALLOC_ERROR_FATAL EVAS_ALLOC_ERROR_NONE
          EVAS_ALLOC_ERROR_RECOVERED EVAS_COLOR_SPACE_AHSV
          EVAS_COLOR_SPACE_ARGB EVAS_HINT_EXPAND EVAS_HINT_FILL
          EVAS_LAYER_MAX EVAS_LAYER_MIN EVAS_LOAD_ERROR_CORRUPT_FILE
          EVAS_LOAD_ERROR_DOES_NOT_EXIST EVAS_LOAD_ERROR_GENERIC
          EVAS_LOAD_ERROR_NONE EVAS_LOAD_ERROR_PERMISSION_DENIED
          EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED
          EVAS_LOAD_ERROR_UNKNOWN_FORMAT
          EVAS_OBJECT_BOX_API_VERSION
          EVAS_PIXEL_FORMAT_ARGB32 EVAS_PIXEL_FORMAT_NONE
          EVAS_PIXEL_FORMAT_YUV420P_601
          EVAS_SMART_CLASS_VERSION
          EVAS_TEXTURE_PAD EVAS_TEXTURE_REFLECT EVAS_TEXTURE_REPEAT
          EVAS_TEXTURE_RESTRICT EVAS_TEXTURE_RESTRICT_REFLECT
          EVAS_TEXTURE_RESTRICT_REPEAT EVAS_TEXT_INVALID
          EVAS_TEXT_SPECIAL),

        {'name' => 'EVAS_ASPECT_CONTROL_BOTH', 'macro' => '1'},
        {
            'name'  => 'EVAS_ASPECT_CONTROL_HORIZONTAL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_ASPECT_CONTROL_NEITHER',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_ASPECT_CONTROL_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_ASPECT_CONTROL_VERTICAL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BORDER_FILL_DEFAULT',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BORDER_FILL_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BORDER_FILL_SOLID',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BUTTON_DOUBLE_CLICK',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BUTTON_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_BUTTON_TRIPLE_CLICK',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_CHANGED_SIZE_HINTS',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_DEL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_FOCUS_IN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_FOCUS_OUT',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_FREE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_HIDE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_HOLD',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_IMAGE_PRELOADED',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_KEY_DOWN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_KEY_UP',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_DOWN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_IN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_MOVE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_OUT',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_UP',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOUSE_WHEEL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_MOVE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_RESIZE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_RESTACK',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_CALLBACK_SHOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_COLORSPACE_ARGB8888',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_COLORSPACE_RGB565_A5P',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_COLORSPACE_YCBCR422P601_PL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_COLORSPACE_YCBCR422P709_PL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_EVENT_FLAG_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_EVENT_FLAG_ON_HOLD',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_FONT_HINTING_AUTO',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_FONT_HINTING_BYTECODE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_FONT_HINTING_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_IMAGE_SCALE_HINT_DYNAMIC',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_IMAGE_SCALE_HINT_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_IMAGE_SCALE_HINT_STATIC',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_OBJECT_POINTER_MODE_AUTOGRAB',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_OBJECT_POINTER_MODE_NOGRAB',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_OBJECT_TABLE_HOMOGENEOUS_ITEM',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_OBJECT_TABLE_HOMOGENEOUS_NONE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_OBJECT_TABLE_HOMOGENEOUS_TABLE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_ADD',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_ADD_REL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_BLEND',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_BLEND_REL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_COPY',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_COPY_REL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_MASK',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_MUL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_SUB',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_SUB_REL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_TINT',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_RENDER_TINT_REL',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXTBLOCK_TEXT_MARKUP',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXTBLOCK_TEXT_PLAIN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXTBLOCK_TEXT_RAW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_FAR_SHADOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_FAR_SOFT_SHADOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_GLOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_OUTLINE',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_OUTLINE_SHADOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_PLAIN',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_SHADOW',
            'macro' => '1'
        },
        {
            'name'  => 'EVAS_TEXT_STYLE_SOFT_OUTLINE',
            'macro' => '1'
        },
        {'name' => 'EVAS_TEXT_STYLE_SOFT_SHADOW', 'macro' => '1'}
    );
    ExtUtils::Constant::WriteConstants(
        'NAME'         => 'EFL::Evas',
        'NAMES'        => \@names,
        'DEFAULT_TYPE' => 'IV',
        'C_FILE'       => 'evas-const-c.inc',
        'XS_FILE'      => 'evas-const-xs.inc',
    );
}
else {
    use File::Copy;
    use File::Spec;
    foreach my $file ('evas-const-c.inc', 'evas-const-xs.inc') {
        my $fallback = File::Spec->catfile('fallback', $file);
        copy($fallback, $file) or die "Can't copy $fallback to $file: $!";
    }
}

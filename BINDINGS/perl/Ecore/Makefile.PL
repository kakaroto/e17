use strict;
use warnings;

use 5.10.0;

use ExtUtils::MakeMaker;
use ExtUtils::PkgConfig;

my %ecore_pkg_info = ExtUtils::PkgConfig->find('ecore');

WriteMakefile(
    NAME          => 'EFL::Ecore',
    VERSION_FROM  => '../lib/EFL/Ecore.pm',
    PREREQ_PM     => {},
    AUTHOR        => 'Adam Flott <adam@npjh.com>',
    LIBS          => $ecore_pkg_info{'libs'},
    INC           => '-Wall -I. ' . $ecore_pkg_info{'cflags'},
    OBJECT        => 'Ecore.o common.o',
    XS            => {'Ecore.xs' => 'Ecore.c'},
    'DEFINE'      => ($ENV{'TEST_MAINTAINER'}) ? '-DDEBUG=1' : '',
);

if (eval { require ExtUtils::Constant; 1 }) {

    # If you edit these definitions to change the constants used by this module,
    # you will need to use the generated const-c.inc and const-xs.inc
    # files to replace their "fallback" counterparts before distributing your
    # changes.
    my @names = (
        qw(ECORE_CALLBACK_CANCEL ECORE_CALLBACK_RENEW
          ECORE_EVENT_COUNT ECORE_EVENT_NONE ECORE_EVENT_SIGNAL_EXIT
          ECORE_EVENT_SIGNAL_HUP ECORE_EVENT_SIGNAL_POWER
          ECORE_EVENT_SIGNAL_REALTIME ECORE_EVENT_SIGNAL_USER
          ECORE_EXE_PRIORITY_INHERIT FALSE TRUE),
        {name => "ECORE_EXE_NOT_LEADER",               macro => "1"},
        {name => "ECORE_EXE_PIPE_AUTO",                macro => "1"},
        {name => "ECORE_EXE_PIPE_ERROR",               macro => "1"},
        {name => "ECORE_EXE_PIPE_ERROR_LINE_BUFFERED", macro => "1"},
        {name => "ECORE_EXE_PIPE_READ",                macro => "1"},
        {name => "ECORE_EXE_PIPE_READ_LINE_BUFFERED",  macro => "1"},
        {name => "ECORE_EXE_PIPE_WRITE",               macro => "1"},
        {name => "ECORE_EXE_RESPAWN",                  macro => "1"},
        {name => "ECORE_EXE_USE_SH",                   macro => "1"},
        {name => "ECORE_FD_ERROR",                     macro => "1"},
        {name => "ECORE_FD_READ",                      macro => "1"},
        {name => "ECORE_FD_WRITE",                     macro => "1"},
        {name => "ECORE_POLLER_CORE",                  macro => "1"}
    );
    ExtUtils::Constant::WriteConstants(
        NAME         => 'EFL::Ecore',
        NAMES        => \@names,
        DEFAULT_TYPE => 'IV',
        C_FILE       => 'ecore-const-c.inc',
        XS_FILE      => 'ecore-const-xs.inc',
    );

}
else {
    use File::Copy;
    use File::Spec;
    foreach my $file ('ecore-const-c.inc', 'ecore-const-xs.inc') {
        my $fallback = File::Spec->catfile('fallback', $file);
        copy($fallback, $file) or die "Can't copy $fallback to $file: $!";
    }
}

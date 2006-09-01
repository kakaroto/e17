use 5.008004;
use ExtUtils::MakeMaker;
use Devel::PPPort;

# dependency check
my $etklib = `etk-config --version`;
die "I can't run etk-config, make sure that etk is installed, and that etk-config is in your path" unless $etklib;

Devel::PPPort::WriteFile();

WriteMakefile(
    DISTNAME          => 'Etk-Perl',
    NAME              => 'Etk',
    VERSION_FROM      => 'lib/Etk.pm', 
    PREREQ_PM         => {}, 
    ($] >= 5.005 ?     
      (ABSTRACT       => 'Perl bindings for the Enlightened ToolKit (Etk)',
       AUTHOR         => q|Chady 'Leviathan' Kassouf <chady.kassouf@gmail.com>|) : ()),
    LIBS              => [`etk-config --libs`], 
    DEFINE            => '', #'-DDEBUG', 
    INC               => "-g -I. `etk-config --cflags`", 
    XSPROTOARG        => '-noprototypes',
    test              => {TESTS=>'t/Etk/*.t'},
    clean             => {FILES=>"ppport.h"},
    OBJECT            => '$(O_FILES)', 
);

sub MY::test_via_harness {
    local $_ = shift()->MM::test_via_harness( @_ );
    s/\bPERL_DL_NONLAZY=1 +//g;
    return $_;
}

sub MY::test_via_script {
    local $_ = shift()->MM::test_via_script( @_ );
    s/\bPERL_DL_NON_LAZY=1 +//g;
    return $_;
}


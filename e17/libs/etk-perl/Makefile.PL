use 5.008004;
use ExtUtils::MakeMaker;
use Devel::PPPort;

# no windows support
die "OS unsupported" if $^O =~ /win/;

# dependency check
print "Checking for Etk...";
my $etklib = `pkg-config --variable=version etk`;
unless ($etklib) {
	print "pkg-config can't find etk, make sure that etk is installed, and that your PKG_CONFIG_PATH is correct\n";
	exit(0);
}
print "Ok\n";

Devel::PPPort::WriteFile();

WriteMakefile(
    DISTNAME          => 'Etk-Perl',
    NAME              => 'Etk',
    VERSION_FROM      => 'lib/Etk.pm', 
    PREREQ_PM         => {}, 
    ($] >= 5.005 ?     
      (ABSTRACT       => 'Perl bindings for the Enlightened ToolKit (Etk)',
       AUTHOR         => q|Chady 'Leviathan' Kassouf <chady.kassouf@gmail.com>|) : ()),
    LIBS              => [`pkg-config --libs etk`], 
    DEFINE            => '', #'-DDEBUG', 
    INC               => "-g -I. `pkg-config --cflags etk`", 
    XSPROTOARG        => '-noprototypes',
    test              => {TESTS=>'t/Etk/*.t'},
    clean             => {FILES=>"ppport.h"},
    OBJECT            => '$(O_FILES)', 
    DIR               => ["xs"],
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


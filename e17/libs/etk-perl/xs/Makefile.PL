use 5.008004;
use ExtUtils::MakeMaker;
use Devel::PPPort;

my $cflags = `pkg-config --cflags etk`;
my $libs = `pkg-config --libs etk`;
if ($cflags eq "" || $libs eq "") {
	die "Cannot find Etk.";
}
# Add the boot section
my $debug = $ARGV[0] eq "debug" ? 1 : 0;
my @list = grep { $_ ne "Etk" } map { s/\.xs$//; $_  } <*.xs>;

open IN, "<", 'Etk.xs.in' or die "Missing Etk.xs.in file: $!\n";
my $contents;
{ 
	local $/;
	$contents = <IN>;
}
close IN;

open OUT, ">", 'Etk.xs' or die "Cannot write Etk.xs file: $!\n";
print OUT $contents;
print OUT qq|BOOT:\n|;
foreach (@list) {
	print OUT qq|printf("Booting $_\\n");\n| if $debug;
	print OUT qq|PUSHMARK(SP);\nboot_Etk__$_();\nPUTBACK;\n|;
}
close OUT;


WriteMakefile(
    DISTNAME          => 'Etk-Perl',
    NAME              => 'Etk',
    VERSION_FROM      => '../lib/Etk.pm', 
    PREREQ_PM         => {}, 
    ($] >= 5.005 ?     
      (ABSTRACT       => 'Perl bindings for the Enlightened ToolKit (Etk)',
       AUTHOR         => q|Chady 'Leviathan' Kassouf <chady.kassouf@gmail.com>|) : ()),
    LIBS              => [$libs],
    DEFINE            => '', #'-DDEBUG', 
    INC               => "-g -I. $cflags",
    XSPROTOARG        => '-noprototypes',
    OBJECT            => '$(O_FILES)', 
    clean             => {FILES=>"Etk.xs"},
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


use 5.008004;
use ExtUtils::MakeMaker;

# dependency check
my $etklib = `etk-config --version`;
die "I can't run etk-config, make sure that etk is installed, and that etk-config is in your path" unless $etklib;

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
    OBJECT            => '$(O_FILES)', 
);
if  (eval {require ExtUtils::Constant; 1}) {
  # If you edit these definitions to change the constants used by this module,
  # you will need to use the generated const-c.inc and const-xs.inc
  # files to replace their "fallback" counterparts before distributing your
  # changes.
  my @names = (qw());
  ExtUtils::Constant::WriteConstants(
                                     NAME         => 'Etk',
                                     NAMES        => \@names,
                                     DEFAULT_TYPE => 'IV',
                                     C_FILE       => 'const-c.inc',
                                     XS_FILE      => 'const-xs.inc',
                                  );

}
else {
  use File::Copy;
  use File::Spec;
  foreach my $file ('const-c.inc', 'const-xs.inc') {
    my $fallback = File::Spec->catfile('fallback', $file);
    copy ($fallback, $file) or die "Can't copy $fallback to $file: $!";
  }
}

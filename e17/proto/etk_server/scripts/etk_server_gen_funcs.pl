package CParser;
use Carp;

sub register {
    {
     extends => [qw(C)],
     overrides => [qw(get_parser)],
    }
}

sub get_parser {
    bless {}, 'CParser'
}

sub code {
    my($self,$code) = @_;
    
    # These regular expressions were derived from Regexp::Common v0.01.
    my $RE_comment_C   = q{(?:(?:\/\*)(?:(?:(?!\*\/)[\s\S])*)(?:\*\/))};
    my $RE_comment_Cpp = q{(?:\/\*(?:(?!\*\/)[\s\S])*\*\/|\/\/[^\n]*\n)};
    my $RE_quoted      = (q{(?:(?:\")(?:[^\\\"]*(?:\\.[^\\\"]*)*)(?:\")}
                         .q{|(?:\')(?:[^\\\']*(?:\\.[^\\\']*)*)(?:\'))});
    our $RE_balanced_brackets =
        qr'(?:[{]((?:(?>[^{}]+)|(??{$RE_balanced_brackets}))*)[}])';
    our $RE_balanced_parens   =
        qr'(?:[(]((?:(?>[^()]+)|(??{$RE_balanced_parens}))*)[)])';

    # First, we crush out anything potentially confusing.
    # The order of these _does_ matter.
    $code =~ s/$RE_comment_C/ /go;
    $code =~ s/$RE_comment_Cpp/ /go;
    $code =~ s/^\#.*(\\\n.*)*//mgo;
    $code =~ s/$RE_quoted/\"\"/go;
    $code =~ s/$RE_balanced_brackets/{ }/go;
    
    $self->{_the_code_most_recently_parsed} = $code; # Simplifies debugging.

    my $normalize_type = sub {
	# Normalize a type for lookup in a typemap.
        my($type) = @_;

        # Remove "extern".
        # But keep "static", "inline", "typedef", etc,
        #  to cause desirable typemap misses.
        $type =~ s/\bextern\b//g;

        # Whitespace: only single spaces, none leading or trailing.
        $type =~ s/\s+/ /g;
        $type =~ s/^\s//; $type =~ s/\s$//;

        # Adjacent "derivative characters" are not separated by whitespace,
        # but _are_ separated from the adjoining text.
        # [ Is really only * (and not ()[]) needed??? ]
        $type =~ s/\*\s\*/\*\*/g;
        $type =~ s/(?<=[^ \*])\*/ \*/g;

        return $type;
    };
    
    # The decision of what is an acceptable declaration was originally
    # derived from Inline::C::grammar.pm version 0.30 (Inline 0.43).

    my $re_plausible_place_to_begin_a_declaration = qr {
	# The beginning of a line, possibly indented.
	# (Accepting indentation allows for C code to be aligned with
	#  its surrounding perl, and for backwards compatibility with
	#  Inline 0.43).
	(?m: ^ ) \s*
    }xo;

    # Instead of using \s , we dont tolerate blank lines.
    # This matches user expectation better than allowing arbitrary
    # vertical whitespace.
    my $sp = qr{[ \t]|\n(?![ \t]*\n)};

    my $re_type = qr {( 
			(?: \w+ $sp* )+? # words
			(?: \*  $sp* )*  # stars
			)}xo;

    my $re_identifier = qr{ (\w+) $sp* }xo;
    
    while($code =~ m{
	$re_plausible_place_to_begin_a_declaration
        ( $re_type $re_identifier $RE_balanced_parens $sp* (\;|\{) )
       }xgo)
    {
        my($type, $identifier, $args, $what) = ($2,$3,$4,$5);

        $args = "" if $args =~ /^\s+$/;

        my $is_decl     = $what eq ';';
        my $function    = $identifier;
        my $return_type = &$normalize_type($type);
	my @arguments   = split ',', $args;

#        goto RESYNC if $is_decl && !$self->{data}{AUTOWRAP};
#        goto RESYNC if $self->{data}{done}{$function};
#	
#        goto RESYNC if !defined
#            $self->{data}{typeconv}{valid_rtypes}{$return_type};
#        
        my(@arg_names,@arg_types);
	my $dummy_name = 'arg1';

	foreach my $arg (@arguments) {

	    if(my($type, $identifier) =
	       $arg =~ /^\s*$re_type(?:$re_identifier)?\s*$/o)
	    {
		my $arg_name = $identifier;
		my $arg_type = &$normalize_type($type);

		if(!defined $arg_name) {
		    goto RESYNC if !$is_decl;
		    $arg_name = $dummy_name++;
		}
#		goto RESYNC if !defined
#		    $self->{data}{typeconv}{valid_types}{$arg_type};
		
		push(@arg_names,$arg_name);
		push(@arg_types,$arg_type);
	    }
	    elsif($arg =~ /^\s*\.\.\.\s*$/) {
		push(@arg_names,'...');
		push(@arg_types,'...');
	    }
	    else {
		goto RESYNC;
	    }
	}

        # Commit.
        push @{$self->{data}{functions}}, $function;
        $self->{data}{function}{$function}{return_type}= $return_type; 
        $self->{data}{function}{$function}{arg_names} = [@arg_names];
        $self->{data}{function}{$function}{arg_types} = [@arg_types];
        $self->{data}{done}{$function} = 1;

        next;

      RESYNC:  # Skip the rest of the current line, and continue.
        $code =~ /\G[^\n]*\n/gc;
    }

   return 1;  # We never fail.
}

1;

package main;

print "Generating etk_server_valid_funcs.c...\n";

my @files;
open(LS, "ls `etk-config --prefix`/include/etk/etk_*.h|");
while(<LS>)
{
    push(@files, $_);
}
close LS;

$outfile = "../src/bin/etk_server_valid_funcs.c";
open OUTFILE, ">$outfile" or die "can't open $file: $!";

print OUTFILE <<EOC
#include "etk_server.h"

extern Evas_Hash *_etk_server_valid_funcs;

void _etk_server_valid_funcs_populate()
{
   Etk_Server_Func *func;
    
EOC
;

for my $file (@files)
{
    
    chomp($file);
    print "Parsing $file...\n";    
    
    my $data;
    {
	local $/;
	open SLURP, $file or die "can't open $file: $!";
	$data = <SLURP>;
	close SLURP or die "cannot close $file: $!";
    } 
    
    my $p = CParser->get_parser;
    $p->code($data);
    
    my $data = $p->{data};
    
    for my $function (@{$data->{functions}}) 
    {
	my $return_type = $data->{function}->{$function}->{return_type};
	my @arg_names = @{$data->{function}->{$function}->{arg_names}};
	my @arg_types = @{$data->{function}->{$function}->{arg_types}};
        
	print OUTFILE <<EOC          
   func = calloc(1, sizeof(Etk_Server_Func));
   func->name = strdup("$function");
   func->return_type = strdup("$return_type");    
   func->arg_names = NULL;
   func->arg_types = NULL;
EOC
;      
	for my $arg (@arg_names)
	{
	    print OUTFILE <<EOC
   func->arg_names = evas_list_append(func->arg_names, strdup("$arg"));
EOC
;	  
	}
    
	for my $arg (@arg_types)
	{
	    print OUTFILE <<EOC
   func->arg_types = evas_list_append(func->arg_types, strdup("$arg"));
EOC
;
	}
    
	print OUTFILE <<EOC
   _etk_server_valid_funcs = evas_hash_add(_etk_server_valid_funcs, "$function", func);

EOC
;      
    }
}
    
print OUTFILE <<EOC
}

EOC
;  
        
close OUTFILE or die "cannot close $file: $!";

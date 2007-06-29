#!/usr/bin/perl -w

use strict;

my $dir = shift;

my %widgets = ();

opendir(DIR, "$dir") or die "Can't open $dir: $!";
while (my $file = readdir(DIR))
{
	next if ($file !~ /group__Ewl__(.*)\.xml/);
	my $widget = "Ewl_$1";
	$widget =~ s/__/_/g;

	open(FH, "$dir/$file") or die "Can't open $dir/$file: $!";
	my @lines = <FH>;
	close(FH);

	foreach my $line (@lines)
	{
		next if ($line !~ /<simplesect kind="remark">\s*<para>.*?<ref [^>]*>(.*?)<\/ref>\./);
		$widgets{$widget} = $1;
		last;
	}
}
closedir(DIR);

foreach my $widget (keys (%widgets))
{
	open(OUT, ">/tmp/$widget.dot") or die "Can't open /tmp/$widget.dot";

	print OUT "digraph G {\n";
	print OUT "$widget";

	my $parent = $widgets{$widget};
	while (defined($parent))
	{
		print OUT " -> $parent";
		$parent = $widgets{$parent};
	}
	print OUT "\n}\n\n";
	close(OUT);

	`dot -Tpng -o $dir/../images/${widget}_inheritance.png /tmp/$widget.dot`;
	unlink("/tmp/$widget.dot");
}



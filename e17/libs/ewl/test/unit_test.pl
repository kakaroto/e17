#!/usr/bin/perl -w

use strict;

my $ewl_test = `which ewl_test`;
chomp($ewl_test);

my $tests = `$ewl_test -list`;
chomp($tests);

my @tests = split('\n', $tests);
shift @tests;

my $failures = 0;
foreach my $test (@tests)
{
	$test =~ s/^\s*//g;
	$test =~ s/\s*$//g;

	print "Executing $test Tests\n";
	my $out = `$ewl_test -unit $test`;
	my $ret = $? / 256;
	$failures += $ret;

	if ($ret)
	{
		chomp($out);
		my @out = split('\n', $out);

		foreach my $o (@out)
		{
			next if ($o !~ /FAILED/);

			$o =~ s/^Running //g;
			$o =~ s/ FAILED//g;
			print "\t$o\n";
		}
	}
}

if ($failures > 0)
{
	print "$failures tests failed.\n";
}
else
{
	print "All tests passed.\n";
}



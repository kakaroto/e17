#!/usr/bin/perl
# CVS Commits List Messages
# Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
#
# For questions, concerns, please consult the e-develop mailing list.
# please see:
# http://lists.sourceforge.net/mailman/listinfo/enlightenment-devel
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies of the Software, its documentation and marketing & publicity
# materials, and acknowledgment shall be given in the documentation, materials
# and software packages that this Software was used.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Here come the bits you'll want to twiddle configuration-wise:

$SENDMAIL    = "/usr/sbin/sendmail -f noreply\@cvs.enlightenment.org";
$MAILFROM    = "Enlightenment CVS <noreply\@cvs.enlightenment.org>";
$MAILREPLYTO = "enlightenment-devel\@lists.sourceforge.net";
$MAILTO      = "enlightenment-cvs\@lists.sourceforge.net";
$cvsroot = $ENV{'CVSROOT'};
$login = $ENV{'LOGNAME'} || getlogin || (getpwuid($<))[0] || "nobody";

@input = <STDIN>;

exit if($pid = fork);

@filesets = split(/\s+/,$ARGV[0]);
$path = shift(@filesets);
foreach(@filesets) {
	chomp;
	my($file,$oldrev,$newrev) = split(/,/);

	$oldrevs{"$file"} = $oldrev;
	$newrevs{"$file"} = $newrev;
	
	push(@files,"$file");
	
}

@paths = split(/\//,$path);

$basename = $paths[0];
$modulename = $paths[1];
if(($modulename eq "apps") || ($modulename eq "libs") || ($modulename eq "proto")) {
	if($paths[2]) {
		$modulename .= "/$paths[2]";
	}
}

$modulename = $basename if(!$modulename);

open(MAIL, "| $SENDMAIL -t");
# open(MAIL, "| cat > /tmp/smlog");
print MAIL "To: $MAILTO\n";
print MAIL "Reply-To: $MAILREPLYTO\n";
print MAIL "Subject: E CVS: $modulename $login\n";
print MAIL "From: $MAILFROM\n";
print MAIL "\n";


print MAIL "Enlightenment CVS committal\n\n";
print MAIL "Author  : $login\n";
print MAIL "Project : $basename\n" if($basename ne $modulename);
print MAIL "Module  : $modulename\n\n";
print MAIL "Dir     : $path";

foreach(@input) {
	s/.*// if(/^In directory/);
	s/.*// if(/^Update of/);
	s/^Log/\n\nLog/;

}

print MAIL @input;

sort(@files);

foreach(@files) {
	$fullpath = $_;
	if ($oldrevs{$fullpath} eq "NONE") {
	} elsif ($newrevs{$fullpath} eq "NONE") {
	} else {
		push(@changed,$fullpath);
	}
}

foreach(@changed) {

	$filename = $_;
	next if ($filename =~ /\.po$/);

	my @tmp = `cvs -f diff -u3 -r $oldrevs{$filename} -r $newrevs{$filename} $filename 2>/dev/null`;

	shift(@tmp);
	print MAIL @tmp;

}

print MAIL "\n\n";
close(MAIL);

# flag www to update
if ($modulename eq "www") {
    print("--- FLAGGING UPDATE OF WWW ---\n");
    system("wget http://web.enlightenment.org/u.php -O /dev/null >& /dev/null");
}
# also flag www if devs updates
if ($basename eq "devs") {
    print("--- FLAGGING UPDATE OF WWW/DEVS ---\n");
    system("wget http://web.enlightenment.org/u.php -O /dev/null >& /dev/null");
}

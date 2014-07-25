#!/usr/bin/perl

use File::Find;
use Getopt::Std;
use Cwd;

if ($#ARGV < 0)
{
print <<OUT
This script is part of the DevGuy Programmers' Canvas Toolkit
http://DevGuy.com/pctk
(c) 2001 DevGuy.com

Use this script to change the CVSROOT for
the sandbox located in the current directory.
Subdirectories are processed recursively.

Usage:
1) Provide the new CVS Root
OUT
;
exit;

}

sub go{
	return if -d $_;
	return unless $_ eq "Root";
	my $save = $_;
	print cwd;
	open (O, ">$_");
	print O $ARGV[0] . "\n";
	close O;
	$_ = $save;
}

File::Find::find( \&go, "." );
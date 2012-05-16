#!/usr/bin/perl
#
# @(#) Perl script to get current source files for Makefile
#
# @(#) (c) Daniel Price, Jan 2011, daniel.price@monash.edu
#
use Text::Wrap;

$Text::Wrap::columns=80;
$Text::Wrap::separator=" \\\n";
#print wrap('PHEADERS= ','          ',@ARGV);
print wrap('SOURCE= ','        ',@ARGV);
print "\n";

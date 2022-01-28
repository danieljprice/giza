#!/usr/bin/perl
#
# @(#) Perl script to convert defs in giza-shared.h to Fortran parameters
# @(#) Written by Daniel Price, daniel.price@monash.edu
#
my $var;
my $val;
open(FILE,"<../src/giza-shared.h");
while(<FILE>) {
  if (m/define\s\S*\s*([0-9]*)/ && !m/GIZA_LEFT_CLICK/) {
    ($var, $val) = m/define\s(\S*)\s*([0-9]*)/;
    my $lvar = lc($var); #sprintf("%-27s",lc($var));
    print "  integer, parameter, public :: $lvar = $var\n";
  }
}

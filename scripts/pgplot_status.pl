#!/usr/bin/perl
#
# This script parses the giza-pgplot.f90 file
# to produce html documentation of the current
# status of the PGPLOT interface to giza
# (that can be built as libpgplot)
#
open(FILE, "<../src/giza-pgplot.f90");
my $module;
my $desc;
my $status;
my $ntot = 0;
my $ndone = 0;
my $npar = 0;
print STDOUT "<table>\n";
while (<FILE>) {
  if ( m/(^.*Module:)/) 
     {
     ($module,$desc) = m/^.*Module: (.*) -- (.*)/;
     print STDOUT "<tr><td><a href=\"http://www.astro.caltech.edu/~tjp/pgplot/subroutines.html#$module\">$module</a></td>";
     }
  elsif ( m/(^.*Status:)/) 
     {
     ($status) = m/^.*Status: (.*)/;
     $_ = $status;
     my $color = "#FF6600"; # default is orange
     if ( m/.*(NOT).*/ ) 
        {
        $color = "#FF0000";  # red
        }
     elsif ( m/^IMPLEMENTED$/ ) 
        {
        $color = "#009900";  # green
        $ndone = $ndone + 1;
        $npar = $npar + 1;
        }
     else
        {
        $npar = $npar + 1;
        }
     $ntot = $ntot + 1;
     print STDOUT "<td bgcolor=$color>$status</td><td>$desc</td></tr>\n";        
     }
}
print STDOUT "</table>\n";
print STDOUT "<p>$npar of $ntot routines implemented, $ndone of $ntot fully implemented.</p>\n";

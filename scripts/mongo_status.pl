#!/usr/bin/env perl
use strict;
use warnings;

my $ref = shift @ARGV || 'mongo-stuff/mongo/source/libmgo.linux.a';
my $lib = shift @ARGV || 'src/.libs/libmgo.a';

sub norm {
    my ($s) = @_;
    $s =~ s/^_//;
    return lc($s);
}

sub symbols {
    my ($path) = @_;
    open my $fh, '-|', 'nm', $path or die "nm $path: $!\n";
    my %sym;
    while (<$fh>) {
        next unless /\sT\s(\S+)/;
        my $s = norm($1);
        $sym{$s} = 1;
    }
    close $fh;
    return \%sym;
}

die "missing giza libmgo archive: $lib (run make -C src libmgo.la)\n" unless -f $lib;

my $new_syms = symbols($lib);
print "giza libmgo symbols: ", scalar(keys %{$new_syms}), "\n";

unless (-f $ref) {
    print "reference archive not found: $ref (comparison skipped)\n";
    exit 0;
}

my $ref_syms = symbols($ref);

my @missing = sort grep {
    !exists $new_syms->{$_} && $_ !~ /^__/
} keys %{$ref_syms};
my @extra = sort grep {
    !exists $ref_syms->{$_} && $_ !~ /^__/
} keys %{$new_syms};

print "reference symbols: ", scalar(keys %{$ref_syms}), "\n";
print "giza libmgo symbols: ", scalar(keys %{$new_syms}), "\n";
print "missing from giza: ", scalar(@missing), "\n";
print "extra in giza: ", scalar(@extra), "\n";

if (@missing) {
    print "\nMissing (first 40):\n";
    print "$_\n" for @missing[0 .. min($#missing, 39)];
}
if (@extra) {
    print "\nExtra (first 20):\n";
    print "$_\n" for @extra[0 .. min($#extra, 19)];
}

exit(@missing ? 1 : 0);

sub min {
    my ($a, $b) = @_;
    return $a < $b ? $a : $b;
}

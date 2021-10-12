#!/usr/bin/perl -w
use v5.10;
use strict;

sub c {
  my ($v) = @_;
  $v = ord($v)-ord('0');
  return $v < 0 ? 0 : $v;
}

my $puzzle  = "5.1.6..24.6.4...73.7....1.5.....72.88.239.5473..284.9...56..4...2....31.946..17..";
my $spuzzle = "581763924269415873473928165694157238812396547357284691135672489728549316946831752";

my $row = 0;
my $sub = 0;
for (my $i = 0; $i < 81; $i=$i+3) {
  my $d = c(substr($puzzle,$i+0,1))
        | c(substr($puzzle,$i+1,1)) << 8
        | c(substr($puzzle,$i+2,1)) << 16;
  printf("  (*(uint32_t*)((size_t)0x%8X)) = 0x%06X;\n",0x30001200|$row<<4|$sub<<2,$d);
  if ( ++$sub == 3 ) {
    $sub = 0; ++$row;
  }
}

printf("\n\n");
$row = 0;
$sub = 0;
for (my $i = 0; $i < 81; $i=$i+3) {
  my $d = c(substr($spuzzle,$i+0,1))
        | c(substr($spuzzle,$i+1,1)) << 8
        | c(substr($spuzzle,$i+2,1)) << 16;
  printf("  result = (*(uint32_t*)((size_t)0x%8X)) == 0x%06X ? result : 0;\n",0x30001200|$row<<4|$sub<<2,$d);
  if ( ++$sub == 3 ) {
    $sub = 0; ++$row;
  }
}

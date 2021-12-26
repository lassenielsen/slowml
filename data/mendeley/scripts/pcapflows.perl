#!/usr/bin/perl -w

use strict;

use Data::Dumper;

sub mysystem {
  my ($s, $donothing) = @_;
  chomp $s;
  print "$s\n";
  if ( defined $donothing ) {
    return;
  }
  my $rv = system "$s > cmd.out";
  if ($? == -1) {
    die "failed to execute: $!\n";
  } elsif ($? & 127) {
    die sprintf "child died with signal %d, %s coredump\n",
      ($? & 127),  ($? & 128) ? 'with' : 'without';
  } elsif( $rv ) {
      $rv = $rv/256;
      die "$s exited with status $rv\n";
  }
  `cat cmd.out`;
}

# return 4-tuples. the protocol is always tcp.
sub identify_tcp_flows {
  my $pcapfn = shift;

  my %flows;

  open F, "tcpdump -n -r ${pcapfn} tcp |" or die "fozzle";
  while (<F>) {
    if ( m{
          \A
          (?<timestamp>
            \d{2} :
            \d{2} :
            \d{2} [\.] \d+
          ) \s+
          IP \s+
          (?<src_ip>
            \S+
          )
          [\.]
          (?<src_port>
            \d+
          ) \s+
          > \s+
          (?<dst_ip>
            \S+
          )
          [\.]
          (?<dst_port>
            \d+ | http
          ) : \s+
          Flags \s+
          [\[]
          (?<flags>
            [^\]]+
          )
          [\]] , \s+
          (
            seq \s+ \d+ , \s+
          )?
          (?<ack>
            ack \s+
            (?<ackbytes>
              \d+
            ) , \s+
          ) ?
        }xms
       ) {
      if ( ! exists $flows{"$+{dst_ip}:$+{dst_port}-$+{src_ip}:$+{src_port}"} ) {
        $flows{"$+{src_ip}:$+{src_port}-$+{dst_ip}:$+{dst_port}"} = {
                                                                        src_ip => $+{src_ip},
                                                                        src_port => $+{src_port},
                                                                        dst_ip => $+{dst_ip},
                                                                        dst_port => $+{dst_port},
                                                                       };
}
    } else {
#      warn "couldn't parse $_";
    }
  }
  \%flows
}

my $pcapfn = $ARGV[0];
my $r_h_flows = identify_tcp_flows $pcapfn;
for my $f ( keys %{ $r_h_flows } ) {
  mysystem "tcpdump -n -r ${pcapfn} -w " . $ARGV[1] . "_$f.pcap \"tcp and host $$r_h_flows{$f}{src_ip} and host $$r_h_flows{$f}{dst_ip} and port $$r_h_flows{$f}{src_port} and port $$r_h_flows{$f}{dst_port} \"";
}

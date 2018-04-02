#!/usr/bin/perl

# Programmed by Mike Schwartz
#
# /proc/meminfo
# /proc/stat
# /proc/loadavg

use Term::Screen;

$sleep = 1;
$compress = 0;

%disk = ();

sub Print {
  my ($row, $col, $prompt, $value) = @_;
  $scr->at($row, $col);
  $scr->bold(); $scr->puts($prompt); $scr->normal();
  $scr->puts(" $value");
}
sub Println {
  my ($row, $col, $prompt, $value) = @_;
  Print($row, $col, $prompt, $value);
  $scr->clreol();
}

sub WindowSize {
  $winch = 0;
  chop($size = `stty size`);
  ($rows, $cols) = split(/\s+/, $size);
  $colsx = $cols-1;
  $scr->resize($rows, $cols);
  $scr->clrscr();
}

sub GetStat {
  my %ret = ();
  open(IN, "/proc/stat");
  while (defined($in = <IN>)) {
    chop($in);
    if ($in =~ /^disk_io/) {
    	$in =~ s/diskio: //;
    	(@drives) = split(/\s+/, $in);
    	$disk = "";
    	$rio = "";
    	$wio = "";
    	$rblk = "";
    	$wblk = "";
    	for ($i=0; $i<=$#drives; $i++) {
    		($x,$y) = split(':', $drives[$i]);
    		$y =~ s/\(//;
    		$y =~ s/\)//;
    		($operations, $readops, $readsectors, $writeops, $writesectors) = split(',', $y);
    		if ($disk eq "") {
    			$disk = $operations;
    			$rio = $readops;
    			$rblk = $readsectors;
    			$wio = $writeops;
    			$wblk = $writesectors;
    		}
    		else {
    			$disk = join(' ', $disk, $operations);
    			$rio = join(' ', $rio, $readops);
    			$rblk = join(' ', $rblk, $readsectors);
    			$wio = join(' ', $wio, $writeops);
    			$wblk = join(' ', $wblk, $writesectors);
    		}
    	}
    	$ret{disk} = $disk;
    	$ret{disk_rio} = $rio;
    	$ret{disk_rblk} = $rblk;
    	$ret{disk_wio} = $wio;
    	$ret{disk_wblk} = $wblk;
    }
    else {
	    ($key, $val) = split(/\s+/, $in, 2);
  	  $ret{$key} = $val;
  	}
  }
  close(IN);
  return %ret;
}

sub InitState {
  %current = ();
  %initial = ();
  %stat = GetStat();
  foreach $key (keys(%stat)) {
    $current{$key} = join(':::', split(/\s+/, $stat{$key}));
    $initial{$key} = join(':::', split(/\s+/, $stat{$key}));
  }
  open(IN, "/proc/sys/kernel/hostname");
  chop($hostname=<IN>);
  close(IN);
  open(IN, "/proc/sys/kernel/osrelease");
  chop($release=<IN>);
  close(IN);
}

sub PrintHeader {
  $date = localtime;
  $row = 0;
  $scr->at($row++,0);
  $scr->reverse();
  $scr->puts(sprintf("%-$colsx.${colsx}s", " systat/$sleep [$hostname/$release] $date"));
  $scr->normal();
  $scr->clreol();
}


sub PrintLoadAverage {
  my (@load);
  open(IN, "/proc/loadavg");
  chop($in = <IN>);
  close(IN);
  ($l1, $l2, $l3, $procs) = split(/\s+/, $in);
  ($p1, $p2) = split('/', $procs);
#  $row++;
  $scr->at($row++, 0);
  $scr->bold();
  $scr->puts("Load Average:");
  $scr->normal();
  $scr->puts(" $l1 $l2 $l3 ");
  $scr->bold(); $scr->puts("Processes:"); $scr->normal(); $scr->puts(" $p2 ");
  $scr->bold(); $scr->puts("Running:"); $scr->normal(); $scr->puts(" $p1");
  $scr->clreol();
}

sub PrintCPU {
  $row++ unless ($compress);
  $scr->at($row++,0);
  $scr->bold()->reverse();
  print "CPU STATES           Current                   Aggregate        ";
  $scr->at($row++,0);
  print "             user   sys  nice   idle    user   sys  nice   idle ";
  $scr->normal();
  foreach $key (sort keys(%stat)) {
    next unless ($key =~ /^cpu/);
    $prompt = $key;
    $prompt =~ tr/a-z/A-Z/;
    ($user, $nice, $system, $idle) = split(/\s+/, $stat{$key});
    ($xuser, $xnice, $xsystem, $xidle) = split(':::', $current{$key});
    $yuser = $user - $xuser;
    $ynice = $nice - $xnice;
    $ysystem = $system - $xsystem;
    $yidle = $idle - $xidle;
    $current{$key} = join(':::', $user, $nice, $system, $idle);
    $total = $yuser + $ynice + $ysystem + $yidle;
    if ($total) {
      $zuser = sprintf("%4.1f", int($yuser/$total*1000+.5)/10);
      $znice = sprintf("%4.1f", int($ynice/$total*1000+.5)/10);
      $zsystem = sprintf("%4.1f", int($ysystem/$total*1000+.5)/10);
      $zidle = sprintf("%5.1f", int($yidle/$total*1000+.5)/10);
    }
    else {
      $zuser = $znice = $zsystem = $zidle = 0;
    }
    $scr->at($row++, 0);
    $scr->bold();
    printf("%-12.12s", $prompt);
    $scr->normal();
    print "$zuser% $zsystem% $znice% $zidle%";
    ($xuser, $xnice, $xsystem, $xidle) = split(':::', $initial{$key});
    $yuser = $user - $xuser;
    $ynice = $nice - $xnice;
    $ysystem = $system - $xsystem;
    $yidle = $idle - $xidle;
    $total = $yuser + $ynice + $ysystem + $yidle;
    if ($total) {
      $zuser = sprintf("%4.1f", int($yuser/$total*1000+.5)/10);
      $znice = sprintf("%4.1f", int($ynice/$total*1000+.5)/10);
      $zsystem = sprintf("%4.1f", int($ysystem/$total*1000+.5)/10);
      $zidle = sprintf("%5.1f", int($yidle/$total*1000+.5)/10);
    }
    else {
      $zuser = $znice = $zsystem = $zidle = 0;
    }
    print "   $zuser% $zsystem% $znice% $zidle%";
#    Println($row++,0, "$prompt States:", "$user% user, $system% system, $nice% nice, $idle% idle");
  }
}

# 2.4 kernel?
#sub PrintMemory {
#	%meminfo = ();
#  open(IN, "/proc/meminfo");
#  <IN>;
#  chop($mem = <IN>);
#  chop($swap = <IN>);
#  close(IN);
#  $row++ unless ($compress);
#  $scr->bold()->reverse();
#  $scr->at($row++, 0);
#  printf("%-10.10s  %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s",
#         "MEMORY", "Total", "Used", "Free", "Shared", "Buffer", "Cache", "Avail");
#  $scr->normal();
#  (@arr) = split(/\s+/, $mem);
#  shift(@arr);
#  $arr[6] = $arr[1] + $arr[2] - $arr[4] - $arr[5];
#  for ($i=0; $i<=$#arr; $i++) { $arr[$i] = int($arr[$i]/1024) . "K"; }
#  Println($row++,0, sprintf("%-10.10s ", "Real"), sprintf("%8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s", @arr));
#  (@arr) = split(/\s+/, $swap);
#  shift(@arr);
#  for ($i=0; $i<=$#arr; $i++) { $arr[$i] = int($arr[$i]/1024) . "K"; }
#  Println($row++,0, sprintf("%-10.10s ", "Swap"), sprintf("%8.8s %8.8s %8.8s", @arr));
#}

# 2.6 kernel
sub PrintMemory {
	%meminfo = ();
  open(IN, "/proc/meminfo");
  while (1) {
  	$in = <IN>;
  	last unless ($in);
  	$in =~ s/\s+//g;
  	($key,$value) = split(':', $in);
  	$value =~ s/B$//;
  	$meminfo{$key} = $value;
  }
  close(IN);
  $meminfo{MemUsed} = $meminfo{MemTotal} - $meminfo{MemFree};
  $meminfo{MemAvail} = $meminfo{MemFree} + $meminfo{Cached} + $meminfo{Buffers};
  $meminfo{SwapUsed} = $meminfo{SwapTotal} - $meminfo{SwapFree};
  $row++ unless ($compress);
  $scr->bold()->reverse();
  $scr->at($row++, 0);
  printf("%-10.10s  %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s",
         "MEMORY", "Total", "Used", "Free", "Shared", "Buffer", "Cache", "Avail");
  $scr->normal();
  Println($row++,0, sprintf("%-10.10s ", "Real"), sprintf("%8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s", 
  				$meminfo{MemTotal}, 
  				$meminfo{MemUsed}, 
  				$meminfo{MemFree},
  				int($meminfo{MemShared}), 
  				$meminfo{Buffers}, 
  				$meminfo{Cached}, 
  				$meminfo{MemAvail}
  				));
  (@arr) = split(/\s+/, $swap);
  shift(@arr);
  for ($i=0; $i<=$#arr; $i++) { $arr[$i] = int($arr[$i]/1024) . "K"; }
  Println($row++,0, sprintf("%-10.10s ", "Swap"), sprintf("%8.8s %8.8s %8.8s", 
  				$meminfo{SwapTotal},
  				$meminfo{SwapUsed},
  				$meminfo{SwapFree},
  				));
}

# kernel 2.4
#sub PrintDisk {
#  $row++ unless ($compress);
#  $scr->at($row++,0);
#  $scr->bold(); $scr->reverse();
#  printf("%-16s", "DISK ACTIVITY");
#  printf("%-24s", "       CURRENT");
#  if ($cols >= 80) {
#    print "   ";
#    printf("%-34s", "           AGGREGATE");
#  }
#  
#  $scr->at($row++,0);
#  printf("%-12.12s", "");
#  printf("%6s ", "Disk 1");
#  printf("%6s ", "Disk 2");
#  printf("%6s ", "Disk 3");
#  printf("%6s ", "Disk 4");
#  if ($cols >= 80) {
#    print " ";
#    printf("%8s ", "Disk 1");
#    printf("%8s ", "Disk 2");
#    printf("%8s ", "Disk 3");
#    printf("%8s ", "Disk 4");
#  }
#  
#  $scr->normal();
#  $scr->clreol();
#  foreach $key (sort keys(%current)) {
##    Println($row++, 0, $key, $current{$key});
#    next unless ($key =~ /^disk/);
#    ($xdisk1, $xdisk2, $xdisk3, $xdisk4) = split(':::', $current{$key});
#    ($zdisk1, $zdisk2, $zdisk3, $zdisk4) = split(':::', $initial{$key});
#    ($disk1, $disk2, $disk3, $disk4) = split(/\s+/, $stat{$key});
#    $ydisk1 = sprintf("%6d", $disk1-$xdisk1);
#    $ydisk2 = sprintf("%6d", $disk2-$xdisk2);
#    $ydisk3 = sprintf("%6d", $disk3-$xdisk3);
#    $ydisk4 = sprintf("%6d", $disk4-$xdisk4);
#    $disk1a = sprintf("%8d", $disk1-$zdisk1);
#    $disk2a = sprintf("%8d", $disk2-$zdisk2);
#    $disk3a = sprintf("%8d", $disk3-$zdisk3);
#    $disk4a = sprintf("%8d", $disk4-$zdisk4);
#    $current{$key} = join(':::', $disk1, $disk2, $disk3, $disk4);
#    $scr->at($row++,0);
#    $scr->bold();
#    printf("%-9s   ", $key);
#    $scr->normal();
#    print "$ydisk1 $ydisk2 $ydisk3 $ydisk4";
#    if ($cols >= 80) {
#      print "  $disk1a $disk2a $disk3a $disk4a";
#    }
#    $scr->clreol();
#  }
#}


# From http://www.mjmwired.net/kernel/Documentation/iostats.txt (/proc/diskstats):
#61  Field  1 -- # of reads issued
#62	    This is the total number of reads completed successfully.
#63	Field  2 -- # of reads merged, field 6 -- # of writes merged
#64	    Reads and writes which are adjacent to each other may be merged for
#65	    efficiency.  Thus two 4K reads may become one 8K read before it is
#66	    ultimately handed to the disk, and so it will be counted (and queued)
#67	    as only one I/O.  This field lets you know how often this was done.
#68	Field  3 -- # of sectors read
#69	    This is the total number of sectors read successfully.
#70	Field  4 -- # of milliseconds spent reading
#71	    This is the total number of milliseconds spent by all reads (as
#72	    measured from __make_request() to end_that_request_last()).
#73	Field  5 -- # of writes completed
#74	    This is the total number of writes completed successfully.
#75	Field  7 -- # of sectors written
#76	    This is the total number of sectors written successfully.
#77	Field  8 -- # of milliseconds spent writing
#78	    This is the total number of milliseconds spent by all writes (as
#79	    measured from __make_request() to end_that_request_last()).
#80	Field  9 -- # of I/Os currently in progress
#81	    The only field that should go to zero. Incremented as requests are
#82	    given to appropriate request_queue_t and decremented as they finish.
#83	Field 10 -- # of milliseconds spent doing I/Os
#84	    This field is increases so long as field 9 is nonzero.
#85	Field 11 -- weighted # of milliseconds spent doing I/Os
#86	    This field is incremented at each I/O start, I/O completion, I/O
#87	    merge, or read of these stats by the number of I/Os in progress
#88	    (field 9) times the number of milliseconds spent doing I/O since the
#89	    last update of this field.  This can provide an easy measure of both
#90	    I/O completion time and the backlog that may be accumulating.
%disk_reads = ();
%disk_writes = ();
sub PrintDisk {
  $row++ unless ($compress);
  $scr->at($row++, 0);
  $scr->bold()->reverse();
#  print "VIRTUAL MEMORY     Current          Aggregate  ";
  print "DISK ACTIVITY      Current          Aggregate  ";
  $scr->at($row++, 0);
  print "                  IN   OUT        IN       OUT ";
  $scr->normal();
  open(IN, "/proc/diskstats");
  while (1) {
  	$in = <IN>;
  	last unless ($in);
  	$in =~ s/^\s+//g;
  	(@arr) = split(/\s+/, $in);
 		$drive = $arr[2];
    next if ($drive =~ /p\d$/ || $arr[3] == 0);
	  $scr->at($row++,0);
	  $scr->bold();
	  printf("%-16.16s", "/dev/" . $drive);
	  $scr->normal();
	  $reads = $arr[5];
	  $delta_reads = $reads - $disk_reads{$drive};
	  $disk_reads{$drive} = $reads;
	  $writes = $arr[8];
	  $delta_writes = $writes - $disk_writes{$drive};
	  $disk_writes{$drive} = $writes;
	  printf("%4d  %4d  %8d  %8d", $delta_reads, $delta_writes, $reads, $writes);
    $scr->clreol();
	}
}

# kernel 2.4?
#sub PrintVM {
#  $row++ unless ($compress);
#  $scr->at($row++, 0);
#  $scr->bold()->reverse();
#  print "VIRTUAL MEMORY     Current          Aggregate  ";
#  $scr->at($row++, 0);
#  print "                  IN   OUT        IN       OUT ";
#  $scr->normal();
#  ($page1, $page2) = split(/\s+/, $stat{page});
#  ($xpage1, $xpage2) = split(':::', $current{page});
#  $ypage1 = $page1 - $xpage1;
#  $ypage2 = $page2 - $xpage2;
#  $current{page} = join(':::', $page1, $page2);
#  $scr->at($row++,0);
#  $scr->bold();
#  printf("%-16.16s", "page");
#  $scr->normal();
#  printf("%4d  %4d", $ypage1, $ypage2);
#  ($xpage1, $xpage2) = split(':::', $initial{page});
#  $ypage1 = $page1 - $xpage1;
#  $ypage2 = $page2 - $xpage2;
#  printf("  %8d  %8d", $ypage1, $ypage2);
#  $scr->clreol();
#  # swap
#  ($swap1, $swap2) = split(/\s+/, $stat{swap});
#  ($xswap1, $xswap2) = split(':::', $current{swap});
#  $yswap1 = $swap1 - $xswap1;
#  $yswap2 = $swap2 - $xswap2;
#  $current{swap} = join(':::', $swap1, $swap2);
#  $scr->at($row++,0);
#  $scr->bold();
#  printf("%-16.16s", "swap");
#  $scr->normal();
#  printf("%4d  %4d", $yswap1, $yswap2);
#  ($xswap1, $xswap2) = split(':::', $initial{swap});
#  $yswap1 = $swap1 - $xswap1;
#  $yswap2 = $swap2 - $xswap2;
#  printf("  %8d  %8d", $yswap1, $yswap2);
#  $scr->clreol();
#}


$lastpgin = 0;
$lastpgout = 0;
$lastswpin = 0;
$lastswpout = 0;

sub PrintVM {
	open(IN, "/proc/vmstat");
	%vmstat = ();
	while (1) {
		$in = <IN>;
		last unless ($in);
		($key, $value) = split(/\s+/, $in);
		$vmstat{$key} = $value;
	}
	close(IN);
	unless ($lastpgin) {
		$lastpgin = $vmstat{pgpgin};
		$lastpgout = $vmstat{pgpgout};
		$lastswpin = $vmstat{pgswpin};
		$lastswpout = $vmstat{pgswpout};
	}
  $row++ unless ($compress);
  $scr->at($row++, 0);
  $scr->bold()->reverse();
  print "VIRTUAL MEMORY     Current          Aggregate  ";
  $scr->at($row++, 0);
  print "                  IN   OUT        IN       OUT ";
  $scr->normal();
  $scr->at($row++,0);
  $scr->bold();
  printf("%-16.16s", "page");
  $scr->normal();
  $pgin = $vmstat{pgpgin};
  $deltain = $pgin - $lastpgin;
  $lastpgin = $pgin;
	$pgout = $vmstat{pgpgout};
	$deltaout = $pgout - $lastpgout;
	$lastpgout = $pgout;
  printf("%4d  %4d  %8d  %8d", $deltain, $deltaout, $pgin, $pgout);
  $scr->clreol();
  $scr->at($row++,0);
  $scr->bold();
  printf("%-16.16s", "swap");
  $scr->normal();
  $swpin = $vmstat{pswpin};
  $deltain = $swpin - $lastswpin;
  $lastswpin = $swpin;
	$swpout = $vmstat{pgswpout};
	$deltaout = $swpout - $lastswpout;
	$lastswpout = $swpout;
  printf("%4d  %4d  %8d  %8d", $deltain, $deltaout, $swpin, $swpout);
  $scr->clreol();
}

InitState();

$winch = 0;
$SIG{'WINCH'} = sub { $winch = 1; };

$scr = new Term::Screen;
WindowSize();

$scr->noecho();
while (1) {
  if ($winch) {
    WindowSize();
  }
  PrintHeader();
  %stat = GetStat();

  PrintLoadAverage();
  PrintCPU();
  PrintMemory();
  PrintDisk();
  PrintVM();
  $scr->at(0,$colsx);
  if ($scr->key_pressed($sleep)) {
    $c = $scr->getch();
    last if ($c eq "q" || ord($c) == 3 || ord($c) == 4);
    if ($c eq "+") {
      $sleep++;
    }
    elsif ($c eq "-") {
      $sleep--;
      $sleep = 1 if ($sleep < 1);
    }
    elsif ($c eq "c") {
      $compress = 1 - $compress;
      $scr->clrscr();
    }
  }
}
$scr->echo();
$scr->clrscr();
$scr->flush_input();
__END__

    1 users    Load  0.00  0.00  0.00                  Tue Jan 18 11:49

Mem:KB    REAL            VIRTUAL                     VN PAGER  SWAP PAGER
        Tot   Share      Tot    Share    Free         in  out     in  out
Act    9152    1284    12844     1304   57188 count
All   70572    1820   710300     2088         pages
                                                          cow    Interrupts
Proc:r  p  d  s  w    Csw  Trp  Sys  Int  Sof  Flt        zfod    229 total
           3  5         2        14  229    2       17476 wire    100 clk0 irq0
                                                    47108 act     128 rtc0 irq8
 0.0%Sys   0.0%Intr  0.2%User  0.0%Nice 99.8%Idl     6092 inact     1 pci irq11
|    |    |    |    |    |    |    |    |    |            cache       wdc0 irq14
                                                    57188 free        fdc0 irq6
                                                          daefr
Namei         Name-cache    Dir-cache                     prcfr
    Calls     hits    %     hits    %                     react
                                                          pdwake
                                                          pdpgs
Discs  wd0                                                intrn
seeks                                                8348 buf
xfers
 blks
 msps



[mykes@db /proc]$ cat stat
cpu  310515 636 233664 1051183769
cpu0 176182 262 118431 525569417
cpu1 134333 374 115233 525614352
disk 4482979 0 0 0
disk_rio 1473115 0 0 0
disk_wio 3009864 0 0 0
disk_rblk 11784884 0 0 0
disk_wblk 24078912 0 0 0
page 10941883 13265050
swap 2993 1850
intr 541463070 525864292 667 0 1181 301 0 3 0 1 0 0 0 443 1 6 0 0 4791345 0 10804830 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 68507417
btime 942974280
processes 117781
[mykes@db /proc]$ 

disk_io: (8,0):(2677742,2098625,53557948,579117,10629650) (8,1):(170555,55313,4888610,115242,10531144)

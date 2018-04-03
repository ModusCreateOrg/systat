# systat
__Real-Time System Status for Mac/Linux.__

## Overview
This program refreshes the console/terminal/screen once per second and prints
useful information about the system resources.  Displayed are:

1) OS and Kernel version
2) Uptime and Load Average
3) CPU state (and per core if the console is tall enough)
4) Memory usage (RAM and Swap), free, buffers, etc.
5) Disk activity (per device) - bytes/IOs for read/write
6) Virtual Memory activity - paging, swapping info
7) Network activity per interface - read/write bytes/second, errors, etc.

It is based on a Perl script I wrote many years ago for Linux that performs most
of these functions.  The Perl script requires you to install Term::Screen from
CPAN.

The Perl script is loosely based upon the systat -vmstat command for BSD
(FreeBSD at least).

The reason I rewrote systat in C++ is to make it portable between MacOS and
Linux, and to assure it uses a minimal amount of resources itself.  You don't
want to be measuring systat performance while diagnosing a busy system!  I also
added the networking information, which was absent in the Perl script.

## Building for MacOS

The MacOS version was built using CLion from JetBrains.  The program uses a lot
of core data structures from numerous system header files.  The CLion debugger
was handy for its ability to inspect struct and class values.

CLion is not required, though.

You can build the MacOS version:
```
cd c
cmake --build ./cmake-build-debug --target systat-mac --
# to run it:
./cmake-build-debug/systat-mac
# to install it:
sudo install ./cmake-build-debug/systat-mac /usr/local/bin/systat
```

## Building for Linux

The Linux version was built using vim and gnu make/Makefiles.  To build it:
```
cd c/linux
make
# to run it:
./systat
# to install it:
make install
```

The Linux version can be built using cmake:
```
cd c
cmake --build ./cmake-build-debug --target systat-linux --
# to run it:
./cmake-build-debug/systat-linux
# to install it:
sudo install ./cmake-build-debug/systat-linux /usr/local/bin/systat
```


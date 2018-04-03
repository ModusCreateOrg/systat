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

The information printed is more detailed if there are enough lines in the
terminal to print it.  That is, if you make your console window tall enough, you
will get CPU information for every core of your CPU.  Otherwise, you only get
the combined CPU state.

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

The Linux version can be built using CLion, too.  In CLion, open the c/
directory and set target to systat-linux.

## How it works

Both MacOS and Linux versions are structured in a similar manner and share the
classes in the common/ directory.  Using Makefile build, libcommon.a is built in
common/, otherwise the classes are built and linked in, and no library is built.

One of the design goals is to make the program build with no prerequisites.  You
won't have to install ncurses or any other library.  The Linux version links
with no libraries other than the standard C++ ones.  The Mac version has to link
with various MacOS frameworks to access the needed APIs.

For each of the following classes, there is a global singleton instance,
declared in the .cpp file for the class.  I like it better to do:
```c++
console.println("hello, world");
```
instead of
```c++
Console::println("hello, world");
```
and I get to take advantage of the class constructors to initialize it all.

There are a few platform specific classes in the c/mac/ and c/linux/
directories.  These directories should contain identical filenames/classes.

The main.cpp file is the main program.  It basically calls each of the platform
specific class' update() methods once per second.  It then calls each of the
platform specific class' print() methods.

The platform specific classes contain the business logic to obtain the useful
information that is printed by systat.  It took quite a bit of discovery to
learn which /proc files to parse on Linux systems, and what OSX/Darwin API calls
to make to get the desired information/samples.

The print() methods take a bool argument "test" that if set, the routine only
returns the number of lines that would be printed.  This is so the main program
can "fit" the displayed information to the console, based upon its height.  The
fit process involves condensing the various output (what the classes would 
print), so the informatioj00jn printed does fit the console.  It is possible 
to make the console too small - this is a TODO to handle this case.

### Console Class

The Console class implements an api to address the cursor, control the display
modes (e.g. bold, fg/bg colors, etc.), print output (printf format style), and
clear lines/whole screen.

It is implemented to use ANSI escape sequences.  This is lightweight, but
probably not perfect.  I tested using several terminal programs for Linux and
iterm2 on Mac.  It works fine.

This satisfies the design goal of not relying on some 3rd party library that
would need to be installed as a dependency.

A SIGINT handler is installed to call the console's destructor, which restores
the TTY to a sane state (e.g. enable cursor, default fg/bg colors, etc.).

The program does not do any keyboard I/O or interaction.  To quit the program,
hit ^C (Control+c).

### Parser Class

This is only used by the Linux version, which mainly gets system information by
parsing files in the /proc filesystem.  Parser does a lot of the work, making
the business logic for parsing these files rather elegant.

### Log Class

If you include "../common/Log.h" in any of the .cpp/.h files, then there will be
a global logger singleton.  The constructor opens/creates /tmp/systat.log, which
you can "tail -f" to see debug output.  Logger.println(FORMAT, ...) to print
debugging to the log file.

## License


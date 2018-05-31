#  systat for Mac
# 
#  Programmed by Mike Schwartz <mike@moduscreate.com>
# 
#  Command line tool that refreshes the terminal/console window each second,
#  showing uptime, load average, CPU usage/stats, Memory/Swap usage, Disk
#  Activity (per drive/device), Virtual Memory activity (paging/swapping), and
#  Network traffic (per interface).
# 
#  Run this on a busy system and you can diagnose if:
#  1) System is CPU bound
#  2) System is RAM bound
#  3) System is Disk bound
#  4) System is Paging/Swapping heavily
#  5) System is Network bound
# 
#  To exit, hit ^C.
 
OBJ=	main.o Platform.o Disk.o Processor.o Memory.o VirtualMemory.o Network.o

CPP= clang++

CFLAGS = -std=c++11 -stdlib=libc++ 

DEBUG?=0

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $*.o $*.cpp -DDEBUG=$(DEBUG)


systat: .depend $(OBJ) ../common/libcommon.a
	$(CPP) -o systat $(OBJ) -L ../common -lcommon -DDEBUG=$(DEBUG) -framework IOKit -framework CoreFoundation


$%.o: %.cpp %.h

../common/libcommon.a: ../common/*.cpp ../common/*.h
	cd ../common && $(MAKE) DEBUG=$(DEBUG)

debug:
	$(MAKE) clean && DEBUG=1 $(MAKE)

clean:
	rm -f *.o systat .depend
	cd ../common && $(MAKE) clean

install: systat
	install systat /usr/local/bin

depend: .depend

.depend:	*.cpp
	rm -f ./.depend
	$(CPP) -MM $^ -MF ./.depend

include .depend


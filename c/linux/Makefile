OBJ=	main.o Platform.o Disk.o Processor.o Memory.o VirtualMemory.o Network.o

CFLAGS = -g

DEBUG?=0

.cpp.o:
	g++ $(CFLAGS) -c -o $*.o $*.cpp -DDEBUG=$(DEBUG)


systat: .depend $(OBJ) ../common/libcommon.a
	g++ -o systat $(OBJ) -L ../common -lcommon -DDEBUG=$(DEBUG)


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
	g++ -MM $^ -MF ./.depend

include .depend


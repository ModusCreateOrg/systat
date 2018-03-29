OBJ=	main.o Parser.o Platform.o Disk.o Processor.o Memory.o VirtualMemory.o

CFLAGS = -g

DEBUG?=0

.cpp.o:
	g++ $(CFLAGS) -c -o $*.o $*.cpp -DDEBUG=$(DEBUG)


systat: $(OBJ) ../common/libcommon.a
	g++ -o systat $(OBJ) -L ../common -lcommon -DDEBUG=$(DEBUG)


$%.o: %.cpp %.h

../common/libcommon.a: ../common/*.cpp ../common/*.h
	cd ../common && make DEBUG=$(DEBUG)

debug:
	make clean && DEBUG=1 make

clean:
	rm -f *.o systat
	cd ../common && make clean

OBJ=	Console.o

.cpp.o:
	g++ -c -o $*.o $*.cpp -DDEBUG=$(DEBUG)

libcommon.a: depend $(OBJ) Console.h
	ar r libcommon.a $(OBJ)

clean:
	rm -f libcommon.a $(OBJ)

debug:
	make clean && DEBUG=1 make

depend: .depend

.depend:	*.cpp
	rm -f ./.depend
	g++ -MM $^ -MF ./.depend

include .depend


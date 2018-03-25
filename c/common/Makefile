OBJ=	Console.o

.cpp.o:
	g++ -c -o $*.o $*.cpp

libcommon.a: depend $(OBJ) Console.h
	ar r libcommon.a $(OBJ)

depend: .depend

.depend:	*.cpp
	rm -f ./.depend
	g++ -MM $^ -MF ./.depend

include .depend


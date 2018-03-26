OBJ=	main.o Parser.o Platform.o Processor.o Memory.o

CFLAGS = -g

.cpp.o:
	g++ $(CFLAGS) -c -o $*.o $*.cpp


systat: $(OBJ) ../common/libcommon.a
	g++ -o systat $(OBJ) -L ../common -lcommon


$%.o: %.cpp %.h

../common/libcommon.a: ../common/*.cpp ../common/*.h
	cd ../common && make

clean:
	rm -f *.o systat

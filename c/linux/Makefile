OBJ=	main.o Parser.o Processor.o

.cpp.o:
	g++ -c -o $*.o $*.cpp


systat: $(OBJ) ../common/libcommon.a
	g++ -o systat $(OBJ) -L ../common -lcommon


$%.o: %.cpp %.h

../common/libcommon.a: ../common/*.cpp ../common/*.h
	cd ../common && make

clean:
	rm -f *.o systat

.DEFAULT_GOAL := systat

OBJDIR=o

OBJ=$(OBJDIR)/main.o $(OBJDIR)/cpu.o $(OBJDIR)/network.o

$(OBJDIR)/%.o: %.cpp
	gcc -g -c -o o/$*.o $*.cpp

$(OBJ): systat.h

systat: $(OBJ)
	gcc -g -o systat $(OBJ)

clean:
	rm -f $(OBJ)



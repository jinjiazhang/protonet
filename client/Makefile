INC_PATH = ../protonet
LIB_PATH = ../bin
BIN_PATH = ../bin
LIB = protonet pthread

CC = g++
CC_FLAG = 
LINK_FLAG = 

PRG = client
OBJ = main.o

all:$(PRG)

$(PRG):$(OBJ)
	$(CC) -o $@ $(OBJ) $(LINK_FLAG) $(LIB_PATH:%=-L%) $(LIB:%=-l%)

.SUFFIXES: .c .cc .o .cpp
.cpp.o:
	$(CC) $(CC_FLAG) $(INC_PATH:%=-I%) -c $*.cpp -o $*.o

.cc.o:
	$(CC) $(CC_FLAG) $(INC_PATH:%=-I%) -c $*.cc -o $*.o

.PRONY:clean
clean:
	rm -f $(OBJ) $(PRG)

install:
	cp -f $(PRG) $(BIN_PATH)


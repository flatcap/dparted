CC	= g++

SRC	= container.cpp device.cpp filesystem.cpp lvm.cpp main.cpp partition.cpp
HDR	= container.h device.h filesystem.h lvm.h partition.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main p

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 libparted --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags p.o

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

p:	p.o
	$(CC) -o $@ p.o $(LDFLAGS)

tags:   force
	ctags *.[ch]

force:


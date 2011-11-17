CC	= g++

SRC	= container.cpp device.cpp filesystem.cpp lvm.cpp main.cpp partition.cpp utils.cpp
HDR	= container.h device.h filesystem.h lvm.h partition.h utils.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main p l

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 libparted lvm2app devmapper --cflags`
LDFLAGS += `pkg-config glibmm-2.4 libparted lvm2app devmapper --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags p.o l.o

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

l:	l.o
	$(CC) -o $@ l.o $(LDFLAGS)

p:	p.o
	$(CC) -o $@ p.o $(LDFLAGS)

tags:   force
	ctags *.[ch]

force:


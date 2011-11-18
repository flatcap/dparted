CC	= g++

SRC	= container.cpp disk.cpp filesystem.cpp lvm.cpp main.cpp partition.cpp utils.cpp
HDR	= container.h disk.h filesystem.h lvm.h partition.h utils.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags p.o l.o

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

l:	l.o
	$(CC) -o $@ $^ $(LDFLAGS)

p:	p.o
	$(CC) -o $@ $^ $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:


CC	= g++

SRC	= container.cpp disk.cpp filesystem.cpp main.cpp partition.cpp utils.cpp volume.cpp volumegroup.cpp
HDR	= container.h disk.h filesystem.h partition.h utils.h volume.h volumegroup.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags l.o m.o p.o l m p

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

l:	l.o
	$(CC) -o $@ $^ $(LDFLAGS)

m:	m.o
	$(CC) -o $@ $^ $(LDFLAGS)

p:	p.o
	$(CC) -o $@ $^ $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:


CC	= g++

SRC	= container.cpp disk.cpp filesystem.cpp main.cpp mount.cpp partition.cpp utils.cpp volume.cpp volumegroup.cpp
HDR	= container.h disk.h filesystem.h mount.h partition.h utils.h volume.h volumegroup.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags l.o m.o p.o s.o l m p s

main: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

l:	l.o
	$(CC) -o $@ $^ $(LDFLAGS)

m:	m.o
	$(CC) -o $@ $^ $(LDFLAGS)

p:	p.o
	$(CC) -o $@ $^ $(LDFLAGS)

s:	s.o
	$(CC) -o $@ $^ $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:

container.o:	container.cpp container.h utils.h
disk.o:		disk.cpp disk.h container.h utils.h
filesystem.o:	filesystem.cpp filesystem.h container.h partition.h utils.h
main.o:		main.cpp container.h disk.h filesystem.h partition.h volumegroup.h volume.h utils.h
mount.o:	mount.cpp mount.h container.h utils.h
partition.o:	partition.cpp partition.h container.h utils.h
utils.o:	utils.cpp utils.h
volume.o:	volume.cpp volume.h container.h utils.h
volumegroup.o:	volumegroup.cpp volumegroup.h container.h utils.h


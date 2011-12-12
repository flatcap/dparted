CC	= g++

SRC	= container.cpp disk.cpp filesystem.cpp main.cpp metadata.cpp mount.cpp msdos.cpp partition.cpp segment.cpp utils.cpp volume.cpp volumegroup.cpp whole.cpp
HDR	= container.h disk.h filesystem.h metadata.h mount.h msdos.h partition.h segment.h utils.h volume.h volumegroup.h whole.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags

main: $(OBJ) tags
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:

container.o:	container.cpp container.h utils.h
disk.o:		disk.cpp disk.h container.h utils.h
filesystem.o:	filesystem.cpp filesystem.h container.h partition.h utils.h
main.o:		main.cpp container.h disk.h filesystem.h mount.h msdos.h partition.h segment.h volumegroup.h whole.h volume.h utils.h
metadata.o:	metadata.cpp metadata.h container.h utils.h
mount.o:	mount.cpp mount.h container.h utils.h
msdos.o:	msdos.cpp msdos.h container.h utils.h
partition.o:	partition.cpp partition.h container.h utils.h
segment.o:	segment.cpp segment.h container.h
utils.o:	utils.cpp utils.h
volume.o:	volume.cpp volume.h whole.h container.h utils.h
volumegroup.o:	volumegroup.cpp volumegroup.h whole.h container.h utils.h
whole.o:	whole.cpp whole.h container.h

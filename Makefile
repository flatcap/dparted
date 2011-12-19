CC	= g++

SRC	= block.cpp container.cpp datapartition.cpp disk.cpp extended.cpp filesystem.cpp main.cpp metadata.cpp msdos.cpp partition.cpp partitiontable.cpp segment.cpp utils.cpp volume.cpp volumegroup.cpp whole.cpp
HDR	= block.h container.h datapartition.h disk.h extended.h filesystem.h metadata.h msdos.h partition.h partitiontable.h segment.h utils.h volume.h volumegroup.h whole.h
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

block.o:		block.cpp block.h container.h
container.o:		container.cpp container.h utils.h stringnum.h
datapartition.o:	datapartition.cpp datapartition.h partition.h container.h utils.h stringnum.h
disk.o:			disk.cpp disk.h block.h container.h utils.h stringnum.h
extended.o:		extended.cpp extended.h partition.h container.h utils.h stringnum.h
filesystem.o:		filesystem.cpp filesystem.h container.h partition.h utils.h stringnum.h
loop.o:			loop.cpp loop.h block.h container.h
main.o:			main.cpp container.h datapartition.h partition.h disk.h block.h extended.h filesystem.h loop.h metadata.h msdos.h partitiontable.h segment.h volumegroup.h whole.h volume.h utils.h stringnum.h
metadata.o:		metadata.cpp metadata.h partition.h container.h utils.h stringnum.h
msdos.o:		msdos.cpp msdos.h partitiontable.h container.h utils.h stringnum.h
partition.o:		partition.cpp partition.h container.h
partitiontable.o:	partitiontable.cpp partitiontable.h container.h
segment.o:		segment.cpp segment.h container.h
utils.o:		utils.cpp utils.h stringnum.h container.h
volume.o:		volume.cpp volume.h whole.h container.h utils.h stringnum.h
volumegroup.o:		volumegroup.cpp volumegroup.h whole.h container.h utils.h stringnum.h
whole.o:		whole.cpp whole.h container.h segment.h
block.o:		block.h container.h
container.o:		container.h
datapartition.o:	datapartition.h partition.h container.h
disk.o:			disk.h block.h container.h
extended.o:		extended.h partition.h container.h
filesystem.o:		filesystem.h container.h
loop.o:			loop.h block.h container.h
metadata.o:		metadata.h partition.h container.h
msdos.o:		msdos.h partitiontable.h container.h
partition.o:		partition.h container.h
partitiontable.o:	partitiontable.h container.h
segment.o:		segment.h container.h
stringnum.o:		stringnum.h container.h
utils.o:		utils.h stringnum.h container.h
volumegroup.o:		volumegroup.h whole.h container.h
volume.o:		volume.h whole.h container.h
whole.o:		whole.h container.h


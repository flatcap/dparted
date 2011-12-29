CC	= g++
RM	= rm -fr

SRC	= block.cpp container.cpp datapartition.cpp disk.cpp extended.cpp \
	  filesystem.cpp gpt.cpp loop.cpp main.cpp metadata.cpp mirror.cpp \
	  msdos.cpp partition.cpp partitiontable.cpp segment.cpp stripe.cpp \
	  utils.cpp  volume.cpp volumegroup.cpp whole.cpp

HDR	= block.h container.h datapartition.h disk.h extended.h filesystem.h \
	  gpt.h loop.h metadata.h mirror.h msdos.h partition.h \
	  partitiontable.h segment.h stringnum.h stripe.h utils.h \
	  volumegroup.h volume.h whole.h

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

distclean: clean
	$(RM) html

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
gpt.o:			gpt.cpp gpt.h partitiontable.h container.h
loop.o:			loop.cpp loop.h block.h container.h
main.o:			main.cpp container.h datapartition.h partition.h disk.h block.h extended.h filesystem.h gpt.h partitiontable.h loop.h metadata.h mirror.h volume.h whole.h msdos.h segment.h stripe.h volumegroup.h utils.h stringnum.h
metadata.o:		metadata.cpp metadata.h partition.h container.h utils.h stringnum.h
mirror.o:		mirror.cpp mirror.h volume.h whole.h container.h utils.h stringnum.h
msdos.o:		msdos.cpp msdos.h partitiontable.h container.h utils.h stringnum.h
partition.o:		partition.cpp partition.h container.h
partitiontable.o:	partitiontable.cpp partitiontable.h container.h
segment.o:		segment.cpp segment.h container.h
stripe.o:		stripe.cpp stripe.h volume.h whole.h container.h utils.h stringnum.h
utils.o:		utils.cpp utils.h stringnum.h container.h
volume.o:		volume.cpp volume.h whole.h container.h utils.h stringnum.h
volumegroup.o:		volumegroup.cpp volumegroup.h whole.h container.h utils.h stringnum.h
whole.o:		whole.cpp whole.h container.h segment.h


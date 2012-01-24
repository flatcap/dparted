CC	= g++
RM	= rm -fr

SRC	= block.cpp container.cpp disk.cpp extended.cpp \
	  file.cpp filesystem.cpp gpt.cpp identify.cpp linear.cpp loop.cpp \
	  main.cpp mirror.cpp msdos.cpp partition.cpp table.cpp \
	  segment.cpp stripe.cpp utils.cpp  volume.cpp volumegroup.cpp whole.cpp

HDR	= block.h container.h disk.h extended.h file.h \
	  filesystem.h gpt.h identify.h linear.h loop.h mirror.h msdos.h \
	  partition.h table.h segment.h stringnum.h stripe.h utils.h \
	  volumegroup.h volume.h whole.h

DEP	= $(SRC:.cpp=.d)

OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(OUT)

dep deps: $(DEP)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o
	$(CC) -MM $(CFLAGS) -c $*.cpp > $*.d 

clean:
	$(RM) $(OUT) $(OBJ) tags

distclean: clean
	$(RM) $(DEP) html

main: $(OBJ) tags
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:

-include $(DEP)


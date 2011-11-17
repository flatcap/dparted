CC	= g++

SRC	= container.cpp device.cpp filesystem.cpp lvm.cpp main.cpp partition.cpp
HDR	= container.h device.h filesystem.h lvm.h partition.h
OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 --cflags`
LDFLAGS += `pkg-config glibmm-2.4 --libs`

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT) $(OBJ) tags

$(OUT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

tags:   force
	ctags *.[ch]

force:


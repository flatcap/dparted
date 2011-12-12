GPP	= g++
GCC	= gcc

OUT	= cast compare format fs inherit loop lvm map parted spawn

CFLAGS	= -g -Wall

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT)

cast:	cast.cpp
	$(GPP) $(CFLAGS) -o $@ cast.cpp

compare:	compare.cpp
	$(GPP) $(CFLAGS) -o $@ compare.cpp

format:	format.cpp
	$(GPP) $(CFLAGS) -o $@ format.cpp

fs:	fs.c
	$(GCC) $(CFLAGS) -o $@ fs.c $(shell pkg-config --cflags --libs libparted)

inherit:	inherit.cpp
	$(GPP) $(CFLAGS) -o $@ inherit.cpp

loop:	loop.c
	$(GCC) $(CFLAGS) -o $@ loop.c $(shell pkg-config --cflags --libs libparted devmapper) -lm

lvm:	lvm.c
	$(GCC) $(CFLAGS) -o $@ lvm.c $(shell pkg-config --cflags --libs lvm2app devmapper) -lm

map:	map.cpp
	$(GPP) $(CFLAGS) -o $@ map.cpp

parted:	parted.c
	$(GCC) $(CFLAGS) -o $@ parted.c $(shell pkg-config --cflags --libs libparted) -lm

spawn:	spawn.cpp
	$(GPP) $(CFLAGS) -o $@ spawn.cpp $(shell pkg-config --cflags --libs glibmm-2.4)

force:


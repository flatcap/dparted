GPP	= g++
GCC	= gcc

OUT	= cast compare enum format fs identify ioctl inherit list loop lvm map \
	  parted random readfile size spawn template

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

enum:	enum.c
	$(GCC) $(CFLAGS) -o $@ enum.c

format:	format.cpp
	$(GPP) $(CFLAGS) -o $@ format.cpp

fs:	fs.c
	$(GCC) $(CFLAGS) -o $@ fs.c $(shell pkg-config --cflags --libs libparted)

identify:	identify.c
	$(GCC) $(CFLAGS) -o $@ identify.c

ioctl:	ioctl.c
	$(GCC) $(CFLAGS) -o $@ ioctl.c

inherit:	inherit.cpp
	$(GPP) $(CFLAGS) -o $@ inherit.cpp

list:	list.cpp
	$(GPP) $(CFLAGS) -o $@ list.cpp

loop:	loop.c
	$(GCC) $(CFLAGS) -o $@ loop.c $(shell pkg-config --cflags --libs libparted devmapper) -lm

lvm:	lvm.c
	$(GCC) $(CFLAGS) -o $@ lvm.c $(shell pkg-config --cflags --libs lvm2app devmapper) -lm

map:	map.cpp
	$(GPP) $(CFLAGS) -o $@ map.cpp

parted:	parted.c
	$(GCC) $(CFLAGS) -o $@ parted.c $(shell pkg-config --cflags --libs libparted) -lm

random:	random.cpp
	$(GPP) $(CFLAGS) -o $@ random.cpp

readfile:	readfile.cpp
	$(GPP) $(CFLAGS) -o $@ readfile.cpp

size:	size.c
	$(GCC) $(CFLAGS) -o $@ size.c

spawn:	spawn.cpp
	$(GPP) $(CFLAGS) -o $@ spawn.cpp $(shell pkg-config --cflags --libs glibmm-2.4)

template:	template.cpp
	$(GPP) $(CFLAGS) -o $@ template.cpp

force:


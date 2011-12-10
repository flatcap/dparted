GPP	= g++
GCC	= gcc

OUT	= compare fs lvm map parted spawn format

CFLAGS	= -g -Wall

all:	$(OUT)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT)

format:	format.cpp
	$(GPP) $(CFLAGS) -o $@ format.cpp

fs:	fs.c
	$(GCC) $(CFLAGS) -o $@ fs.c $(shell pkg-config --cflags --libs libparted)

lvm:	lvm.c
	$(GCC) $(CFLAGS) -o $@ lvm.c $(shell pkg-config --cflags --libs lvm2app devmapper) -lm

parted:	parted.c
	$(GCC) $(CFLAGS) -o $@ parted.c $(shell pkg-config --cflags --libs libparted) -lm

compare:	compare.cpp
	$(GPP) $(CFLAGS) -o $@ compare.cpp

map:	map.cpp
	$(GPP) $(CFLAGS) -o $@ map.cpp

spawn:	spawn.cpp
	$(GPP) $(CFLAGS) -o $@ spawn.cpp $(shell pkg-config --cflags --libs glibmm-2.4)

force:


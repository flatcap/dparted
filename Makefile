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

DEPDIR	= .deps

OBJ	= $(SRC:.cpp=.o)

OUT	= main

CFLAGS	= -g -Wall
CFLAGS  += `pkg-config glibmm-2.4 lvm2app devmapper libparted --cflags`
LDFLAGS += `pkg-config glibmm-2.4 lvm2app devmapper libparted --libs`

all:	$(DEPDIR) $(OUT)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o
	@$(CC) -MM $(CFLAGS) -c $*.cpp > $(DEPDIR)/$*.d 
	@cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d
	@rm -f $(DEPDIR)/$*.d.tmp

$(DEPDIR):
	mkdir -p $@

clean:
	$(RM) $(OUT) $(OBJ) tags

distclean: clean
	$(RM) $(DEPDIR) html

main: $(OBJ) tags
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

tags:   force
	ctags *.cpp *.h /usr/include/parted/*.h

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


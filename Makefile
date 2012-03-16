CC	= g++
RM	= rm -fr
MKDIR	= mkdir -p

SRC	= block.cpp container.cpp disk.cpp extended.cpp log.cpp \
	  file.cpp filesystem.cpp gpt.cpp identify.cpp linear.cpp loop.cpp \
	  main.cpp mirror.cpp msdos.cpp partition.cpp table.cpp \
	  segment.cpp stripe.cpp utils.cpp  volume.cpp volumegroup.cpp whole.cpp

SRC	+= dparted.cpp drawingarea.cpp treeview.cpp theme.cpp

HDR	= block.h container.h disk.h extended.h file.h log.h \
	  filesystem.h gpt.h identify.h linear.h loop.h mirror.h msdos.h \
	  partition.h table.h segment.h stringnum.h stripe.h utils.h \
	  volumegroup.h volume.h whole.h

HDR	+= dparted.h drawingarea.h treeview.h theme.h

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= main

CFLAGS	= -g -Wall
CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED

PACKAGES = gtkmm-3.0

CFLAGS	+= $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs   $(PACKAGES))

V	?= 0

ifeq ($(V),1)
	quiet=
else
	quiet=quiet_
endif

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	quiet=silent_
endif

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

# If quiet is set, only print short version of command
cmd	= @$(if $($(quiet)cmd_$(1)),\
		echo '$($(quiet)cmd_$(1))' &&) $(cmd_$(1))

# ----------------------------------------------------------------------------

quiet_cmd_TAGS	= CTAGS	$@
      cmd_TAGS	= ctags $(SRC)

tags:	$(SRC) $(HDR)
	$(call cmd,TAGS)

# ----------------------------------------------------------------------------

# TODO
# Execute command, saving output to a TMP file
#	g++ -g -Wall `pkg-config glibmm-2.4 lvm2app --cflags` -c gpt.cpp -o .obj/gpt.o
# $? = 0 && TMP file empty
# 	echo "CC	gpt.c"
# ?$ = 0 && TMP file non-empty
# 	echo "CC	gpt.c"
# 	cat TMP file
# ?$ = 1
# 	echo "CC	gpt.c"
# 	cat TMP file
# 	stop compilation

quiet_cmd_CC	= CC	$<
      cmd_CC	= $(CC) $(CFLAGS) -c $< -o $@ && (												\
		  $(CC) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d;							\
		  cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp;											\
		  sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d;		\
		  rm -f $(DEPDIR)/$*.d.tmp)

$(OBJDIR)/%.o: %.cpp
	$(call cmd,CC)

# ----------------------------------------------------------------------------

quiet_cmd_LN	= LN	$@
      cmd_LN	= $(CC) -o $@ $(OBJ) $(LDFLAGS)

main: $(OBJ)
	$(call cmd,LN)

# ----------------------------------------------------------------------------

quiet_cmd_MKDIR	= MKDIR	$@
      cmd_MKDIR	= $(MKDIR) $@

$(DEPDIR) $(OBJDIR):
	$(call cmd,MKDIR)

# ----------------------------------------------------------------------------

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags html

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


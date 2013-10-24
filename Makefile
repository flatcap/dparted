CC	= g++
RM	= rm -fr
MKDIR	= mkdir -p

# Library
SRC	= block.cpp container.cpp disk.cpp extended.cpp file.cpp \
	  filesystem.cpp gpt.cpp fs_identify.cpp log.cpp loop.cpp lvm_group.cpp \
	  lvm_linear.cpp lvm_mirror.cpp lvm_partition.cpp lvm_stripe.cpp \
	  lvm_table.cpp lvm_volume.cpp main.cpp misc.cpp msdos.cpp \
	  partition.cpp table.cpp utils.cpp volume.cpp whole.cpp \
	  dot.cpp leak.cpp md_table.cpp app.cpp fs_get.cpp fs_usage.cpp

HDR	= block.h container.h disk.h extended.h file.h filesystem.h \
	  gpt.h fs_identify.h log.h loop.h lvm_group.h lvm_linear.h lvm_mirror.h \
	  lvm_partition.h lvm_stripe.h lvm_table.h lvm_volume.h misc.h msdos.h \
	  partition.h stringnum.h table.h utils.h volume.h whole.h \
	  main.h dot.h leak.h log_trace.h md_table.h app.h fs_get.h fs_usage.h

# GUI
SRC	+= dparted.cpp drawingarea.cpp treeview.cpp theme.cpp
HDR	+= dparted.h drawingarea.h treeview.h theme.h

DEPDIR	= .dep
OBJDIR	= .obj

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

OUT	= main

CFLAGS	+= -std=c++11 -pedantic
CFLAGS	+= -g -Wall
#CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#CFLAGS	+= -pg -fprofile-arcs -ftest-coverage

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-inline-functions
CFLAGS	+= -fno-inline-functions-called-once
CFLAGS	+= -fno-optimize-sibling-calls
CFLAGS	+= -O0

#LDFLAGS	+= -pg -fprofile-arcs

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
#	echo "CC	gpt.c"
# ?$ = 0 && TMP file non-empty
#	echo "CC	gpt.c"
#	cat TMP file
# ?$ = 1
#	echo "CC	gpt.c"
#	cat TMP file
#	stop compilation

quiet_cmd_CC	= CC	$<
      cmd_CC	= $(CC) $(CFLAGS) -c $< -o $@ && (												\
		  $(CC) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d;							\
		  cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp;											\
		  sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d;		\
		  rm -f $(DEPDIR)/$*.d.tmp)

$(OBJDIR)/%.o: %.cpp
	$(call cmd,CC)

# ----------------------------------------------------------------------------

quiet_cmd_LD	= LD	$@
      cmd_LD	= $(CC) -o $@ $(OBJ) $(LDFLAGS)

main:	$(OBJ)
	$(call cmd,LD)

# ----------------------------------------------------------------------------

quiet_cmd_MKDIR	= MKDIR	$@
      cmd_MKDIR	= $(MKDIR) $@

$(DEPDIR) $(OBJDIR):
	$(call cmd,MKDIR)

# ----------------------------------------------------------------------------

clean:	force
	$(RM) $(OUT) $(OBJ) gmon.out

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags html

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)

wc:	force
	@wc -l $(SRC) $(HDR)


CC	= g++
RM	= rm -fr
MKDIR	= mkdir -p

# Library
SRC	+= app.cpp block.cpp container.cpp disk.cpp extended.cpp \
	   file.cpp filesystem.cpp fs_get.cpp fs_identify.cpp fs_usage.cpp \
	   gpt.cpp icon_manager.cpp log.cpp loop.cpp lvm_group.cpp \
	   lvm_linear.cpp lvm_metadata.cpp lvm_mirror.cpp lvm_partition.cpp \
	   lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp lvm_volume.cpp main.cpp \
	   md_group.cpp md_table.cpp misc.cpp msdos.cpp partition.cpp \
	   table.cpp utils.cpp variant.cpp volume.cpp whole.cpp

SRC	+= dot_visitor.cpp

# GUI
SRC	+= dparted.cpp drawingarea.cpp treeview.cpp theme.cpp

HDR	= $(SRC:%.cpp=%.h)

HDR	+= log_trace.h lvm2.h pointers.h stringnum.h visitor.h

DEPDIR	= .dep
OBJDIR	= .obj

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

OUT	= main

CFLAGS	+= -std=c++11 -pedantic
CFLAGS	+= -g -Wall

CFLAGS	+= -DCAIROMM_DISABLE_DEPRECATED
CFLAGS	+= -DGDKMM_DISABLE_DEPRECATED
CFLAGS	+= -DGDK_DISABLE_DEPRECATED
CFLAGS	+= -DGDK_PIXBUF_DISABLE_DEPRECATED
CFLAGS	+= -DGIOMM_DISABLE_DEPRECATED
CFLAGS	+= -DGLIBMM_DISABLE_DEPRECATED
CFLAGS	+= -DGLIBMM_G_DISABLE_DEPRECATED_UNDEFED
CFLAGS	+= -DGTKMM_GTKMM_DISABLE_DEPRECATED_UNDEFED
CFLAGS	+= -DGTK_DISABLE_DEPRECATED
CFLAGS	+= -DHB_DISABLE_DEPRECATED
CFLAGS	+= -DPANGOMM_DISABLE_DEPRECATED
CFLAGS	+= -DPANGO_DISABLE_DEPRECATED

#CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#CFLAGS	+= -DG_DISABLE_DEPRECATED

#CFLAGS	+= -pg -fprofile-arcs -ftest-coverage

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-inline-functions
CFLAGS	+= -fno-inline-functions-called-once
CFLAGS	+= -fno-optimize-sibling-calls
CFLAGS	+= -O0

#LDFLAGS	+= -pg -fprofile-arcs

PACKAGES = gtkmm-3.0 libconfig++

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


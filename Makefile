##
# Copyright (c) 2013 Richard Russon (FlatCap)
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place - Suite 330, Boston, MA 02111-1307, USA.


CC	= g++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= main

# Core Objects
OBJ_SRC	+= block.cpp container.cpp disk.cpp extended.cpp file.cpp filesystem.cpp gpt.cpp loop.cpp lvm_group.cpp \
	   lvm_linear.cpp lvm_metadata.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp \
	   lvm_volume.cpp md_group.cpp md_table.cpp misc.cpp msdos.cpp partition.cpp table.cpp volume.cpp whole.cpp

# Library - Non-graphical miscellany
LIB_SRC	+= app.cpp dot_visitor.cpp dump_visitor.cpp fs_get.cpp fs_identify.cpp fs_usage.cpp gfx_container.cpp log.cpp \
	   main.cpp prop_visitor.cpp question.cpp utils.cpp variant.cpp

# GUI - Graphical objects
GUI_SRC	+= dparted.cpp drawingarea.cpp treeview.cpp theme.cpp gui_app.cpp icon_manager.cpp

SRC	= $(OBJ_SRC) $(LIB_SRC) $(GUI_SRC)
HDR	= $(SRC:%.cpp=%.h)

# Misc header files
HDR	+= log_trace.h lvm2.h mmap.h stringnum.h visitor.h

OBJ_OBJ	= $(OBJ_SRC:%.cpp=$(OBJDIR)/%.o)
LIB_OBJ	= $(LIB_SRC:%.cpp=$(OBJDIR)/%.o)
GUI_OBJ	= $(GUI_SRC:%.cpp=$(OBJDIR)/%.o)

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

all:	$(OBJDIR) $(DEPDIR) $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

# If quiet is set, only print short version of command
cmd	= @$(if $($(quiet)cmd_$(1)),\
		echo '$($(quiet)cmd_$(1))' &&) $(cmd_$(1))

# ----------------------------------------------------------------------------

quiet_cmd_TAGS	= CTAGS	$@
      cmd_TAGS	= ctags $(SRC) $(HDR)

tags:	$(SRC) $(HDR)
	$(call cmd,TAGS)

# ----------------------------------------------------------------------------

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
      cmd_LD	= $(CC) -o $@ $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) $(LDFLAGS)

main:	$(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ)
	$(call cmd,LD)

# ----------------------------------------------------------------------------

quiet_cmd_MKDIR	= MKDIR	$@
      cmd_MKDIR	= $(MKDIR) $@

$(DEPDIR) $(OBJDIR):
	$(call cmd,MKDIR)

# ----------------------------------------------------------------------------

clean:	force
	$(RM) $(OUT) $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) gmon.out

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags html

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


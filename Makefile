##
# Copyright (c) 2014 Richard Russon (FlatCap)
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

CC	= clang++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

# Core Objects
OBJ_SRC	+= device.cpp container.cpp disk.cpp extended.cpp file.cpp filesystem.cpp gpt.cpp loop.cpp lvm_group.cpp \
	   lvm_linear.cpp lvm_metadata.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp \
	   lvm_volume.cpp md_group.cpp md_table.cpp misc.cpp msdos.cpp partition.cpp table.cpp volume.cpp whole.cpp \
	   luks.cpp extfs.cpp btrfs.cpp ntfs.cpp piece.cpp md_partition.cpp

# Library - Non-graphical miscellany
LIB_SRC	+= app.cpp dot_visitor.cpp dump_visitor.cpp fs_get.cpp fs_identify.cpp fs_usage.cpp log.cpp \
	   prop_visitor.cpp question.cpp utils.cpp property.cpp config.cpp config_file.cpp message.cpp

# GUI - Graphical objects
GUI_SRC	+= window.cpp drawing_area.cpp gfx_container.cpp tree_view.cpp theme.cpp gui_app.cpp icon_manager.cpp \
	   main.cpp default_theme.cpp properties_dialog.cpp base_drawing_area.cpp prop_drawing_area.cpp \
	   password_dialog.cpp option_group.cpp

SRC	= $(OBJ_SRC) $(LIB_SRC) $(GUI_SRC)
HDR	= $(SRC:%.cpp=%.h)

# Misc header files
HDR	+= log_trace.h lvm2.h mmap.h stringnum.h visitor.h config_manager.h

OBJ_OBJ	= $(OBJ_SRC:%.cpp=$(OBJDIR)/%.o)
LIB_OBJ	= $(LIB_SRC:%.cpp=$(OBJDIR)/%.o)
GUI_OBJ	= $(GUI_SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= -std=c++11 -pedantic
CFLAGS	+= -g -Wall

#CFLAGS	+= -pg -fprofile-arcs -ftest-coverage

CFLAGS	+= -fno-omit-frame-pointer
#CFLAGS	+= -fno-inline-functions
#CFLAGS	+= -fno-inline-functions-called-once
CFLAGS	+= -fno-optimize-sibling-calls
CFLAGS	+= -O0

GUI_CFLAGS	+= -DCAIROMM_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGDKMM_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGDK_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGDK_PIXBUF_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGIOMM_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGLIBMM_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DGLIBMM_G_DISABLE_DEPRECATED_UNDEFED
GUI_CFLAGS	+= -DGTKMM_GTKMM_DISABLE_DEPRECATED_UNDEFED
GUI_CFLAGS	+= -DGTK_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DHB_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DPANGOMM_DISABLE_DEPRECATED
GUI_CFLAGS	+= -DPANGO_DISABLE_DEPRECATED

GUI_CFLAGS	+= -Qunused-arguments

#GUI_CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#GUI_CFLAGS	+= -DG_DISABLE_DEPRECATED

#LDFLAGS	+= -pg -fprofile-arcs

PACKAGES = gtkmm-3.0 libconfig++

GUI_CFLAGS	+= $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS		+= $(shell pkg-config --libs   $(PACKAGES))

V	?= 0

ifeq ($(V),1)
	quiet=
else
	quiet=quiet_
endif

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	quiet=silent_
endif

#CFLAGS += $(GUI_CFLAGS)
$(GUI_OBJ):	CFLAGS += $(GUI_CFLAGS)

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
      cmd_CC	= $(CC) $(CFLAGS) -c $< -o $@ && (											\
		  $(CC) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d;						\
		  cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp;										\
		  sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d;	\
		  rm -f $(DEPDIR)/$*.d.tmp)

$(OBJDIR)/%.o: %.cpp
	$(call cmd,CC)

# ----------------------------------------------------------------------------

quiet_cmd_LD	= LD	$@
      cmd_LD	= $(CC) -o $@ $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) $(LDFLAGS)

$(OUT):	$(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ)
	$(call cmd,LD)

# ----------------------------------------------------------------------------

quiet_cmd_MKDIR	= MKDIR	$@
      cmd_MKDIR	= $(MKDIR) $@

$(DEPDIR) $(OBJDIR):
	$(call cmd,MKDIR)

# ----------------------------------------------------------------------------

docs:	force
	doxygen docs/doxygen.conf

clean:	force
	$(RM) $(OUT) $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) gmon.out

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags html

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


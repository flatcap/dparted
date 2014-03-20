##
# Copyright (c) 2014 Richard Russon (FlatCap)
#
# This file is part of DParted.
#
# DParted is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DParted is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DParted.  If not, see <http://www.gnu.org/licenses/>.

CC	= g++	#clang++
SCAN	= scan-build
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

LINKS	= misc test

# Core Objects
OBJ_SRC	+= block.cpp btrfs.cpp container.cpp disk.cpp extfs.cpp file.cpp filesystem.cpp loop.cpp misc.cpp ntfs.cpp partition.cpp table.cpp volume.cpp whole.cpp

# Library - Non-graphical miscellany
LIB_SRC	+= app.cpp config.cpp config_file.cpp log.cpp message.cpp property.cpp question.cpp type_visitor.cpp utils.cpp uuid_visitor.cpp

# GUI - Graphical objects
GUI_SRC	+= base_drawing_area.cpp default_theme.cpp drawing_area.cpp gfx_container.cpp gui_app.cpp icon_manager.cpp \
	   main.cpp option_group.cpp password_dialog.cpp properties_dialog.cpp prop_drawing_area.cpp theme.cpp \
	   tree_view.cpp window.cpp

FS_DOT	?= 0
ifeq ($(FS_DOT),1)
	LIB_SRC	+= dot_visitor.cpp
	CFLAGS	+= -DDP_FS_DOT
endif

FS_HEX	?= 0
ifeq ($(FS_HEX),1)
	LIB_SRC	+= hex_visitor.cpp
	CFLAGS	+= -DDP_FS_HEX
endif

FS_LIST	?= 0
ifeq ($(FS_LIST),1)
	LIB_SRC	+= list_visitor.cpp
	CFLAGS	+= -DDP_FS_LIST
endif

FS_MISC	?= 0
ifeq ($(FS_MISC),1)
	LIB_SRC	+= fs_get.cpp fs_identify.cpp fs_usage.cpp
	CFLAGS	+= -DDP_FS_MISC
endif

FS_PROP	?= 0
ifeq ($(FS_PROP),1)
	LIB_SRC	+= prop_visitor.cpp
	CFLAGS	+= -DDP_FS_PROP
endif

GPT	?= 0
ifeq ($(GPT),1)
	OBJ_SRC	+= gpt.cpp gpt_partition.cpp
	CFLAGS	+= -DDP_GPT
endif

LUKS	?= 0
ifeq ($(LUKS),1)
	OBJ_SRC	+= luks_partition.cpp luks_table.cpp
	CFLAGS	+= -DDP_LUKS
endif

LVM	?= 0
ifeq ($(LVM),1)
	OBJ_SRC	+= lvm_group.cpp lvm_linear.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp lvm_volume.cpp
	HDR	+= lvm2.h
	CFLAGS	+= -DDP_LVM
endif

MD	?= 0
ifeq ($(MD),1)
	OBJ_SRC	+= md_linear.cpp md_mirror.cpp md_partition.cpp md_raid.cpp md_stripe.cpp md_table.cpp md_volume.cpp
	CFLAGS	+= -DDP_MD
endif

MSDOS	?= 0
ifeq ($(MSDOS),1)
	OBJ_SRC	+= extended.cpp msdos.cpp msdos_partition.cpp
	CFLAGS	+= -DDP_MSDOS
endif

# Library - Non-graphical miscellany
LIB_SRC	+= app.cpp config.cpp config_file.cpp dot_visitor.cpp hex_visitor.cpp list_visitor.cpp log.cpp message.cpp property.cpp prop_visitor.cpp question.cpp type_visitor.cpp utils.cpp uuid_visitor.cpp

# GUI - Graphical objects
GUI_SRC	+= base_drawing_area.cpp default_theme.cpp drawing_area.cpp gfx_container.cpp gui_app.cpp main.cpp option_group.cpp password_dialog.cpp properties_dialog.cpp prop_drawing_area.cpp theme.cpp tree_view.cpp window.cpp

# Misc header files
HDR	+= log_trace.h mmap.h stringnum.h visitor.h

BTRFS	?= 0
ifeq ($(BTRFS),1)
	LIB_SRC	+= btrfs.cpp
	CFLAGS	+= -DDP_BTRFS
endif

DOT	?= 0
ifeq ($(DOT),1)
	LIB_SRC	+= dot_visitor.cpp
	CFLAGS	+= -DDP_DOT
endif

EXTFS	?= 0
ifeq ($(EXTFS),1)
	LIB_SRC	+= extfs.cpp
	CFLAGS	+= -DDP_EXTFS
endif

FS_MISC	?= 0
ifeq ($(FS_MISC),1)
	LIB_SRC	+= fs_get.cpp fs_identify.cpp fs_usage.cpp
	CFLAGS	+= -DDP_FS_MISC
endif

GPT	?= 0
ifeq ($(GPT),1)
	OBJ_SRC	+= gpt.cpp gpt_partition.cpp
	CFLAGS	+= -DDP_GPT
endif

HEX	?= 0
ifeq ($(HEX),1)
	LIB_SRC	+= hex_visitor.cpp
	CFLAGS	+= -DDP_HEX
endif

LIST	?= 0
ifeq ($(LIST),1)
	LIB_SRC	+= list_visitor.cpp
	CFLAGS	+= -DDP_LIST
endif

LUKS	?= 0
ifeq ($(LUKS),1)
	OBJ_SRC	+= luks_partition.cpp luks_table.cpp
	CFLAGS	+= -DDP_LUKS
endif

LVM	?= 0
ifeq ($(LVM),1)
	OBJ_SRC	+= lvm_group.cpp lvm_linear.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp lvm_volume.cpp
	HDR	+= lvm2.h
	CFLAGS	+= -DDP_LVM
endif

MD	?= 0
ifeq ($(MD),1)
	OBJ_SRC	+= md_linear.cpp md_mirror.cpp md_partition.cpp md_raid.cpp md_stripe.cpp md_table.cpp md_volume.cpp
	CFLAGS	+= -DDP_MD
endif

MSDOS	?= 0
ifeq ($(MSDOS),1)
	OBJ_SRC	+= extended.cpp msdos.cpp msdos_partition.cpp
	CFLAGS	+= -DDP_MSDOS
endif

NTFS	?= 0
ifeq ($(NTFS),1)
	LIB_SRC	+= ntfs.cpp
	CFLAGS	+= -DDP_NTFS
endif

PROP	?= 0
ifeq ($(PROP),1)
	LIB_SRC	+= prop_visitor.cpp
	CFLAGS	+= -DDP_PROP
endif

UNUSED	?= 0
ifeq ($(UNUSED),1)
	LIB_SRC	+= icon_manager.cpp
	HDR	+= config_manager.h
	CFLAGS	+= -DDP_UNUSED
endif

SRC	+= $(OBJ_SRC) $(LIB_SRC) $(GUI_SRC)
HDR	+= $(SRC:%.cpp=%.h)

# Misc header files
HDR	+= config_manager.h log_trace.h mmap.h stringnum.h visitor.h

OBJ_OBJ	= $(OBJ_SRC:%.cpp=$(OBJDIR)/%.o)
LIB_OBJ	= $(LIB_SRC:%.cpp=$(OBJDIR)/%.o)
GUI_OBJ	= $(GUI_SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= -std=c++11
CFLAGS	+= -ggdb
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
#CFLAGS	+= -fcolor-diagnostics

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-optimize-sibling-calls
CFLAGS	+= -DDEBUG

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

# bug in ccache
#GUI_CFLAGS	+= -Qunused-arguments

#GUI_CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#GUI_CFLAGS	+= -DG_DISABLE_DEPRECATED


PACKAGES = gtkmm-3.0 libconfig++

GUI_CFLAGS	+= $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS		+= $(shell pkg-config --libs   $(PACKAGES))

V	?= 0

ifeq ($(V),1)
	quiet=
else
	quiet=quiet_
endif

P	?= 0

ifeq ($(P),1)
	CFLAGS	+= -O0
	CFLAGS	+= -fno-inline-functions
	CFLAGS	+= -pg -fprofile-arcs -ftest-coverage
	CFLAGS	+= -fno-inline-functions-called-once
	LDFLAGS	+= -pg -fprofile-arcs
endif

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	quiet=silent_
endif

$(GUI_OBJ):	CFLAGS += $(GUI_CFLAGS)

all:	$(OBJDIR) $(DEPDIR) $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

# If quiet is set, only print short version of command
cmd	= @$(if $($(quiet)cmd_$(1)),\
		echo '$($(quiet)cmd_$(1))' &&) $(cmd_$(1))

# ----------------------------------------------------------------------------

quiet_cmd_TAGS	= CTAGS	$@
      cmd_TAGS	= ctags -I UNUSED -f - $(SRC) $(HDR) | grep -v -e "^_[A-Z0-9_]\+_H_	" -e "^[A-Za-z]\+Ptr	" > tags

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

stats:	force
	$(RM) stats
	gitstats . stats

xxx:	force
	@grep --exclude xxx.txt -rHno "//[X]XX.*" . \
		| expand -t8 \
		| sed -e 's/  \+/ /g' -e 's/^..//' -e 's!//[X]XX *!!' \
		| sort > xxx.txt
	@wc -l xxx.txt

links:	$(LINKS)

$(LINKS):
	ln -s ../dparted-$@ $@

clean:	force
	$(RM) $(OUT) $(OBJ_OBJ) $(LIB_OBJ) $(GUI_OBJ) gmon.out *.gcov

distclean: clean
	$(RM) $(DEPDIR) $(OBJDIR) tags html stats xxx.txt

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


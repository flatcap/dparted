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

# Configurables
V	?= 0		# Verbose
P	?= 0		# Profiling
BTRFS	?= 0
DOT	?= 0
EXTFS	?= 1
FS_MISC	?= 0
GPT	?= 0
GUI	?= 0
HEX	?= 0
LIST	?= 1
LUKS	?= 0
LVM	?= 1
MD	?= 0
MSDOS	?= 0
NTFS	?= 0
PROP	?= 0
UNUSED	?= 0

CC	= g++	#clang++
SCAN	= scan-build
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

LINKS	= misc test

# Core Objects
SRC	+= block.cpp container.cpp disk.cpp file.cpp filesystem.cpp loop.cpp misc.cpp partition.cpp table.cpp volume.cpp whole.cpp

# Library - Non-graphical miscellany
SRC	+= app.cpp config.cpp config_file.cpp log.cpp message.cpp property.cpp question.cpp type_visitor.cpp utils.cpp uuid_visitor.cpp

# GUI - Graphical objects
SRC	+= gui_app.cpp main.cpp option_group.cpp

# Misc header files
HDR	+= log_trace.h mmap.h stringnum.h visitor.h

CFLAGS-$(BTRFS)		+= -DDP_BTRFS
CFLAGS-$(DOT)		+= -DDP_DOT
CFLAGS-$(EXTFS)		+= -DDP_EXTFS
CFLAGS-$(FS_MISC)	+= -DDP_FS_MISC
CFLAGS-$(GPT)		+= -DDP_GPT
CFLAGS-$(GUI)		+= -DDP_GUI
CFLAGS-$(HEX)		+= -DDP_HEX
CFLAGS-$(LIST)		+= -DDP_LIST
CFLAGS-$(LUKS)		+= -DDP_LUKS
CFLAGS-$(LVM)		+= -DDP_LVM
CFLAGS-$(MD)		+= -DDP_MD
CFLAGS-$(MSDOS)		+= -DDP_MSDOS
CFLAGS-$(NTFS)		+= -DDP_NTFS
CFLAGS-$(PROP)		+= -DDP_PROP
CFLAGS-$(UNUSED)	+= -DDP_UNUSED

SRC-$(BTRFS)		+= btrfs.cpp
SRC-$(DOT)		+= dot_visitor.cpp
SRC-$(EXTFS)		+= extfs.cpp
SRC-$(FS_MISC)		+= fs_get.cpp fs_identify.cpp fs_usage.cpp
SRC-$(GPT)		+= gpt.cpp gpt_partition.cpp
SRC-$(GUI)		+= base_drawing_area.cpp default_theme.cpp drawing_area.cpp gfx_container.cpp password_dialog.cpp properties_dialog.cpp prop_drawing_area.cpp theme.cpp tree_view.cpp window.cpp
SRC-$(HEX)		+= hex_visitor.cpp
SRC-$(LIST)		+= list_visitor.cpp
SRC-$(LUKS)		+= luks_partition.cpp luks_table.cpp
SRC-$(LVM)		+= lvm_group.cpp lvm_linear.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp lvm_volume.cpp
SRC-$(MD)		+= md_linear.cpp md_mirror.cpp md_partition.cpp md_raid.cpp md_stripe.cpp md_table.cpp md_volume.cpp
SRC-$(MSDOS)		+= extended.cpp msdos.cpp msdos_partition.cpp
SRC-$(NTFS)		+= ntfs.cpp
SRC-$(PROP)		+= prop_visitor.cpp
SRC-$(UNUSED)		+= icon_manager.cpp

SRC	+= $(SRC-1)
HDR	+= $(SRC:%.cpp=%.h)

HDR	+= lvm2.h config_manager.h

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= $(CFLAGS-1)
CFLAGS	+= -std=c++11
CFLAGS	+= -ggdb
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
#CFLAGS	+= -fcolor-diagnostics

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-optimize-sibling-calls
CFLAGS	+= -DDEBUG

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

# bug in ccache
#CFLAGS	+= -Qunused-arguments

#CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#CFLAGS	+= -DG_DISABLE_DEPRECATED

PACKAGES = gtkmm-3.0 libconfig++

CFLAGS	+= $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS	+= $(shell pkg-config --libs   $(PACKAGES))

ifeq ($(P),1)
	CFLAGS	+= -O0
	CFLAGS	+= -fno-inline-functions
	CFLAGS	+= -pg -fprofile-arcs -ftest-coverage
	CFLAGS	+= -fno-inline-functions-called-once
	LDFLAGS	+= -pg -fprofile-arcs
endif

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

#
# Pretty print
#
V	      = @
Q	      = $(V:1=)
QUIET_CC      = $(Q:@=@echo 'CC      '$^;)
QUIET_LINK    = $(Q:@=@echo 'LINK    '$@;)
QUIET_TAGS    = $(Q:@=@echo 'TAGS    '$@;)
QUIET_CLEAN   = $(Q:@=@echo 'CLEAN   '$@;)

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	QUIET_CC      =
	QUIET_LINK    =
	QUIET_TAGS    =
	QUIET_CLEAN   =
endif

# ----------------------------------------------------------------------------

$(OBJDIR)/%.o: %.cpp
	$(QUIET_CC)$(CC) $(CFLAGS) -c $< -o $@ && (										\
	$(CC) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d;						\
	cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp;										\
	sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d;		\
	rm -f $(DEPDIR)/$*.d.tmp)

# ----------------------------------------------------------------------------

$(OUT):	$(OBJ)
	$(QUIET_LINK)$(CC) -o $@ $(OBJ) $(LDFLAGS)

$(DEPDIR) $(OBJDIR):
	$(Q)$(MKDIR) $@

# ----------------------------------------------------------------------------

tags:
	$(QUIET_TAGS)ctags -I UNUSED -f - $(SRC) $(HDR) | grep -v -e "^_[A-Z0-9_]\+_H_	" -e "^[A-Za-z]\+Ptr	" > tags

docs:
	doxygen docs/doxygen.conf

stats:
	$(RM) stats
	gitstats . stats

xxx:
	@grep --exclude xxx.txt -rHno "//[X]XX.*" . \
		| expand -t8 \
		| sed -e 's/  \+/ /g' -e 's/^..//' -e 's!//[X]XX *!!' \
		| sort > xxx.txt
	@wc -l xxx.txt

links:	$(LINKS)

$(LINKS):
	ln -s ../dparted-$@ $@

clean:
	$(Q)$(RM) $(OUT) $(OBJ) gmon.out *.gcov

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(OBJDIR) tags html stats xxx.txt

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)

.PHONY:	tags docs stats xxx


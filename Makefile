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

# ----------------------------------------------------------------------------

# Configurables
# A=All, V=Verbose, P=Profiling, L=LogCheck, D=Debug, T=Thread
A	?= 1
V	?= 0
P	?= 0
L	?= 0
D	?= 1
T	?= 1

GUI	?= 1
TREE	?= 0
AREA	?= 1

DISK	?= 0
FILE	?= $(A)
LOOP	?= $(A)
LVM	?= $(A)

GPT	?= $(A)
MSDOS	?= $(A)
MD	?= $(A)
LUKS	?= $(A)

BTRFS	?= $(A)
EXTFS	?= $(A)
FS_MISC	?= $(A)
NTFS	?= $(A)

LIST	?= $(A)
PROP	?= $(A)
DOT	?= $(A)
HEX	?= $(A)

TEST	?= 0
UNUSED	?= $(A)

# ----------------------------------------------------------------------------

CXX	?= g++
LN	= ln -s
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

LINKS	= misc test

# Core Objects
SRC	+= block.cpp container.cpp filesystem.cpp misc.cpp partition.cpp table.cpp volume.cpp whole.cpp

# Library - Non-graphical miscellany
SRC	+= app.cpp config.cpp config_file.cpp log.cpp log_handler.cpp log_object.cpp property.cpp question.cpp type_visitor.cpp utils.cpp uuid_visitor.cpp text_app.cpp timeline.cpp transaction.cpp

# GUI - Graphical objects
SRC	+= main.cpp

# Misc header files
HDR	+= action.h config_manager.h container_listener.h endian.h gfx_container_listener.h log_macro.h log_severity.h log_trace.h lvm2.h mmap.h option.h stringnum.h theme_listener.h visitor.h

CFLAGS-$(BTRFS)		+= -DDP_BTRFS
CFLAGS-$(DISK)		+= -DDP_DISK
CFLAGS-$(DOT)		+= -DDP_DOT
CFLAGS-$(EXTFS)		+= -DDP_EXTFS
CFLAGS-$(FILE)		+= -DDP_FILE
CFLAGS-$(FS_MISC)	+= -DDP_FS_MISC
CFLAGS-$(GPT)		+= -DDP_GPT
CFLAGS-$(GUI)		+= -DDP_GUI
ifeq ($(GUI),1)
	CFLAGS-$(TREE)	+= -DDP_TREE
	CFLAGS-$(AREA)	+= -DDP_AREA
endif
CFLAGS-$(HEX)		+= -DDP_HEX
CFLAGS-$(LIST)		+= -DDP_LIST
CFLAGS-$(LOOP)		+= -DDP_LOOP
CFLAGS-$(LUKS)		+= -DDP_LUKS
CFLAGS-$(LVM)		+= -DDP_LVM
CFLAGS-$(MD)		+= -DDP_MD
CFLAGS-$(MSDOS)		+= -DDP_MSDOS
CFLAGS-$(NTFS)		+= -DDP_NTFS
CFLAGS-$(PROP)		+= -DDP_PROP
ifeq ($(D),1)
	CFLAGS-$(TEST)	+= -DDP_TEST
endif
CFLAGS-$(UNUSED)	+= -DDP_UNUSED

SRC-$(BTRFS)		+= btrfs.cpp
SRC-$(DISK)		+= disk.cpp
SRC-$(DOT)		+= dot_visitor.cpp
SRC-$(EXTFS)		+= extfs.cpp
SRC-$(FILE)		+= file.cpp
SRC-$(FS_MISC)		+= fs_get.cpp fs_identify.cpp fs_usage.cpp
SRC-$(GPT)		+= gpt.cpp gpt_partition.cpp
SRC-$(GUI)		+= change_password_dialog.cpp default_theme.cpp delete_dialog.cpp dialog.cpp error_dialog.cpp gfx_container.cpp gui_app.cpp info_dialog.cpp option_group.cpp password_dialog.cpp question_dialog.cpp theme.cpp warning_dialog.cpp window.cpp
SRC-$(HEX)		+= hex_visitor.cpp
SRC-$(LIST)		+= list_visitor.cpp
SRC-$(LOOP)		+= loop.cpp
SRC-$(LUKS)		+= luks_partition.cpp luks_table.cpp
SRC-$(LVM)		+= lvm_group.cpp lvm_linear.cpp lvm_mirror.cpp lvm_partition.cpp lvm_raid.cpp lvm_stripe.cpp lvm_table.cpp lvm_volume.cpp
SRC-$(MD)		+= md_linear.cpp md_mirror.cpp md_partition.cpp md_raid.cpp md_stripe.cpp md_table.cpp md_volume.cpp
SRC-$(MSDOS)		+= extended.cpp msdos.cpp msdos_partition.cpp
SRC-$(NTFS)		+= ntfs.cpp
SRC-$(PROP)		+= prop_visitor.cpp
SRC-$(TEST)		+= test.cpp

ifeq ($(GUI),1)
	SRC-$(TREE)	+= tree_view.cpp
	SRC-$(AREA)	+= base_drawing_area.cpp drawing_area.cpp prop_drawing_area.cpp properties_dialog.cpp
	SRC-$(UNUSED)	+= icon_manager.cpp
endif

SRC	+= $(SRC-1)
HDR	+= $(SRC:%.cpp=%.h)

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= $(CFLAGS-1)
CFLAGS	+= -std=c++11
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
#CFLAGS	+= -fcolor-diagnostics
LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-optimize-sibling-calls

ifeq ($(GUI),1)
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
endif

#CFLAGS	+= -DGTKMM_DISABLE_DEPRECATED
#CFLAGS	+= -DG_DISABLE_DEPRECATED

# bug in ccache
#CFLAGS	+= -Qunused-arguments

PACKAGES += libconfig++
ifeq ($(GUI),1)
	PACKAGES += gtkmm-3.0
endif

ifeq ($(L),1)
	CFLAGS	+= -DDP_LOG_CHECK
	CFLAGS	+= -Wformat=2
endif

ifeq ($(D),1)
	CFLAGS	+= -DDEBUG
	CFLAGS	+= -include iostream
	CFLAGS	+= -ggdb
else
	CFLAGS	+= -D_GLIBCXX_IOSTREAM
endif

ifeq ($(T),1)
	CFLAGS	+= -DDP_THREADED
endif

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
# Show the Makefile config options -- clumsy, but useful

VAR_LIST	= A V P L D T GUI TREE AREA DISK FILE LOOP LVM GPT MSDOS MD LUKS BTRFS EXTFS FS_MISC NTFS LIST PROP DOT HEX UNUSED
vars:
	@echo -e $(foreach var, $(VAR_LIST), "$(var) = $($(var))\n") | column -t

# ----------------------------------------------------------------------------

#
# Pretty print
#
ifneq ($(V),1)
Q	:= @
endif
QUIET_CC      = $(Q:@=@echo 'CC      '$<;)
QUIET_LINK    = $(Q:@=@echo 'LD      '$@;)
QUIET_LN      = $(Q:@=@echo 'LN      '$@;)
QUIET_TAGS    = $(Q:@=@echo 'TAGS    '$@;)

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	QUIET_CC      =
	QUIET_LINK    =
	QUIET_TAGS    =
endif

# ----------------------------------------------------------------------------

$(OBJDIR)/%.o: %.cpp
	$(QUIET_CC)$(CXX) $(CFLAGS) -c $< -o $@ && (										\
	$(CXX) -MM $(CFLAGS) -c $< | sed 's/.*:/'$(OBJDIR)'\/\0/' > $(DEPDIR)/$*.d;						\
	cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp;										\
	sed -e 's/.*://' -e 's/\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d;		\
	rm -f $(DEPDIR)/$*.d.tmp)

# ----------------------------------------------------------------------------

$(OUT):	$(OBJ)
	$(QUIET_LINK)$(CXX) -o $@ $(OBJ) $(LDFLAGS)

$(DEPDIR) $(OBJDIR):
	$(Q)$(MKDIR) $@

# ----------------------------------------------------------------------------

tags:	$(SRC) $(HDR)
	$(QUIET_TAGS)ctags -I UNUSED -f - $(SRC) $(HDR) | grep -v -e "^_[A-Z0-9_]\+_H_	" -e "^[A-Za-z]\+Ptr	" > tags

docs:
	doxygen docs/doxygen.conf

stats:
	$(RM) stats
	gitstats . stats
	firefox stats/index.html

xxx:
	@grep --exclude xxx.txt -rHno "//[X]XX.*" . \
		| expand -t8 \
		| sed -e 's/  \+/ /g' -e 's/^..//' -e 's!//[X]XX *!!' \
		| sort > xxx.txt
	@wc -l xxx.txt

links:	$(LINKS)

$(LINKS):
	$(QUIET_LN)$(LN) ../dparted-$@ $@

clean c:
	$(Q)$(RM) $(OUT) $(OBJ) gmon.out *.gcov

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(OBJDIR) $(LINKS) tags html stats xxx.txt

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)

.PHONY:	docs stats xxx


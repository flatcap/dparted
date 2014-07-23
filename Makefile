# A=All, V=Verbose, P=Profiling, L=LogCheck, D=Debug, T=Thread
A	?= 0
V	?= 0
P	?= 0
L	?= 0
D	?= 1
T	?= 1

GUI	?= 1
TREE	?= 1
AREA	?= 1

DISK	?= 0
FILE	?= $(A)
LOOP	?= 1
LVM	?= $(A)

GPT	?= 1
MSDOS	?= $(A)
MD	?= $(A)
LUKS	?= $(A)

BTRFS	?= 1
EXTFS	?= 1
FS_MISC	?= 1
NTFS	?= 1

LIST	?= $(A)
PROP	?= $(A)
DOT	?= $(A)
HEX	?= $(A)

UNUSED	?= 0

# ----------------------------------------------------------------------------

CXX	?= g++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

LINKS	= misc test

# Core Objects
SRC	+= block.cpp container.cpp filesystem.cpp misc.cpp partition.cpp table.cpp volume.cpp whole.cpp

# Library - Non-graphical miscellany
SRC	+= app.cpp config.cpp config_file.cpp log.cpp log_handler.cpp log_object.cpp property.cpp question.cpp type_visitor.cpp utils.cpp uuid_visitor.cpp text_app.cpp

# GUI - Graphical objects
SRC	+= main.cpp

# Misc header files
HDR	+= action.h config_manager.h container_listener.h endian.h gfx_container_listener.h log_macro.h log_severity.h log_trace.h lvm2.h mmap.h stringnum.h theme_listener.h visitor.h

SRC-$(LIST)		+= list_visitor.cpp

SRC	+= $(SRC-1)
HDR	+= $(SRC:%.cpp=%.h)

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= $(CFLAGS-1)
CFLAGS	+= -std=c++11
CFLAGS	+= -ggdb
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic

CFLAGS	+= -fno-omit-frame-pointer
CFLAGS	+= -fno-optimize-sibling-calls

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

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT)

# ----------------------------------------------------------------------------

#
# Pretty print
#
V	      = @
Q	      = $(V:1=)
QUIET_CC      = $(Q:@=@echo 'CC      '$<;)
QUIET_LINK    = $(Q:@=@echo 'LD      '$@;)
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

clean:
	$(Q)$(RM) $(OUT) $(OBJ) gmon.out *.gcov

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(OBJDIR)

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


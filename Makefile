CXX	?= g++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= main

SRC	+= app.cpp dir.cpp main.cpp
HDR	+= app.h dir.h
#HDR	+= $(SRC:%.cpp=%.h)

PLUGIN	+= plugin.h theme.cpp

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= -std=c++11
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
CFLAGS	+= -fpic
CFLAGS	+= -DDEBUG
CFLAGS	+= -ggdb

LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT) tags

# ----------------------------------------------------------------------------

#
# Pretty print
#
ifneq ($(V),1)
Q	:= @
endif
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

tags:	$(SRC) $(HDR)
	$(QUIET_TAGS)ctags -I UNUSED -f - $(SRC) $(HDR) | grep -v -e "^_[A-Z0-9_]\+_H_	" -e "^[A-Za-z]\+Ptr	" > tags

clean:
	$(Q)$(RM) $(OUT) $(OBJ)

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(OBJDIR) $(LINKS) tags

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


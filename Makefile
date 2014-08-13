CXX	?= g++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj
PLUGDIR	= plugins

OUT_MAIN	= main
OUT_PLUG_THEME	= $(PLUGDIR)/theme.so

SRC	+= app.cpp dir.cpp main.cpp theme.cpp
HDR	+= app.h dir.h plugin.h

OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= -std=c++11
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
CFLAGS	+= -fpic
CFLAGS	+= -ggdb

LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic
LDFLAGS += -ldl

all:	$(OBJDIR) $(DEPDIR) $(PLUGDIR) $(OBJ) $(OUT_MAIN) $(OUT_PLUG_THEME) tags

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

$(OUT_MAIN):	$(OBJ)
	$(QUIET_LINK)$(CXX) -o $@ $(OBJ) $(LDFLAGS)

$(DEPDIR) $(OBJDIR) $(PLUGDIR):
	$(Q)$(MKDIR) $@

$(OUT_PLUG_THEME):	$(OBJ)
	$(QUIET_LINK)$(CXX) $(LDFLAGS) -shared -o $@ $(OBJ)

# ----------------------------------------------------------------------------

tags:	$(SRC) $(HDR)
	$(QUIET_TAGS)ctags -I UNUSED -f - $(SRC) $(HDR) | grep -v -e "^_[A-Z0-9_]\+_H_	" -e "^[A-Za-z]\+Ptr	" > tags

clean:
	$(Q)$(RM) $(OUT) $(OBJ) $(OUT_MAIN) $(PLUGDIR)/*

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(PLUGDIR) $(OBJDIR) $(LINKS) tags

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


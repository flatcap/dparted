CXX	?= g++
RM	= rm -fr
MKDIR	= mkdir -p

DEPDIR	= .dep
OBJDIR	= .obj

OUT	= dparted

SRC	+= container.cpp main.cpp list_visitor.cpp
OBJ	= $(SRC:%.cpp=$(OBJDIR)/%.o)

CFLAGS	+= -std=c++11
CFLAGS	+= -ggdb
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
CFLAGS	+= -O2
LDFLAGS	+= -pthread
LDFLAGS	+= -rdynamic

all:	$(OBJDIR) $(DEPDIR) $(OBJ) $(OUT)

# ----------------------------------------------------------------------------

#
# Pretty print
#
V	      = @
Q	      = $(V:1=)
QUIET_CC      = $(Q:@=@echo 'CC      '$<;)
QUIET_LINK    = $(Q:@=@echo 'LD      '$@;)

ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
	QUIET_CC      =
	QUIET_LINK    =
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
	$(Q)$(RM) $(OUT) $(OBJ)

distclean: clean
	$(Q)$(RM) $(DEPDIR) $(OBJDIR)

force:

-include $(SRC:%.cpp=$(DEPDIR)/%.d)


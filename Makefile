CXX	= g++
RM	= rm -fr

OUT	= main

SRC	+= container.cpp main.cpp
OBJ	= $(SRC:%.cpp=%.o)

CFLAGS	+= -std=c++11
CFLAGS	+= -Wall
CFLAGS	+= -Wextra
CFLAGS	+= -Wpedantic
LDFLAGS	+= -pthread

all:	$(OBJ) $(OUT)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OUT):	$(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OUT) $(OBJ)

distclean: clean
	$(RM) core.[0-9]*


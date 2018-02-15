CXX=g++

CXXFLAGS=-c -Wall
LDFLAGS=

TARGET=blbarchive

SRCDIR=src/
LIBDIR=lib/
OBJDIR=obj/
TARGETDIR=bin/

SRC=blbarchive.cpp main.cpp
LIB=blast.c
OBJ=$(SRC:.cpp=.o) $(LIB:.c=.o)

.PHONY: init clean

all: init blbarchive

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $(addprefix $(OBJDIR), $^) -o $(TARGETDIR)$@

%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) $< -o $(OBJDIR)$@

%.o: $(LIBDIR)%.c
	$(CXX) $(CXXFLAGS) $< -o $(OBJDIR)$@

init:
	mkdir -p $(OBJDIR)
	mkdir -p $(TARGETDIR)

clean:
	rm -rf $(TARGETDIR)
	rm -rf $(OBJDIR)

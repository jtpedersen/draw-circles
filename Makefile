CXX := g++
CC := gcc
LINK := g++

CFLAGS += -Wall 
CFLAGS += -Werror	

ifeq ($(debug),1) 
 COMMONFLAGS += -g
 COMMONFLAGS += -O0
endif

ifeq ($(release),1) 
 COMMONFLAGS += -O3
 COMMONFLAGS += -DNDEBUG
endif

ifeq ($(profile),1)
 CFLAGS += -pg	
endif


BUILDDIR = build
BINDIR = .
SRCDIR = .

# Common flags
#COMMONFLAGS += $(INCLUDES)
CXXFLAGS += $(COMMONFLAGS)
CFLAGS += -lm -lrt
CFLAGS += $(COMMONFLAGS)

UTILS += timer.cpp

EXEC= main
OBJS= main.cpp

BUILD_OBJS =$(addprefix $(BUILDDIR)/, $(addsuffix .o, $(OBJS)))
UTILS_BUILD_OBJS =$(addprefix $(BUILDDIR)/, $(addsuffix .o, $(UTILS)))


all: main

init:
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)

.SUFFIXES: .c .cpp .o

$(BUILDDIR)/%.c.o: $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

main:  $(BUILD_OBJS) $(UTILS_BUILD_OBJS) 
	$(LINK) $(BUILD_OBJS) $(UTILS_BUILD_OBJS) $(CFLAGS) -o$(BINDIR)/$(EXEC)


clean :
	rm -rf $(BUILDDIR)/*
	rm -f $(BINDIR)/$(EXEC)
	find . -name "*~" -exec rm {} \;

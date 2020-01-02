BINDIR = bin
OBJDIR = obj

CXX = g++-8

release: OPTIMISATION := -g0 -O3
release: example.elf

debug: OPTIMISATION := -g3 -O0
debug: example.elf

CXXFLAGS = $(OPTIMISATION) -no-pie -Wall -Werror -Wfatal-errors -pthread -std=c++17 -static

INCDIRS = ../include
LIBDIRS = $(OBJDIR)
LIBS    = dl stdc++fs

example.elf: | $(BINDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/example.elf main.cpp $(foreach D,$(INCDIRS),-I$D ) $(foreach D,$(LIBDIRS),-L$D ) $(foreach L,$(LIBS),-l$L )

.PHONY: example.elf

clean:
	rm -f $(BINDIR)/*
	rm -f $(OBJDIR)/*

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

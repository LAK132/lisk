BINDIR = bin
OBJDIR = obj
SRCDIR = src
INCDIR = inc

CXX = g++-8

CXXFLAGS = $(OPTIMISATION) -no-pie -Wall -Werror -Wfatal-errors -pthread -std=c++17 -static

INCDIRS = $(INCDIR)
LIBDIRS = $(OBJDIR)
LIBS    = dl stdc++fs

OUTPUT = $(BINDIR)/example.elf

release: OPTIMISATION := -g0 -O3
release: $(OUTPUT)

debug: OPTIMISATION := -g3 -O0
debug: $(OUTPUT)

COMPILED =
define COMPILE_TEMPLATE =
$(OBJDIR)/$(2).o: $(1)/$(2) $(3) Makefile | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $$@ -c $$< $(foreach D,$(INCDIRS),-I$D ) $(foreach D,$(LIBDIRS),-L$D ) $(foreach L,$(LIBS),-l$L )
COMPILED += $(OBJDIR)/$(2).o
endef

HEADERS = $(foreach D,atom.hpp callable.hpp environment.hpp eval.hpp eval.inl expression.hpp functor.hpp lambda.hpp lisk.hpp number.hpp pointer.hpp pointer.inl shared_list.hpp shared_list.inl string.hpp,$(INCDIR)/lisk/$D )

$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),atom.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),callable.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),environment.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),eval.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),expression.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),functor.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),lambda.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),lisk.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),number.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),pointer.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,$(SRCDIR),string.cpp,$(HEADERS)))
$(eval $(call COMPILE_TEMPLATE,example,main.cpp,$(HEADERS)))

$(BINDIR)/example.elf: $(COMPILED) | $(BINDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(COMPILED) $(foreach D,$(INCDIRS),-I$D ) $(foreach D,$(LIBDIRS),-L$D ) $(foreach L,$(LIBS),-l$L )

.PHONY: $(OUTPUT)

clean:
	rm -f $(BINDIR)/*
	rm -f $(OBJDIR)/*

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

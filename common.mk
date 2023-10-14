#############################################################################
# File       [ common.mk ]
# Author     [ littleshamoo ]
# Synopsis   [ project layout ]
# Date       [ 2010/10/07 created ]
#############################################################################

#############################################################################
# Block      [ external header files and libraries ]
# Synopsis   [ 'EXTINCLOC':    paths of external headers
#              'EXTOPTLIBLOC': paths of external optimized libraries
#              'EXTDBGLIBLOC': paths of external debug libraries
#            ]
#############################################################################
EXTINCLOC    =
EXTOPTLIBLOC =
EXTDBGLIBLOC =


# directory names
SRCDIR = src
PKGDIR = pkg
LIBDIR = lib
DBGDIR = dbg
OPTDIR = opt
INCDIR = include
BINDIR = bin

# System constant variables
SHELL = /bin/sh

# mode
MODE = opt

# Path for source makefiles
G_PKGDIR = ../../$(PKGDIR)
G_LIBDIR = ../../$(LIBDIR)
G_INCDIR = ../../$(INCDIR)
G_BINDIR = ../../$(BINDIR)
G_DBGDIR = ../../$(LIBDIR)/$(DBGDIR)
G_OPTDIR = ../../$(LIBDIR)/$(OPTDIR)

# C and C++ Sources
CSRCS     = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.c)
CHDRS     = $(wildcard $(SRCDIR)/*.h)
BASECSRCS = $(notdir $(CSRCS))
BASECHDRS = $(notdir $(CHDRS))
CDEPS     = $(addsuffix .d, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/, $(BASECSRCS))))
COBJS     = $(addsuffix .o, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/, $(BASECSRCS))))

# package libraries
CLIBS     = $(wildcard $(LIBDIR)/$(MODE)/*.a) \
			$(wildcard $(LIBDIR)/$(MODE)/*.so)
BASECLIBS = $(notdir $(CLIBS))

# lex and yacc
LNYDIR    = lex_n_yacc

LSRCS     = $(wildcard $(SRCDIR)/*.l)
BASELSRCS = $(notdir $(LSRCS))
LDEPS     = $(addsuffix .yy.d, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASELSRCS))))
LSECS     = $(addsuffix .yy.cpp, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASELSRCS))))
LOBJS     = $(addsuffix .yy.o, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASELSRCS))))

YSRCS     = $(wildcard $(SRCDIR)/*.y)
BASEYSRCS = $(notdir $(YSRCS))
BASEYHDRS = $(addsuffix .tab.h, $(basename $(BASEYSRCS)))
YDEPS     = $(addsuffix .tab.d, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASEYSRCS))))
YSECS     = $(addsuffix .tab.cpp, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASEYSRCS)))) \
			$(addsuffix .tab.hpp, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASEYSRCS))))
YOBJS     = $(addsuffix .tab.o, $(basename $(addprefix \
			$(LIBDIR)/$(MODE)/$(LNYDIR)/, $(BASEYSRCS))))

# Compiler variables
LEX          = flex
YACC         = bison
CXX          = g++ -std=c++11
CC           = gcc
OBJS         = $(COBJS) $(LOBJS) $(YOBJS)
DEPS         = $(CDEPS) $(LDEPS) $(YDEPS)
INCLOC       = -I$(SRCDIR) -I$(G_INCDIR) -I$(LIBDIR)/$(MODE)/$(LNYDIR) \
               $(addprefix -I,$(EXTINCLOC))

ifeq "$(MODE)" "$(DBGDIR)"
    CFLAGS   = -Wall -g -D DEBUG
    LIBLOC   = -L$(G_DBGDIR) $(addprefix -L,$(EXTDBGLIBLOC))
else
    CFLAGS   = -Wall -O3 -Wno-stringop-truncation -Wno-restrict -Wno-format-overflow
    LIBLOC   = -L$(G_OPTDIR) $(addprefix -L,$(EXTOPTLIBLOC))
endif

# Archive variables
AR      = ar
ARFLAGS = cr

# Self-defined functions
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)), $(1)))) \
          $(firstword $(1))

finddep = $(if $(word 1, $(1)), $(word 1, $(1)) $(call finddep, \
          $(wordlist 2, $(words $(1)), $(1)) $($(word 1, $(1))_DEP)), )


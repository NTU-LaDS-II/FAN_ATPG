#############################################################################
# File       [ rule.mk ]
# Author     [ littleshamoo ]
# Synopsis   [ rules for make targets ]
# Date       [ 2010/10/08 created ]
#############################################################################

# determine package name
PKGNAME = $(notdir $(CURDIR))

# determine package type
PKGTYPE = $(if $(filter $(PKGNAME),$(DYNPKGS)),dyn,sta)

# determine package dependency
PKGDEP =
ifdef $(PKGNAME)_DEP
    PKGDEP = $($(PKGNAME)_DEP)
endif

# determine external libraries
PKGEXT =
ifeq "$(MODE)" "dbg"
    ifdef $(PKGNAME)_DBGEXT
        PKGEXT = $($(PKGNAME)_DBGEXT)
    endif
else
    ifdef $(PKGNAME)_OPTEXT
        PKGEXT = $($(PKGNAME)_OPTEXT)
    endif
endif

# determine link libraries for exe package
LNKLIBS = -l$(PKGNAME)
ifdef $(PKGNAME)_DEP
    LNKLIBS += $(addprefix -l,$(call finddep,$($(PKGNAME)_DEP)))
else
    LNKLIBS += $(addprefix -l,$(PKGNAME))
endif
LNKLIBS += $(addprefix -l,$(PKGEXT))

# determine required libraries for exe package
REQLIBS =
ifdef $(PKGNAME)_DEP
    REQLIBS += $(sort $(foreach pkg,$(call finddep,$($(PKGNAME)_DEP)),$(if $(filter $(pkg),$(DYNPKGS)),$(G_LIBDIR)/$(MODE)/lib$(pkg).so,$(G_LIBDIR)/$(MODE)/lib$(pkg).a)))
endif

# determine required object files for library
NOLIBOBJS = $(addsuffix .o,$(basename $(addprefix $(LIBDIR)/$(MODE)/,$($(PKGNAME)_EXE))))
LIBOBJS = $(filter-out $(NOLIBOBJS),$(OBJS))

# determine binary files
PKGEXE =
ifdef $(PKGNAME)_EXE
    PKGEXE = $(basename $($(PKGNAME)_EXE))
endif

# determine package c flags
PKGFLAGS =
ifdef $(PKGNAME)_CFLAGS
    PKGFLAGS = $($(PKGNAME)_CFLAGS)
endif

# include package binary rules
PKGBININC = $(addsuffix .exe,$(addprefix $(LIBDIR)/$(MODE)/,$(PKGEXE)))

# determine package targets
PKGBIN    = $(addprefix $(BINDIR)/$(MODE)/,$(foreach src,$($(PKGNAME)_EXE),$(if $($(src)_$(PKGNAME)_BIN),$($(src)_$(PKGNAME)_BIN),$(basename $(src)))))
PKGBINLNK = $(addprefix $(G_BINDIR)/$(MODE)/,$(notdir $(PKGBIN)))
PKGHDRLNK = $(addprefix $(G_INCDIR)/$(PKGNAME)/,$(BASECHDRS))

ifeq "$(PKGTYPE)" "dyn"
    PKGFLAGS  += -fPIC
    PKGLIB     = $(LIBDIR)/$(MODE)/lib$(PKGNAME).so
    PKGLIBLNK  = $(G_LIBDIR)/$(MODE)/lib$(PKGNAME).so
else
    PKGFLAGS  +=
    PKGLIB     = $(LIBDIR)/$(MODE)/lib$(PKGNAME).a
    PKGLIBLNK  = $(G_LIBDIR)/$(MODE)/lib$(PKGNAME).a
endif

.PHONY: all install global uninstall clean

.SECONDARY: $(LSECS) $(YSECS)

all : install
	@echo -n

install :
	@echo "  PKG   $(PKGNAME) ($(MODE))"
	@for pkg in $(PKGDEP); do \
		if [ ! -d $(G_PKGDIR)/$$pkg ]; then \
			echo "!! Warning: package \`$$pkg' not found and ignored"; \
			continue; \
		fi; \
		echo "  CHECK $$pkg"; \
		cd $(G_PKGDIR)/$$pkg; \
		make install --no-print-directory MODE="$(MODE)"; \
		cd $(G_PKGDIR)/$(PKGNAME); \
	done
	@make global --no-print-directory MODE="$(MODE)"

global : $(PKGLIBLNK) $(PKGHDRLNK) $(PKGBINLNK)
	@echo -n

uninstall :
	@echo "  UNINS $(PKGNAME)"
	@rm -f $(PKGLIBLNK)
	@rm -f $(PKGHDRLNK)
	@rm -Rf $(G_INCDIR)/$(PKGNAME)
	@rm -f $(PKGBINLNK)

clean : uninstall
	@echo "  CLN   $(PKGNAME)"
	@rm -f $(LIBDIR)/$(MODE)/*.d
	@rm -f $(LIBDIR)/$(MODE)/*.o
	@rm -Rf $(LIBDIR)/$(MODE)/$(LNYDIR)
	@rm -f $(PKGBIN)
	@rm -f $(PKGBININC)
	@rm -f $(PKGLIB)


# Binary rules
$(LIBDIR)/$(MODE)/%.exe : $(SRCDIR)/%.cpp ../../info.mk
	@echo "    EXE   $(notdir $@)"
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@binname=$(if $($(notdir $<)_$(PKGNAME)_BIN),$(BINDIR)/$(MODE)/$($(notdir $<)_$(PKGNAME)_BIN),$(BINDIR)/$(MODE)/$(notdir $(basename $<))); \
	echo "$$binname : $(addprefix $(LIBDIR)/$(MODE)/,$(addsuffix .o,$(basename $(notdir $<)))) \$$(LIBOBJS) \$$(REQLIBS)" > $@
	@echo "	@if [ ! -d \$$(BINDIR)/\$$(MODE) ]; then \\" >> $@
	@echo "		mkdir -p \$$(BINDIR)/\$$(MODE); \\" >> $@
	@echo "	fi" >> $@
	@echo "	@echo \"    LD    \$$(notdir \$$@)\"" >> $@
	@echo "	@\$$(CXX) \$$(CFLAGS) \$$(PKGFLAGS) \$$(INCLOC) \$$< \$$(LIBOBJS) \$$(LIBLOC) \$$(LNKLIBS) -o \$$@" >> $@
	@echo "" >> $@


$(LIBDIR)/$(MODE)/%.exe : $(SRCDIR)/%.c ../../info.mk
	@echo "    EXE   $(notdir $@)"
	@mkdir -p $(LIBDIR)/$(MODE);
	@echo "$($(notdir $<)_$(PKGNAME)_BIN)"
	@binname=$(if $($(notdir $<)_$(PKGNAME)_BIN),$(BINDIR)/$(MODE)/$($(notdir $<)_$(PKGNAME)_BIN),$(BINDIR)/$(MODE)/$(notdir $(basename $<))); \
	echo "$$binname : $(addprefix $(LIBDIR)/$(MODE)/,$(addsuffix .o,$(basename $(notdir $<)))) \$$(LIBOBJS) \$$(REQLIBS)" > $@
	@echo "	@if [ ! -d \$$(BINDIR)/\$$(MODE) ]; then \\" >> $@
	@echo "		mkdir -p \$$(BINDIR)/\$$(MODE); \\" >> $@
	@echo "	fi" >> $@
	@echo "	@echo \"    LD    \$$(notdir \$$@)\"" >> $@
	@echo "	@\$$(CC) \$$(CFLAGS) \$$(PKGFLAGS) \$$(INCLOC) \$$< \$$(LIBOBJS) \$$(LIBLOC) \$$(LNKLIBS) -o \$$@" >> $@
	@echo "" >> $@


$(G_BINDIR)/$(MODE)/% : $(BINDIR)/$(MODE)/%
	@if [ ! -d $(G_BINDIR)/$(MODE) ]; then \
		mkdir -p $(G_BINDIR)/$(MODE); \
	fi
	@echo "    INS   $(notdir $@)"
	@cp $< $@


# Library rules
$(LIBDIR)/$(MODE)/lib$(PKGNAME).a : $(LIBOBJS)
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    AR    $(notdir $@)"
	@$(AR) $(ARFLAGS) $@ $^

$(LIBDIR)/$(MODE)/lib$(PKGNAME).so : $(LIBOBJS)
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    BUILD $(notdir $@)"
	@$(CXX) -shared -Wl,-soname,$@ -o $@ $^

$(G_LIBDIR)/$(MODE)/%.a : $(LIBDIR)/$(MODE)/%.a
	@if [ ! -d $(G_LIBDIR)/$(MODE) ]; then \
		mkdir -p $(G_LIBDIR)/$(MODE); \
	fi
	@echo "    INS   $(notdir $@)"
	@cp $< $@

$(G_LIBDIR)/$(MODE)/%.so : $(LIBDIR)/$(MODE)/%.so
	@if [ ! -d $(G_LIBDIR)/$(MODE) ]; then \
		mkdir -p $(G_LIBDIR)/$(MODE); \
	fi
	@echo "    INS   $(notdir $@)"
	@cp $< $@

# C and C++ rules
$(G_INCDIR)/$(PKGNAME)/%.h : $(SRCDIR)/%.h
	@if [ ! -d $(G_INCDIR)/$(PKGNAME) ]; then \
		mkdir -p $(G_INCDIR)/$(PKGNAME); \
	fi
	@echo "    INS   $(notdir $@)"
	@cp $< $@

$(LIBDIR)/$(MODE)/%.d : $(SRCDIR)/%.cpp
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    DEP   $(notdir $@)"
	@$(CXX) -MM $(INCLOC) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(LIBDIR)/$(MODE)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm $@.$$$$

$(LIBDIR)/$(MODE)/%.d : $(SRCDIR)/%.c
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    DEP   $(notdir $@)"
	@$(CC) -MM $(INCLOC) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(LIBDIR)/$(MODE)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm $@.$$$$

$(LIBDIR)/$(MODE)/%.o : $(SRCDIR)/%.cpp
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    CXX   $(notdir $<)"
	@$(CXX) $(CFLAGS) $(PKGFLAGS) $(INCLOC) -c -o $@ $<

$(LIBDIR)/$(MODE)/%.o : $(SRCDIR)/%.c
	@if [ ! -d $(LIBDIR)/$(MODE) ]; then \
		mkdir -p $(LIBDIR)/$(MODE); \
	fi
	@echo "    CC    $(notdir $<)"
	@$(CC) $(CFLAGS) $(PKGFLAGS) $(INCLOC) -c -o $@ $<


# lex and yacc rules
$(LIBDIR)/$(MODE)/$(LNYDIR)/%.yy.cpp : $(SRCDIR)/%.l
	@if [ ! -d $(LIBDIR)/$(MODE)/$(LNYDIR) ]; then \
		mkdir -p $(LIBDIR)/$(MODE)/$(LNYDIR); \
	fi
	@echo "    LEX   $(notdir $@)"
	@$(LEX) -o$@ -P$(subst .yy.cpp,,$(notdir $@)) $<

$(LIBDIR)/$(MODE)/$(LNYDIR)/%.tab.cpp : $(SRCDIR)/%.y
	@if [ ! -d $(LIBDIR)/$(MODE)/$(LNYDIR) ]; then \
		mkdir -p $(LIBDIR)/$(MODE)/$(LNYDIR); \
	fi
	@echo "    YACC  $(notdir $@)"
	@$(YACC) -v -g -d -p $(subst .tab.cpp,,$(notdir $@)) -o $@ $<

$(LIBDIR)/$(MODE)/$(LNYDIR)/%.d : $(LIBDIR)/$(MODE)/$(LNYDIR)/%.cpp
	@if [ ! -d $(LIBDIR)/$(MODE)/$(LNYDIR) ]; then \
		mkdir -p $(LIBDIR)/$(MODE)/$(LNYDIR); \
	fi
	@echo "    DEP   $(notdir $@)"
	@$(CXX) -MM $(INCLOC) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(LIBDIR)/$(MODE)/$(LNYDIR)/\1.o $@ : ,g' \
	< $@.$$$$ > $@; \
	rm $@.$$$$

$(LIBDIR)/$(MODE)/$(LNYDIR)/%.o : $(LIBDIR)/$(MODE)/$(LNYDIR)/%.cpp
	@if [ ! -d $(LIBDIR)/$(MODE)/$(LNYDIR) ]; then \
		mkdir -p $(LIBDIR)/$(MODE)/$(LNYDIR); \
	fi
	@echo "    CXX   $(notdir $<)"
	@$(CXX) $(CFLAGS) $(PKGFLAGS) $(INCLOC) -c -o $@ $<


# Include dependency files
ifeq "$(MAKECMDGOALS)" "global"
    -include $(DEPS)
endif


# Include binary rules
ifeq "$(MAKECMDGOALS)" "global"
    -include $(PKGBININC)
endif


#
# Makefile snippet
#

schemelibdir = $(pkgdatadir)/$(VERSION)

guilemoduledir = $(datadir)/guile/gnome
guilegwmoduledir = $(datadir)/guile/gnome/gw

AM_CFLAGS = -I. -I$(srcdir) $(WARN_CFLAGS) $(DEBUG_CFLAGS)

# For overriding from the command line (e.g. --debug)
GUILE_FLAGS = 

SUFFIXES = .x .doc

GUILE_SNARF_CFLAGS = $(DEFS) $(AM_CFLAGS) $(GUILE_CFLAGS) $(G_WRAP_CFLAGS)

PKG_PATH = $(shell echo $(AG_PACKAGES:%=$(top_srcdir)/% $(srcdir)) | sed 's, ,:,g')
@MK@ifneq ($(top_srcdir),$(top_builddir))
	PKG_PATH += $(shell echo $(AG_PACKAGES:%=$(top_builddir)/%) | sed 's, ,:,g')
@MK@endif

GUILE_LOAD_PATH := $(PKG_PATH):${G_WRAP_MODULE_DIR}:${GUILE_LOAD_PATH}
export GUILE_LOAD_PATH

.c.x:
	guile-snarf $(GUILE_SNARF_CFLAGS) $< > $@ \
	|| { rm $@; false; }
.c.doc:
	-(guile-func-name-check $<)
	(guile-snarf-docs $(GUILE_SNARF_CFLAGS) $< | \
	guile_filter_doc_snarfage --filter-snarfage) > $@ || { rm $@; false; }

%.scm guile-gnome-gw-%.c: %-spec.scm
	guile $(GUILE_FLAGS) -c \
	  "(debug-set! stack 100000) \
	   (use-modules (g-wrap)) \
	   (use-modules (g-wrap guile)) \
	   (use-modules (gnome gw $*-spec)) \
	   (generate-wrapset guile 'gnome-$* \"guile-gnome-gw-$*\")"
	mv guile-gnome-gw-$*.scm $*.scm

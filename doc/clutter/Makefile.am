################################################################################
## guile-gnome-clutter.info

include $(top_srcdir)/generate-docs.mk

docbook_xml_wildcard = ~/src/clutter-1.8.4/doc/reference/clutter/xml/clutter-*.xml
docbook_xml_files = $(filter-out %eglnative.xml %win32.xml %x11.xml %glx.xml,$(wildcard $(docbook_xml_wildcard)))

wrapset_stem = clutter

EXTRA_DIST = overrides.texi

info_TEXINFOS=guile-gnome-clutter.texi
guile_gnome_clutter_TEXINFOS=overview.texi \
	$(wildcard section-*.texi) $(wildcard defuns-*.texi) undocumented.texi

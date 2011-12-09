################################################################################
## guile-gnome-clutter.info

include $(top_srcdir)/generate-docs.mk

docbook_xml_wildcard = ~/src/clutter/clutter/doc/reference/clutter/xml/*.xml
docbook_xml_files = $(filter %x11.xml %glx.xml,$(wildcard $(docbook_xml_wildcard)))

wrapset_stem = clutter-glx

EXTRA_DIST = overrides.texi

info_TEXINFOS=guile-gnome-clutter-glx.texi
guile_gnome_clutter_glx_TEXINFOS=overview.texi \
	$(wildcard section-*.texi) $(wildcard defuns-*.texi) undocumented.texi

################################################################################
## guile-gnome-gstreamer.info

include $(top_srcdir)/generate-docs.mk

docbook_xml_wildcard = ~/src/gst-all/gstreamer/docs/gst/xml/*.xml
docbook_xml_files = $(wildcard $(docbook_xml_wildcard))

wrapset_stem = gstreamer

EXTRA_DIST = overrides.texi

info_TEXINFOS=guile-gnome-gstreamer.texi
guile_gnome_gstreamer_TEXINFOS=overview.texi \
	$(wildcard section-*.texi) $(wildcard defuns-*.texi)

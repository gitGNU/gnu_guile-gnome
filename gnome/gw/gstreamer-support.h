#include <gst/gst.h>
#include <libguile.h>
#include <guile-gnome-gobject.h>


void		gst_bin_attach_iterator (GstBin *bin);
void		gst_bin_remove_iterator (GstBin *bin);

SCM		_wrap_gst_buffer_get_data (GstBuffer *buf, SCM prot);
void		_wrap_gst_buffer_set_data (GstBuffer *buf, SCM uvect);

GstClockReturn 	_wrap_gst_clock_id_wait_async (GstClockID id,
                                               SCM callback);

void		gst_debug_use_custom_handler (void);

void		_wrap_gst_pad_set_chain_function (GstPad *pad, SCM chain_function);
void		_wrap_gst_pad_set_link_function (GstPad *pad, SCM link_function);
void		_wrap_gst_pad_set_get_function (GstPad *pad, SCM get_function);

/* Macro helpers */
GstPad*		gst_pad_realize (GstPad *pad);
const gchar*	gst_pad_template_get_name_template (GstPadTemplate *templ);
GstPadDirection	gst_pad_template_get_direction (GstPadTemplate *templ);
GstPadPresence	gst_pad_template_get_presence (GstPadTemplate *templ);
const gchar*	gst_plugin_feature_get_name (GstPluginFeature *feature);

void		_wrap_gst_structure_for_each (GstStructure *str, SCM proc);

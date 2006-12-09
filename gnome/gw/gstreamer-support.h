#include <gst/gst.h>
#include <libguile.h>
#include <guile-gnome-gobject.h>


SCM		_wrap_gst_buffer_get_data (GstBuffer *buf, SCM prot);
void		_wrap_gst_buffer_set_data (GstBuffer *buf, SCM uvect);

GstClockReturn 	_wrap_gst_clock_id_wait_async (GstClockID id,
                                               SCM callback);

void		gst_debug_use_custom_handler (void);

void		_wrap_gst_pad_set_chain_function (GstPad *pad, SCM chain_function);
void		_wrap_gst_pad_set_setcaps_function (GstPad *pad, SCM setcaps_function);
void		_wrap_gst_pad_set_getrange_function (GstPad *pad, SCM getrange_function);

/* Macro helpers */
const gchar*	gst_pad_template_get_name_template (GstPadTemplate *templ);
GstPadDirection	gst_pad_template_get_direction (GstPadTemplate *templ);
GstPadPresence	gst_pad_template_get_presence (GstPadTemplate *templ);
const gchar*	gst_plugin_feature_get_name (GstPluginFeature *feature);

void		_wrap_gst_structure_for_each (GstStructure *str, SCM proc);
GstStructure*	_wrap_gst_structure_from_string (const gchar *str);

SCM		scm_from_gst_fourcc (const GValue *value);
void		scm_to_gst_fourcc (SCM scm, GValue *value);
SCM		scm_from_gst_fraction (const GValue *value);
void		scm_to_gst_fraction (SCM scm, GValue *value);
SCM		scm_from_gst_int_range (const GValue *value);
void		scm_to_gst_int_range (SCM scm, GValue *value);
SCM		scm_from_gst_double_range (const GValue *value);
void		scm_to_gst_double_range (SCM scm, GValue *value);
SCM		scm_from_gst_fraction_range (const GValue *value);
void		scm_to_gst_fraction_range (SCM scm, GValue *value);
SCM		scm_from_gst_list (const GValue *value);
void		scm_to_gst_list (SCM scm, GValue *value);
SCM		scm_from_gst_array (const GValue *value);
void		scm_to_gst_array (SCM scm, GValue *value);

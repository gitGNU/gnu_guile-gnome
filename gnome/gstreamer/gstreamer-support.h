#include <gst/gst.h>
#include <libguile.h>
#include <guile-gnome-gobject.h>


void		gst_bin_attach_iterator (GstBin *bin);
void		gst_bin_remove_iterator (GstBin *bin);

SCM		_wrap_gst_buffer_get_data (GstBuffer *buf, SCM prot);
void		_wrap_gst_buffer_set_data (GstBuffer *buf, SCM uvect);

GstClockReturn 	_wrap_gst_clock_id_wait_async (GstClockID id,
                                               SCM callback);

void		_wrap_gst_pad_set_chain_function (SCM pad, SCM chain_function);
void		_wrap_gst_pad_set_link_function (SCM pad, SCM link_function);
void		_wrap_gst_pad_set_get_function (SCM pad, SCM get_function);

void		_wrap_gst_structure_for_each (GstStructure *str, SCM proc);

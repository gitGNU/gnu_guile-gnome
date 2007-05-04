#include "gstreamer-support.h"

#include <string.h> /* strlen */
#include <unistd.h> /* getpid */

#define GRUNTIME_ERROR(format, func_name, args...) \
  scm_error (scm_str2symbol ("gruntime-error"), func_name, format, \
             ##args, SCM_EOL)


/***********************************************************************
 * GstBuffer
 */

typedef SCM (*uvec_proc)(void*data, size_t n);
struct uvec_wrapper {
  const char *sym;
  uvec_proc proc;
  int bits;
};

static const struct uvec_wrapper uvec_wrappers[] =
{{"u8", (uvec_proc)scm_take_u8vector, 8},
 {"s8", (uvec_proc)scm_take_s8vector, 8},
 {"u16", (uvec_proc)scm_take_u16vector, 16},
 {"s16", (uvec_proc)scm_take_s16vector, 16},
 {"u32", (uvec_proc)scm_take_u32vector, 32},
 {"s32", (uvec_proc)scm_take_s32vector, 32},
 {"u64", (uvec_proc)scm_take_u64vector, 64},
 {"s64", (uvec_proc)scm_take_s64vector, 64},
 {"f32", (uvec_proc)scm_take_f32vector, 32},
 {"f64", (uvec_proc)scm_take_f64vector, 64}};

/* code stolen from unif.c:scm_make_uve */
SCM
_wrap_gst_buffer_get_data (GstBuffer *buf, SCM prot)
#define FUNC_NAME "gst-buffer-get-data"
{
  guint size, i;
  void *data;

  if (GST_BUFFER_SIZE (buf) == 0 || GST_BUFFER_DATA (buf) == NULL)
    return SCM_BOOL_F;

  
  size = GST_BUFFER_SIZE (buf);
  data = GST_BUFFER_DATA (buf);
  
  for (i=0; i<G_N_ELEMENTS(uvec_wrappers); i++) {
    struct uvec_wrapper wrapper = uvec_wrappers[i];
    if (scm_is_eq (prot, scm_from_locale_symbol (wrapper.sym)))
      /* copy :-( */
      return wrapper.proc (data, i / (wrapper.bits/8));
  }

  GRUNTIME_ERROR ("Unrecognized format specifier.",
                  "gst-buffer-get-data", SCM_EOL);
}
#undef FUNC_NAME

void
_wrap_gst_buffer_set_data (GstBuffer *buf, SCM uvect)
{
  scm_t_array_handle handle;
  const void *data = NULL;
  size_t len = 0;
  ssize_t inc = 0;
  
  if (!scm_is_uniform_vector (uvect))
    GRUNTIME_ERROR ("Data must be a uniform vector.",
                    "gst-buffer-set-data", uvect);

  data = scm_uniform_vector_elements (uvect, &handle, &len, &inc);

  if (GST_BUFFER_MALLOCDATA (buf))
    g_free (GST_BUFFER_MALLOCDATA (buf));

  if (data && len)
    GST_BUFFER_DATA (buf) = g_memdup (data, (guint)(len * inc));
  else
    GST_BUFFER_DATA (buf) = NULL;
  
  GST_BUFFER_MALLOCDATA (buf) = GST_BUFFER_DATA (buf);
  GST_BUFFER_SIZE (buf) = (guint)(len * inc);
}


/***********************************************************************
 * GstClock
 */

static gboolean
call_async_clock_wait (GstClock *clock, GstClockTime time, GstClockID id, SCM closure)
{
  /* should pass the clock as well... */
  return SCM_NFALSEP (scm_call_1 (scm_glib_gc_unprotect_object (closure),
                                  scm_ulong_long2num (time)));
}

GstClockReturn
_wrap_gst_clock_id_wait_async (GstClockID id,
                               SCM callback)
{
  return gst_clock_id_wait_async (id, (GstClockCallback)call_async_clock_wait,
                                  SCM_PACK (scm_glib_gc_protect_object (callback)));
}


/***********************************************************************
 * GstDebug
 */

/* temporary hack until gst_debug_print_object */
static gchar *
gst_debug_print_object (gpointer ptr)
{
  GObject *object = (GObject *) ptr;

#ifdef unused
  /* This is a cute trick to detect unmapped memory, but is unportable,
   * slow, screws around with madvise, and not actually that useful. */
  {
    int ret;

    ret = madvise ((void *) ((unsigned long) ptr & (~0xfff)), 4096, 0);
    if (ret == -1 && errno == ENOMEM) {
      buffer = g_strdup_printf ("%p (unmapped memory)", ptr);
    }
  }
#endif

  /* nicely printed object */
  if (object == NULL) {
    return g_strdup ("(NULL)");
  }
  if (*(GType *) ptr == GST_TYPE_CAPS) {
    return gst_caps_to_string ((GstCaps *) ptr);
  }
  if (*(GType *) ptr == GST_TYPE_STRUCTURE) {
    return gst_structure_to_string ((GstStructure *) ptr);
  }
#ifdef USE_POISONING
  if (*(guint32 *) ptr == 0xffffffff) {
    return g_strdup_printf ("<poisoned@%p>", ptr);
  }
#endif
  if (GST_IS_PAD (object) && GST_OBJECT_NAME (object)) {
    return g_strdup_printf ("<%s:%s>", GST_DEBUG_PAD_NAME (object));
  }
  if (GST_IS_OBJECT (object) && GST_OBJECT_NAME (object)) {
    return g_strdup_printf ("<%s>", GST_OBJECT_NAME (object));
  }
  if (G_IS_OBJECT (object)) {
    return g_strdup_printf ("<%s@%p>", G_OBJECT_TYPE_NAME (object), object);
  }

  return g_strdup_printf ("%p", ptr);
}

/* define our own custom handler so that we can tweak it. knobs to come
 * later. */
void
gst_guile_debug_logger (GstDebugCategory * category, GstDebugLevel level,
    const gchar * file, const gchar * function, gint line,
    GObject * object, GstDebugMessage * message, gpointer unused)
{
  gboolean verbose = FALSE;
  gchar *color;
  gchar *clear;
  gchar *obj;
  gchar *pidcolor;
  gint pid;
  GTimeVal now;

  if (level > gst_debug_category_get_threshold (category))
    return;

  pid = getpid ();

  /* color info */
  if (gst_debug_is_colored ()) {
    color =
        gst_debug_construct_term_color (gst_debug_category_get_color
        (category));
    clear = "\033[00m";
    if (verbose)
      pidcolor = g_strdup_printf ("\033[3%1dm", pid % 6 + 31);
  } else {
    color = g_strdup ("");
    clear = "";
    if (verbose)
      pidcolor = g_strdup ("");
  }

  obj = object ? gst_debug_print_object (object) : g_strdup ("");

  if (verbose) {
    g_get_current_time (&now);
    g_printerr ("%s (%p - %" GST_TIME_FORMAT
                ") %s%15s%s(%s%5d%s) %s%s(%d):%s:%s%s %s\n",
                gst_debug_level_get_name (level), g_thread_self (),
                GST_TIME_ARGS (GST_TIMEVAL_TO_TIME (now)),
                color, gst_debug_category_get_name (category), clear,
                pidcolor, pid, clear,
                color, file, line, function, obj, clear,
                gst_debug_message_get (message));
  } else {
    g_printerr ("%s%15.15s%s %-30.30s %s %s\n",
                color, gst_debug_category_get_name (category), clear,
                function, obj,
                gst_debug_message_get (message));
  }
    
  g_free (color);
  if (verbose)
    g_free (pidcolor);
  g_free (obj);
}

void
gst_debug_use_custom_handler (void)
{
  static gboolean customized = FALSE;
  if (!customized) {
    customized = TRUE;
    gst_debug_remove_log_function (gst_debug_log_default);
    gst_debug_add_log_function (gst_guile_debug_logger, NULL);
  }
}

/***********************************************************************
 * GstMessage
 */
GstMessageType
_wrap_gst_message_get_type (GstMessage *msg)
{
  return GST_MESSAGE_TYPE (msg);
}

GstObject*
_wrap_gst_message_get_src (GstMessage *msg)
{
  return GST_MESSAGE_SRC (msg);
}

/***********************************************************************
 * GstPad
 */

typedef struct {
  SCM setcaps_function;
  SCM chain_function;
  SCM getrange_function;
} GuileGstPadPrivate;

static void
free_pad_private (gpointer data)
{
  GuileGstPadPrivate *private = data;

  if (scm_is_true (private->setcaps_function))
    scm_glib_gc_unprotect_object (private->setcaps_function);
  
  if (scm_is_true (private->getrange_function))
    scm_glib_gc_unprotect_object (private->getrange_function);
  
  if (scm_is_true (private->chain_function))
    scm_glib_gc_unprotect_object (private->chain_function);
}

static GuileGstPadPrivate*
pad_private(GstPad *pad)
{
  GuileGstPadPrivate* private;
  static GQuark padprivate = 0;

  if (!padprivate)
    padprivate = g_quark_from_static_string ("%guile-pad-private");
  
  private = g_object_get_qdata (G_OBJECT (pad), padprivate);
  if (private == NULL) {
    private = g_new (GuileGstPadPrivate, 1);
    private->setcaps_function = SCM_BOOL_F;
    private->chain_function = SCM_BOOL_F;
    private->getrange_function = SCM_BOOL_F;
    g_object_set_qdata_full (G_OBJECT (pad), padprivate, private,
                             free_pad_private);
  }

  return private;
}

static GstFlowReturn
call_chain_function (GstPad *pad, GstBuffer* buf)
{
  SCM scm_buf, ret;
  GuileGstPadPrivate *private;
  GValue *tmp;
  GstFlowReturn fret;
  
  private = pad_private (pad);
  scm_buf = scm_c_make_gvalue (GST_TYPE_BUFFER);
  g_value_set_boxed ((GValue*)SCM_SMOB_DATA (scm_buf), buf);

  ret = scm_call_2 (private->chain_function,
                    scm_c_gtype_instance_to_scm ((GTypeInstance*)pad),
                    scm_buf);
  tmp = scm_c_scm_to_gvalue (GST_TYPE_FLOW_RETURN, ret);
  fret = g_value_get_enum (tmp);
  g_value_unset (tmp);
  g_free (tmp);
  return fret;
}

void
_wrap_gst_pad_set_chain_function (GstPad *pad, SCM chain_function)
{
#define FUNC_NAME "gst-pad-set-chain-function"
  GuileGstPadPrivate *private;
  
  SCM_VALIDATE_PROC (2, chain_function);

  private = pad_private (pad);
  if (SCM_NFALSEP (private->chain_function))
    scm_glib_gc_unprotect_object (private->chain_function);

  private->chain_function = scm_glib_gc_protect_object (chain_function);

  gst_pad_set_chain_function ((GstPad*)pad, call_chain_function);
#undef FUNC_NAME
}

static GstFlowReturn
call_getrange_function (GstPad *pad, guint64 offset, guint len, GstBuffer **buf)
{
  SCM ret;
  GuileGstPadPrivate *private;
  GValue *tmp;
  GstFlowReturn fret;
  
  private = pad_private (pad);
  ret = scm_call_2 (private->getrange_function,
                    scm_from_uint64 (offset),
                    scm_from_uint (len));
  if (SCM_TYP16_PREDICATE (scm_tc16_gtype_instance, ret)) {
    GTypeInstance *tbuf = (GTypeInstance*)SCM_SMOB_DATA (ret);
    *buf = (GstBuffer*) tbuf;
    return GST_FLOW_OK;
  } else {
    tmp = scm_c_scm_to_gvalue (GST_TYPE_FLOW_RETURN, ret);
    fret = g_value_get_enum (tmp);
    g_value_unset (tmp);
    g_free (tmp);
    return fret;
  }
}

void
_wrap_gst_pad_set_getrange_function (GstPad* pad, SCM getrange_function)
{
#define FUNC_NAME "gst-pad-set-getrange-function"
  GuileGstPadPrivate *private;
  
  SCM_VALIDATE_PROC (2, getrange_function);

  private = pad_private (pad);
  if (SCM_NFALSEP (private->getrange_function))
    scm_glib_gc_unprotect_object (private->getrange_function);

  private->getrange_function = scm_glib_gc_protect_object (getrange_function);

  gst_pad_set_getrange_function (pad, call_getrange_function);
#undef FUNC_NAME
}

static gboolean
call_setcaps_function(GstPad *pad, GstCaps *caps)
{
  SCM scm_caps, ret;
  GuileGstPadPrivate *private = pad_private (pad);
  
  scm_caps = scm_c_make_gvalue (GST_TYPE_CAPS);
  g_value_set_boxed ((GValue*)SCM_SMOB_DATA (scm_caps), caps);

  ret = scm_call_2 (private->setcaps_function,
                    scm_c_gtype_instance_to_scm ((GTypeInstance*)pad),
                    scm_caps);

  return scm_is_true (ret);
}

void
_wrap_gst_pad_set_setcaps_function (GstPad *pad, SCM setcaps_function)
{
#define FUNC_NAME "gst-pad-set-setcaps-function"
  GuileGstPadPrivate *private;
  
  SCM_VALIDATE_PROC (2, setcaps_function);

  private = pad_private (pad);
  if (SCM_NFALSEP (private->setcaps_function))
    scm_glib_gc_unprotect_object (private->setcaps_function);

  private->setcaps_function = scm_glib_gc_protect_object (setcaps_function);

  gst_pad_set_setcaps_function (pad, call_setcaps_function);
#undef FUNC_NAME
}

/* Macro helpers */
const gchar*
gst_pad_template_get_name_template (GstPadTemplate *templ)
{
  return GST_PAD_TEMPLATE_NAME_TEMPLATE (templ);
}
GstPadDirection
gst_pad_template_get_direction (GstPadTemplate *templ)
{
  return GST_PAD_TEMPLATE_DIRECTION (templ);
}
GstPadPresence
gst_pad_template_get_presence (GstPadTemplate *templ)
{
  return GST_PAD_TEMPLATE_PRESENCE (templ);
}
const gchar*
gst_plugin_feature_get_name (GstPluginFeature *feature)
{
  return GST_PLUGIN_FEATURE_NAME (feature);
}

static void
struct_for_each_func (GQuark id, GValue *value, SCM proc)
{
  GValue *tmp;
  SCM svalue;
  
  tmp = g_new0 (GValue, 1);
  g_value_init (tmp, G_VALUE_TYPE (value));
  g_value_copy (value, tmp);
  SCM_NEWSMOB (svalue, scm_tc16_gvalue, tmp);
  
  scm_call_2 (proc, scm_makfrom0str (g_quark_to_string (id)), svalue);
}
              
void
_wrap_gst_structure_for_each (GstStructure *str, SCM proc)
#define FUNC_NAME "gst-structure-for-each"
{
  SCM_VALIDATE_PROC (2, proc);

  gst_structure_foreach (str, (GstStructureForeachFunc)struct_for_each_func, proc);
}
#undef FUNC_NAME

GstStructure*
_wrap_gst_structure_from_string (const gchar *str)
{
  return gst_structure_from_string (str, NULL);
}

SCM
scm_from_gst_fourcc (const GValue *value)
{
  return scm_take_locale_string
    (g_strdup_printf ("%"GST_FOURCC_FORMAT,
                      GST_FOURCC_ARGS (gst_value_get_fourcc (value))));
}

void
scm_to_gst_fourcc (SCM scm, GValue *value)
{
  char *str = scm_to_locale_string (scm);
  g_value_init (value, GST_TYPE_FOURCC);
  if (strlen (str) >= 4) {
    gst_value_set_fourcc (value,
                        GST_STR_FOURCC (str));
    free (scm);
  }
}
  
SCM
scm_from_gst_fraction (const GValue *value)
{
  return scm_divide (scm_from_int (gst_value_get_fraction_numerator (value)),
                     scm_from_int (gst_value_get_fraction_denominator (value)));
}

void
scm_to_gst_fraction (SCM scm, GValue *value)
{
  g_value_init (value, GST_TYPE_FRACTION);
  gst_value_set_fraction (value,
                          scm_to_int (scm_numerator (scm)),
                          scm_to_int (scm_denominator (scm)));
}
  
SCM
scm_from_gst_int_range (const GValue *value)
{
  return scm_cons (scm_from_int (gst_value_get_int_range_min (value)),
                   scm_from_int (gst_value_get_int_range_max (value)));
}

void
scm_to_gst_int_range (SCM scm, GValue *value)
{
  g_value_init (value, GST_TYPE_INT_RANGE);
  gst_value_set_int_range (value,
                           scm_to_int (scm_car (scm)),
                           scm_to_int (scm_cdr (scm)));
}
  
SCM
scm_from_gst_double_range (const GValue *value)
{
  return scm_cons (scm_from_double (gst_value_get_double_range_min (value)),
                   scm_from_double (gst_value_get_double_range_max (value)));
}

void
scm_to_gst_double_range (SCM scm, GValue *value)
{
  g_value_init (value, GST_TYPE_DOUBLE_RANGE);
  gst_value_set_double_range (value,
                              scm_to_double (scm_car (scm)),
                              scm_to_double (scm_cdr (scm)));
}
  
SCM
scm_from_gst_fraction_range (const GValue *value)
{
  return scm_cons (scm_from_gst_fraction (gst_value_get_fraction_range_min (value)),
                   scm_from_gst_fraction (gst_value_get_fraction_range_max (value)));
}

void
scm_to_gst_fraction_range (SCM scm, GValue *value)
{
  g_value_init (value, GST_TYPE_FRACTION_RANGE);
  gst_value_set_fraction_range_full (value,
                                     scm_to_int (scm_numerator (scm_car (scm))),
                                     scm_to_int (scm_denominator (scm_car (scm))),
                                     scm_to_int (scm_numerator (scm_cdr (scm))),
                                     scm_to_int (scm_denominator (scm_cdr (scm))));
}
  
SCM
scm_from_gst_list (const GValue *value)
{
  SCM ret = SCM_EOL;
  guint i;
  for (i = gst_value_list_get_size (value); i > 0; i--) {
    ret = scm_cons (scm_c_gvalue_to_scm (gst_value_list_get_value (value, i-1)),
                    ret);
  }
  return ret;
}

void
scm_to_gst_list (SCM scm, GValue *value)
{
  SCM head = scm;

  g_value_init (value, GST_TYPE_LIST);

  for (scm = head; !scm_is_null (scm); scm = scm_cdr (scm)) {
    if (scm_is_false (scm_gvalue_p (scm_car (scm))))
      /* we require all values in a list to already have been coerced; otherwise
         there's no sane way to sniff the types... */
      return;
  }
  
  for (scm = head; !scm_is_null (scm); scm = scm_cdr (scm)) {
    GValue *v = NULL;
    /* note: this does *not* actually copy the value. good in this case but a
     * crack api... */
#define FUNC_NAME "%scm->gst-list"
    SCM_VALIDATE_GVALUE_COPY (0, scm_car (scm), v);
#undef FUNC_NAME
    gst_value_list_append_value (value, v);
  }
}
  
SCM
scm_from_gst_array (const GValue *value)
{
  SCM ret;
  guint i, size = gst_value_array_get_size (value);

  ret = scm_c_make_vector (size, SCM_EOL);
  
  for (i = 0; i < size; i++) {
    scm_c_vector_set_x (ret, i,
                        scm_c_gvalue_to_scm (gst_value_array_get_value (value, i)));
  }
  return ret;
}

void
scm_to_gst_array (SCM scm, GValue *value)
{
  guint i, size = scm_c_vector_length (scm);
  
  g_value_init (value, GST_TYPE_ARRAY);

  for (i = 0; i < size; i++) {
    if (scm_is_false (scm_gvalue_p (scm_c_vector_ref (scm, i))))
      /* we require all values in a list to already have been coerced; otherwise
         there's no sane way to sniff the types... */
      return;
  }
  
  for (i = 0; i < size; i++) {
    GValue *v = NULL;
    /* note: this does *not* actually copy the value. good in this case but a
     * crack api... */
#define FUNC_NAME "%scm->gst-array"
    SCM_VALIDATE_GVALUE_COPY (0, scm_c_vector_ref (scm, i), v);
#undef FUNC_NAME
    gst_value_array_append_value (value, v);
  }
}
  
static /*can't be const*/ scm_t_gtype_instance_funcs miniobject_funcs = {
  0 /* GST_TYPE_MINI_OBJECT, init this at runtime */,
  (scm_t_gtype_instance_ref)gst_mini_object_ref,
  (scm_t_gtype_instance_unref)gst_mini_object_unref,
  NULL,
  NULL
};

#ifndef GST_TYPE_IS_MINI_OBJECT
#define GST_TYPE_IS_MINI_OBJECT(t) (g_type_is_a ((t), GST_TYPE_MINI_OBJECT))
#endif

static SCM
scm_gst_mini_object_primitive_create (SCM class, SCM type, SCM object)
#define s_scm_gst_mini_object_primitive_create "gst-mini-object-primitive-create"
#define FUNC_NAME s_scm_gst_mini_object_primitive_create
{
  GstMiniObject *mini = NULL;
  GType gtype;
  SCM smob;
  
  SCM_VALIDATE_GTYPE_CLASS (1, class);
  SCM_VALIDATE_GTYPE_COPY (2, type, gtype);
  SCM_ASSERT (SCM_IS_A_P (SCM_CLASS_OF (object), scm_class_gtype_class), object, 3, FUNC_NAME);
  SCM_ASSERT (GST_TYPE_IS_MINI_OBJECT (gtype), type, 2, FUNC_NAME);

  mini = gst_mini_object_new (gtype);

  if (!mini)
    scm_c_gruntime_error (FUNC_NAME, "gst_mini_object_new() failed: ~S line ~S: ~A", \
                          SCM_LIST3 (scm_makfrom0str (__FILE__), SCM_MAKINUM (__LINE__), type));

  smob = scm_c_make_gtype_instance ((GTypeInstance *) mini);

  scm_slot_set_x (object, scm_from_locale_symbol ("gtype-instance"), smob);

  return object;
}
#undef FUNC_NAME

void
scm_init_gstreamer (void)
{
  gst_init (NULL, NULL);
  miniobject_funcs.type = GST_TYPE_MINI_OBJECT;
  scm_register_gtype_instance_funcs (&miniobject_funcs);
  scm_c_define ("<gst-mini-object>",
                scm_c_gtype_to_class (GST_TYPE_MINI_OBJECT));
  scm_c_export ("<gst-mini-object>", NULL);
  scm_c_define_gsubr (s_scm_gst_mini_object_primitive_create, 3, 0, 0,
                      (SCM (*)()) scm_gst_mini_object_primitive_create);
  scm_c_export (s_scm_gst_mini_object_primitive_create, NULL);

  scm_c_register_gtype_instance_gvalue_wrappers
    (GST_TYPE_MINI_OBJECT,
     (SCMGValueGetTypeInstanceFunc)gst_value_get_mini_object,
     (SCMGValueSetTypeInstanceFunc)gst_value_set_mini_object);
}


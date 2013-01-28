
/* Generated data (by glib-mkenums) */

#include <glib-object.h>
#include "../glib/ntrack-enums.h"

/* enumerations from "../glib/ntrack-enums.h" */
GType
n_track_glib_state_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { NTRACK_GLIB_STATE_UNSET, "NTRACK_GLIB_STATE_UNSET", "unset" },
        { NTRACK_GLIB_STATE_ONLINE, "NTRACK_GLIB_STATE_ONLINE", "online" },
        { NTRACK_GLIB_STATE_OFFLINE, "NTRACK_GLIB_STATE_OFFLINE", "offline" },
        { NTRACK_GLIB_STATE_BLOCKED, "NTRACK_GLIB_STATE_BLOCKED", "blocked" },
        { NTRACK_GLIB_STATE_UNKNOWN, "NTRACK_GLIB_STATE_UNKNOWN", "unknown" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("NTrackGlibState"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}

GType
n_track_glib_event_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
    {
      static const GEnumValue values[] = {
        { NTRACK_GLIB_EVENT_UNSET, "NTRACK_GLIB_EVENT_UNSET", "unset" },
        { NTRACK_GLIB_EVENT_CONNECT, "NTRACK_GLIB_EVENT_CONNECT", "connect" },
        { NTRACK_GLIB_EVENT_DISCONNECT, "NTRACK_GLIB_EVENT_DISCONNECT", "disconnect" },
        { NTRACK_GLIB_EVENT_RECONNECT, "NTRACK_GLIB_EVENT_RECONNECT", "reconnect" },
        { 0, NULL, NULL }
      };
      GType g_define_type_id =
        g_enum_register_static (g_intern_static_string ("NTrackGlibEvent"), values);
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

  return g_define_type_id__volatile;
}


/* Generated data ends here */


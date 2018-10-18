
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <syslog.h>
#include <dbus/dbus-glib.h>

#include "NDK_Sec.h"
#include "sdk_vpp.h"
#include "secp_proxy.h"
#include "secp_dbus.h"

#define _DBUS_POINTER_SHIFT(p)   ((void*) (((char*)p) + sizeof (void*)))
#define _DBUS_POINTER_UNSHIFT(p) ((void*) (((char*)p) - sizeof (void*)))
#define DBUS_CONNECTION_FROM_G_CONNECTION(x)     ((DBusConnection*) _DBUS_POINTER_UNSHIFT(x))
#define DBUS_G_CONNECTION_FROM_CONNECTION(x)     ((DBusGConnection*) _DBUS_POINTER_SHIFT(x))


void dbus_g_connection_close( DBusGConnection *  connection )
{
    return dbus_connection_close(DBUS_CONNECTION_FROM_G_CONNECTION(connection));
}

int secp_methodcall(int method_id, void *arg, int arglen, void *out, int maxoutlen, int *outlen)
{
    static DBusGConnection *bus = NULL;
    static DBusGProxy *remote_object = NULL;
    GError *error = NULL;
    gint result;
    GArray* in_array = NULL;
    GArray* out_array = NULL;
    GMainContext* main_context = NULL;

    SECDEBUG("secp_methodcall method_id=%d arglen=%d\n", method_id, arglen);
    g_type_init ();
	dbus_g_thread_init();
    main_context = g_main_context_new();
    if (bus == NULL) {
        bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
        if (!bus) {
            SECDEBUG("Couldn't connect to session bus:%s\n", (char *)error);
            return -1;
        }
    }
    if (remote_object == NULL) {
        remote_object = dbus_g_proxy_new_for_name (bus,
                        PHOENIX_DBUS_NAME,
                        PHOENIX_DBUS_OBJECT,
                        PHOENIX_DBUS_INTERFACE);
        if(!remote_object) {
            result = -1;
            goto err1 ;
        }
    }
    SECDEBUG("secp_methodcall 11111111111\n");
    if ((in_array = g_array_new(FALSE, FALSE, sizeof(guint8)))==NULL) {
        SECDEBUG("%s,%d %d %d\n",__FUNCTION__,__LINE__,method_id, arglen) ;
        result = -1;
        goto err2;
    }

    //申请输入参数内存
    if (arg!=NULL && arglen>0) {
        SECDEBUG("secp_methodcall --------\n");
        g_array_append_vals(in_array, arg, arglen);
    } else {
        g_array_append_vals(in_array, "", 1);
    }

    SECDEBUG("secp_methodcall 22222222\n");
    if(nl_secp_interface_request(remote_object, method_id, in_array, &out_array, &result, &error)==TRUE) {
        //输出拷贝
        SECDEBUG("secp_methodcall 33333333333333\n");
        if (out_array!=NULL) {
            if(out_array->len>0) {
                if ((maxoutlen<out_array->len) || (out==NULL)) { //输出结构长度不对
                    SECDEBUG("maxoutlen=%d, len=%d", maxoutlen, out_array->len);
                    result = -1;
                    goto secp_methodcall_exit;
                }
                memcpy(out, out_array->data, out_array->len);
                if(outlen!=NULL) *outlen = out_array->len;
            }
        }
    } else {
        SECDEBUG("call nl_secp_interface_request return fail\r\n");
        result = -1;
    }

secp_methodcall_exit:
    SECDEBUG("call g_array_free.. \r\n");
    if(in_array) g_array_free(in_array,TRUE);
    if(out_array) g_array_free(out_array,TRUE);

err2:
    SECDEBUG("call g_object_unref \r\n");
//  g_object_unref (G_OBJECT (remote_object));
err1:
//  dbus_g_connection_close(bus);
//  dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);

    return result;
}

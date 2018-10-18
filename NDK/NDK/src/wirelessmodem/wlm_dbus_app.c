
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <config.h>
#include <dbus/dbus-glib.h>

#include "wlm_proxy.h"

#define DEBUG
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "[WLM_DBUS_APP]: "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

#define WLM_DEFAULT_BUF_LEN 1024    /*缺省的数据缓冲长度*/

typedef struct {
    char return_string[WLM_DEFAULT_BUF_LEN];
} ReturnString;

void wlm_methodcall_gsmmux(int method_id,int *ret)
{
    static DBusGConnection *bus = NULL;
    static DBusGProxy *remote_object = NULL;
    GError *error = NULL;
    gint result =-2;
    GMainContext* main_context = NULL;

    int i;
    *ret = -1;

    g_type_init ();
	dbus_g_thread_init();
    main_context = g_main_context_new();

//  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
//  if (!bus)
//    PDEBUG ("Couldn't connect to session bus:%s\n", error->message);
    if(bus == NULL) {
        bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
        if (!bus) {
            PDEBUG("Couldn't connect to session bus:%s\n", (char *)error);
            return -1;
        }
    }
    if(remote_object==NULL) {
        remote_object = dbus_g_proxy_new_for_name (bus,
                        "nl.phoenix.commserv",
                        "/CommServObject",
                        "nl.commserv.interface");
        if(!remote_object) {
            PDEBUG("[%s] connect to remote_object error\n",__func__);
            goto WLM_DBUS_ERR ;
        }
    }
//  if(nl_wlm_interface_wlm_function (remote_object,method_id, in_array, &out_array, &result, &error)==TRUE)
    if(nl_commserv_interface_wlm_gsmmux (remote_object, method_id, &result, &error)==TRUE) {
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
WLM_DBUS_ERR:
    g_main_context_unref(main_context);
    *ret = result;
}


void wlm_methodcall(int method_id,void * commserv_arg,int commserv_arg_len,void* commserv_out,int *ret)
{
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    GArray* in_array = NULL;
    GArray* out_array = NULL;
    int i;
    *ret = -1;
    g_type_init ();
	dbus_g_thread_init();
    bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
    if (!bus)
        PDEBUG ("Couldn't connect to session bus:%s\n", error->message);
    remote_object = dbus_g_proxy_new_for_name (bus,
                    "nl.phoenix.commserv",
                    "/CommServObject",
                    "nl.commserv.interface");
    if(!remote_object) {
        PDEBUG("[%s] connect to remote_object error\n",__func__);
        return ;
    }
    in_array = g_array_new(FALSE, FALSE, sizeof(guint8));
    if (!in_array) {
        return ;
    }
    g_array_append_vals(in_array, commserv_arg,commserv_arg_len);
    if(nl_commserv_interface_wlm_function (remote_object,method_id, in_array, &out_array, &result, &error)==TRUE) {
        if((out_array->data)!=NULL) {
            memcpy(commserv_out,out_array->data,sizeof(ReturnString));
            fprintf(stderr,"[%s]nl_commserv_interface_wlm_function Succ!\n",__func__);
        }
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    if(in_array)
        g_array_free(in_array,TRUE);
    if(out_array)
        g_array_free(out_array,TRUE);
}

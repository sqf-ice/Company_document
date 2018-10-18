
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

//#include "wifi_proxy.h"
#include "commserv_proxy.h"

extern void dbus_g_connection_close( DBusGConnection *  connection );

//#define DEBUG
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "[WIFI_DBUS_APP]: "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

/*
void wifi_methodcall_status(int method_id,int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    *ret = -1;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
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
    if(nl_commserv_interface_wifi_status (remote_object, method_id, &result, &error)==TRUE) {
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}

void wifi_methodcall_enable(int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    *ret = -1;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
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
    if(nl_commserv_interface_wifi_enable (remote_object, &result, &error)==TRUE) {
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}

void wifi_methodcall_disable(int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    *ret = -1;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
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
    if(nl_commserv_interface_wifi_disable (remote_object, &result, &error)==TRUE) {
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}

*/

void comm_methodcall_general(int method_id, int input_data, int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    *ret = -2;
    if(access("/tmp/.commserv_dbus",F_OK)<0)
        return;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
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
    if(nl_commserv_interface_general_function (remote_object, method_id, input_data, &result, &error)==TRUE) {
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}


void wifi_methodcall_info(int method_id,void* commserv_out,int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    GArray* out_array = NULL;
    *ret = -2;
    if(access("/tmp/.commserv_dbus",F_OK)<0)
        return;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
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

    if(nl_commserv_interface_wifi_info_function (remote_object,method_id,  &out_array, &result, &error)==TRUE) {
        if((out_array->data)!=NULL) {
            memcpy(commserv_out,out_array->data,out_array->len);
            //fprintf(stderr,"[%s]nl_wifi_interface_commserv_function Succ!\n",__func__);
        }
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    if(out_array)
        g_array_free(out_array,TRUE);
    //g_strfreev(out_array);
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}

void wifi_methodcall_connect(int method_id,void * commserv_arg,int commserv_arg_len,int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result =-2;
    GArray* in_array = NULL;
    *ret = -2;
    if(access("/tmp/.commserv_dbus",F_OK)<0)
        return;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    //bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
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
    if(nl_commserv_interface_wifi_connect(remote_object,method_id, in_array,  &result, &error)==TRUE) {
        PDEBUG("[%s]nl_wifi_interface_commserv_function Succ!\n",__func__);
    } else {
        PDEBUG("[%s] dbus_g_proxy_call error \n",__func__);
    }
    *ret = result;
    if(in_array)
        g_array_free(in_array,TRUE);
    //g_strfreev(result);
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}








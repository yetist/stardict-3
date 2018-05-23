/* vi: set sw=4 ts=4 wrap ai: */
/*
 * stardict-server.c: This file is part of ____
 *
 * Copyright (C) 2018 yetist <yetist@yetibook>
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * */
#include "stardict.h"
#include "stardict-dbus-generated.h"
#include "stardict-server.h"

#define STARDICT_DBUS_NAME "org.stardict.Server"
#define STARDICT_DBUS_PATH "/org/stardict/Server"

static StarDictServer *skeleton;

static gboolean stardict_server_grab_focus ( StarDictServer *object, GDBusMethodInvocation *invocation)
{
	gtk_window_present (GTK_WINDOW (gpAppFrame->window));
	star_dict_server_complete_grab_focus (object, invocation);
	return TRUE;
}

static gboolean stardict_server_hide ( StarDictServer *object, GDBusMethodInvocation *invocation)
{
	gtk_widget_hide(gpAppFrame->window);
	star_dict_server_complete_hide (object, invocation);
	return TRUE;
}

static gboolean stardict_server_query_word ( StarDictServer *object, GDBusMethodInvocation *invocation, const gchar *arg_word)
{
	gpAppFrame->Query(arg_word);
	star_dict_server_complete_query_word ( object, invocation);
	return TRUE;
}

static gboolean stardict_server_quit ( StarDictServer *object, GDBusMethodInvocation *invocation)
{
	gpAppFrame->Quit();
	star_dict_server_complete_quit ( object, invocation);
	return TRUE;
}

static void bus_acquired_handler_cb (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	GError *error = NULL;
	gboolean exported;

	g_signal_connect (skeleton, "handle-grab-focus",  G_CALLBACK (stardict_server_grab_focus), NULL);
	g_signal_connect (skeleton, "handle-hide",  G_CALLBACK (stardict_server_hide), NULL);
	g_signal_connect (skeleton, "handle-query-word", G_CALLBACK (stardict_server_query_word), NULL);
	g_signal_connect (skeleton, "handle-quit", G_CALLBACK (stardict_server_quit), NULL);

	exported = g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (skeleton),
			connection, STARDICT_DBUS_PATH, &error);

	if (!exported)
	{
		g_warning ("Failed to export interface: %s", error->message);
		g_error_free (error);

		gtk_main_quit();
	}
}

static void name_lost_handler_cb (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	g_debug("bus name lost\n");
	gtk_main_quit();
}

gboolean stardict_server_new (void)
{
	StarDictServer *proxy;
	guint bus_name_id;

	proxy = star_dict_server_proxy_new_for_bus_sync (
			G_BUS_TYPE_SESSION,
			G_DBUS_PROXY_FLAGS_NONE,
			STARDICT_DBUS_NAME,
			STARDICT_DBUS_PATH,
			NULL,
			NULL);
	if (star_dict_server_call_grab_focus_sync (proxy, NULL, NULL)) {
		gdk_notify_startup_complete();
		star_dict_server_call_grab_focus_sync (proxy, NULL, NULL);
		g_object_unref(proxy);
		return FALSE;
	} else {
		g_object_unref(proxy);
		bus_name_id = g_bus_own_name (G_BUS_TYPE_SESSION,
				STARDICT_DBUS_NAME, G_BUS_NAME_OWNER_FLAGS_NONE,
				bus_acquired_handler_cb, NULL,
				name_lost_handler_cb, NULL, NULL);
		skeleton = star_dict_server_skeleton_new();
		return TRUE;
	}
}

void stardict_server_free (void)
{
	if (skeleton != NULL) {
		GDBusInterfaceSkeleton *skel;
		skel = G_DBUS_INTERFACE_SKELETON (skeleton);
		g_dbus_interface_skeleton_unexport (skel);
		g_clear_object (&skeleton);
	}
}

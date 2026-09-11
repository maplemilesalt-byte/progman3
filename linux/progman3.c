/*
 * progman3.c - experimental Linux Program Manager frontend
 *
 * First experimental stage of the NT4 Program Manager port.
 * Keeps the project as a normal desktop application instead of a shell.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

static void launch_item(GtkWidget *widget, gpointer data)
{
    const char *command = data;
    GError *error = NULL;

    if (!command || !*command)
        return;

    if (!g_spawn_command_line_async(command, &error)) {
        g_warning("Could not launch '%s': %s", command,
                  error ? error->message : "unknown error");
        g_clear_error(&error);
    }
}

static GtkWidget *make_group(const char *title,
                             const char *name,
                             const char *command)
{
    GtkWidget *frame = gtk_frame_new(title);
    GtkWidget *button = gtk_button_new_with_label(name);

    gtk_widget_set_margin_start(button, 8);
    gtk_widget_set_margin_end(button, 8);
    gtk_widget_set_margin_top(button, 8);
    gtk_widget_set_margin_bottom(button, 8);

    g_signal_connect(button, "clicked", G_CALLBACK(launch_item),
                     (gpointer)command);

    gtk_frame_set_child(GTK_FRAME(frame), button);
    return frame;
}

static void activate(GtkApplication *app, gpointer unused)
{
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *groups = gtk_flow_box_new();
    GtkWidget *header = gtk_header_bar_new();

    (void)unused;

    gtk_window_set_title(GTK_WINDOW(window), "Program Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 480);

    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header),
                                    gtk_label_new("Program Manager"));
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);

    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(groups),
                                    GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(groups), 4);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(groups), 10);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(groups), 10);

    /* Temporary proof-of-concept group.
     * The next stage will replace this with PGROUP/PITEM data loaded from
     * the original NT4 .GRP format via the existing convgrp code.
     */
    gtk_flow_box_append(GTK_FLOW_BOX(groups),
                        make_group("Accessories", "Terminal", "xterm"));
    gtk_flow_box_append(GTK_FLOW_BOX(groups),
                        make_group("Games", "Mines", "gnome-mines"));
    gtk_flow_box_append(GTK_FLOW_BOX(groups),
                        make_group("Utilities", "Calculator", "gnome-calculator"));

    gtk_box_append(GTK_BOX(box), groups);
    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.maplemilesalt.progman3",
                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

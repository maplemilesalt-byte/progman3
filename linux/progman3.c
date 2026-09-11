/*
 * progman3.c - experimental Linux Program Manager frontend
 *
 * Stage 2: load real NT4 .GRP files and expose their items as GTK buttons.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include "grp_parser.h"

static void launch_item(GtkWidget *widget, gpointer data)
{
    const char *command = data;
    GError *error = NULL;
    (void)widget;

    if (!command || !*command)
        return;

    if (!g_spawn_command_line_async(command, &error)) {
        g_warning("Could not launch '%s': %s", command,
                  error ? error->message : "unknown error");
        g_clear_error(&error);
    }
}

static GtkWidget *make_item(const PM3Item *item)
{
    GtkWidget *button;
    char *command;

    button = gtk_button_new_with_label(item->name[0] ? item->name : "(unnamed)");
    gtk_widget_set_margin_start(button, 8);
    gtk_widget_set_margin_end(button, 8);
    gtk_widget_set_margin_top(button, 8);
    gtk_widget_set_margin_bottom(button, 8);

    command = g_strdup(item->command);
    g_signal_connect_data(button, "clicked", G_CALLBACK(launch_item), command,
                          (GClosureNotify)g_free, 0);
    return button;
}

static GtkWidget *make_group(const PM3Group *group)
{
    GtkWidget *frame = gtk_frame_new(group->name[0] ? group->name : "Program Group");
    GtkWidget *grid = gtk_grid_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_widget_set_margin_start(grid, 8);
    gtk_widget_set_margin_end(grid, 8);
    gtk_widget_set_margin_top(grid, 8);
    gtk_widget_set_margin_bottom(grid, 8);

    for (size_t i = 0; i < group->item_count; ++i) {
        GtkWidget *button = make_item(&group->items[i]);
        gtk_grid_attach(GTK_GRID(grid), button, (int)(i % 4), (int)(i / 4), 1, 1);
    }

    gtk_frame_set_child(GTK_FRAME(frame), grid);
    return frame;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    char **paths = user_data;
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *groups = gtk_flow_box_new();

    gtk_window_set_title(GTK_WINDOW(window), "Program Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 480);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(groups), GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(groups), 3);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(groups), 10);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(groups), 10);
    gtk_widget_set_margin_start(groups, 12);
    gtk_widget_set_margin_end(groups, 12);
    gtk_widget_set_margin_top(groups, 12);
    gtk_widget_set_margin_bottom(groups, 12);

    if (paths) {
        for (size_t i = 0; paths[i]; ++i) {
            PM3Group group;
            if (pm3_load_group(paths[i], &group) == 0) {
                gtk_flow_box_append(GTK_FLOW_BOX(groups), make_group(&group));
                pm3_free_group(&group);
            } else {
                g_warning("Could not load group file: %s", paths[i]);
            }
        }
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), groups);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;
    char **paths = NULL;

    /* Arguments after the executable are treated as NT4 .GRP files. */
    if (argc > 1)
        paths = &argv[1];

    app = gtk_application_new("com.maplemilesalt.progman3",
                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), paths);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

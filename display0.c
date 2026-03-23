#include <gtk/gtk.h>

typedef struct {
    GtkWidget *entry;
    GtkWidget *text_view;
} ChatWidgets;

// This function now receives the entire struct as user_data
static void on_button_clicked(GtkButton *btn, gpointer user_data) {
    // Cast user_data to our struct
    ChatWidgets *widgets = (ChatWidgets *)user_data;

    // 1. Get text from the entry inside the struct
    GtkEditable *entry = GTK_EDITABLE(widgets->entry);
    const char *text = gtk_editable_get_text(entry);

    if (g_strcmp0(text, "") == 0) return;

    // 2. Get the buffer from the textView inside the struct
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->text_view));

    // 3. Insert the text
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, "1.1.1.1: ", -1);
    gtk_text_buffer_insert(buffer, &end, text, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
    // Inside on_button_clicked after inserting text:
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(gtk_widget_get_parent(widgets->text_view)));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));

    // 4. Clear the entry
    gtk_editable_set_text(entry, "");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new_from_file("chatapp.ui");
    ChatWidgets *widgets = g_new0(ChatWidgets, 1);
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "appWindow"));
    GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, "sendButton"));
    GtkTextBuffer *buffer;
    widgets->entry = GTK_WIDGET(gtk_builder_get_object(builder, "textEntry"));
    widgets->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "textView"));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->text_view), FALSE);

    // Checks if IDs are set correctly
    if (builder == NULL || window == NULL || widgets->entry == NULL || button == NULL) {
        if(builder == NULL) {
            g_error("\nCould not load UI file! Check if the filename is correct.");
        }
        if(window == NULL) {
            printf("Window ID not set correctly\n");
        }
        if(widgets->entry == NULL) {
            printf("Text Entry ID not set correctly\n");
        }
        if(button == NULL) {
            printf("Button ID not set correctly\n");
        }

        printf("\nWidget(s) failed to load, double check Object ID.\n");

        exit(-1);
    }

    // Window creation
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);

    // Button CLicked
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    // Show window
    gtk_window_present(GTK_WINDOW(window));

    // Cleanup
    g_object_unref(builder);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("edu.bab6939.psu", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
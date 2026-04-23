#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>


static int serverFD = -1;
static char g_client_label[64] = "me";

typedef struct {
    GtkWidget *entry;
    GtkWidget *text_view;
} ChatWidgets;

typedef struct { // Deliver message to GTK main thread
    GtkWidget *text_view;
    char *message;
} RecvPayload;


static gboolean append_message_to_view(gpointer data) {
    RecvPayload *p = (RecvPayload *)data;

    // Append Message
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->text_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_insert(buf, &end, p->message, -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);

    /* Auto-scroll: push the vertical adjustment to its maximum value */
    GtkWidget *parent = gtk_widget_get_parent(p->text_view);
    if (GTK_IS_SCROLLED_WINDOW(parent)) {
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(
                                 GTK_SCROLLED_WINDOW(parent));
        gtk_adjustment_set_value(adj,
            gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    }

    g_free(p->message);
    g_free(p);
    return G_SOURCE_REMOVE;
}

static void *threadFuncRecv(void *arg) {
    ChatWidgets *widgets = (ChatWidgets *)arg;
    char buffer[268];

    while (1) {
        ssize_t n = recv(serverFD, buffer, sizeof(buffer) - 1, 0);

        if (n == 0) {
            /* Server closed the connection gracefully */
            RecvPayload *p = g_new0(RecvPayload, 1);
            p->text_view = widgets->text_view;
            p->message   = g_strdup("*** Server closed the connection. ***");
            g_idle_add(append_message_to_view, p);
            return NULL;

        } else if (n > 0) {
            buffer[n] = '\0';
            RecvPayload *p = g_new0(RecvPayload, 1);
            p->text_view = widgets->text_view;
            p->message   = g_strdup(buffer);
            g_idle_add(append_message_to_view, p);

        } else {
            /* recv() returned -1 — real error */
            perror("recv failed");
            break;
        }
    }

    return NULL;
}


static void on_button_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn; 
    ChatWidgets *widgets = (ChatWidgets *)user_data;

    GtkEditable *entry = GTK_EDITABLE(widgets->entry);
    const char  *text  = gtk_editable_get_text(entry);

    // Ignore empty sends
    if (g_strcmp0(text, "") == 0){
     return;
    }

    /* Transmit to server */
    if (serverFD >= 0) {
        send(serverFD, text, strlen(text), 0);
    }

    /* Local echo */
    char *line = g_strdup_printf("You (%s): %s", g_client_label, text);

    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->text_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_insert(buf, &end, line, -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);
    g_free(line);

    /* Auto-scroll */
    GtkWidget *parent = gtk_widget_get_parent(widgets->text_view);
    if (GTK_IS_SCROLLED_WINDOW(parent)) {
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(
                                 GTK_SCROLLED_WINDOW(parent));
        gtk_adjustment_set_value(adj,
            gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    }

    /* 4. Clear entry */
    gtk_editable_set_text(entry, "");
}


static void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD < 0) {
        perror("CLIENT: socket() failed");
        return;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("CLIENT: Invalid address");
        close(serverFD);
        serverFD = -1;
        return;
    }

    if (connect(serverFD, (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0) {
        perror("CLIENT: connect() failed");
        close(serverFD);
        serverFD = -1;
        return;
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(serverFD, (struct sockaddr *)&local_addr, &addr_len) == 0) {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &local_addr.sin_addr, ip_str, sizeof(ip_str));
        snprintf(g_client_label, sizeof(g_client_label),
                 "%s:%d", ip_str, ntohs(local_addr.sin_port));
    }

    printf("CLIENT: Connected as %s\n", g_client_label);

    GtkBuilder *builder = gtk_builder_new_from_file("chatapp.ui");
    if (builder == NULL) {
        g_error("CLIENT: Could not load chatapp.ui — check the filename.");
        return;
    }

    GtkWidget *window  = GTK_WIDGET(gtk_builder_get_object(builder, "appWindow"));
    GtkWidget *button  = GTK_WIDGET(gtk_builder_get_object(builder, "sendButton"));

    ChatWidgets *widgets = g_new0(ChatWidgets, 1);
    widgets->entry     = GTK_WIDGET(gtk_builder_get_object(builder, "textEntry"));
    widgets->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "textView"));

    if (!window || !button || !widgets->entry || !widgets->text_view) {
        if (!window)          fprintf(stderr, "CLIENT: 'appWindow' ID not found in UI file\n");
        if (!button)          fprintf(stderr, "CLIENT: 'sendButton' ID not found in UI file\n");
        if (!widgets->entry)  fprintf(stderr, "CLIENT: 'textEntry' ID not found in UI file\n");
        if (!widgets->text_view) fprintf(stderr, "CLIENT: 'textView' ID not found in UI file\n");
        g_free(widgets);
        g_object_unref(builder);
        close(serverFD);
        return;
    }

    /* Chat history area is read-only — the user types in the entry only */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets->text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(widgets->text_view), FALSE);

    /* Window setup */
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    /* Connect the Send button */
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    /* Show the window */
    gtk_window_present(GTK_WINDOW(window));
    g_object_unref(builder);

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, threadFuncRecv, widgets) == 0) {
        pthread_detach(recv_thread);
        printf("CLIENT: Receive thread started\n");
    } else {
        perror("CLIENT: pthread_create failed");
        /* App still runs — user just won't see incoming messages */
    }
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("edu.bab6939.psu", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

#include <gtk/gtk.h>


static void on_hello_clicked(GtkWidget *widget, gpointer data) {
    g_print("confio\n");
}

static void on_quit_clicked(GtkWidget *widget, gpointer data) {
    gtk_window_close(GTK_WINDOW(data));
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Crear ventana principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Ventana con Dos Botones");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crear contenedor horizontal
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Crear botón 1
    GtkWidget *btn_hello = gtk_button_new_with_label("Yo soy...");
    g_signal_connect(btn_hello, "clicked", G_CALLBACK(on_hello_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_hello, TRUE, TRUE, 0);

    // Crear botón 2
    GtkWidget *btn_quit = gtk_button_new_with_label("Cerrar");
    g_signal_connect(btn_quit, "clicked", G_CALLBACK(on_quit_clicked), window);
    gtk_box_pack_start(GTK_BOX(box), btn_quit, TRUE, TRUE, 0);

    // Mostrar todo
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


//gcc $(pkg-config --cflags gtk+-3.0) Main.c $(pkg-config --libs gtk+-3.0) -o Main.exe

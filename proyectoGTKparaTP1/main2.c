#include <gtk/gtk.h>

void on_saludo_clicked(GtkButton *button, gpointer user_data) {
    g_print("¡Hola desde la interfaz GTK con Glade!\n");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new_from_file("interfaz.glade");
    GtkWidget *ventana = GTK_WIDGET(gtk_builder_get_object(builder, "ventana"));
    GtkWidget *boton = GTK_WIDGET(gtk_builder_get_object(builder, "btnInventario"));
    GtkWidget *boton1 = GTK_WIDGET(gtk_builder_get_object(builder, "btnInventario"));
    GtkWidget *boton2 = GTK_WIDGET(gtk_builder_get_object(builder, "btnInventario"));
    GtkWidget *boton3 = GTK_WIDGET(gtk_builder_get_object(builder, "btnInventario"));
    GtkWidget *boton4 = GTK_WIDGET(gtk_builder_get_object(builder, "btnInventario"));

    g_signal_connect(boton, "clicked", G_CALLBACK(on_saludo_clicked), NULL);
    g_signal_connect(ventana, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(ventana);
    gtk_main();
    return 0;
}

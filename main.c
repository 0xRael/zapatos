#include <gtk/gtk.h>

static GtkBuilder *builder = NULL;

/* Prototipo del callback para cerrar la ventana */
static void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

G_MODULE_EXPORT void on_btnInventario_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_inventario")  // o el ID que le diste
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

G_MODULE_EXPORT void on_btnProductos_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_prueba")  // o el ID que le diste
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

int main(int argc, char *argv[]) {
    //GtkBuilder *builder;
    GtkWidget  *window;
    GError     *error = NULL;

    /* Inicializar GTK */
    gtk_init(&argc, &argv);

    /* Crear el GtkBuilder y cargar el archivo .glade */
    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "interfaz.glade", &error)) {
        g_printerr("Error al cargar UI: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    /* Obtener la ventana principal por su ID definido en Glade */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "ventana"));
    if (!window) {
        g_printerr("No se encontró widget 'ventana' en el archivo .glade\n");
        return 1;
    }

    /* Conectar señales definidas en Glade con nuestros callbacks */
    gtk_builder_connect_signals(builder, NULL);

    /* Ya no necesitamos el builder */
    //g_object_unref(builder);

    /* Mostrar la ventana y todos sus hijos */
    gtk_widget_show_all(window);

    /* Bucle principal de GTK */
    gtk_main();

    return 0;
}

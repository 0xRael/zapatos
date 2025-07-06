#include <gtk/gtk.h>

static GtkBuilder *builder = NULL;

typedef struct {
	char codigo[50];
	char nombre[50];
	char descripcion[200];
	char categoria[50];
	
	int cantidad; // Numero de unidades en Stock
	
	char ubicacion[50];
	char proveedor[50];
	
	int fecha; // Fecha de adquisicion
	
	float precio_compra;
	float precio_venta;
	
	// Movimiento de inventario:
	int entradas;
	int salidas;
	
	int stock_minimo; // Nivel de stock minimo
} Producto;

typedef struct {
	char nombre[50];
	char apellido[50];
	char telefono[50];
	char direccion[50];
	char cedula[50];
} Cliente;

typedef struct {
	char nombre[50];
	char apellido[50];
	char cedula[50];
	char telefono[50];
	char direccion[50];
	int numero;
} Trabajador;

/* Prototipo del callback para cerrar la ventana */
static void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

G_MODULE_EXPORT void on_btnInventario_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_inventario") 
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

G_MODULE_EXPORT void on_btnProductos_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_prueba")
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

enum {
    COL_CODIGO,
    COL_NOMBRE,
    COL_DESCRIPCION,
    COL_CATEGORIA,
    COL_CANTIDAD,
    N_COLUMNS
};

void init_inventario(int n, Producto inventario[]) {
    GtkTreeIter   iter;

    /* 1. Obtener TreeView y modelo */
    GtkListStore *store = gtk_list_store_new(
    N_COLUMNS,
    G_TYPE_STRING,  /* codigo */
    G_TYPE_STRING,  /* nombre */
    G_TYPE_STRING,  /* descripcion */
    G_TYPE_STRING,  /* categoria */
    G_TYPE_INT      /* cantidad */
);

GtkTreeView *tree = GTK_TREE_VIEW(
    gtk_builder_get_object(builder, "tree_inventario")
);
gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
g_object_unref(store);

    /* 2. Limpiar datos previos */
    gtk_list_store_clear(store);

    /* 4. Población */
    for (int i = 0; i < n; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,     inventario[i].codigo,
            1, inventario[i].nombre,
            2,  inventario[i].descripcion,
            3,  inventario[i].categoria,
            4,  inventario[i].cantidad,
            -1
        );
    }

    gint count = 0;
    GtkTreeModel *model = GTK_TREE_MODEL(store);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do { count++; } while (gtk_tree_model_iter_next(model, &iter));
    }
    g_print(">> Filas en inventario: %d\n", count);

}

int main(int argc, char *argv[]) {
    //GtkBuilder *builder;
    GtkWidget  *window;
    GError     *error = NULL;

    Producto inventario[] = {
        {"000", "Jordan Plus asd", "Los zapatos mas epicos w", "Deportivo", 10},
        {"0123", "VIva chavez", "Chavez vive el hambre sigue", "Deportivo",  5},
        {"8585", "OBS Studio Mod Apk", "Descarga ahora OBS Studio crackeado gratis", "Casual",    8},
    };
    int n = 3;

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
    init_inventario(n, inventario);

    /* Ya no necesitamos el builder */
    //g_object_unref(builder);

    /* Mostrar la ventana y todos sus hijos */
    gtk_widget_show_all(window);

    /* Bucle principal de GTK */
    gtk_main();

    return 0;
}

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>


/*
  DEFINICIONES
*/

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


static GtkBuilder *builder = NULL;

// Inventario en memoria
static Producto inventario[] = {
    {"000", "Jordan Plus asd", "Los zapatos mas epicos w", "Deportivo", 10},
    {"0123", "VIva chavez", "Chavez vive el hambre sigue", "Deportivo",  5},
    {"8585", "OBS Studio Mod Apk", "Descarga ahora OBS Studio crackeado gratis", "Casual",    8},
};
static int n_inventario = 3;

/* Prototipo del callback para cerrar la ventana */
static void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

/* Prototipos de funcion */
G_MODULE_EXPORT void on_btnInventario_clicked(GtkButton *button, gpointer user_data);
G_MODULE_EXPORT void on_btnProductos_clicked(GtkButton *button, gpointer user_data);

void init_inventario();
void refresh_inventario(const char *filtro);
G_MODULE_EXPORT void on_invEliminar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_invFiltro_search_changed(GtkButton *btn, gpointer user_data);












/*
FUNCIONES DE LA BARRA LATERAL
*/

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












/*
FUNCIONES DEL INVENTARIO
*/

enum {
    COL_CODIGO,
    COL_NOMBRE,
    COL_DESCRIPCION,
    COL_CATEGORIA,
    COL_CANTIDAD,
    N_COLUMNS
};

void init_inventario()
{
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
    for (int i = 0; i < n_inventario; i++) {
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

void refresh_inventario(const char *filtro) {
    GtkTreeView  *tree  = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_inventario"));
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(tree));
    GtkTreeIter   iter;

    /* Limpia filas previas */
    gtk_list_store_clear(store);

    /* Recorrer el inventario y añadir las filas que pasen el filtro */
    for (gint i = 0; i < n_inventario; i++) {
        /* Si hay filtro, comprobamos si aparece en código o nombre */
        if (filtro && *filtro) {
        	// Pasamos ambos a minusculas durante la busqueda por nombre para q las mayusculas no sean relevantes
            gchar *nombre = g_ascii_strdown(inventario[i].nombre, -1);
			gchar *filtro_minus = g_ascii_strdown(filtro,   -1);
			
            if (!strstr(nombre, filtro_minus) &&
                !strstr(inventario[i].codigo, filtro))
                continue;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            COL_CODIGO,      inventario[i].codigo,
            COL_NOMBRE,      inventario[i].nombre,
            COL_DESCRIPCION, inventario[i].descripcion,
            COL_CATEGORIA,   inventario[i].categoria,
            COL_CANTIDAD,    inventario[i].cantidad,
            -1
        );
    }
}

G_MODULE_EXPORT void on_invEliminar_clicked(GtkButton *btn, gpointer user_data)
{
    GtkTreeView      *tree    = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_inventario"));
    GtkTreeSelection *sel     = gtk_tree_view_get_selection(tree);
    GtkTreeIter       iter;
    GtkTreeModel     *model;

    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return;

    /* Obtener código y buscar índice */
    gchar *cod;
    gtk_tree_model_get(model, &iter, COL_CODIGO, &cod, -1);
    gint idx = -1;
    for (gint i = 0; i < n_inventario; i++) {
        if (strcmp(inventario[i].codigo, cod) == 0) { idx = i; break; }
    }
    g_free(cod);
    if (idx < 0) return;

    /* Mover a la izquierda el arreglo */
    for (gint i = idx; i < n_inventario - 1; i++) {
        inventario[i] = inventario[i + 1];
    }
    n_inventario--;

    /* Refrescar la vista */
    refresh_inventario(NULL);
}

G_MODULE_EXPORT void on_invFiltro_search_changed(GtkButton *btn, gpointer user_data)
{
    const char *texto   = gtk_entry_get_text(GTK_ENTRY(
        gtk_builder_get_object(builder, "invFiltro")));

    /* Simplemente repuebla usando el texto como filtro */
    refresh_inventario( texto);
}











/*
 CARGADO DEL ARCHIVO GLADE
*/



int main(int argc, char *argv[]) {
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

    /* Conectar señales definidas en Glade con nuestros callbacks */
    gtk_builder_connect_signals(builder, NULL);
    init_inventario();

    /* Mostrar la ventana y todos sus hijos */
    gtk_widget_show_all(window);

    /* Bucle principal de GTK */
    gtk_main();

    return 0;
}

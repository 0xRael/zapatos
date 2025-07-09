#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>        /* para time_t */
#include <sys/stat.h>  // mkdir
#include <unistd.h>    // access

/*
  DEFINICIONES DE ESTRUCTURAS
*/
#define MAX_PRODUCTOS     100
#define MAX_CLIENTES      100
#define MAX_TRABAJADORES  100
#define IVA_RATE          16.0f

typedef struct {
    char codigo[50];
    char nombre[50];
    char descripcion[200];
    char categoria[50];
    int  cantidad;
    char ubicacion[50];
    char proveedor[50];
    time_t  fecha_registro;
    float precio_compra;
    float precio_venta;
    int  entradas;
    int  salidas;
    int  stock_minimo;
} Producto;

typedef struct {
    char cedula[50];
    char nombre[50];
    char apellido[50];
    char telefono[50];
    char direccion[200];
} Cliente;

typedef struct {
    char cedula[50];
    char nombre[50];
    char apellido[50];
    char telefono[50];
    char direccion[200];
} Trabajador;

/* 3) Única definición de Factura */
typedef struct Factura {
    char     tienda_nombre[100];
    int      num;
    time_t   fecha;
    char     cedula_cliente[50];
    char     cliente_nombre[100];
    char     cliente_direccion[200];
    Producto productos[50];
    int      n_productos;
    float    subtotal;
    float    iva;
    float    total;
} Factura;

/*
  VARIABLES GLOBALES (una sola vez)
*/
static GtkBuilder *builder        = NULL;
static char        nombre_tienda[50];
static Producto    inventario[MAX_PRODUCTOS];
static Cliente     clientes[MAX_CLIENTES];
static Trabajador  trabajadores[MAX_TRABAJADORES];

static int n_inventario = 0;
static int n_clientes = 0;
static int n_trabajadores = 0;

/* Prototipo del callback para cerrar la ventana */
G_MODULE_EXPORT void on_main_window_destroy(GtkButton *button, gpointer user_data) {
    gtk_main_quit();
}

/* Prototipos de funcion */
void preguntarNombre();

// Barra lateral
G_MODULE_EXPORT void on_btnInventario_clicked(GtkButton *button, gpointer user_data);
G_MODULE_EXPORT void on_btnCliente_clicked(GtkButton *button, gpointer user_data);
G_MODULE_EXPORT void on_btnTrabajadores_clicked(GtkButton *button, gpointer user_data);
G_MODULE_EXPORT void on_btnFacturas_clicked(GtkButton *button, gpointer user_data);
G_MODULE_EXPORT void on_btnProductos_clicked(GtkButton *button, gpointer user_data);

// Inventario
void init_inventario(char *nombre_inv);
void refresh_inventario(const char *filtro);
int obtener_seleccion();
G_MODULE_EXPORT void on_invEliminar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_invAgregar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_invEditar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_invFiltro_search_changed(GtkButton *btn, gpointer user_data);

// Clientes
void init_clientes();
void refresh_clientes(const char *filtro);
int obtener_seleccion_cli();
G_MODULE_EXPORT void on_cliEliminar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_cliAgregar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_cliEditar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_cliFiltro_search_changed(GtkButton *btn, gpointer user_data);

// Trabajadores
void init_trabajadores();
void refresh_trabajadores(const char *filtro);
int obtener_seleccion_tra();
G_MODULE_EXPORT void on_traEliminar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_traAgregar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_traEditar_clicked(GtkButton *btn, gpointer user_data);
G_MODULE_EXPORT void on_traFiltro_search_changed(GtkButton *btn, gpointer user_data);

// Facturas
void     mostrar_alerta(const char *mensaje);
int      siguiente_numero_factura(void);
gboolean guardar_factura(Factura *factura);





// FUNCION PARA GUARDAR
// save_bin("inventario.bin", inventario, sizeof(Producto), n_inventario);
void save_bin(const char *nombre_de_archivo,
              const void *array,
              size_t elem_size,
              size_t count)
{
    FILE *f = fopen(g_strconcat("data/", nombre_de_archivo, NULL), "wb");
    if (!f) {
        perror("Error al guardar");
        return;
    }
    fwrite(&count, sizeof(count), 1, f);
    fwrite(array, elem_size, count, f);
    fclose(f);
}


// FUNCION PARA CARGAR
// load_bin("inventario.bin", inventario, sizeof(Producto), n_inventario);
size_t load_bin(const char *nombre_de_archivo,
                void *array,
                size_t elem_size,
                size_t max_count)
{
    FILE *f = fopen(g_strconcat("data/", nombre_de_archivo, NULL), "rb");
    if (!f) return 0;

    size_t count;
    fread(&count, sizeof(count), 1, f);
    if (count > max_count) count = max_count;
    fread(array, elem_size, count, f);
    fclose(f);
    return count;
}

void crear_directorio(const char *ruta) {
    if (access(ruta, F_OK) == -1) {
        mkdir(ruta);
    }
}

void cargar_todo(){
	crear_directorio("data/");
	n_inventario = load_bin("inventario.bin", inventario, sizeof(Producto), MAX_PRODUCTOS);
	n_clientes = load_bin("clientes.bin", clientes, sizeof(Cliente), MAX_CLIENTES);
	n_trabajadores = load_bin("trabajadores.bin", trabajadores, sizeof(Trabajador), MAX_TRABAJADORES);
}



void preguntarNombre()
{
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_nombre_tienda")
    );
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "nombreTienda")));

        strncpy(nombre_tienda, nom, sizeof(nombre_tienda)-1);
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}


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

G_MODULE_EXPORT void on_btnCliente_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_clientes") 
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

G_MODULE_EXPORT void on_btnTrabajadores_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "page_trabajadores") 
    );

    // Muestra esa página
    gtk_stack_set_visible_child(stack, page);
}

G_MODULE_EXPORT void on_btnFacturas_clicked(GtkButton *button, gpointer user_data)
{
    GtkStack *stack = GTK_STACK(user_data);
    // Recupera el widget hijo por su ID de Glade
    GtkWidget *page = GTK_WIDGET(
        gtk_builder_get_object(builder, "facturacion") 
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
    COL_STOCK,
    COL_COMPRA,
    COL_VENTA,
    COL_UBICACION,
    COL_PROVEEDOR,
    COL_ENTRADAS,
    COL_SALIDAS,
    N_COLUMNS
};

void init_inventario(char *nombre_inv)
{
    GtkTreeIter   iter;

    /* 1. Obtener TreeView y modelo */
    GtkListStore *store = gtk_list_store_new(
	    N_COLUMNS,
	    G_TYPE_STRING,  /* codigo */
	    G_TYPE_STRING,  /* nombre */
	    G_TYPE_STRING,  /* descripcion */
	    G_TYPE_STRING,  /* categoria */
	    G_TYPE_INT,     /* cantidad */
	    G_TYPE_INT,     /* stock minimo */
	    G_TYPE_FLOAT,   /* precio compra */
	    G_TYPE_FLOAT,   /* precio venta */
	    G_TYPE_STRING,  /* ubicacion */
	    G_TYPE_STRING,  /* proveedor */
	    G_TYPE_INT,     /* entradas */
	    G_TYPE_INT      /* salidas */
	);
	
	GtkTreeView *tree = GTK_TREE_VIEW(
	    gtk_builder_get_object(builder, nombre_inv)
	);
	gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
	g_object_unref(store);

    /* 2. Limpiar datos previos */
    gtk_list_store_clear(store);

    /* 3. Población */
    for (int i = 0; i < n_inventario; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  inventario[i].codigo,
            1,  inventario[i].nombre,
            2,  inventario[i].descripcion,
            3,  inventario[i].categoria,
            4,  inventario[i].cantidad,
            5,  inventario[i].stock_minimo,
            6,  inventario[i].precio_compra,
            7,  inventario[i].precio_venta,
            8,  inventario[i].ubicacion,
            9,  inventario[i].proveedor,
            10, inventario[i].entradas,
            11, inventario[i].salidas,
            -1
        );
    }

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
            gchar *prove = g_ascii_strdown(inventario[i].proveedor, -1);
            gchar *desc = g_ascii_strdown(inventario[i].descripcion, -1);
			gchar *filtro_minus = g_ascii_strdown(filtro,   -1);
			
            if (!strstr(nombre, filtro_minus) &&
            	!strstr(prove, filtro_minus) &&
            	!strstr(desc, filtro_minus) &&
                !strstr(inventario[i].codigo, filtro))
                continue;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0, inventario[i].codigo,
            1, inventario[i].nombre,
            2, inventario[i].descripcion,
            3, inventario[i].categoria,
            4, inventario[i].cantidad,
            5, inventario[i].stock_minimo,
            6, inventario[i].precio_compra,
            7, inventario[i].precio_venta,
            8,  inventario[i].ubicacion,
            9,  inventario[i].proveedor,
            10, inventario[i].entradas,
            11, inventario[i].salidas,
            -1
        );
    }
}

int obtener_seleccion()
{
	GtkTreeView      *tree    = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_inventario"));
    GtkTreeSelection *sel     = gtk_tree_view_get_selection(tree);
    GtkTreeIter       iter;
    GtkTreeModel     *model;

    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return -1;

    /* Obtener código y buscar índice */
    gchar *cod;
    gtk_tree_model_get(model, &iter, COL_CODIGO, &cod, -1);
    int idx = -1;
    for (int i = 0; i < n_inventario; i++) {
        if (strcmp(inventario[i].codigo, cod) == 0) { idx = i; break; }
    }
    g_free(cod);
    return idx;
}

G_MODULE_EXPORT void on_invEliminar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion();
    if (idx < 0) return;

    /* Mover a la izquierda el arreglo */
    for (gint i = idx; i < n_inventario - 1; i++) {
        inventario[i] = inventario[i + 1];
    }
    n_inventario--;

    /* Refrescar la vista */
    refresh_inventario(NULL);
    save_bin("inventario.bin", inventario, sizeof(Producto), n_inventario);
}

G_MODULE_EXPORT void on_invAgregar_clicked(GtkButton *btn, gpointer user_data)
{
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_inv_agregar")
    );
    
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCodigo")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggNombre")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggDesc")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCategoria")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCantidad")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggMinimo")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggVenta")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCompra")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggUbicacion")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggProveedor")),  "");
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK && n_inventario < MAX_PRODUCTOS) {
        /* Leer campos */
        const char *cod  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCodigo")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggNombre")));
        const char *desc  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggDesc")));
        const char *cat  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCategoria")));
        const char *cant = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCantidad")));
        const char *min = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggMinimo")));
        const char *venta = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggVenta")));
        const char *compra = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCompra")));
        const char *ubi = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggUbicacion")));
        const char *prove = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggProveedor")));

        /* Añadir al arreglo */
        Producto *p = &inventario[n_inventario++];
        strncpy(p->codigo,      cod,  sizeof(p->codigo)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->descripcion,desc,  sizeof(p->descripcion)-1);
        strncpy(p->categoria,   cat,  sizeof(p->categoria)-1);
        strncpy(p->ubicacion,   ubi,  sizeof(p->ubicacion)-1);
        strncpy(p->proveedor, prove,  sizeof(p->proveedor)-1);
        
        p->cantidad = atoi(cant);
        p->stock_minimo = atoi(min);
        p->precio_compra = atof(compra);
        p->precio_venta = atof(venta);
		
		p->fecha_registro = time(NULL);
		p->entradas = atoi(cant);
		p->salidas = 0;

        /* Refrescar la vista */
        refresh_inventario(NULL);
    	save_bin("inventario.bin", inventario, sizeof(Producto), n_inventario);
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_invEditar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion();
    if (idx < 0) return;
    
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_inv_agregar")
    );
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCodigo")),  inventario[idx].codigo);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggNombre")),  inventario[idx].nombre);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggDesc")),  inventario[idx].descripcion);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCategoria")),  inventario[idx].categoria);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCantidad")),  g_strdup_printf("%d", inventario[idx].cantidad));
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggMinimo")),  g_strdup_printf("%d", inventario[idx].stock_minimo));
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggVenta")),  g_strdup_printf("%f", inventario[idx].precio_venta));
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggCompra")),  g_strdup_printf("%f", inventario[idx].precio_compra));
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggUbicacion")),  inventario[idx].ubicacion);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "aggProveedor")),  inventario[idx].proveedor);
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK && n_inventario < MAX_PRODUCTOS) {
        /* Leer campos */
        const char *cod  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCodigo")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggNombre")));
        const char *desc  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggDesc")));
        const char *cat  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCategoria")));
        const char *cant = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCantidad")));
        const char *min = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggMinimo")));
        const char *venta = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggVenta")));
        const char *compra = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggCompra")));
        const char *ubi = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggUbicacion")));
        const char *prove = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "aggProveedor")));

        /* Añadir al arreglo */
        Producto *p = &inventario[idx];
        strncpy(p->codigo,      cod,  sizeof(p->codigo)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->descripcion,desc,  sizeof(p->descripcion)-1);
        strncpy(p->categoria,   cat,  sizeof(p->categoria)-1);
        strncpy(p->ubicacion,   ubi,  sizeof(p->ubicacion)-1);
        strncpy(p->proveedor, prove,  sizeof(p->proveedor)-1);
        
		p->entradas += atoi(cant) - p->cantidad;
        
        p->cantidad = atoi(cant);
        p->stock_minimo = atoi(min);
        p->precio_compra = atof(compra);
        p->precio_venta = atof(venta);

        /* Refrescar la vista */
        refresh_inventario(NULL);
    	save_bin("inventario.bin", inventario, sizeof(Producto), n_inventario);
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}


G_MODULE_EXPORT void on_invFiltro_search_changed(GtkButton *btn, gpointer user_data)
{
    const char *texto   = gtk_entry_get_text(GTK_ENTRY(
        gtk_builder_get_object(builder, "invFiltro")));

    /* Simplemente repuebla usando el texto como filtro */
    refresh_inventario(texto);
}







/*
FUNCIONES DE CLIENTES
*/

enum {
    COL_CEDULA,
    COL_CNOMBRE,
    COL_APELLIDO,
    COL_TELEFONO,
    COL_DIRECCION,
    N_COLUMNS_CLI
};

void init_clientes()
{
    GtkTreeIter   iter;

    /* 1. Obtener TreeView y modelo */
    GtkListStore *store = gtk_list_store_new(
	    N_COLUMNS_CLI,
	    G_TYPE_STRING,  /* cedula */
	    G_TYPE_STRING,  /* nombre */
	    G_TYPE_STRING,  /* apellido */
	    G_TYPE_STRING,  /* telefono */
	    G_TYPE_STRING  /* direccion */
	);
	
	GtkTreeView *tree = GTK_TREE_VIEW(
	    gtk_builder_get_object(builder, "tree_clientes")
	);
	gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
	g_object_unref(store);

    /* 2. Limpiar datos previos */
    gtk_list_store_clear(store);

    /* 3. Población */
    for (int i = 0; i < n_clientes; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  clientes[i].cedula,
            1,  clientes[i].nombre,
            2,  clientes[i].apellido,
            3,  clientes[i].telefono,
            4,  clientes[i].direccion,
            -1
        );
    }
}

void refresh_clientes(const char *filtro) {
    GtkTreeView  *tree  = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_clientes"));
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(tree));
    GtkTreeIter   iter;

    /* Limpia filas previas */
    gtk_list_store_clear(store);

    /* Recorrer el inventario y añadir las filas que pasen el filtro */
    for (gint i = 0; i < n_clientes; i++) {
        /* Si hay filtro, comprobamos si aparece en código o nombre */
        if (filtro && *filtro) {
        	// Pasamos ambos a minusculas durante la busqueda por nombre para q las mayusculas no sean relevantes
            gchar *nombre = g_ascii_strdown(clientes[i].nombre, -1);
            gchar *apellido = g_ascii_strdown(clientes[i].apellido, -1);
			gchar *filtro_minus = g_ascii_strdown(filtro,   -1);
			
            if (!strstr(nombre, filtro_minus) &&
            	!strstr(apellido, filtro_minus) &&
                !strstr(clientes[i].cedula, filtro))
                continue;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  clientes[i].cedula,
            1,  clientes[i].nombre,
            2,  clientes[i].apellido,
            3,  clientes[i].telefono,
            4,  clientes[i].direccion,
            -1
        );
    }
}

int obtener_seleccion_cli()
{
	GtkTreeView      *tree    = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_clientes"));
    GtkTreeSelection *sel     = gtk_tree_view_get_selection(tree);
    GtkTreeIter       iter;
    GtkTreeModel     *model;

    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return -1;

    /* Obtener cedula y buscar índice */
    gchar *ci;
    gtk_tree_model_get(model, &iter, COL_CEDULA, &ci, -1);
    int idx = -1;
    for (int i = 0; i < n_clientes; i++) {
        if (strcmp(clientes[i].cedula, ci) == 0) { idx = i; break; }
    }
    g_free(ci);
    return idx;
}

G_MODULE_EXPORT void on_cliEliminar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_cli();
    if (idx < 0) return;

    /* Mover a la izquierda el arreglo */
    for (gint i = idx; i < n_clientes - 1; i++) {
        clientes[i] = clientes[i + 1];
    }
    n_clientes--;

    /* Refrescar la vista */
    refresh_clientes(NULL);
    save_bin("clientes.bin", clientes, sizeof(Cliente), n_clientes);
}

G_MODULE_EXPORT void on_cliAgregar_clicked(GtkButton *btn, gpointer user_data)
{
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_cli_agregar")
    );
    
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")),  "");
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *ci  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")));
        const char *apel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")));
        const char *tel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")));
        const char *dir = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")));

        /* Añadir al arreglo */
        Cliente *p = &clientes[n_clientes++];
        strncpy(p->cedula,      ci,  sizeof(p->cedula)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->apellido,   apel,  sizeof(p->apellido)-1);
        strncpy(p->telefono,    tel,  sizeof(p->telefono)-1);
        strncpy(p->direccion,   dir,  sizeof(p->direccion)-1);

        /* Refrescar la vista */
        refresh_clientes(NULL);
    	save_bin("clientes.bin", clientes, sizeof(Cliente), n_clientes);
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_cliEditar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_cli();
    if (idx < 0) return;
    
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_cli_agregar")
    );
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")),  clientes[idx].cedula);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")),  clientes[idx].nombre);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")),  clientes[idx].apellido);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")),  clientes[idx].telefono);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")),  clientes[idx].direccion);
    
       gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *ci  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")));
        const char *apel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")));
        const char *tel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")));
        const char *dir = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")));

        /* Añadir al arreglo */
        Cliente *p = &clientes[idx];
        strncpy(p->cedula,      ci,  sizeof(p->cedula)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->apellido,   apel,  sizeof(p->apellido)-1);
        strncpy(p->telefono,    tel,  sizeof(p->telefono)-1);
        strncpy(p->direccion,   dir,  sizeof(p->direccion)-1);

        /* Refrescar la vista */
        refresh_clientes(NULL);
    	save_bin("clientes.bin", clientes, sizeof(Cliente), n_clientes);
    }
 
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_cliFiltro_search_changed(GtkButton *btn, gpointer user_data)
{
    const char *texto   = gtk_entry_get_text(GTK_ENTRY(
        gtk_builder_get_object(builder, "cliFiltro")));

    /* Simplemente repuebla usando el texto como filtro */
    refresh_clientes(texto);
}













/*
FUNCIONES DE TRABAJADORES
*/

enum {
    COL_TCEDULA,
    COL_TNOMBRE,
    COL_TAPELLIDO,
    COL_TTELEFONO,
    COL_TDIRECCION,
    N_COLUMNS_TRA
};

void init_trabajadores()
{
    GtkTreeIter   iter;

    /* 1. Obtener TreeView y modelo */
    GtkListStore *store = gtk_list_store_new(
	    N_COLUMNS_TRA,
	    G_TYPE_STRING,  /* cedula */
	    G_TYPE_STRING,  /* nombre */
	    G_TYPE_STRING,  /* apellido */
	    G_TYPE_STRING,  /* telefono */
	    G_TYPE_STRING  /* direccion */
	);
	
	GtkTreeView *tree = GTK_TREE_VIEW(
	    gtk_builder_get_object(builder, "tree_trabajadores")
	);
	gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
	g_object_unref(store);

    /* 2. Limpiar datos previos */
    gtk_list_store_clear(store);

    /* 3. Población */
    for (int i = 0; i < n_trabajadores; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  trabajadores[i].cedula,
            1,  trabajadores[i].nombre,
            2,  trabajadores[i].apellido,
            3,  trabajadores[i].telefono,
            4,  trabajadores[i].direccion,
            -1
        );
    }

}

void refresh_trabajadores(const char *filtro) {
    GtkTreeView  *tree  = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_trabajadores"));
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(tree));
    GtkTreeIter   iter;

    /* Limpia filas previas */
    gtk_list_store_clear(store);

    /* Recorrer el inventario y añadir las filas que pasen el filtro */
    for (gint i = 0; i < n_trabajadores; i++) {
        /* Si hay filtro, comprobamos si aparece en código o nombre */
        if (filtro && *filtro) {
        	// Pasamos ambos a minusculas durante la busqueda por nombre para q las mayusculas no sean relevantes
            gchar *nombre = g_ascii_strdown(trabajadores[i].nombre, -1);
            gchar *apellido = g_ascii_strdown(trabajadores[i].apellido, -1);
			gchar *filtro_minus = g_ascii_strdown(filtro,   -1);
			
            if (!strstr(nombre, filtro_minus) &&
            	!strstr(apellido, filtro_minus) &&
                !strstr(trabajadores[i].cedula, filtro))
                continue;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  trabajadores[i].cedula,
            1,  trabajadores[i].nombre,
            2,  trabajadores[i].apellido,
            3,  trabajadores[i].telefono,
            4,  trabajadores[i].direccion,
            -1
        );
    }
}

int obtener_seleccion_tra()
{
	GtkTreeView      *tree    = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_trabajadores"));
    GtkTreeSelection *sel     = gtk_tree_view_get_selection(tree);
    GtkTreeIter       iter;
    GtkTreeModel     *model;

    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return -1;

    /* Obtener cedula y buscar índice */
    gchar *ci;
    gtk_tree_model_get(model, &iter, COL_TCEDULA, &ci, -1);
    int idx = -1;
    for (int i = 0; i < n_trabajadores; i++) {
        if (strcmp(trabajadores[i].cedula, ci) == 0) { idx = i; break; }
    }
    g_free(ci);
    return idx;
}

G_MODULE_EXPORT void on_traEliminar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_tra();
    if (idx < 0) return;

    /* Mover a la izquierda el arreglo */
    for (gint i = idx; i < n_trabajadores - 1; i++) {
        trabajadores[i] = trabajadores[i + 1];
    }
    n_trabajadores--;

    /* Refrescar la vista */
    refresh_trabajadores(NULL);
    save_bin("trabajadores.bin", trabajadores, sizeof(Trabajador), n_trabajadores);
}

G_MODULE_EXPORT void on_traAgregar_clicked(GtkButton *btn, gpointer user_data)
{
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_cli_agregar")
    );
    
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")),  "");
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")),  "");
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *ci  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")));
        const char *apel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")));
        const char *tel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")));
        const char *dir = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")));

        /* Añadir al arreglo */
       	Trabajador *p = &trabajadores[n_trabajadores++];
        strncpy(p->cedula,      ci,  sizeof(p->cedula)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->apellido,   apel,  sizeof(p->apellido)-1);
        strncpy(p->telefono,    tel,  sizeof(p->telefono)-1);
        strncpy(p->direccion,   dir,  sizeof(p->direccion)-1);

        /* Refrescar la vista */
        refresh_trabajadores(NULL);
    	save_bin("trabajadores.bin", trabajadores, sizeof(Trabajador), n_trabajadores);
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_traEditar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_tra();
    if (idx < 0) return;
    
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "dialog_cli_agregar")
    );
    // vaciar los espacios previamente
    
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")),  trabajadores[idx].cedula);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")),  trabajadores[idx].nombre);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")),  trabajadores[idx].apellido);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")),  trabajadores[idx].telefono);
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")),  trabajadores[idx].direccion);
    
       gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *ci  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliCedula")));
        const char *nom  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliNombre")));
        const char *apel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliApellido")));
        const char *tel  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliTelefono")));
        const char *dir = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "cliDireccion")));

        /* Añadir al arreglo */
        Trabajador *p = &trabajadores[idx];
        strncpy(p->cedula,      ci,  sizeof(p->cedula)-1);
        strncpy(p->nombre,      nom,  sizeof(p->nombre)-1);
        strncpy(p->apellido,   apel,  sizeof(p->apellido)-1);
        strncpy(p->telefono,    tel,  sizeof(p->telefono)-1);
        strncpy(p->direccion,   dir,  sizeof(p->direccion)-1);

        /* Refrescar la vista */
        refresh_trabajadores(NULL);
    	save_bin("trabajadores.bin", trabajadores, sizeof(Trabajador), n_trabajadores);
    }
 
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_traFiltro_search_changed(GtkButton *btn, gpointer user_data)
{
    const char *texto   = gtk_entry_get_text(GTK_ENTRY(
        gtk_builder_get_object(builder, "traFiltro")));

    /* Simplemente repuebla usando el texto como filtro */
    refresh_trabajadores(texto);
}




/*
FUNCIONES DE FACTURAS
*/
///////////////////
// Instancia global
/////////////////////

static Factura factura_actual;
static float tasa_bcv = 112.13;
static float tasa_cop = 4052.0;
static float tasa_euro = 131.59;


float dolar_a_bs(float cant){
	return cant * tasa_bcv;
}

float dolar_a_cop(float cant){
	return cant * tasa_cop;
}

float dolar_a_euro(float cant){
	float bs = dolar_a_bs(cant);
	return bs / tasa_euro;
}

void refresh_factura() {
    GtkTreeIter   iter;

    /* 1. Obtener TreeView y modelo */
    GtkListStore *store = gtk_list_store_new(
	    5,
	    G_TYPE_STRING,  /* codigo */
	    G_TYPE_STRING,  /* nombre */
	    G_TYPE_FLOAT,  /* precio */
	    G_TYPE_INT,    /* cant */
	    G_TYPE_FLOAT  /* subtotal */
	);
	
	GtkTreeView *tree = GTK_TREE_VIEW(
	    gtk_builder_get_object(builder, "tree_factura")
	);
	gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
	g_object_unref(store);

    /* 2. Limpiar datos previos */
    gtk_list_store_clear(store);

	factura_actual.total = 0.0;
	
    /* 3. Población */
    for (int i = 0; i < factura_actual.n_productos; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0,  factura_actual.productos[i].codigo,
            1,  factura_actual.productos[i].nombre,
            2,  factura_actual.productos[i].precio_venta,
            3,  factura_actual.productos[i].cantidad,
            4,  factura_actual.productos[i].precio_venta * factura_actual.productos[i].cantidad,
            -1
        );
        
        factura_actual.total += factura_actual.productos[i].precio_venta * factura_actual.productos[i].cantidad;
    }
    
    factura_actual.total += factura_actual.total * (IVA_RATE/100.0);
    
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "montoDolar")),
		g_strconcat(g_strdup_printf("%.2f", factura_actual.total), " $", NULL));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "montoEuro")),
		g_strconcat(g_strdup_printf("%.2f", dolar_a_euro(factura_actual.total)), " €", NULL));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "montoBolivar")),
		g_strconcat(g_strdup_printf("%.2f", dolar_a_bs(factura_actual.total)), " Bs.", NULL));
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "montoCop")),
		g_strconcat(g_strdup_printf("%.2f", dolar_a_cop(factura_actual.total)), " Cop", NULL));
}
        
G_MODULE_EXPORT void on_invCompra_clicked(GtkButton *btn, gpointer user_data)
{
	int idx = obtener_seleccion();
	if(idx < 0) return;
	
	factura_actual.productos[factura_actual.n_productos] = inventario[idx];
	factura_actual.productos[factura_actual.n_productos].cantidad = 1;
	factura_actual.n_productos++;
	
	refresh_factura();
}

// Muestra un diálogo para confirmar o ingresar datos del cliente
static gboolean confirmar_datos_cliente() {
    GtkDialog *dlg = GTK_DIALOG(gtk_builder_get_object(builder, "dlg_confirm_cliente"));
    if (gtk_dialog_run(dlg) != GTK_RESPONSE_OK) {
        gtk_widget_hide(GTK_WIDGET(dlg));
        return FALSE;
    }

    GtkEntry *eCedula = GTK_ENTRY(gtk_builder_get_object(builder, "entryClienteCedula"));

    strncpy(
        factura_actual.cedula_cliente,
        gtk_entry_get_text(eCedula),
        sizeof(factura_actual.cedula_cliente) - 1
    );
    factura_actual.cedula_cliente[sizeof(factura_actual.cedula_cliente) - 1] = '\0';

    gtk_widget_hide(GTK_WIDGET(dlg));
    return TRUE;
}


// Construye y muestra el resumen de la factura
static void mostrar_resumen_factura() {
    // Calcular subtotales e IVA
    factura_actual.subtotal = 0.0f;
    for (int i = 0; i < factura_actual.n_productos; i++) {
        Producto *p = &factura_actual.productos[i];
        factura_actual.subtotal += p->precio_venta * p->cantidad;
    }
    factura_actual.iva   = factura_actual.subtotal * IVA_RATE / 100.0f;
    factura_actual.total = factura_actual.subtotal + factura_actual.iva;

    // Obtener diálogo y widgets
    GtkDialog  *dlg     = GTK_DIALOG(
        gtk_builder_get_object(builder, "dlg_invoice_summary")
    );
    GtkLabel   *lStore  = GTK_LABEL(
        gtk_builder_get_object(builder, "lblStoreName")
    );
    GtkLabel   *lNum    = GTK_LABEL(
        gtk_builder_get_object(builder, "lblInvoiceNumber")
    );
    GtkLabel   *lDate   = GTK_LABEL(
        gtk_builder_get_object(builder, "lblDate")
    );
    GtkLabel   *lClient = GTK_LABEL(
        gtk_builder_get_object(builder, "lblClientData")
    );
    GtkTreeView *tree   = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "treeInvoiceDetails")
    );

    // Actualizar etiquetas de encabezado
    gtk_label_set_text(lStore, factura_actual.tienda_nombre);

    char buf[128];
    snprintf(buf, sizeof(buf), "Factura #%d", factura_actual.num);
    gtk_label_set_text(lNum, buf);

    struct tm *tm_info = localtime(&factura_actual.fecha);
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", tm_info);
    gtk_label_set_text(lDate, buf);

    snprintf(buf, sizeof(buf), "%s  Cédula: %s\n%s",
             factura_actual.cliente_nombre,
             factura_actual.cedula_cliente,
             factura_actual.cliente_direccion);
    gtk_label_set_text(lClient, buf);

    // Crear y poblar el ListStore con detalle de productos
    GtkListStore *store = gtk_list_store_new(
        4,
        G_TYPE_INT,
        G_TYPE_STRING,
        G_TYPE_FLOAT,
        G_TYPE_FLOAT
    );
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_list_store_clear(store);

    GtkTreeIter iter;
    for (int i = 0; i < factura_actual.n_productos; i++) {
        Producto *p = &factura_actual.productos[i];
        float subtotal_line = p->precio_venta * p->cantidad;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter,
            0, p->cantidad,
            1, p->nombre,
            2, p->precio_venta,
            3, subtotal_line,
            -1
        );
    }

    // Mostrar totales al pie
    GtkLabel *lSub   = GTK_LABEL(gtk_builder_get_object(builder, "lblSubtotal"));
    GtkLabel *lIVA   = GTK_LABEL(gtk_builder_get_object(builder, "lblIVA"));
    GtkLabel *lTotal = GTK_LABEL(gtk_builder_get_object(builder, "lblTotal"));

    snprintf(buf, sizeof(buf), "%.2f $", factura_actual.subtotal);
    gtk_label_set_text(lSub, buf);
    snprintf(buf, sizeof(buf), "%.2f $ (%.1f%%)", factura_actual.iva, IVA_RATE);
    gtk_label_set_text(lIVA, buf);
    snprintf(buf, sizeof(buf), "%.2f $", factura_actual.total);
    gtk_label_set_text(lTotal, buf);

    // Mostrar diálogo
    gtk_widget_show_all(GTK_WIDGET(dlg));
    gtk_dialog_run(dlg);
    gtk_widget_hide(GTK_WIDGET(dlg));
}

// Callback del botón Facturar
G_MODULE_EXPORT void on_btnFacturar_clicked(GtkButton *btn, gpointer user_data) {
    // 1. Validaciones básicas
    g_print("Botón Facturar presionado\n");

    if (factura_actual.n_productos == 0) {
        mostrar_alerta("Agrega al menos un producto.");
        return;
    }
    // 2. Confirmar o ingresar datos del cliente
    if (!confirmar_datos_cliente()) {
        return;
    }
    // 3. Asignar número y fecha
    factura_actual.num   = siguiente_numero_factura();
    factura_actual.fecha = time(NULL);

    // 4. Mostrar resumen y calcular totales con IVA
    mostrar_resumen_factura();

    // 5. Guardar la factura (archivo/BD) si todo está correcto
    if (!guardar_factura(&factura_actual)) {
        mostrar_alerta("Error al guardar la factura.");
    } else {
        mostrar_alerta("Factura guardada con éxito.");
    }
    
    // se recorre los productos facturados
    for(int i = 0; i < factura_actual.n_productos; i++){
	    int idx = -1;
	    // se busca el producto equivalente en el inventario, con el mismo codigo
	    for (int j = 0; j < n_inventario; j++) {
	        if (strcmp(inventario[j].codigo, factura_actual.productos[i].codigo) == 0) { 
	        	// se reduce la cantidad registrada en el inventario
				inventario[j].cantidad -= factura_actual.productos[i].cantidad;
				inventario[j].salidas += factura_actual.productos[i].cantidad;
			}
	    }
	}
    
    struct Factura f; //factura vacia
    factura_actual = f;
    
    refresh_factura();
    refresh_inventario(NULL);
}



int obtener_seleccion_fact()
{
	GtkTreeView      *tree    = GTK_TREE_VIEW(
        gtk_builder_get_object(builder, "tree_factura"));
    GtkTreeSelection *sel     = gtk_tree_view_get_selection(tree);
    GtkTreeIter       iter;
    GtkTreeModel     *model;

    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) return -1;

    /* Obtener código y buscar índice */
    gchar *cod;
    gtk_tree_model_get(model, &iter, COL_CODIGO, &cod, -1);
    int idx = -1;
    for (int i = 0; i < factura_actual.n_productos; i++) {
        if (strcmp(factura_actual.productos[i].codigo, cod) == 0) { idx = i; break; }
    }
    g_free(cod);
    return idx;
}

G_MODULE_EXPORT void on_tree_factura_row_activated(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_fact();
    if (idx < 0) return;
    
    GtkDialog    *dialog  = GTK_DIALOG(
        gtk_builder_get_object(builder, "factura_editar_cantidad")
    );
    gtk_entry_set_text(
		GTK_ENTRY(gtk_builder_get_object(builder, "factura_nueva_cantidad")),
		g_strdup_printf("%d", factura_actual.productos[idx].cantidad));
    
    gint resp = gtk_dialog_run(dialog);
    if (resp == GTK_RESPONSE_OK) {
        /* Leer campos */
        const char *cant  = gtk_entry_get_text(
            GTK_ENTRY(gtk_builder_get_object(builder, "factura_nueva_cantidad")));

        /* Añadir al arreglo */
        factura_actual.productos[idx].cantidad = atoi(cant);

        /* Refrescar la vista */
        refresh_factura();
    }
    gtk_widget_hide(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT void on_factEliminar_clicked(GtkButton *btn, gpointer user_data)
{
    int idx = obtener_seleccion_fact();
    if (idx < 0) return;

    /* Mover a la izquierda el arreglo */
    for (gint i = idx; i < factura_actual.n_productos - 1; i++) {
        factura_actual.productos[i] = factura_actual.productos[i + 1];
    }
    factura_actual.n_productos--;

    /* Refrescar la vista */
    refresh_factura();
}





//////////////
// Muestra un mensaje en un cuadro de diálogo
void mostrar_alerta(const char *mensaje) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", mensaje);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Genera un número de factura secuencial
int siguiente_numero_factura(void) {
    static int contador = 1000; // Puedes guardar y cargar desde archivo si deseas persistencia
    return contador++;
}

// Simula guardar la factura (puedes ampliar esta función para escribirla en archivo o BD)
gboolean guardar_factura(Factura *factura) {
    // Por ahora, siempre retorna TRUE
    return TRUE;
}

//////////////

/*
 CARGADO DEL ARCHIVO GLADE
*/



int main(int argc, char *argv[]) {
    GtkWidget  *window;
    GError     *error = NULL;

    /* Inicializar GTK */
    gtk_init(&argc, &argv);
    
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings,
                 "gtk-application-prefer-dark-theme", TRUE,
                 NULL);

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
    
    
    cargar_todo();
    init_inventario("tree_inventario");
    init_clientes();
    init_trabajadores();

    /* Mostrar la ventana y todos sus hijos */
    gtk_widget_show_all(window);
    
    
    preguntarNombre();

    /* Bucle principal de GTK */
    gtk_main();

    return 0;
}

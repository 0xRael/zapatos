# Zapatos w

Una aplicacion de escritorio para manejar el inventario, productos, clientes, facturas y empleados de una tienda de zapatos. Construida en C con GTK y Glade para una interfaz moderna.

---

## Requisitos

- GTK 3 (o GTK 4)  
- Glade UI Designer  
- GCC con pkg-config  
- Librerías estándar de C (`string.h`, `stdlib.h`)  

---

## Instalación

1. Clona el repositorio  
   ```bash
   git clone https://github.com/0xRael/zapateria.git
   cd zapateria
   ```  

2. Compila la aplicación  
   ```bash
   gcc  main.c -o main `pkg-config --cflags --libs gtk+-3.0`
   ```  

3. Asegúrate de tener el archivo `interfaz.glade` en el mismo directorio que el ejecutable.

---

## Estructura del proyecto

- `main.c`  
- `interfaz.glade` 
- `README.md`  

---

## Uso

1. Ejecuta la aplicación  
   ```bash
   ./main
   ```  

2. Navega por el menú lateral para acceder a:  
   - Inventario  
   - Productos  
   - Clientes  
   - Facturas  
   - Trabajadores  

3. En **Inventario** usa los botones:  
   - Agregar  
   - Eliminar  
   - Editar  
   - Filtrar  

   para gestionar los registros de zapatos.

---

## Diseño de la interfaz

- **GtkApplicationWindow** con HeaderBar personalizado  
- Panel de navegación vertical con `GtkButton` + icono  
- Área de contenido con `GtkStack` para cada sección  
- `GtkTreeView` + `GtkListStore` en Inventario y Facturas  
- `GtkFlowBox` para listar productos en miniaturas  
- Formularios en `GtkGrid` para Clientes y Trabajadores  
- Diálogos diseñados en Glade para Agregar y Editar  

---

## Funcionalidades destacadas

- Carga y refresco de datos desde un arreglo de structs  
- Filtros en tiempo real por código o nombre  
- Ordenamiento y selección de filas en tablas  
- Edición directa mediante diálogos modal en Glade  
- Eliminación sincronizada entre `ListStore` y arreglo en memoria  

---

## Extensiones posibles

- Persistencia en archivo JSON o base de datos SQLite  
- Exportar facturas a CSV o PDF  
- Modo oscuro con CSS personalizado  
- Reportes de stock mínimo y alertas visuales  
- Integración con API de proveedores para actualización automática  

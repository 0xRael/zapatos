# Chiara

Una aplicacion de escritorio para manejar el inventario, productos, clientes, facturas y empleados de una tienda de zapatos. Construida en C con GTK y Glade para una interfaz moderna.

---

## Requisitos

- GTK 3 (o GTK 4)  
- Glade UI Designer  
- GCC con pkg-config  
- Librerias estandar de C (`string.h`, `stdlib.h`)  

---

## Instalaci�n

1. Clona el repositorio  
   ```bash
   git clone https://github.com/0xRael/zapateria.git
   cd zapateria
   ```  
c:/Users/user/OneDrive/Documentos/GitHub/zapatos
2. Compila la aplicacion  
   ```bash
   gcc  main.c -o main `pkg-config --cflags --libs gtk+-3.0`
   ```  

3. Asegurate de tener el archivo `interfaz.glade` en el mismo directorio que el ejecutable.

---

## Estructura del proyecto

- `main.c`  
- `interfaz.glade` 
- `README.md`  

---

## Uso

1. Ejecuta la aplicacion  
   ```bash
   ./main
   ```  

2. Navega por el menu lateral para acceder a:  
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

## Diseno de la interfaz

- **GtkApplicationWindow** con HeaderBar personalizado  
- Panel de navegacion vertical con `GtkButton` + icono  
- Area de contenido con `GtkStack` para cada seccion  
- `GtkTreeView` + `GtkListStore` en Inventario y Facturas  
- `GtkFlowBox` para listar productos en miniaturas  
- Formularios en `GtkGrid` para Clientes y Trabajadores  
- Dialogos disenados en Glade para Agregar y Editar  

---

## Funcionalidades destacadas

- Carga y refresco de datos desde un arreglo de structs  
- Filtros en tiempo real por codigo o nombre  
- Ordenamiento y seleccion de filas en tablas  
- Edicion directa mediante diologos modal en Glade  
- Eliminacion sincronizada entre `ListStore` y arreglo en memoria  

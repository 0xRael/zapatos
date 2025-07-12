# Zapatos w

Una aplicacion de escritorio para manejar el inventario, productos, clientes, facturas y empleados de una tienda de zapatos. Construida en C con GTK y Glade para una interfaz moderna.

---

## Requisitos

- GTK 3 (o GTK 4)  
- Glade UI Designer  
- GCC con pkg-config  
- Librer�as est�ndar de C (`string.h`, `stdlib.h`)  

---

## Instalaci�n

1. Clona el repositorio  
   ```bash
   git clone https://github.com/0xRael/zapateria.git
   cd zapateria
   ```  
c:/Users/user/OneDrive/Documentos/GitHub/zapatos
2. Compila la aplicaci�n  
   ```bash
   gcc  main.c -o main `pkg-config --cflags --libs gtk+-3.0`
   ```  

3. Aseg�rate de tener el archivo `interfaz.glade` en el mismo directorio que el ejecutable.

---

## Estructura del proyecto

- `main.c`  
- `interfaz.glade` 
- `README.md`  

---

## Uso

1. Ejecuta la aplicaci�n  
   ```bash
   ./main
   ```  

2. Navega por el men� lateral para acceder a:  
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

## Dise�o de la interfaz

- **GtkApplicationWindow** con HeaderBar personalizado  
- Panel de navegaci�n vertical con `GtkButton` + icono  
- �rea de contenido con `GtkStack` para cada secci�n  
- `GtkTreeView` + `GtkListStore` en Inventario y Facturas  
- `GtkFlowBox` para listar productos en miniaturas  
- Formularios en `GtkGrid` para Clientes y Trabajadores  
- Di�logos dise�ados en Glade para Agregar y Editar  

---

## Funcionalidades destacadas

- Carga y refresco de datos desde un arreglo de structs  
- Filtros en tiempo real por c�digo o nombre  
- Ordenamiento y selecci�n de filas en tablas  
- Edici�n directa mediante di�logos modal en Glade  
- Eliminaci�n sincronizada entre `ListStore` y arreglo en memoria  

---

## Extensiones posibles

- Persistencia en archivo JSON o base de datos SQLite  
- Exportar facturas a CSV o PDF  
- Modo oscuro con CSS personalizado  
- Reportes de stock m�nimo y alertas visuales  
- Integraci�n con API de proveedores para actualizaci�n autom�tica  

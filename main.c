#include <stdio.h>
#include <Windows.h>

struct Producto {
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
};

struct Cliente {
	char nombre[50];
	char apellido[50];
	char telefono[50];
	char direccion[50];
	char cedula[50];
};

struct Trabajador {
	char nombre[50];
	char apellido[50];
	char cedula[50];
	char telefono[50];
	char direccion[50];
	int numero;
};

// cant = cantidad
// tasa = precio de dolar con respecto al bolivar segun BCV
float dolar_a_bs(float cant, float tasa){
	return cant * tasa;
}

// cant = cantidad
// tasa = precio de dolar con respecto al peso colombiano segun Google
float dolar_a_cop(float cant, float tasa){
	return cant * tasa;
}

// cant = cantidad
// tasa_dolar = precio del dolar con respecto al bolivar segun BCV
// tasa_euro = precio del euro con respecto al bolivar segun BCV
float dolar_a_euro(float cant, float tasa_dolar, float tasa_euro){
	float bs = dolar_a_bs(cant, tasa_dolar);
	return bs / tasa_euro;
}

void mostrar(struct Producto zapato, float tasa_bcv, float tasa_euro, float tasa_cop){
	printf("%s - %s (%s):\n", zapato.codigo, zapato.nombre, zapato.categoria);
	// [0101203] Zapato PRUEBA (De vestir)
	printf("\t%s\n", zapato.descripcion);
	// 	El zapato que necesitas para lorem ipsum dolor sit anem concepteur adipsit elit.
	printf("- Precio Dolar: %f\n", zapato.precio_venta);
	printf("- Precio Euro: %f\n", dolar_a_euro(zapato.precio_venta, tasa_bcv, tasa_euro));
	printf("- Precio Bolivares: %f\n", dolar_a_bs(zapato.precio_venta, tasa_bcv));
	printf("- Precio Peso Colombiano: %f\n", dolar_a_cop(zapato.precio_venta, tasa_cop));
	system("pause");
}

int main() {
	float tasa_bcv=102.16, tasa_euro=117.9, tasa_cop = 4102.0;
	
	int n = 0;
	do {
        system("cls");
        printf("Menu de opciones\n");
        printf("1. Productos\n");
        //printf("Opcion 2\n");
        //printf("Opcion 3\n");
        //printf("Opcion 4\n");
        printf("0. (Salir)\n");
        
        scanf("%d", &n);
        
        if (n == 1){
        	struct Producto zapato;
        	
        	printf("CREACION DE PRODUCTOS\n\n");
        	printf("Ingrese el codigo>>");
        	scanf(" %[^\n]", &zapato.cantidad);
        	printf("Ingrese el nombre>>");
        	scanf(" %[^\n]", &zapato.nombre);
        	printf("Ingrese la descripcion>>");
        	scanf(" %[^\n]", &zapato.descripcion);
        	printf("Ingrese la categoria>>");
        	scanf(" %[^\n]", &zapato.categoria);
        	printf("Ingrese la cantidad>>");
        	scanf("%d", &zapato.cantidad);
        	printf("Ingrese el precio compra>>");
        	scanf("%f", &zapato.precio_compra);
        	printf("Ingrese el precio venta>>");
        	scanf("%f", &zapato.precio_venta);
        	printf("\n\n");
        	
        	mostrar(zapato, tasa_bcv, tasa_euro, tasa_cop);
		}
    } while (n != 0);
}
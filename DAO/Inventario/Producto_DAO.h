#ifndef PRODUCTO_DAO_H
#define PRODUCTO_DAO_H

#include "../../BO/Inventario/Producto_BO.h"

class ProductoDAO {
public:
    static int Registrar(int codigo, const char nombre[], int cantidad, float precio);
    static void Listar(void);
    static int Consultar(const char id[]);
    static int Obtener(const char id[], Producto *prod);
    static int Existe_Codigo(int codigo);
    static int Actualizar(const char id[], const char nombre[], int cantidad, float precio);
    static int Eliminar(const char id[]);
    static int Mover_Stock(const char id[], int cantidad, int entrada);
};

// Funciones compatibles con codigo original
int Producto_DAO_Registrar(int codigo, char nombre[], int cantidad, float precio);
void Producto_DAO_Listar(void);
int Producto_DAO_Consultar(char id[]);
int Producto_DAO_Obtener(const char id[], Producto *prod);
int Producto_DAO_Existe_Codigo(int codigo);
int Producto_DAO_Actualizar(char id[], char nombre[], int cantidad, float precio);
int Producto_DAO_Eliminar(char id[]);
int Producto_DAO_Mover_Stock(char id[], int cantidad, int entrada);

#endif // PRODUCTO_DAO_H

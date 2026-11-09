#ifndef INVENTARIO_CTRL_H
#define INVENTARIO_CTRL_H

#include "../../BO/Inventario/Producto_BO.h"

int Inventario_CTRL_Registrar(int codigo, const char nombre[], int cantidad, float precio);
void Inventario_CTRL_Listar(void);
int Inventario_CTRL_Consultar(const char criterio[]);
int Inventario_CTRL_Obtener(const char criterio[], Producto *prod);
int Inventario_CTRL_Existe_Codigo(int codigo);
int Inventario_CTRL_Actualizar(const char id[], const char nombre[], int cantidad, float precio);
int Inventario_CTRL_Eliminar(const char id[]);
int Inventario_CTRL_Mover_Stock(const char id[], int cantidad, int entrada);

#endif // INVENTARIO_CTRL_H

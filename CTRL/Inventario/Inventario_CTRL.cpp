#include "Inventario_CTRL.h"
#include "../../DAO/Inventario/Producto_DAO.h"

int Inventario_CTRL_Registrar(int codigo, const char nombre[], int cantidad, float precio)
{
    return Producto_DAO_Registrar(codigo, (char *)nombre, cantidad, precio);
}

void Inventario_CTRL_Listar(void)
{
    Producto_DAO_Listar();
}

int Inventario_CTRL_Consultar(const char criterio[])
{
    return Producto_DAO_Consultar((char *)criterio);
}

int Inventario_CTRL_Obtener(const char criterio[], Producto *prod)
{
    return Producto_DAO_Obtener(criterio, prod);
}

int Inventario_CTRL_Existe_Codigo(int codigo)
{
    return Producto_DAO_Existe_Codigo(codigo);
}

int Inventario_CTRL_Actualizar(const char id[], const char nombre[], int cantidad, float precio)
{
    return Producto_DAO_Actualizar((char *)id, (char *)nombre, cantidad, precio);
}

int Inventario_CTRL_Eliminar(const char id[])
{
    return Producto_DAO_Eliminar((char *)id);
}

int Inventario_CTRL_Mover_Stock(const char id[], int cantidad, int entrada)
{
    return Producto_DAO_Mover_Stock((char *)id, cantidad, entrada);
}

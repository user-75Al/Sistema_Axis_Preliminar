#ifndef PERMISOS_BO_H
#define PERMISOS_BO_H

#include "Usuario_BO.h"

/*
 * Matriz de permisos del sistema.
 * Toda decision de acceso debe pasar por estas funciones, de modo que
 * la jerarquia no dependa unicamente de lo que muestre la interfaz.
 */
int Permisos_PuedeVerInventario(int rol);
int Permisos_PuedeModificarInventario(int rol);

int Permisos_PuedeAccederRRHH(int rol);
int Permisos_PuedeConsultarOtrosEmpleados(int rol);
int Permisos_PuedeRegistrarEmpleado(int rol);
int Permisos_PuedeActualizarEmpleado(int rol);
int Permisos_PuedeEliminarEmpleado(int rol);
int Permisos_PuedeRegistrarRostro(int rol);
int Permisos_PuedeRegistrarAsistencia(int rol);

int Permisos_PuedeAuditar(int rol);
int Permisos_PuedeGestionarUsuarios(int rol);

const char *Permisos_DescripcionRol(int rol);

#endif // PERMISOS_BO_H

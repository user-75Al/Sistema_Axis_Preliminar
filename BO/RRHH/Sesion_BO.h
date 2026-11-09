#ifndef SESION_BO_H
#define SESION_BO_H

#include "Usuario_BO.h"

int Sesion_Activar(const Usuario *usuario);
void Sesion_Cerrar(void);
int Sesion_EstaActiva(void);

const Usuario *Sesion_Actual(void);
const char *Sesion_UsuarioNombre(void);
const char *Sesion_RolNombre(void);
const char *Sesion_IdEmpleado(void);

int Sesion_Rol(void);
int Sesion_TieneNivel(int nivelRequerido);
int Sesion_EsAdministrador(void);

#endif // SESION_BO_H

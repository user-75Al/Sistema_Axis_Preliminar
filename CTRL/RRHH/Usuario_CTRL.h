#ifndef USUARIO_CTRL_H
#define USUARIO_CTRL_H

#include "../../BO/RRHH/Usuario_BO.h"

int Usuario_CTRL_IniciarSesion(const char usuario[], const char clave[], Usuario *salida);
void Usuario_CTRL_CerrarSesion(void);
int Usuario_CTRL_Listar(void);

#endif // USUARIO_CTRL_H

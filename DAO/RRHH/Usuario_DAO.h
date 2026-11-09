#ifndef USUARIO_DAO_H
#define USUARIO_DAO_H

#include "../../BO/RRHH/Usuario_BO.h"

int Usuario_DAO_Autenticar(const char usuario[], const char clave[], Usuario *salida);
int Usuario_DAO_Existe(const char usuario[]);
int Usuario_DAO_Listar(void);

#endif // USUARIO_DAO_H

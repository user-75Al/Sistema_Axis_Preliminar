#ifndef EMPLEADO_CTRL_H
#define EMPLEADO_CTRL_H

#include "../../BO/RRHH/Empleado_BO.h"

int Empleado_CTRL_Existe_Numero(const char numero[]);
int Empleado_CTRL_Registrar(const char numero_Empleado[], const char nombre[],
                            const char cargo[], const char telefono[],
                            const char correo[], const char fecha_Nacimiento[],
                            float salario_Diario, float jornada_Horas);
int Empleado_CTRL_Listar(void);
int Empleado_CTRL_Consultar(const char criterio[]);
int Empleado_CTRL_Obtener(const char criterio[], Empleado *emp);
int Empleado_CTRL_Actualizar(const char idBuscado[], const char numero_Empleado[],
                             const char nombre[], const char cargo[],
                             const char telefono[], const char correo[],
                             const char fecha_Nacimiento[],
                             float salario_Diario, float jornada_Horas);
int Empleado_CTRL_Eliminar(const char idBuscado[]);

#endif // EMPLEADO_CTRL_H

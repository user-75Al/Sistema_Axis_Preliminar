#ifndef EMPLEADO_DAO_H
#define EMPLEADO_DAO_H

#include "../../BO/RRHH/Empleado_BO.h"

class EmpleadoDAO {
public:
    static int Existe_Numero(const char numeroBuscado[]);
    static int Registrar(const char numero_Empleado[], const char nombre[], const char cargo[],
                         const char telefono[], const char correo[],
                         const char fecha_Nacimiento[], float salario_Diario,
                         float jornada_Horas);
    static void Listar(void);
    static int Consultar(const char criterio[]);
    static int Obtener(const char criterio[], Empleado *emp);
    static int Actualizar(const char idBuscado[], const char numero_Empleado[],
                          const char nombre[], const char cargo[], const char telefono[],
                          const char correo[], const char fecha_Nacimiento[],
                          float salario_Diario, float jornada_Horas);
    static int Eliminar(const char idBuscado[]);
};

// Funciones compatibles con codigo original
int Empleado_DAO_Existe_Numero(char numeroBuscado[]);
int Empleado_DAO_Registrar(char numero_Empleado[], char nombre[], char cargo[],
                           char telefono[], char correo[],
                           char fecha_Nacimiento[], float salario_Diario,
                           float jornada_Horas);
void Empleado_DAO_Listar(void);
int Empleado_DAO_Consultar(char criterio[]);
int Empleado_DAO_Obtener(const char criterio[], Empleado *emp);
int Empleado_DAO_Actualizar(char idBuscado[], char numero_Empleado[],
                            char nombre[], char cargo[], char telefono[],
                            char correo[], char fecha_Nacimiento[],
                            float salario_Diario, float jornada_Horas);
int Empleado_DAO_Eliminar(char idBuscado[]);

#endif // EMPLEADO_DAO_H

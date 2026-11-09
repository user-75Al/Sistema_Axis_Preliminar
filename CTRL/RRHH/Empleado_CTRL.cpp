#include <cstring>
#include "Empleado_CTRL.h"
#include "../../DAO/RRHH/Empleado_DAO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"

static const char *ajustarCriterioPorRol(const char criterio[], char propio[], int maxTam)
{
    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        const char *idVinculado = Sesion_IdEmpleado();

        if (idVinculado == NULL || idVinculado[0] == '\0')
            return NULL;

        strncpy(propio, idVinculado, maxTam - 1);
        propio[maxTam - 1] = '\0';
        return propio;
    }

    return criterio;
}

int Empleado_CTRL_Existe_Numero(const char numero[])
{
    return EmpleadoDAO::Existe_Numero(numero);
}

int Empleado_CTRL_Registrar(const char numero_Empleado[], const char nombre[],
                            const char cargo[], const char telefono[],
                            const char correo[], const char fecha_Nacimiento[],
                            float salario_Diario, float jornada_Horas)
{
    if (!Permisos_PuedeRegistrarEmpleado(Sesion_Rol()))
        return -3;

    return EmpleadoDAO::Registrar(numero_Empleado, nombre, cargo, telefono,
                                  correo, fecha_Nacimiento, salario_Diario,
                                  jornada_Horas);
}

int Empleado_CTRL_Listar(void)
{
    if (!Permisos_PuedeConsultarOtrosEmpleados(Sesion_Rol()))
        return -3;

    EmpleadoDAO::Listar();
    return 1;
}

int Empleado_CTRL_Consultar(const char criterio[])
{
    char criterioPropio[20];
    const char *criterioFinal;

    criterioFinal = ajustarCriterioPorRol(criterio, criterioPropio, sizeof(criterioPropio));

    if (criterioFinal == NULL)
        return -3;

    if (Sesion_Rol() != ROL_EMPLEADO &&
        !Permisos_PuedeConsultarOtrosEmpleados(Sesion_Rol()))
        return -3;

    return EmpleadoDAO::Consultar(criterioFinal);
}

int Empleado_CTRL_Obtener(const char criterio[], Empleado *emp)
{
    char criterioPropio[20];
    const char *criterioFinal;

    if (emp == NULL)
        return 0;

    criterioFinal = ajustarCriterioPorRol(criterio, criterioPropio, sizeof(criterioPropio));

    if (criterioFinal == NULL)
        return -3;

    if (Sesion_Rol() != ROL_EMPLEADO &&
        !Permisos_PuedeConsultarOtrosEmpleados(Sesion_Rol()))
        return -3;

    return EmpleadoDAO::Obtener(criterioFinal, emp);
}

int Empleado_CTRL_Actualizar(const char idBuscado[], const char numero_Empleado[],
                             const char nombre[], const char cargo[],
                             const char telefono[], const char correo[],
                             const char fecha_Nacimiento[],
                             float salario_Diario, float jornada_Horas)
{
    if (!Permisos_PuedeActualizarEmpleado(Sesion_Rol()))
        return -3;

    return EmpleadoDAO::Actualizar(idBuscado, numero_Empleado, nombre, cargo,
                                   telefono, correo, fecha_Nacimiento,
                                   salario_Diario, jornada_Horas);
}

int Empleado_CTRL_Eliminar(const char idBuscado[])
{
    if (!Permisos_PuedeEliminarEmpleado(Sesion_Rol()))
        return -3;

    return EmpleadoDAO::Eliminar(idBuscado);
}

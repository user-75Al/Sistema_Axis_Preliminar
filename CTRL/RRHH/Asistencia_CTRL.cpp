#include <cstdio>
#include <cstring>
#include "Asistencia_CTRL.h"
#include "../../DAO/RRHH/Asistencia_DAO.h"
#include "../../DAO/RRHH/Empleado_DAO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"
#include "../../BO/Validaciones_BO.h"

int Asistencia_CTRL_Registrar(const char idEmpleado[], char tipoRegistrado[], int maxTam)
{
    Empleado empleado;
    char ultimoEstado[12];
    char horaActual[20];
    char log[180];
    const char *tipo;
    const char *idFinal = idEmpleado;

    if (idEmpleado == NULL || tipoRegistrado == NULL || maxTam <= 0)
        return 0;

    if (!Permisos_PuedeRegistrarAsistencia(Sesion_Rol()))
        return -2;

    /* Un empleado solo puede registrar su propia asistencia. */
    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        const char *idVinculado = Sesion_IdEmpleado();

        if (idVinculado == NULL || idVinculado[0] == '\0')
            return -2;

        idFinal = idVinculado;
    }

    if (!EmpleadoDAO::Obtener(idFinal, &empleado))
        return -1;

    if (Asistencia_DAO_UltimoEstado(empleado.id, ultimoEstado, sizeof(ultimoEstado)) &&
        strcmp(ultimoEstado, "ENTRADA") == 0)
        tipo = "SALIDA";
    else
        tipo = "ENTRADA";

    if (!Asistencia_DAO_Registrar(empleado.id, tipo))
        return 0;

    strncpy(tipoRegistrado, tipo, maxTam - 1);
    tipoRegistrado[maxTam - 1] = '\0';

    Asistencia_Obtener_Hora_Actual(horaActual, sizeof(horaActual));

    sprintf(log, "%s ASISTENCIA: [%s] N. Empleado: %s, %s, Hora: %s",
            tipo, empleado.id, empleado.numero_Empleado, empleado.nombre, horaActual);
    Guardar_Fecha_Hora(log);

    return 1;
}

int Asistencia_CTRL_UltimoEstado(const char idEmpleado[], char estado[], int maxTam)
{
    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        const char *idVinculado = Sesion_IdEmpleado();

        if (idVinculado == NULL || idVinculado[0] == '\0')
            return 0;

        return Asistencia_DAO_UltimoEstado(idVinculado, estado, maxTam);
    }

    return Asistencia_DAO_UltimoEstado(idEmpleado, estado, maxTam);
}

int Asistencia_CTRL_Listar(void)
{
    if (!Permisos_PuedeConsultarOtrosEmpleados(Sesion_Rol()))
        return -3;

    Asistencia_DAO_Listar();
    return 1;
}

#include <cstring>
#include "Usuario_BO.h"
#include "../Compatibilidad.h"

const char *Usuario_NombreRol(int rol)
{
    switch (rol)
    {
        case ROL_ADMINISTRADOR:    return "ADMINISTRADOR";
        case ROL_GERENTE_RRHH:     return "GERENTE_RRHH";
        case ROL_SUPERVISOR_RRHH:  return "SUPERVISOR_RRHH";
        case ROL_EMPLEADO:         return "EMPLEADO";
        default:                   return "INVITADO";
    }
}

int Usuario_RolDesdeTexto(const char *texto)
{
    if (texto == NULL)
        return ROL_INVITADO;

    if (axis_stricmp(texto, "ADMINISTRADOR") == 0 || axis_stricmp(texto, "ADMIN") == 0)
        return ROL_ADMINISTRADOR;

    if (axis_stricmp(texto, "GERENTE_RRHH") == 0 || axis_stricmp(texto, "GERENTE") == 0)
        return ROL_GERENTE_RRHH;

    if (axis_stricmp(texto, "SUPERVISOR_RRHH") == 0 || axis_stricmp(texto, "SUPERVISOR") == 0)
        return ROL_SUPERVISOR_RRHH;

    if (axis_stricmp(texto, "EMPLEADO") == 0)
        return ROL_EMPLEADO;

    return ROL_INVITADO;
}

int Usuario_NivelRol(int rol)
{
    if (rol < ROL_INVITADO)
        return ROL_INVITADO;

    if (rol > ROL_ADMINISTRADOR)
        return ROL_ADMINISTRADOR;

    return rol;
}

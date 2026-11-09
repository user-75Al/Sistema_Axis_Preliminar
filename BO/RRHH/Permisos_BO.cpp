#include "Permisos_BO.h"

int Permisos_PuedeVerInventario(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_EMPLEADO;
}

int Permisos_PuedeModificarInventario(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_EMPLEADO;
}

int Permisos_PuedeAccederRRHH(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_EMPLEADO;
}

int Permisos_PuedeConsultarOtrosEmpleados(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_SUPERVISOR_RRHH;
}

int Permisos_PuedeRegistrarEmpleado(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_SUPERVISOR_RRHH;
}

int Permisos_PuedeActualizarEmpleado(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_SUPERVISOR_RRHH;
}

int Permisos_PuedeEliminarEmpleado(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_GERENTE_RRHH;
}

int Permisos_PuedeRegistrarRostro(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_SUPERVISOR_RRHH;
}

int Permisos_PuedeRegistrarAsistencia(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_EMPLEADO;
}

int Permisos_PuedeAuditar(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_ADMINISTRADOR;
}

int Permisos_PuedeGestionarUsuarios(int rol)
{
    return Usuario_NivelRol(rol) >= ROL_ADMINISTRADOR;
}

const char *Permisos_DescripcionRol(int rol)
{
    switch (Usuario_NivelRol(rol))
    {
        case ROL_ADMINISTRADOR:
            return "Acceso total. Administra usuarios, auditoria, RRHH e inventario.";
        case ROL_GERENTE_RRHH:
            return "Gestion completa de RRHH: registrar, consultar, actualizar y eliminar.";
        case ROL_SUPERVISOR_RRHH:
            return "RRHH operativo: registrar, consultar y actualizar empleados (sin eliminar).";
        case ROL_EMPLEADO:
            return "Acceso personal: consulta su ficha y registra su asistencia facial.";
        default:
            return "Sin acceso al sistema.";
    }
}

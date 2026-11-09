#include <cstdio>
#include "Auditoria_CTRL.h"
#include "../../DAO/Admin/Auditoria_DAO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"
#include "../../BO/Validaciones_BO.h"

static int verificarAccesoAuditoria(void)
{
    if (Permisos_PuedeAuditar(Sesion_Rol()))
        return 1;

    printf("\n[Acceso denegado] La auditoria solo esta disponible para el rol ADMINISTRADOR.\n");
    Guardar_Fecha_Hora("AUDITORIA: intento de acceso denegado");
    return 0;
}

int Auditoria_CTRL_Listar(void)
{
    if (!verificarAccesoAuditoria())
        return -3;

    return Auditoria_DAO_Listar();
}

int Auditoria_CTRL_Buscar(const char criterio[])
{
    if (!verificarAccesoAuditoria())
        return -3;

    return Auditoria_DAO_Buscar(criterio);
}

int Auditoria_CTRL_FiltrarPorFecha(const char desde[], const char hasta[])
{
    if (!verificarAccesoAuditoria())
        return -3;

    return Auditoria_DAO_FiltrarPorFecha(desde, hasta);
}

int Auditoria_CTRL_Resumen(void)
{
    if (!verificarAccesoAuditoria())
        return -3;

    Auditoria_DAO_Resumen();
    return 1;
}

int Auditoria_CTRL_Contar(void)
{
    if (!verificarAccesoAuditoria())
        return -3;

    return Auditoria_DAO_Contar();
}

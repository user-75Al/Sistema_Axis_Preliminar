#include <cstring>
#include "Sesion_BO.h"

static Usuario g_usuarioActual;
static int g_sesionActiva = 0;

int Sesion_Activar(const Usuario *usuario)
{
    if (usuario == NULL)
        return 0;

    g_usuarioActual = *usuario;
    g_sesionActiva = 1;
    return 1;
}

void Sesion_Cerrar(void)
{
    memset(&g_usuarioActual, 0, sizeof(g_usuarioActual));
    g_sesionActiva = 0;
}

int Sesion_EstaActiva(void)
{
    return g_sesionActiva;
}

const Usuario *Sesion_Actual(void)
{
    return g_sesionActiva ? &g_usuarioActual : NULL;
}

const char *Sesion_UsuarioNombre(void)
{
    if (!g_sesionActiva || g_usuarioActual.usuario[0] == '\0')
        return "SISTEMA";

    return g_usuarioActual.usuario;
}

const char *Sesion_RolNombre(void)
{
    if (!g_sesionActiva)
        return "INVITADO";

    return Usuario_NombreRol(g_usuarioActual.rol);
}

const char *Sesion_IdEmpleado(void)
{
    if (!g_sesionActiva)
        return "";

    return g_usuarioActual.id_Empleado;
}

int Sesion_Rol(void)
{
    if (!g_sesionActiva)
        return ROL_INVITADO;

    return g_usuarioActual.rol;
}

int Sesion_TieneNivel(int nivelRequerido)
{
    return g_sesionActiva && Usuario_NivelRol(g_usuarioActual.rol) >= nivelRequerido;
}

int Sesion_EsAdministrador(void)
{
    return Sesion_TieneNivel(ROL_ADMINISTRADOR);
}

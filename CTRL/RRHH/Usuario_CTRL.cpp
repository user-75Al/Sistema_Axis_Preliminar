#include <cstdio>
#include "Usuario_CTRL.h"
#include "../../DAO/RRHH/Usuario_DAO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"
#include "../../BO/Validaciones_BO.h"

int Usuario_CTRL_IniciarSesion(const char usuario[], const char clave[], Usuario *salida)
{
    Usuario autenticado;
    char log[120];
    int resultado;

    if (usuario == NULL || clave == NULL || salida == NULL)
        return 0;

    resultado = Usuario_DAO_Autenticar(usuario, clave, &autenticado);

    if (resultado == 1)
    {
        if (!Sesion_Activar(&autenticado))
            return 0;

        *salida = autenticado;

        sprintf(log, "ACCESO: inicio de sesion (%s)", Usuario_NombreRol(autenticado.rol));
        Guardar_Fecha_Hora(log);
        return 1;
    }

    sprintf(log, "ACCESO FALLIDO: usuario '%s'", usuario);
    Guardar_Fecha_Hora(log);

    return resultado;
}

void Usuario_CTRL_CerrarSesion(void)
{
    if (Sesion_EstaActiva())
        Guardar_Fecha_Hora("ACCESO: cierre de sesion");

    Sesion_Cerrar();
}

int Usuario_CTRL_Listar(void)
{
    if (!Permisos_PuedeGestionarUsuarios(Sesion_Rol()))
        return -3;

    return Usuario_DAO_Listar();
}

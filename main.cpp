#include <cstdio>
#include <windows.h>

#include "BO/Validaciones_BO.h"
#include "BO/RRHH/Sesion_BO.h"
#include "BO/RRHH/Permisos_BO.h"

#include "CTRL/RRHH/Usuario_CTRL.h"

#include "GUI/Login/Login_GUI.h"
#include "GUI/Inventario/Inventario_GUI.h"
#include "GUI/RRHH/Empleado_GUI.h"
#include "GUI/Admin/Admin_GUI.h"

static void Mostrar_Menu_Principal(void)
{
    printf("\n");
    printf("==================================================\n");
    printf("                      AXIS                        \n");
    printf("==================================================\n");
    printf("  Usuario: %-20.20s Rol: %s\n",
           Sesion_UsuarioNombre(), Sesion_RolNombre());
    printf("  ");
    Mostrar_Fecha_Hora();
    printf("--------------------------------------------------\n");
    printf("              SISTEMA DE GESTION INTEGRAL         \n");
    printf("--------------------------------------------------\n");
    printf("  [1]  Modulo de Inventario\n");
    printf("  [2]  Modulo de Recursos Humanos\n");

    if (Sesion_EsAdministrador())
        printf("  [3]  Administracion y Auditoria\n");

    printf("--------------------------------------------------\n");
    printf("  [8]  Cerrar sesion / cambiar de usuario\n");
    printf("  [0]  Salir de AXIS\n");
    printf("==================================================\n");
    printf("  %s\n", Permisos_DescripcionRol(Sesion_Rol()));
    printf("==================================================\n");
    printf("  Seleccione una opcion: ");
}

static void Mostrar_Despedida(void)
{
    printf("\n==================================================\n");
    printf("  Gracias por utilizar el Sistema AXIS.\n");
    printf("  Hasta pronto!\n");
    printf("==================================================\n");
}

int main(void)
{
    int opcion = -1;
    int estado;
    char linea[64];

    if (!Login_Mostrar())
    {
        clearScreen();
        Mostrar_Despedida();
        Sleep(1500);
        return 0;
    }

    do
    {
        clearScreen();
        Mostrar_Menu_Principal();

        if (fgets(linea, sizeof(linea), stdin) == NULL)
            opcion = -1;
        else if (sscanf(linea, "%d", &opcion) != 1)
            opcion = -1;

        switch (opcion)
        {
            case 1:
                if (!Permisos_PuedeVerInventario(Sesion_Rol()))
                {
                    printf("\n[Acceso denegado] Su rol no tiene acceso al inventario.\n");
                    Sleep(1800);
                    break;
                }

                clearScreen();
                printf("Cargando modulo de Inventario...\n");
                Sleep(1000);

                estado = Inventario_Mostrar_Menu();
                if (estado == 0)
                    opcion = 0;
                break;

            case 2:
                if (!Permisos_PuedeAccederRRHH(Sesion_Rol()))
                {
                    printf("\n[Acceso denegado] Su rol no tiene acceso a Recursos Humanos.\n");
                    Sleep(1800);
                    break;
                }

                clearScreen();
                printf("Cargando modulo de Recursos Humanos...\n");
                Sleep(1000);

                estado = Empleado_Mostrar_Menu();
                if (estado == 0)
                    opcion = 0;
                break;

            case 3:
                clearScreen();
                estado = Admin_Mostrar_Menu();
                if (estado == 0)
                    opcion = 0;
                break;

            case 8:
                clearScreen();
                printf("Cerrando sesion de %s...\n", Sesion_UsuarioNombre());
                Usuario_CTRL_CerrarSesion();
                Sleep(1200);

                if (!Login_Mostrar())
                    opcion = 0;
                break;

            case 0:
                clearScreen();
                printf("Cerrando AXIS...\n");
                Sleep(1500);
                break;

            default:
                printf("\nOpcion no valida, intente de nuevo.\n");
                Sleep(1500);
                break;
        }

    } while (opcion != 0);

    Usuario_CTRL_CerrarSesion();

    clearScreen();
    Mostrar_Despedida();
    Sleep(2000);

    return 0;
}

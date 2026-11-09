#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Admin_GUI.h"
#include "../../CTRL/Admin/Auditoria_CTRL.h"
#include "../../CTRL/RRHH/Usuario_CTRL.h"
#include "../../BO/Validaciones_BO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"

static void leerFechaAdmin(const char *mensaje, char salida[], int maxTam)
{
    while (1)
    {
        Leer_Texto_Simple(mensaje, salida, maxTam);
        if (Validar_Fecha(salida))
            return;
        printf("  [Error] Fecha invalida. Use el formato DD/MM/AAAA.\n");
    }
}

static int mostrarSubmenuAuditoria(void)
{
    int opcion;
    char linea[64];

    do
    {
        clearScreen();
        printf("\n");
        printf("==================================================\n");
        printf("              AXIS | AUDITORIA                    \n");
        printf("==================================================\n");
        printf("  ");
        Mostrar_Fecha_Hora();
        printf("--------------------------------------------------\n");
        printf("  [1]  Ver historial completo de movimientos\n");
        printf("  [2]  Buscar movimientos (ID, modulo, accion)\n");
        printf("  [3]  Resumen estadistico\n");
        printf("  [4]  Filtrar por rango de fechas\n");
        printf("--------------------------------------------------\n");
        printf("  [5]  Regresar al menu de administracion\n");
        printf("==================================================\n");
        printf("  Seleccione una opcion: ");

        if (fgets(linea, sizeof(linea), stdin) == NULL)
            opcion = -1;
        else if (sscanf(linea, "%d", &opcion) != 1)
            opcion = -1;

        switch (opcion)
        {
            case 1:
                clearScreen();
                Auditoria_CTRL_Listar();
                printf("\n");
                system("pause");
                break;

            case 2:
            {
                char criterio[100];
                clearScreen();
                printf("=== BUSCAR EN AUDITORIA ===\n\n");
                Leer_Texto_Simple("Texto a buscar: ", criterio, sizeof(criterio));
                clearScreen();
                Auditoria_CTRL_Buscar(criterio);
                printf("\n");
                system("pause");
                break;
            }

            case 3:
                clearScreen();
                Auditoria_CTRL_Resumen();
                printf("\n");
                system("pause");
                break;

            case 4:
            {
                char desde[12], hasta[12];
                clearScreen();
                printf("=== AUDITORIA POR RANGO DE FECHAS ===\n\n");
                leerFechaAdmin("Fecha inicial (DD/MM/AAAA): ", desde, sizeof(desde));
                leerFechaAdmin("Fecha final   (DD/MM/AAAA): ", hasta, sizeof(hasta));
                clearScreen();
                Auditoria_CTRL_FiltrarPorFecha(desde, hasta);
                printf("\n");
                system("pause");
                break;
            }

            case 5:
                return 1;

            default:
                printf("\nOpcion no valida.\n");
                Sleep(1500);
                break;
        }

    } while (opcion != 5);

    return 1;
}

int Admin_Mostrar_Menu(void)
{
    int opcion;
    char linea[64];

    if (!Permisos_PuedeAuditar(Sesion_Rol()))
    {
        clearScreen();
        printf("\n[Acceso denegado] El panel de administracion solo esta disponible\n");
        printf("para el rol ADMINISTRADOR.\n");
        Guardar_Fecha_Hora("ADMIN: intento de acceso denegado");
        Sleep(2500);
        return 1;
    }

    do
    {
        clearScreen();
        printf("\n");
        printf("==================================================\n");
        printf("           AXIS | ADMINISTRACION                  \n");
        printf("==================================================\n");
        printf("  Usuario: %-20.20s Rol: %s\n",
               Sesion_UsuarioNombre(), Sesion_RolNombre());
        printf("  ");
        Mostrar_Fecha_Hora();
        printf("--------------------------------------------------\n");
        printf("  [1]  Auditoria del sistema\n");
        printf("  [2]  Usuarios y jerarquia de acceso\n");
        printf("--------------------------------------------------\n");
        printf("  [9]  Regresar al menu principal\n");
        printf("  [0]  Cerrar AXIS\n");
        printf("==================================================\n");
        printf("  Seleccione una opcion: ");

        if (fgets(linea, sizeof(linea), stdin) == NULL)
            opcion = -1;
        else if (sscanf(linea, "%d", &opcion) != 1)
            opcion = -1;

        switch (opcion)
        {
            case 1:
                mostrarSubmenuAuditoria();
                break;

            case 2:
                clearScreen();
                if (Usuario_CTRL_Listar() == -3)
                    printf("\n[Acceso denegado] Solo el administrador puede ver los usuarios.\n");
                printf("\n");
                system("pause");
                break;

            case 9:
                return 1;

            case 0:
                clearScreen();
                printf("Cerrando AXIS...\n");
                Sleep(2000);
                return 0;

            default:
                printf("\nOpcion no valida.\n");
                Sleep(1500);
                break;
        }

    } while (opcion != 0);

    return 0;
}

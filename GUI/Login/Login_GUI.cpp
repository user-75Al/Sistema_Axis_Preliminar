#include <cstdio>
#include <windows.h>
#include "Login_GUI.h"
#include "../../CTRL/RRHH/Usuario_CTRL.h"
#include "../../BO/Validaciones_BO.h"
#include "../../BO/RRHH/Sesion_BO.h"

int Login_Mostrar(void)
{
    char usuario[30];
    char clave[40];
    Usuario autenticado;
    int intentos = 0;
    int resultado;

    while (intentos < 3)
    {
        clearScreen();

        printf("\n");
        printf("==================================================\n");
        printf("                   SISTEMA AXIS                   \n");
        printf("==================================================\n");
        printf("  ");
        Mostrar_Fecha_Hora();
        printf("--------------------------------------------------\n");
        printf("              CONTROL DE ACCESO                   \n");
        printf("--------------------------------------------------\n");
        printf("  Jerarquia de acceso:\n");
        printf("   ADMINISTRADOR  >  GERENTE_RRHH  >  SUPERVISOR_RRHH  >  EMPLEADO\n");
        printf("--------------------------------------------------\n");

        Leer_Texto_Simple("  Usuario: ", usuario, sizeof(usuario));
        Leer_Clave_Segura("  Clave: ", clave, sizeof(clave));

        resultado = Usuario_CTRL_IniciarSesion(usuario, clave, &autenticado);

        if (resultado == 1)
        {
            clearScreen();
            printf("\n==================================================\n");
            printf("  Bienvenido, %s\n", autenticado.usuario);
            printf("  Rol: %s\n", Usuario_NombreRol(autenticado.rol));
            printf("==================================================\n");
            Sleep(1500);
            return 1;
        }

        if (resultado == -1)
        {
            printf("\n[Error] No se pudo abrir Data/Usuarios.txt.\n");
            Sleep(2500);
            return 0;
        }

        intentos++;
        printf("\n[Error] Usuario o clave incorrectos. Intento %d de 3.\n", intentos);
        Sleep(1800);
    }

    clearScreen();
    printf("\nDemasiados intentos fallidos. El sistema se cerrara.\n");
    Sleep(2000);
    return 0;
}

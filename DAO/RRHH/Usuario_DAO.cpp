#include <cstdio>
#include <cstring>
#include "Usuario_DAO.h"
#include "../../BO/Compatibilidad.h"

#define RUTA_USUARIOS "Data/Usuarios.txt"

static int parsearUsuarioLinea(const char *linea, Usuario *usuario)
{
    char rolTexto[30];
    char idEmpleado[20];

    linea = axis_saltar_bom(linea);

    if (linea == NULL || usuario == NULL || linea[0] == '\n' || linea[0] == '\0')
        return 0;

    idEmpleado[0] = '\0';

    int campos = sscanf(linea, "%29[^|]|%39[^|]|%29[^|]|%19[^\r\n]",
                        usuario->usuario, usuario->clave, rolTexto, idEmpleado);

    if (campos < 3)
        return 0;

    usuario->rol = Usuario_RolDesdeTexto(rolTexto);
    if (usuario->rol == ROL_INVITADO)
        return 0;

    if (campos >= 4)
    {
        strncpy(usuario->id_Empleado, idEmpleado, sizeof(usuario->id_Empleado) - 1);
        usuario->id_Empleado[sizeof(usuario->id_Empleado) - 1] = '\0';
    }
    else
    {
        usuario->id_Empleado[0] = '\0';
    }

    return 1;
}

static void asegurarArchivoUsuarios(void)
{
    FILE *archivo = fopen(RUTA_USUARIOS, "r");
    if (archivo != NULL)
    {
        fclose(archivo);
        return;
    }

    archivo = fopen(RUTA_USUARIOS, "w");
    if (archivo == NULL)
        return;

    /*
     * Usuarios de demostracion.
     * Formato: usuario|clave|rol|id_empleado
     * En produccion las claves deben almacenarse con hash.
     */
    fprintf(archivo, "admin|admin123|ADMINISTRADOR|\n");
    fprintf(archivo, "gerente|gerente123|GERENTE_RRHH|\n");
    fprintf(archivo, "supervisor|supervisor123|SUPERVISOR_RRHH|\n");
    fprintf(archivo, "empleado|empleado123|EMPLEADO|XWL-Z43\n");

    fclose(archivo);
}

int Usuario_DAO_Autenticar(const char usuario[], const char clave[], Usuario *salida)
{
    FILE *archivo;
    char linea[256];
    Usuario leido;

    if (usuario == NULL || clave == NULL || salida == NULL)
        return 0;

    asegurarArchivoUsuarios();

    archivo = fopen(RUTA_USUARIOS, "r");
    if (archivo == NULL)
        return -1;

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearUsuarioLinea(linea, &leido))
        {
            if (strcmp(leido.usuario, usuario) == 0 &&
                strcmp(leido.clave, clave) == 0)
            {
                *salida = leido;
                fclose(archivo);
                return 1;
            }
        }
    }

    fclose(archivo);
    return 0;
}

int Usuario_DAO_Existe(const char usuario[])
{
    FILE *archivo;
    char linea[256];
    Usuario leido;

    if (usuario == NULL)
        return 0;

    asegurarArchivoUsuarios();

    archivo = fopen(RUTA_USUARIOS, "r");
    if (archivo == NULL)
        return 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearUsuarioLinea(linea, &leido) &&
            axis_stricmp(leido.usuario, usuario) == 0)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int Usuario_DAO_Listar(void)
{
    FILE *archivo;
    char linea[256];
    Usuario leido;
    int total = 0;

    asegurarArchivoUsuarios();

    archivo = fopen(RUTA_USUARIOS, "r");
    if (archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo de usuarios.\n");
        return 0;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("                         AXIS | USUARIOS DEL SISTEMA\n");
    printf("================================================================================\n");
    printf(" USUARIO              ROL                    EMPLEADO VINCULADO\n");
    printf("--------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearUsuarioLinea(linea, &leido))
        {
            total++;
            printf(" %-20.20s %-22.22s %-18.18s\n",
                   leido.usuario,
                   Usuario_NombreRol(leido.rol),
                   leido.id_Empleado[0] != '\0' ? leido.id_Empleado : "(no aplica)");
        }
    }

    printf("--------------------------------------------------------------------------------\n");
    printf(" Total de usuarios: %d\n", total);
    printf(" Jerarquia: EMPLEADO < SUPERVISOR_RRHH < GERENTE_RRHH < ADMINISTRADOR\n");
    printf("================================================================================\n");

    fclose(archivo);
    return total;
}

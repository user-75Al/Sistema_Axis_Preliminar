#include <cstdio>
#include <cstring>
#include <cctype>
#include "Auditoria_DAO.h"
#include "../../BO/Compatibilidad.h"

static int contieneTextoCI(const char *texto, const char *busqueda)
{
    int largoTexto;
    int largoBusqueda;

    if (texto == NULL || busqueda == NULL || busqueda[0] == '\0')
        return 0;

    largoTexto = (int)strlen(texto);
    largoBusqueda = (int)strlen(busqueda);

    if (largoBusqueda > largoTexto)
        return 0;

    for (int i = 0; i <= largoTexto - largoBusqueda; i++)
    {
        if (axis_strnicmp(texto + i, busqueda, largoBusqueda) == 0)
            return 1;
    }

    return 0;
}

static int convertirFecha(const char *texto)
{
    int dia, mes, anio;

    if (texto == NULL)
        return -1;

    if (sscanf(texto, "%d/%d/%d", &dia, &mes, &anio) != 3)
        return -1;

    if (anio < 1900 || mes < 1 || mes > 12 || dia < 1 || dia > 31)
        return -1;

    return anio * 10000 + mes * 100 + dia;
}

static void imprimirEncabezado(const char *titulo)
{
    printf("\n");
    printf("================================================================================\n");
    printf("                         AXIS | AUDITORIA DEL SISTEMA\n");
    printf("================================================================================\n");
    printf(" %s\n", titulo);
    printf("--------------------------------------------------------------------------------\n");
}

static void imprimirPie(int total)
{
    printf("--------------------------------------------------------------------------------\n");
    printf(" Total de movimientos mostrados: %d\n", total);
    printf("================================================================================\n");
}

int Auditoria_DAO_Contar(void)
{
    FILE *archivo;
    char linea[512];
    int total = 0;

    archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL)
        return 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (linea[0] != '\n' && linea[0] != '\0')
            total++;
    }

    fclose(archivo);
    return total;
}

int Auditoria_DAO_Listar(void)
{
    FILE *archivo;
    char linea[512];
    int total = 0;

    archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL)
    {
        printf("\nNo existe el historial de movimientos (Data/Historial.txt).\n");
        return 0;
    }

    imprimirEncabezado("HISTORIAL COMPLETO DE MOVIMIENTOS");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        const char *visible = axis_saltar_bom(linea);

        if (visible[0] == '\n' || visible[0] == '\0')
            continue;

        total++;
        printf(" %04d | %s", total, visible);

        if (visible[strlen(visible) - 1] != '\n')
            printf("\n");
    }

    imprimirPie(total);
    fclose(archivo);
    return total;
}

int Auditoria_DAO_Buscar(const char criterio[])
{
    FILE *archivo;
    char linea[512];
    int total = 0;

    if (criterio == NULL || criterio[0] == '\0')
        return 0;

    archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL)
    {
        printf("\nNo existe el historial de movimientos (Data/Historial.txt).\n");
        return 0;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("                    AXIS | BUSQUEDA EN AUDITORIA\n");
    printf("================================================================================\n");
    printf(" Criterio: %s\n", criterio);
    printf("--------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        const char *visible = axis_saltar_bom(linea);

        if (visible[0] != '\n' && visible[0] != '\0' &&
            contieneTextoCI(visible, criterio))
        {
            total++;
            printf(" %04d | %s", total, visible);

            if (visible[strlen(visible) - 1] != '\n')
                printf("\n");
        }
    }

    imprimirPie(total);
    fclose(archivo);
    return total;
}

int Auditoria_DAO_FiltrarPorFecha(const char desde[], const char hasta[])
{
    FILE *archivo;
    char linea[512];
    int total = 0;
    int fechaDesde;
    int fechaHasta;

    fechaDesde = convertirFecha(desde);
    fechaHasta = convertirFecha(hasta);

    if (fechaDesde < 0 || fechaHasta < 0 || fechaDesde > fechaHasta)
    {
        printf("\n[Error] Rango de fechas invalido. Use DD/MM/AAAA.\n");
        return 0;
    }

    archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL)
    {
        printf("\nNo existe el historial de movimientos (Data/Historial.txt).\n");
        return 0;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("                  AXIS | AUDITORIA POR RANGO DE FECHAS\n");
    printf("================================================================================\n");
    printf(" Desde: %s   Hasta: %s\n", desde, hasta);
    printf("--------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        const char *visible = axis_saltar_bom(linea);
        int fechaLinea;

        if (visible[0] == '\n' || visible[0] == '\0')
            continue;

        fechaLinea = convertirFecha(visible);
        if (fechaLinea < 0)
            continue;

        if (fechaLinea >= fechaDesde && fechaLinea <= fechaHasta)
        {
            total++;
            printf(" %04d | %s", total, visible);

            if (visible[strlen(visible) - 1] != '\n')
                printf("\n");
        }
    }

    imprimirPie(total);
    fclose(archivo);
    return total;
}

void Auditoria_DAO_Resumen(void)
{
    FILE *archivo;
    char linea[512];

    int total = 0;
    int altas = 0;
    int actualizaciones = 0;
    int eliminaciones = 0;
    int stock = 0;
    int asistencias = 0;
    int accesos = 0;
    int usuarios = 0;
    int auditoria = 0;
    int otros = 0;

    char primeraFecha[11] = "Sin datos";
    char ultimaFecha[11] = "Sin datos";

    archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL)
    {
        printf("\nNo existe el historial de movimientos (Data/Historial.txt).\n");
        return;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        const char *visible = axis_saltar_bom(linea);

        if (visible[0] == '\n' || visible[0] == '\0')
            continue;

        total++;

        if (total == 1)
        {
            strncpy(primeraFecha, visible, 10);
            primeraFecha[10] = '\0';
        }
        strncpy(ultimaFecha, visible, 10);
        ultimaFecha[10] = '\0';

        if (contieneTextoCI(visible, "ELIMINACION"))
            eliminaciones++;
        else if (contieneTextoCI(visible, "ACTUALIZACION"))
            actualizaciones++;
        else if (contieneTextoCI(visible, "ENTRADA ASISTENCIA") ||
                 contieneTextoCI(visible, "SALIDA ASISTENCIA") ||
                 contieneTextoCI(visible, "ASISTENCIA"))
            asistencias++;
        else if (contieneTextoCI(visible, "AUDITORIA"))
            auditoria++;
        else if (contieneTextoCI(visible, "ACCESO"))
            accesos++;
        else if (contieneTextoCI(visible, "ENTRADA STOCK") ||
                 contieneTextoCI(visible, "SALIDA STOCK") ||
                 contieneTextoCI(visible, "STOCK:"))
            stock++;
        else if (contieneTextoCI(visible, "REGISTRO:") ||
                 contieneTextoCI(visible, "EMPLEADO:"))
            altas++;
        else if (contieneTextoCI(visible, "USUARIO"))
            usuarios++;
        else
            otros++;
    }

    fclose(archivo);

    printf("\n");
    printf("================================================================================\n");
    printf("                    AXIS | RESUMEN DE AUDITORIA\n");
    printf("================================================================================\n");
    printf(" Total de movimientos registrados : %d\n", total);
    printf(" Primer movimiento                : %s\n", primeraFecha);
    printf(" Ultimo movimiento                : %s\n", ultimaFecha);
    printf("--------------------------------------------------------------------------------\n");
    printf(" Altas de productos/empleados     : %d\n", altas);
    printf(" Actualizaciones                  : %d\n", actualizaciones);
    printf(" Eliminaciones                    : %d\n", eliminaciones);
    printf(" Movimientos de stock             : %d\n", stock);
    printf(" Registros de asistencia          : %d\n", asistencias);
    printf(" Inicios de sesion                : %d\n", accesos);
    printf(" Gestion de usuarios              : %d\n", usuarios);
    printf(" Eventos de auditoria             : %d\n", auditoria);
    printf(" Otros movimientos                : %d\n", otros);
    printf("================================================================================\n");
}

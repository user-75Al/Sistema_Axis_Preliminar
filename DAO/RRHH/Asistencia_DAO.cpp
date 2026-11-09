#include <cstdio>
#include <cstring>
#include <ctime>
#include "Asistencia_DAO.h"
#include "../../BO/Compatibilidad.h"

static void obtenerFechaHoraIso(char destino[], int maxTam)
{
    time_t ahora = time(NULL);
    struct tm *fechaHora = localtime(&ahora);

    if (fechaHora != NULL)
        strftime(destino, maxTam, "%Y-%m-%d %H:%M:%S", fechaHora);
    else
        strcpy(destino, "0000-00-00 00:00:00");
}

static void obtenerHoraSimple(char destino[], int maxTam)
{
    time_t ahora = time(NULL);
    struct tm *fechaHora = localtime(&ahora);

    if (fechaHora != NULL)
        strftime(destino, maxTam, "%H:%M:%S", fechaHora);
    else
        strcpy(destino, "00:00:00");
}

static int parsearAsistenciaLinea(const char *linea, char id[20],
                                  char tipo[10], char fecha[40])
{
    linea = axis_saltar_bom(linea);

    if (linea == NULL || linea[0] == '\n' || linea[0] == '\0')
        return 0;

    fecha[0] = '\0';

    int campos = sscanf(linea, "%19[^|]|%9[^|]|%39[^\r\n]", id, tipo, fecha);

    if (campos < 2)
        return 0;

    return 1;
}

int Asistencia_DAO_Registrar(const char idEmpleado[], const char tipo[])
{
    char fechaHora[40];
    FILE *archivo;

    if (idEmpleado == NULL || tipo == NULL)
        return 0;

    if (strcmp(tipo, "ENTRADA") != 0 && strcmp(tipo, "SALIDA") != 0)
        return 0;

    obtenerFechaHoraIso(fechaHora, sizeof(fechaHora));

    archivo = fopen(RUTA_ASISTENCIAS, "a");
    if (archivo == NULL)
        return 0;

    fprintf(archivo, "%s|%s|%s\n", idEmpleado, tipo, fechaHora);
    fclose(archivo);

    return 1;
}

int Asistencia_DAO_UltimoEstado(const char idEmpleado[], char estado[], int maxTam)
{
    FILE *archivo;
    char linea[256];
    char id[20], tipo[10], fecha[40];
    int encontrado = 0;

    if (idEmpleado == NULL || estado == NULL || maxTam <= 0)
        return 0;

    estado[0] = '\0';

    archivo = fopen(RUTA_ASISTENCIAS, "r");
    if (archivo == NULL)
        return 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearAsistenciaLinea(linea, id, tipo, fecha) &&
            axis_stricmp(id, idEmpleado) == 0)
        {
            strncpy(estado, tipo, maxTam - 1);
            estado[maxTam - 1] = '\0';
            encontrado = 1;
        }
    }

    fclose(archivo);
    return encontrado;
}

void Asistencia_DAO_Listar(void)
{
    FILE *archivo;
    char linea[256];
    char id[20], tipo[10], fecha[40];
    int total = 0;

    archivo = fopen(RUTA_ASISTENCIAS, "r");
    if (archivo == NULL)
    {
        printf("\nNo hay registros de asistencia.\n");
        return;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("                        AXIS | REGISTRO DE ASISTENCIA\n");
    printf("================================================================================\n");
    printf(" ID EMPLEADO          TIPO        FECHA Y HORA\n");
    printf("--------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearAsistenciaLinea(linea, id, tipo, fecha))
        {
            total++;
            printf(" %-20.20s %-10.10s  %-19.19s\n", id, tipo, fecha);
        }
    }

    printf("--------------------------------------------------------------------------------\n");
    printf(" Total de registros: %d\n", total);
    printf("================================================================================\n");

    fclose(archivo);
}

/* Funciones de utilidad para el historial (usadas por el CTRL) */
void Asistencia_Obtener_Hora_Actual(char destino[], int maxTam)
{
    obtenerHoraSimple(destino, maxTam);
}

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "Empleado_DAO.h"
#include "../../BO/Validaciones_BO.h"
#include "../../BO/Compatibilidad.h"

#define DIAS_MES_COMERCIAL 30
#define CONTADOR_INICIAL 40

static float Calcular_Salario_Mensual(float salario_Diario)
{
    return salario_Diario * DIAS_MES_COMERCIAL;
}

static int parsearEmpleadoLinea(const char *linea, char id[20],
    char numero_Empleado[20], char nombre[50], char cargo[40],
    char telefono[20], char correo[50], char fecha_Nacimiento[12],
    float *salario_Diario, float *jornada_Horas,
    char fecha_Registro[30], char fecha_Modificacion[30])
{
    linea = axis_saltar_bom(linea);

    if (linea == NULL || linea[0] == '\n' || linea[0] == '\0')
        return 0;

    strcpy(fecha_Registro, "Sin fecha");
    strcpy(fecha_Modificacion, "Sin fecha");

    int campos = sscanf(linea,
        "%19[^|]|%19[^|]|%49[^|]|%39[^|]|%19[^|]|%49[^|]|%11[^|]|%f|%f|%29[^|]|%29[^\r\n]",
        id, numero_Empleado, nombre, cargo, telefono, correo,
        fecha_Nacimiento, salario_Diario, jornada_Horas,
        fecha_Registro, fecha_Modificacion);

    if (campos < 9)
        return 0;

    if (strlen(fecha_Registro) < 5)
        strcpy(fecha_Registro, "Sin fecha");

    if (strlen(fecha_Modificacion) < 5)
        strcpy(fecha_Modificacion, "Sin fecha");

    return 1;
}

static int contiene_subcadena_ci(const char *cadena, const char *sub)
{
    if (cadena == NULL || sub == NULL || *sub == '\0')
        return 0;

    for (int i = 0; cadena[i] != '\0'; i++)
        if (axis_strnicmp(cadena + i, sub, strlen(sub)) == 0)
            return 1;

    return 0;
}

static int Empleado_Coincide(const char *id, const char *numero_Empleado,
    const char *nombre, const char *correo, const char *criterio)
{
    if (criterio == NULL || *criterio == '\0')
        return 0;

    if (axis_stricmp(id, criterio) == 0 ||
        axis_stricmp(numero_Empleado, criterio) == 0 ||
        axis_stricmp(correo, criterio) == 0 ||
        axis_stricmp(nombre, criterio) == 0 ||
        contiene_subcadena_ci(nombre, criterio))
        return 1;

    return 0;
}

int EmpleadoDAO::Existe_Numero(const char numeroBuscado[])
{
    FILE *archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
        return 0;

    char linea[256], id[20], numero_Empleado[20], nombre[50], cargo[40];
    char telefono[20], correo[50], fecha_Nacimiento[12];
    char fecha_Registro[30], fecha_Modificacion[30];
    float salario_Diario, jornada_Horas;

    while (fgets(linea, sizeof(linea), archivo))
    {
        if (parsearEmpleadoLinea(linea, id, numero_Empleado, nombre, cargo,
                telefono, correo, fecha_Nacimiento, &salario_Diario,
                &jornada_Horas, fecha_Registro, fecha_Modificacion)
            && axis_stricmp(numero_Empleado, numeroBuscado) == 0)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int EmpleadoDAO::Registrar(const char numero_Empleado[], const char nombre[], const char cargo[],
                           const char telefono[], const char correo[],
                           const char fecha_Nacimiento[], float salario_Diario,
                           float jornada_Horas)
{
    FILE *archivo;
    FILE *contadorArchivo;
    int contador = CONTADOR_INICIAL;
    char id[20];
    time_t ahora = time(NULL);
    struct tm *fechaHora = localtime(&ahora);
    char fechaRegistro[30];
    char fechaModificacion[30];

    if (Existe_Numero(numero_Empleado))
    {
        return -2;
    }

    if (fechaHora == NULL)
    {
        return 0;
    }

    strftime(fechaRegistro, sizeof(fechaRegistro),
             "%d/%m/%Y %H:%M:%S", fechaHora);

    strcpy(fechaModificacion, fechaRegistro);

    contadorArchivo = fopen("Data/Empleados_Contador.txt", "r");

    if (contadorArchivo != NULL)
    {
        if (fscanf(contadorArchivo, "%d", &contador) != 1)
        {
            contador = CONTADOR_INICIAL;
        }
        fclose(contadorArchivo);
    }

    sprintf(id, "XWL-Z%d", contador);

    archivo = fopen("Data/Empleados.txt", "a");

    if (archivo == NULL)
    {
        return 0;
    }

    fprintf(archivo, "%s|%s|%s|%s|%s|%s|%s|%.2f|%.2f|%s|%s\n",
            id, numero_Empleado, nombre, cargo, telefono, correo,
            fecha_Nacimiento, salario_Diario, jornada_Horas,
            fechaRegistro, fechaModificacion);

    fclose(archivo);

    contadorArchivo = fopen("Data/Empleados_Contador.txt", "w");

    if (contadorArchivo != NULL)
    {
        fprintf(contadorArchivo, "%d", contador + 1);
        fclose(contadorArchivo);
    }

    char logHistorial[256];
    sprintf(logHistorial, "EMPLEADO: [%s] N. Empleado: %s, %s, Cargo: %s, Sal. diario: $%.2f, Jornada: %.2f h",
            id, numero_Empleado, nombre, cargo, salario_Diario, jornada_Horas);
    Guardar_Fecha_Hora(logHistorial);

    return 1;
}

void EmpleadoDAO::Listar(void)
{
    FILE *archivo;
    char id[20];
    char numero_Empleado[20];
    char nombre[50];
    char cargo[40];
    char telefono[20];
    char correo[50];
    char fecha_Nacimiento[12];
    float salario_Diario;
    float jornada_Horas;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];
    int total = 0;
    float nominaMensual = 0.0f;

    time_t ahora;
    struct tm *fechaHora;
    char fechaConsulta[30];

    archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
    {
        printf("\nNo hay empleados registrados (archivo no encontrado).\n");
        return;
    }

    ahora = time(NULL);
    fechaHora = localtime(&ahora);

    if (fechaHora != NULL)
    {
        strftime(fechaConsulta, sizeof(fechaConsulta),
                 "%d/%m/%Y %H:%M:%S", fechaHora);
    }
    else
    {
        strcpy(fechaConsulta, "No disponible");
    }

    printf("\n");
    printf("==========================================================================================================================\n");
    printf("                                      AXIS | RECURSOS HUMANOS\n");
    printf("==========================================================================================================================\n");
    printf(" Consulta: %s\n", fechaConsulta);
    printf("--------------------------------------------------------------------------------------------------------------------------\n");
    printf(" ID        N.EMPLEADO  NOMBRE                     CARGO                 JORNADA   SAL. DIARIO   SAL. MENSUAL   TELEFONO\n");
    printf("--------------------------------------------------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearEmpleadoLinea(linea, id, numero_Empleado, nombre, cargo,
                                 telefono, correo, fecha_Nacimiento,
                                 &salario_Diario, &jornada_Horas,
                                 fecha_Registro, fecha_Modificacion))
        {
            float salarioMensual = Calcular_Salario_Mensual(salario_Diario);

            total++;
            nominaMensual += salarioMensual;

            printf(" %-9s %-11s %-25.25s %-20.20s %5.2f h   $%9.2f   $%10.2f   %-12.12s\n",
                    id, numero_Empleado, nombre, cargo, jornada_Horas,
                    salario_Diario, salarioMensual, telefono);
        }
    }

    printf("--------------------------------------------------------------------------------------------------------------------------\n");

    if (total == 0)
    {
        printf(" No hay empleados registrados.\n");
    }
    else
    {
        printf(" Empleados: %d        Nomina mensual: $%.2f\n", total, nominaMensual);
    }

    printf("==========================================================================================================================\n");

    fclose(archivo);
}

int EmpleadoDAO::Consultar(const char criterio[])
{
    FILE *archivo;
    char id[20];
    char numero_Empleado[20];
    char nombre[50];
    char cargo[40];
    char telefono[20];
    char correo[50];
    char fecha_Nacimiento[12];
    float salario_Diario;
    float jornada_Horas;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearEmpleadoLinea(linea, id, numero_Empleado, nombre, cargo,
                                    telefono, correo, fecha_Nacimiento,
                                    &salario_Diario, &jornada_Horas,
                                    fecha_Registro, fecha_Modificacion))
        {
            if (Empleado_Coincide(id, numero_Empleado, nombre, correo, criterio))
            {
                printf("\n");
                printf("================================================================================\n");
                printf("                         AXIS | RECURSOS HUMANOS\n");
                printf("================================================================================\n");
                printf("                         CONSULTA DE EMPLEADO\n");
                printf("\n");
                printf(" Nombre: %s\n", nombre);
                printf(" ID: %s\n", id);
                printf("--------------------------------------------------------------------------------\n");
                printf(" INFORMACION GENERAL\n");
                printf("--------------------------------------------------------------------------------\n");
                printf(" Numero de empleado   : %s\n", numero_Empleado);
                printf(" Cargo                : %s\n", cargo);
                printf(" Telefono             : %s\n", telefono);
                printf(" Correo               : %s\n", correo);
                printf(" Fecha de nacimiento  : %s\n", fecha_Nacimiento);
                printf("--------------------------------------------------------------------------------\n");
                printf(" INFORMACION LABORAL\n");
                printf("--------------------------------------------------------------------------------\n");
                printf(" Jornada laboral      : %.2f horas por dia\n", jornada_Horas);
                printf(" Salario diario       : $%.2f\n", salario_Diario);
                printf(" Salario mensual      : $%.2f (%d dias)\n",
                        Calcular_Salario_Mensual(salario_Diario), DIAS_MES_COMERCIAL);
                printf("--------------------------------------------------------------------------------\n");
                printf(" CONTROL DE REGISTRO\n");
                printf("--------------------------------------------------------------------------------\n");
                printf(" Fecha de registro      : %s\n", fecha_Registro);
                printf(" Ultima modificacion    : %s\n", fecha_Modificacion);
                printf("================================================================================\n");
                printf("                            FIN DE LA CONSULTA\n");
                printf("================================================================================\n");

                fclose(archivo);
                return 1;
            }
        }
    }

    fclose(archivo);
    return 0;
}

int EmpleadoDAO::Obtener(const char criterio[], Empleado *emp)
{
    if (criterio == NULL || emp == NULL)
        return 0;

    FILE *archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
        return 0;

    char linea[256];

    while (fgets(linea, sizeof(linea), archivo))
    {
        if (parsearEmpleadoLinea(linea, emp->id, emp->numero_Empleado,
                emp->nombre, emp->cargo, emp->telefono, emp->correo,
                emp->fecha_Nacimiento, &emp->salario_Diario,
                &emp->jornada_Horas, emp->fecha_Registro,
                emp->fecha_Modificacion)
            && Empleado_Coincide(emp->id, emp->numero_Empleado,
                emp->nombre, emp->correo, criterio))
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int EmpleadoDAO::Actualizar(const char idBuscado[], const char numero_Empleado[],
                            const char nombre[], const char cargo[], const char telefono[],
                            const char correo[], const char fecha_Nacimiento[],
                            float salario_Diario, float jornada_Horas)
{
    FILE *archivo;
    FILE *temporal;
    char id[20];
    char numeroActual[20];
    char nombreActual[50];
    char cargoActual[40];
    char telefonoActual[20];
    char correoActual[50];
    char fechaNacActual[12];
    float salarioActual;
    float jornadaActual;
    int encontrado = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    temporal = fopen("Data/Empleados_Temporal.txt", "w");

    if (temporal == NULL)
    {
        fclose(archivo);
        return 0;
    }

    time_t ahora = time(NULL);
    struct tm *fechaHora = localtime(&ahora);
    char nuevafecha[30];

    if (fechaHora == NULL)
    {
        fclose(archivo);
        fclose(temporal);
        remove("Data/Empleados_Temporal.txt");
        return 0;
    }

    strftime(nuevafecha, sizeof(nuevafecha),
             "%d/%m/%Y %H:%M:%S", fechaHora);

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearEmpleadoLinea(linea, id, numeroActual, nombreActual,
                                 cargoActual, telefonoActual, correoActual,
                                 fechaNacActual, &salarioActual,
                                 &jornadaActual, fecha_Registro,
                                 fecha_Modificacion))
        {
            if (axis_stricmp(id, idBuscado) == 0)
            {
                if (strcmp(fecha_Registro, "Sin fecha") == 0)
                {
                    strcpy(fecha_Registro, nuevafecha);
                }
                fprintf(temporal, "%s|%s|%s|%s|%s|%s|%s|%.2f|%.2f|%s|%s\n",
                        id, numero_Empleado, nombre, cargo, telefono, correo,
                        fecha_Nacimiento, salario_Diario, jornada_Horas,
                        fecha_Registro, nuevafecha);
                encontrado = 1;
            }
            else
            {
                fprintf(temporal, "%s|%s|%s|%s|%s|%s|%s|%.2f|%.2f|%s|%s\n",
                        id, numeroActual, nombreActual, cargoActual,
                        telefonoActual, correoActual, fechaNacActual,
                        salarioActual, jornadaActual, fecha_Registro,
                        fecha_Modificacion);
            }
        }
    }

    fclose(archivo);
    fclose(temporal);

    if (encontrado == 1)
    {
        remove("Data/Empleados.txt");
        rename("Data/Empleados_Temporal.txt", "Data/Empleados.txt");

        char logHistorial[256];
        sprintf(logHistorial, "ACTUALIZACION DE EMPLEADO: [%s] N. Empleado: %s, %s, Sal. diario: $%.2f, Jornada: %.2f h",
                idBuscado, numero_Empleado, nombre, salario_Diario, jornada_Horas);
        Guardar_Fecha_Hora(logHistorial);
    }
    else
    {
        remove("Data/Empleados_Temporal.txt");
    }

    return encontrado;
}

int EmpleadoDAO::Eliminar(const char idBuscado[])
{
    FILE *archivo;
    FILE *temporal;
    char id[20];
    char numero_Empleado[20];
    char nombre[50];
    char cargo[40];
    char telefono[20];
    char correo[50];
    char fecha_Nacimiento[12];
    float salario_Diario;
    float jornada_Horas;
    int encontrado = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Empleados.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    temporal = fopen("Data/Empleados_Temporal.txt", "w");

    if (temporal == NULL)
    {
        fclose(archivo);
        return 0;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearEmpleadoLinea(linea, id, numero_Empleado, nombre, cargo,
                                    telefono, correo, fecha_Nacimiento,
                                    &salario_Diario, &jornada_Horas,
                                    fecha_Registro, fecha_Modificacion))
        {
            if (axis_stricmp(id, idBuscado) == 0)
            {
                encontrado = 1;
            }
            else
            {
                fprintf(temporal, "%s|%s|%s|%s|%s|%s|%s|%.2f|%.2f|%s|%s\n",
                        id, numero_Empleado, nombre, cargo, telefono, correo,
                        fecha_Nacimiento, salario_Diario, jornada_Horas,
                        fecha_Registro, fecha_Modificacion);
            }
        }
    }

    fclose(archivo);
    fclose(temporal);

    if (encontrado == 1)
    {
        remove("Data/Empleados.txt");
        rename("Data/Empleados_Temporal.txt", "Data/Empleados.txt");

        char logHistorial[256];
        sprintf(logHistorial, "ELIMINACION DE EMPLEADO: [%s]", idBuscado);
        Guardar_Fecha_Hora(logHistorial);
    }
    else
    {
        remove("Data/Empleados_Temporal.txt");
    }

    return encontrado;
}

// Funciones compatibles con codigo original
int Empleado_DAO_Existe_Numero(char numeroBuscado[]) {
    return EmpleadoDAO::Existe_Numero(numeroBuscado);
}
int Empleado_DAO_Registrar(char numero_Empleado[], char nombre[], char cargo[],
                           char telefono[], char correo[],
                           char fecha_Nacimiento[], float salario_Diario,
                           float jornada_Horas) {
    return EmpleadoDAO::Registrar(numero_Empleado, nombre, cargo, telefono, correo,
                                  fecha_Nacimiento, salario_Diario, jornada_Horas);
}
void Empleado_DAO_Listar(void) {
    EmpleadoDAO::Listar();
}
int Empleado_DAO_Consultar(char criterio[]) {
    return EmpleadoDAO::Consultar(criterio);
}
int Empleado_DAO_Obtener(const char criterio[], Empleado *emp) {
    return EmpleadoDAO::Obtener(criterio, emp);
}
int Empleado_DAO_Actualizar(char idBuscado[], char numero_Empleado[],
                            char nombre[], char cargo[], char telefono[],
                            char correo[], char fecha_Nacimiento[],
                            float salario_Diario, float jornada_Horas) {
    return EmpleadoDAO::Actualizar(idBuscado, numero_Empleado, nombre, cargo, telefono,
                                   correo, fecha_Nacimiento, salario_Diario, jornada_Horas);
}
int Empleado_DAO_Eliminar(char idBuscado[]) {
    return EmpleadoDAO::Eliminar(idBuscado);
}

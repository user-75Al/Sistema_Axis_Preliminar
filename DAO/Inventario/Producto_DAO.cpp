#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "Producto_DAO.h"
#include "../../BO/Validaciones_BO.h"
#include "../../BO/Compatibilidad.h"

static int parsearProductoLinea(const char *linea, char id[20],
    int *codigo, char nombre[50], int *cantidad, float *precio,
    char fecha_Registro[30], char fecha_Modificacion[30])
{
    linea = axis_saltar_bom(linea);

    if (linea == NULL || linea[0] == '\n' || linea[0] == '\0')
        return 0;

    strcpy(fecha_Registro, "Sin fecha");
    strcpy(fecha_Modificacion, "Sin fecha");

    int campos = sscanf(linea, "%19[^|]|%d|%49[^|]|%d|%f|%29[^|]|%29[^\r\n]",
        id, codigo, nombre, cantidad, precio,
        fecha_Registro, fecha_Modificacion);

    if (campos < 5)
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

static int Producto_Coincide(const char *id, int codigo,
    const char *nombre, const char *criterio)
{
    if (criterio == NULL || *criterio == '\0')
        return 0;

    if (axis_stricmp(id, criterio) == 0 ||
        axis_stricmp(nombre, criterio) == 0 ||
        contiene_subcadena_ci(nombre, criterio))
        return 1;

    int numCrit = atoi(criterio);

    if (numCrit > 0 && (codigo == numCrit ||
        (strrchr(id, '-') != NULL &&
         atoi(strrchr(id, '-') + 1) == numCrit)))
        return 1;

    return 0;
}

int ProductoDAO::Existe_Codigo(int codigoBuscado)
{
    FILE *archivo = fopen("Data/Inventario.txt", "r");
    if (archivo == NULL)
        return 0;

    char linea[256], id[20], nombre[50];
    char fecha_Registro[30], fecha_Modificacion[30];
    int codigo, cantidad;
    float precio;

    while (fgets(linea, sizeof(linea), archivo))
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre,
            &cantidad, &precio, fecha_Registro, fecha_Modificacion)
            && codigo == codigoBuscado)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int ProductoDAO::Obtener(const char idBuscado[], Producto *prod)
{
    if (idBuscado == NULL || prod == NULL)
        return 0;

    FILE *archivo = fopen("Data/Inventario.txt", "r");
    if (archivo == NULL)
        return 0;

    char linea[256];

    while (fgets(linea, sizeof(linea), archivo))
    {
        if (parsearProductoLinea(linea, prod->id, &prod->codigo,
            prod->nombre, &prod->cantidad, &prod->precio,
            prod->fecha_Registro, prod->fecha_Modificacion)
            && Producto_Coincide(prod->id, prod->codigo,
                prod->nombre, idBuscado))
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int ProductoDAO::Registrar(int codigo, const char nombre[], int cantidad, float precio)
{
    FILE *archivo;
    FILE *contadorArchivo;
    int contador = 1;
    char id[20];
    time_t ahora = time(NULL);
    struct tm *fechaHora = localtime(&ahora);
    char fechaRegistro[30];
    char fechaModificacion[30];

    if (Existe_Codigo(codigo))
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

    contadorArchivo = fopen("Data/Inventario_Contador.txt", "r");

    if (contadorArchivo != NULL)
    {
        fscanf(contadorArchivo, "%d", &contador);
        fclose(contadorArchivo);
    }

    sprintf(id, "ZW99-%d", contador);

    archivo = fopen("Data/Inventario.txt", "a");

    if (archivo == NULL)
    {
        return 0;
    }

    fprintf(archivo, "%s|%d|%s|%d|%.2f|%s|%s\n",
            id, codigo, nombre, cantidad, precio, fechaRegistro, fechaModificacion);

    fclose(archivo);

    contadorArchivo = fopen("Data/Inventario_Contador.txt", "w");

    if (contadorArchivo != NULL)
    {
        fprintf(contadorArchivo, "%d", contador + 1);
        fclose(contadorArchivo);
    }

    char logHistorial[256];
    sprintf(logHistorial, "REGISTRO: [%s] Cod: %d, %s, Cant: %d, Precio: $%.2f",
            id, codigo, nombre, cantidad, precio);
    Guardar_Fecha_Hora(logHistorial);

    return 1;
}

void ProductoDAO::Listar(void)
{
    FILE *archivo;
    char id[20];
    char nombre[50];
    int codigo;
    int cantidad;
    float precio;
    int total = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    time_t ahora;
    struct tm *fechaHora;
    char fechaConsulta[30];

    archivo = fopen("Data/Inventario.txt", "r");

    if (archivo == NULL)
    {
        printf("\nNo hay productos registrados (archivo no encontrado).\n");
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
    printf("===================================================================================================\n");
    printf("                                      AXIS | INVENTARIO\n");
    printf("===================================================================================================\n");
    printf(" Consulta: %s\n", fechaConsulta);
    printf("---------------------------------------------------------------------------------------------------\n");
    printf(" ID       CODIGO     PRODUCTO                  EXIST.   PRECIO       REGISTRO       MODIFICACION\n");
    printf("---------------------------------------------------------------------------------------------------\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre,
                                    &cantidad, &precio,
                                    fecha_Registro, fecha_Modificacion))
        {
            total++;

            printf(" %-8s %-10d %-25.25s %6d   $%7.2f   %-14.10s %-14.10s\n",
                    id, codigo, nombre, cantidad, precio,
                    fecha_Registro, fecha_Modificacion);
        }
    }

    printf("---------------------------------------------------------------------------------------------------\n");

    if (total == 0)
    {
        printf(" No hay productos registrados en el inventario.\n");
    }
    else
    {
        printf(" Productos: %d\n", total);
    }

    printf("===================================================================================================\n");

    fclose(archivo);
}

int ProductoDAO::Consultar(const char idBuscado[])
{
    FILE *archivo;
    char id[20];
    char nombre[50];
    int codigo;
    int cantidad;
    float precio;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Inventario.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre,
                                 &cantidad, &precio,
                                 fecha_Registro, fecha_Modificacion))
        {
            if (Producto_Coincide(id, codigo, nombre, idBuscado))
            {
                printf("\n");
                printf("================================================================================\n");
                printf("                         AXIS | INVENTARIO\n");
                printf("================================================================================\n");
                printf("                           CONSULTA DE PRODUCTO\n");
                printf("\n");
                printf(" Nombre: %s\n", nombre);
                printf(" ID: %s\n", id);
                printf("--------------------------------------------------------------------------------\n");
                printf(" INFORMACION GENERAL\n");
                printf("--------------------------------------------------------------------------------\n");
                printf(" Codigo                 : %d\n", codigo);
                printf(" Existencias            : %d unidades\n", cantidad);
                printf(" Precio unitario        : $%.2f\n", precio);
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

int ProductoDAO::Actualizar(const char idBuscado[], const char nuevoNombre[],
                            int nuevaCantidad, float nuevoPrecio)
{
    FILE *archivo;
    FILE *temporal;
    char id[20];
    char nombre[50];
    int codigo;
    int cantidad;
    float precio;
    int encontrado = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Inventario.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    temporal = fopen("Data/Temporal.txt", "w");

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
        remove("Data/Temporal.txt");
        return 0;
    }

    strftime(nuevafecha, sizeof(nuevafecha),
             "%d/%m/%Y %H:%M:%S", fechaHora);

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre, &cantidad, &precio, fecha_Registro, fecha_Modificacion))
        {
            if (Producto_Coincide(id, codigo, nombre, idBuscado))
            {
                if (strcmp(fecha_Registro, "Sin fecha") == 0)
                {
                    strcpy(fecha_Registro, nuevafecha);
                }
                fprintf(temporal, "%s|%d|%s|%d|%.2f|%s|%s\n",
                        id, codigo, nuevoNombre, nuevaCantidad, nuevoPrecio, fecha_Registro, nuevafecha);
                encontrado = 1;
            }
            else
            {
                fprintf(temporal, "%s|%d|%s|%d|%.2f|%s|%s\n",
                        id, codigo, nombre, cantidad, precio, fecha_Registro, fecha_Modificacion);
            }
        }
    }

    fclose(archivo);
    fclose(temporal);

    if (encontrado == 1)
    {
        remove("Data/Inventario.txt");
        rename("Data/Temporal.txt", "Data/Inventario.txt");

        char logHistorial[256];
        sprintf(logHistorial, "ACTUALIZACION: [%s] %s, Cant: %d, Precio: $%.2f",
                idBuscado, nuevoNombre, nuevaCantidad, nuevoPrecio);
        Guardar_Fecha_Hora(logHistorial);
    }
    else
    {
        remove("Data/Temporal.txt");
    }

    return encontrado;
}

int ProductoDAO::Eliminar(const char idBuscado[])
{
    FILE *archivo;
    FILE *temporal;
    char id[20];
    char nombre[50];
    int codigo;
    int cantidad;
    float precio;
    int encontrado = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    char linea[256];

    archivo = fopen("Data/Inventario.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    temporal = fopen("Data/Temporal.txt", "w");

    if (temporal == NULL)
    {
        fclose(archivo);
        return 0;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre, &cantidad, &precio, fecha_Registro, fecha_Modificacion))
        {
            if (Producto_Coincide(id, codigo, nombre, idBuscado))
            {
                encontrado = 1;
            }
            else
            {
                fprintf(temporal, "%s|%d|%s|%d|%.2f|%s|%s\n",
                        id, codigo, nombre, cantidad, precio, fecha_Registro, fecha_Modificacion);
            }
        }
    }

    fclose(archivo);
    fclose(temporal);

    if (encontrado == 1)
    {
        remove("Data/Inventario.txt");
        rename("Data/Temporal.txt", "Data/Inventario.txt");

        char logHistorial[256];
        sprintf(logHistorial, "ELIMINACION: [%s]", idBuscado);
        Guardar_Fecha_Hora(logHistorial);
    }
    else
    {
        remove("Data/Temporal.txt");
    }

    return encontrado;
}

int ProductoDAO::Mover_Stock(const char idBuscado[], int cantidadMovimiento, int entrada)
{
    FILE *archivo;
    FILE *temporal;
    char id[20];
    char nombre[50];
    int codigo;
    int cantidad;
    float precio;
    int encontrado = 0;
    int insuficiente = 0;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
    time_t ahora;
    struct tm *fechaHora;
    char nuevafecha[30];
    char linea[256];

    archivo = fopen("Data/Inventario.txt", "r");

    if (archivo == NULL)
    {
        return 0;
    }

    temporal = fopen("Data/Temporal.txt", "w");
    if (temporal == NULL)
    {
        fclose(archivo);
        return 0;
    }

    ahora = time(NULL);
    fechaHora = localtime(&ahora);

    if (fechaHora == NULL)
    {
        fclose(archivo);
        fclose(temporal);
        remove("Data/Temporal.txt");
        return 0;
    }

    strftime(nuevafecha, sizeof(nuevafecha),
             "%d/%m/%Y %H:%M:%S", fechaHora);

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (parsearProductoLinea(linea, id, &codigo, nombre, &cantidad, &precio, fecha_Registro, fecha_Modificacion))
        {
            if (Producto_Coincide(id, codigo, nombre, idBuscado))
            {
                encontrado = 1;

                if (strcmp(fecha_Registro, "Sin fecha") == 0)
                {
                    strcpy(fecha_Registro, nuevafecha);
                }

                if (entrada == 1)
                {
                    cantidad = cantidad + cantidadMovimiento;
                    strcpy(fecha_Modificacion, nuevafecha);
                }
                else if (cantidad >= cantidadMovimiento)
                {
                    cantidad = cantidad - cantidadMovimiento;
                    strcpy(fecha_Modificacion, nuevafecha);
                }
                else
                {
                    insuficiente = 1;
                }
            }

            fprintf(temporal, "%s|%d|%s|%d|%.2f|%s|%s\n",
                    id, codigo, nombre, cantidad, precio, fecha_Registro, fecha_Modificacion);
        }
    }

    fclose(archivo);
    fclose(temporal);

    if (encontrado == 1 && insuficiente == 0)
    {
        remove("Data/Inventario.txt");
        rename("Data/Temporal.txt", "Data/Inventario.txt");

        char logHistorial[256];
        sprintf(logHistorial, "%s STOCK: [%s] Cantidad: %d",
                entrada == 1 ? "ENTRADA" : "SALIDA", idBuscado, cantidadMovimiento);
        Guardar_Fecha_Hora(logHistorial);
    }
    else
    {
        remove("Data/Temporal.txt");
    }

    if (insuficiente == 1)
    {
        return -1;
    }

    return encontrado;
}

// Funciones compatibles con codigo original
int Producto_DAO_Registrar(int codigo, char nombre[], int cantidad, float precio) {
    return ProductoDAO::Registrar(codigo, nombre, cantidad, precio);
}
void Producto_DAO_Listar(void) {
    ProductoDAO::Listar();
}
int Producto_DAO_Consultar(char id[]) {
    return ProductoDAO::Consultar(id);
}
int Producto_DAO_Obtener(const char id[], Producto *prod) {
    return ProductoDAO::Obtener(id, prod);
}
int Producto_DAO_Existe_Codigo(int codigo) {
    return ProductoDAO::Existe_Codigo(codigo);
}
int Producto_DAO_Actualizar(char id[], char nombre[], int cantidad, float precio) {
    return ProductoDAO::Actualizar(id, nombre, cantidad, precio);
}
int Producto_DAO_Eliminar(char id[]) {
    return ProductoDAO::Eliminar(id);
}
int Producto_DAO_Mover_Stock(char id[], int cantidad, int entrada) {
    return ProductoDAO::Mover_Stock(id, cantidad, entrada);
}

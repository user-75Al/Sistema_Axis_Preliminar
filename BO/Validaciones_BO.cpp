#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <conio.h>
#include "Validaciones_BO.h"
#include "RRHH/Sesion_BO.h"

void ValidacionesBO::ClearScreen(void)
{
    system("cls");
}

void ValidacionesBO::MostrarFechaHora(void)
{
    time_t tiempoActual = time(NULL);
    struct tm *fechaHora = localtime(&tiempoActual);
    char textoFecha[30];

    if (fechaHora != NULL)
    {
        strftime(textoFecha, sizeof(textoFecha), "%d/%m/%Y %H:%M:%S", fechaHora);
        printf("Hoy es: %s\n", textoFecha);
    }
}

void ValidacionesBO::GuardarFechaHora(const char *operacion)
{
    time_t tiempoActual = time(NULL);
    struct tm *fechaHora = localtime(&tiempoActual);
    char textoFecha[30];

    if (fechaHora == NULL || operacion == NULL)
        return;

    strftime(textoFecha, sizeof(textoFecha), "%d/%m/%Y %H:%M:%S", fechaHora);

    FILE *archivo = fopen("Data/Historial.txt", "a");
    if (archivo != NULL)
    {
        // Formato nuevo: fecha | usuario | operacion
        // Las lineas antiguas (fecha | operacion) siguen siendo legibles.
        fprintf(archivo, "%s | %s | %s\n",
                textoFecha, Sesion_UsuarioNombre(), operacion);
        fclose(archivo);
    }
}

void ValidacionesBO::LimpiarBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int ValidacionesBO::ValidarNombreProducto(const char *nombre)
{
    if (nombre == NULL || strlen(nombre) == 0)
    {
        printf("  [Error] El nombre no puede estar vacio.\n");
        return 0;
    }

    if (strchr(nombre, '|') != NULL)
    {
        printf("  [Error] El nombre no puede contener el caracter '|'.\n");
        return 0;
    }

    int soloEspacios = 1;
    for (int i = 0; nombre[i] != '\0'; i++)
    {
        if (nombre[i] != ' ' && nombre[i] != '\t' && nombre[i] != '\r' && nombre[i] != '\n')
        {
            soloEspacios = 0;
            break;
        }
    }

    if (soloEspacios)
    {
        printf("  [Error] El nombre no puede estar compuesto unicamente por espacios.\n");
        return 0;
    }

    return 1;
}

int ValidacionesBO::LeerEnteroSeguro(const char *mensaje, int *salida, int min, int max)
{
    char linea[100];
    int valor;

    while (1)
    {
        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) == NULL)
        {
            return 0;
        }

        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) == 0)
        {
            printf("  [Error] Debe ingresar un numero.\n");
            continue;
        }

        char extra;
        if (sscanf(linea, "%d %c", &valor, &extra) == 1)
        {
            if (valor >= min && valor <= max)
            {
                *salida = valor;
                return 1;
            }
            printf("  [Error] El valor debe estar entre %d y %d.\n", min, max);
        }
        else
        {
            printf("  [Error] Entrada no valida. Debe ingresar un numero entero.\n");
        }
    }
}

int ValidacionesBO::LeerFlotanteSeguro(const char *mensaje, float *salida, float min, float max)
{
    char linea[100];
    float valor;

    while (1)
    {
        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) == NULL)
        {
            return 0;
        }

        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) == 0)
        {
            printf("  [Error] Debe ingresar un valor numerico.\n");
            continue;
        }

        char extra;
        if (sscanf(linea, "%f %c", &valor, &extra) == 1)
        {
            if (valor >= min && valor <= max)
            {
                *salida = valor;
                return 1;
            }
            printf("  [Error] El valor debe estar entre %.2f y %.2f.\n", min, max);
        }
        else
        {
            printf("  [Error] Entrada no valida. Ingrese un numero decimal (ej. 19.50).\n");
        }
    }
}

void ValidacionesBO::LeerCadenaSegura(const char *mensaje, char *salida, int maxTam)
{
    char linea[200];

    while (1)
    {
        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) != NULL)
        {
            linea[strcspn(linea, "\r\n")] = '\0';
            if (ValidarNombreProducto(linea))
            {
                strncpy(salida, linea, maxTam - 1);
                salida[maxTam - 1] = '\0';
                return;
            }
        }
    }
}

void ValidacionesBO::TrimEspacios(char *str)
{
    if (str == NULL) return;
    char *p = str;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    if (p != str) memmove(str, p, strlen(p) + 1);
    int len = strlen(str);
    while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\t' || str[len-1] == '\r' || str[len-1] == '\n'))
    {
        str[len-1] = '\0';
        len--;
    }
}

void ValidacionesBO::LeerTextoSimple(const char *mensaje, char *salida, int maxTam)
{
    char linea[200];

    while (1)
    {
        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) != NULL)
        {
            linea[strcspn(linea, "\r\n")] = '\0';
            TrimEspacios(linea);
            if (strlen(linea) > 0)
            {
                strncpy(salida, linea, maxTam - 1);
                salida[maxTam - 1] = '\0';
                return;
            }
            printf("  [Error] Este campo no puede estar vacio.\n");
        }
    }
}

void ValidacionesBO::LeerClaveSegura(const char *mensaje, char *salida, int maxTam)
{
    int i = 0;
    int ch;

    printf("%s", mensaje);
    fflush(stdout);

    while ((ch = _getch()) != '\r' && ch != '\n')
    {
        if (ch == 0 || ch == 224)
        {
            _getch();
            continue;
        }

        if (ch == 8)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
            continue;
        }

        if (ch >= 32 && ch <= 126 && i < maxTam - 1)
        {
            salida[i++] = (char)ch;
            printf("*");
        }
    }

    salida[i] = '\0';
    printf("\n");
}

int ValidacionesBO::PreguntarModificarOMantener(const char *campo, const char *valorActual)
{
    printf("\n  %s actual: %s\n", campo, valorActual);
    printf("  Presione [S] para modificar o CUALQUIER OTRA TECLA para mantener: ");

    int ch = _getch();
    if (ch == 0 || (unsigned char)ch == 224)
    {
        _getch();
    }

    if (ch == 's' || ch == 'S')
    {
        printf("[Modificar]\n");
        return 1;
    }
    else
    {
        printf("[Mantener actual: %s]\n", valorActual);
        return 0;
    }
}

int ValidacionesBO::ValidarTelefono(const char telefono[])
{
    int digitos = 0;

    if (telefono == NULL || telefono[0] == '\0')
        return 0;

    for (int i = 0; telefono[i] != '\0'; i++)
    {
        if (telefono[i] >= '0' && telefono[i] <= '9')
            digitos++;
        else if (telefono[i] != ' ' && telefono[i] != '-' && telefono[i] != '+')
            return 0;
    }

    return (digitos >= 10 && digitos <= 15);
}

int ValidacionesBO::ValidarCorreo(const char correo[])
{
    int arrobas = 0;
    int posicionArroba = -1;
    int longitud;

    if (correo == NULL)
        return 0;

    longitud = (int)strlen(correo);

    if (longitud < 6)
        return 0;

    for (int i = 0; correo[i] != '\0'; i++)
    {
        if (correo[i] == '@')
        {
            arrobas++;
            posicionArroba = i;
        }
    }

    if (arrobas != 1 || posicionArroba == 0)
        return 0;

    if (posicionArroba >= longitud - 4)
        return 0;

    if (strchr(correo, ' ') != NULL)
        return 0;

    if (strchr(correo + posicionArroba + 1, '.') == NULL)
        return 0;

    return 1;
}

int ValidacionesBO::ValidarFecha(const char fecha[])
{
    int dia, mes, anio;
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (fecha == NULL || strlen(fecha) != 10)
        return 0;

    if (fecha[2] != '/' || fecha[5] != '/')
        return 0;

    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue;

        if (fecha[i] < '0' || fecha[i] > '9')
            return 0;
    }

    dia = (fecha[0] - '0') * 10 + (fecha[1] - '0');
    mes = (fecha[3] - '0') * 10 + (fecha[4] - '0');
    anio = (fecha[6] - '0') * 1000 +
           (fecha[7] - '0') * 100 +
           (fecha[8] - '0') * 10 +
            (fecha[9] - '0');

    if (anio < 1900 || mes < 1 || mes > 12)
        return 0;

    if (anio % 400 == 0 || (anio % 4 == 0 && anio % 100 != 0))
    {
        diasMes[1] = 29;
    }

    if (dia < 1 || dia > diasMes[mes - 1])
        return 0;

    return 1;
}

int ValidacionesBO::ValidarFechaNacimiento(const char fecha[])
{
    return ValidarFecha(fecha);
}

// Funciones compatibles con codigo original
void clearScreen(void) { ValidacionesBO::ClearScreen(); }
void Mostrar_Fecha_Hora(void) { ValidacionesBO::MostrarFechaHora(); }
void Guardar_Fecha_Hora(const char *operacion) { ValidacionesBO::GuardarFechaHora(operacion); }
void Limpiar_Buffer(void) { ValidacionesBO::LimpiarBuffer(); }
int Validar_Nombre_Producto(const char *nombre) { return ValidacionesBO::ValidarNombreProducto(nombre); }
int Leer_Entero_Seguro(const char *mensaje, int *salida, int min, int max) { return ValidacionesBO::LeerEnteroSeguro(mensaje, salida, min, max); }
int Leer_Flotante_Seguro(const char *mensaje, float *salida, float min, float max) { return ValidacionesBO::LeerFlotanteSeguro(mensaje, salida, min, max); }
void Leer_Cadena_Segura(const char *mensaje, char *salida, int maxTam) { ValidacionesBO::LeerCadenaSegura(mensaje, salida, maxTam); }
void Leer_Texto_Simple(const char *mensaje, char *salida, int maxTam) { ValidacionesBO::LeerTextoSimple(mensaje, salida, maxTam); }
void Leer_Clave_Segura(const char *mensaje, char *salida, int maxTam) { ValidacionesBO::LeerClaveSegura(mensaje, salida, maxTam); }
int Preguntar_Modificar_O_Mantener(const char *campo, const char *valorActual) { return ValidacionesBO::PreguntarModificarOMantener(campo, valorActual); }
void Trim_Espacios(char *str) { ValidacionesBO::TrimEspacios(str); }
int Validar_Telefono(const char telefono[]) { return ValidacionesBO::ValidarTelefono(telefono); }
int Validar_Correo(const char correo[]) { return ValidacionesBO::ValidarCorreo(correo); }
int Validar_Fecha(const char fecha[]) { return ValidacionesBO::ValidarFecha(fecha); }
int Validar_Fecha_Nacimiento(const char fecha[]) { return ValidacionesBO::ValidarFechaNacimiento(fecha); }

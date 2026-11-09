#ifndef VALIDACIONES_BO_H
#define VALIDACIONES_BO_H

#include <cstddef>

class ValidacionesBO {
public:
    static void ClearScreen(void);
    static void MostrarFechaHora(void);
    static void GuardarFechaHora(const char *operacion);
    static void LimpiarBuffer(void);
    static int ValidarNombreProducto(const char *nombre);
    static int LeerEnteroSeguro(const char *mensaje, int *salida, int min, int max);
    static int LeerFlotanteSeguro(const char *mensaje, float *salida, float min, float max);
    static void LeerCadenaSegura(const char *mensaje, char *salida, int maxTam);
    static void LeerTextoSimple(const char *mensaje, char *salida, int maxTam);
    static void LeerClaveSegura(const char *mensaje, char *salida, int maxTam);
    static int PreguntarModificarOMantener(const char *campo, const char *valorActual);
    static void TrimEspacios(char *str);
    static int ValidarTelefono(const char telefono[]);
    static int ValidarCorreo(const char correo[]);
    static int ValidarFecha(const char fecha[]);
    static int ValidarFechaNacimiento(const char fecha[]);
};

// Declaraciones de funciones compatibles con el codigo original
void clearScreen(void);
void Mostrar_Fecha_Hora(void);
void Guardar_Fecha_Hora(const char *operacion);
void Limpiar_Buffer(void);
int Validar_Nombre_Producto(const char *nombre);
int Leer_Entero_Seguro(const char *mensaje, int *salida, int min, int max);
int Leer_Flotante_Seguro(const char *mensaje, float *salida, float min, float max);
void Leer_Cadena_Segura(const char *mensaje, char *salida, int maxTam);
void Leer_Texto_Simple(const char *mensaje, char *salida, int maxTam);
void Leer_Clave_Segura(const char *mensaje, char *salida, int maxTam);
int Preguntar_Modificar_O_Mantener(const char *campo, const char *valorActual);
void Trim_Espacios(char *str);
int Validar_Telefono(const char telefono[]);
int Validar_Correo(const char correo[]);
int Validar_Fecha(const char fecha[]);
int Validar_Fecha_Nacimiento(const char fecha[]);

#endif // VALIDACIONES_BO_H

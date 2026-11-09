#ifndef VERIFICACION_FACIAL_H
#define VERIFICACION_FACIAL_H

/*
 * Modulo biometrico facial de AXIS.
 *
 * Implementacion nativa en C++ sobre OpenCV. Sustituye al antiguo script
 * de Python (verificacion_facial.py). Si el proyecto se compila sin OpenCV,
 * las funciones devuelven 0 y muestran un aviso, sin romper el resto del
 * sistema.
 */

int Facial_Disponible(void);
int Facial_Registrar(const char idEmpleado[]);
int Facial_Verificar(const char idEmpleado[]);

#endif // VERIFICACION_FACIAL_H

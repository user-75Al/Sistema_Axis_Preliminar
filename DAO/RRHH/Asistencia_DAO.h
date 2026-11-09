#ifndef ASISTENCIA_DAO_H
#define ASISTENCIA_DAO_H

#define RUTA_ASISTENCIAS "Data/Asistencias.txt"

int Asistencia_DAO_Registrar(const char idEmpleado[], const char tipo[]);
int Asistencia_DAO_UltimoEstado(const char idEmpleado[], char estado[], int maxTam);
void Asistencia_DAO_Listar(void);
void Asistencia_Obtener_Hora_Actual(char destino[], int maxTam);

#endif // ASISTENCIA_DAO_H

#ifndef ASISTENCIA_CTRL_H
#define ASISTENCIA_CTRL_H

int Asistencia_CTRL_Registrar(const char idEmpleado[], char tipoRegistrado[], int maxTam);
int Asistencia_CTRL_UltimoEstado(const char idEmpleado[], char estado[], int maxTam);
int Asistencia_CTRL_Listar(void);

#endif // ASISTENCIA_CTRL_H

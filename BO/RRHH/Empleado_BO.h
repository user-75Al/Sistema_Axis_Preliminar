#ifndef EMPLEADO_BO_H
#define EMPLEADO_BO_H

typedef struct
{
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
} Empleado;

#endif // EMPLEADO_BO_H

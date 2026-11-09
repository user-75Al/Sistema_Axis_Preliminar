#ifndef PRODUCTO_BO_H
#define PRODUCTO_BO_H

typedef struct
{
    char id[20];
    int codigo;
    char nombre[50];
    int cantidad;
    float precio;
    char fecha_Registro[30];
    char fecha_Modificacion[30];
} Producto;

#endif // PRODUCTO_BO_H

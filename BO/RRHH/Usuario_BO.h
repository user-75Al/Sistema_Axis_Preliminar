#ifndef USUARIO_BO_H
#define USUARIO_BO_H

/*
 * Jerarquia de acceso del sistema AXIS
 * -----------------------------------
 * El nivel numerico define la jerarquia: a mayor numero, mayor privilegio.
 * Cada modulo consulta los permisos a traves de Permisos_BO.
 */
#define ROL_INVITADO         0
#define ROL_EMPLEADO         1
#define ROL_SUPERVISOR_RRHH  2
#define ROL_GERENTE_RRHH     3
#define ROL_ADMINISTRADOR    4

typedef struct
{
    char usuario[30];
    char clave[40];
    int rol;
    char id_Empleado[20];
} Usuario;

const char *Usuario_NombreRol(int rol);
int Usuario_RolDesdeTexto(const char *texto);
int Usuario_NivelRol(int rol);

#endif // USUARIO_BO_H

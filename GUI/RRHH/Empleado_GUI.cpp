#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Empleado_GUI.h"
#include "verificacion_facial.h"
#include "../../CTRL/RRHH/Empleado_CTRL.h"
#include "../../CTRL/RRHH/Asistencia_CTRL.h"
#include "../../BO/Validaciones_BO.h"
#include "../../BO/RRHH/Sesion_BO.h"
#include "../../BO/RRHH/Permisos_BO.h"

static void leerTelefonoValidado(const char *mensaje, char salida[], int maxTam)
{
    while (1)
    {
        Leer_Texto_Simple(mensaje, salida, maxTam);
        if (Validar_Telefono(salida))
            return;
        printf("  [Error] Telefono invalido. Use entre 10 y 15 digitos (+ - espacios permitidos).\n");
    }
}

static void leerCorreoValidado(const char *mensaje, char salida[], int maxTam)
{
    while (1)
    {
        Leer_Texto_Simple(mensaje, salida, maxTam);
        if (Validar_Correo(salida))
            return;
        printf("  [Error] Correo invalido. Ejemplo: usuario@dominio.com\n");
    }
}

static void leerFechaValidada(const char *mensaje, char salida[], int maxTam)
{
    while (1)
    {
        Leer_Texto_Simple(mensaje, salida, maxTam);
        if (Validar_Fecha(salida))
            return;
        printf("  [Error] Fecha invalida. Use el formato DD/MM/AAAA.\n");
    }
}

static void registrarEmpleado(void)
{
    char numero[20], nombre[50], cargo[40], telefono[20], correo[50], fecha[12];
    float salarioDiario, jornadaHoras;
    char confirmacion[32];
    int resultado;

    clearScreen();
    printf("=== REGISTRAR EMPLEADO ===\n\n");
    printf("Nota: el ID (XWL-Z##) lo genera el sistema automaticamente.\n\n");

    Leer_Texto_Simple("Numero de empleado: ", numero, sizeof(numero));

    if (Empleado_CTRL_Existe_Numero(numero))
    {
        printf("\n[Error] El numero de empleado %s ya esta registrado.\n", numero);
        Sleep(2000);
        return;
    }

    Leer_Cadena_Segura("Nombre completo: ", nombre, sizeof(nombre));
    Leer_Cadena_Segura("Cargo: ", cargo, sizeof(cargo));
    leerTelefonoValidado("Telefono (10-15 digitos): ", telefono, sizeof(telefono));
    leerCorreoValidado("Correo electronico: ", correo, sizeof(correo));
    leerFechaValidada("Fecha de nacimiento (DD/MM/AAAA): ", fecha, sizeof(fecha));
    Leer_Flotante_Seguro("Salario diario: ", &salarioDiario, 0.01f, 1000000.0f);
    Leer_Flotante_Seguro("Jornada laboral (horas por dia): ", &jornadaHoras, 1.0f, 24.0f);

    printf("\n--------------------------------------------------\n");
    printf(" Resumen del registro:\n");
    printf("  N. Empleado       : %s\n", numero);
    printf("  Nombre            : %s\n", nombre);
    printf("  Cargo             : %s\n", cargo);
    printf("  Telefono          : %s\n", telefono);
    printf("  Correo            : %s\n", correo);
    printf("  Fecha nacimiento  : %s\n", fecha);
    printf("  Jornada laboral   : %.2f horas por dia\n", jornadaHoras);
    printf("  Salario diario    : $%.2f\n", salarioDiario);
    printf("  Salario mensual   : $%.2f (30 dias)\n", salarioDiario * 30.0f);
    printf("--------------------------------------------------\n");
    printf(" Desea guardar el empleado? (s/n): ");

    if (fgets(confirmacion, sizeof(confirmacion), stdin) == NULL ||
        (confirmacion[0] != 's' && confirmacion[0] != 'S'))
    {
        printf("\nRegistro cancelado.\n");
        Sleep(2000);
        return;
    }

    resultado = Empleado_CTRL_Registrar(numero, nombre, cargo, telefono, correo,
                                        fecha, salarioDiario, jornadaHoras);

    if (resultado == 1)
    {
        Empleado empleado;

        printf("\nEmpleado registrado correctamente.\n");

        if (Empleado_CTRL_Obtener(numero, &empleado) == 1)
        {
            printf("ID asignado por el sistema: %s\n", empleado.id);
            printf("Iniciando captura facial del empleado...\n");
            Sleep(1500);

            if (Facial_Disponible())
            {
                if (Facial_Registrar(empleado.id))
                    printf("Captura facial completada correctamente.\n");
                else
                    printf("Nota: la captura facial no se completo.\n");
            }
            else
            {
                printf("Nota: binario sin OpenCV; la captura facial no esta disponible.\n");
            }
        }
    }
    else if (resultado == -2)
    {
        printf("\n[Error] El numero de empleado ya se encuentra registrado.\n");
    }
    else if (resultado == -3)
    {
        printf("\n[Acceso denegado] Su rol no puede registrar empleados.\n");
    }
    else
    {
        printf("\nNo se pudo registrar el empleado.\n");
    }

    Sleep(2500);
}

static void consultarEmpleado(void)
{
    char criterio[100];
    int resultado;

    clearScreen();
    printf("=== CONSULTAR EMPLEADO ===\n\n");

    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        printf("Mostrando la ficha vinculada a su usuario...\n");
        criterio[0] = '\0';
    }
    else
    {
        Leer_Texto_Simple("Ingrese ID, Numero de empleado, Nombre o Correo: ",
                          criterio, sizeof(criterio));
    }

    resultado = Empleado_CTRL_Consultar(criterio);

    if (resultado == 0)
        printf("\nEmpleado no encontrado.\n");
    else if (resultado == -3)
        printf("\n[Acceso denegado] Su rol no puede consultar esta ficha.\n");

    printf("\n");
    system("pause");
}

static void listarEmpleados(void)
{
    clearScreen();

    if (Empleado_CTRL_Listar() == -3)
        printf("\n[Acceso denegado] Su rol no puede listar empleados.\n");

    printf("\n");
    system("pause");
}

static void actualizarEmpleado(void)
{
    char criterio[100];
    char nuevoNumero[20], nombre[50], cargo[40], telefono[20], correo[50], fecha[12];
    float salario, jornada;
    char confirmacion[32];
    Empleado emp;
    int resultado;

    clearScreen();
    printf("=== ACTUALIZAR EMPLEADO ===\n\n");
    Leer_Texto_Simple("ID, Numero de empleado, Nombre o Correo: ", criterio, sizeof(criterio));

    resultado = Empleado_CTRL_Obtener(criterio, &emp);

    if (resultado == 0)
    {
        printf("\nEmpleado no encontrado.\n");
        Sleep(2000);
        return;
    }

    if (resultado == -3)
    {
        printf("\n[Acceso denegado] Su rol no puede actualizar empleados.\n");
        Sleep(2000);
        return;
    }

    printf("\n--------------------------------------------------\n");
    printf(" Empleado encontrado:\n");
    printf("  ID         : %s\n", emp.id);
    printf("  N.Empleado : %s\n", emp.numero_Empleado);
    printf("  Nombre     : %s\n", emp.nombre);
    printf("  Cargo      : %s\n", emp.cargo);
    printf("  Telefono   : %s\n", emp.telefono);
    printf("  Correo     : %s\n", emp.correo);
    printf("  F.Nac.     : %s\n", emp.fecha_Nacimiento);
    printf("  Jornada    : %.2f horas por dia\n", emp.jornada_Horas);
    printf("  Salario    : $%.2f diarios / $%.2f mensuales\n",
           emp.salario_Diario, emp.salario_Diario * 30.0f);
    printf("--------------------------------------------------\n");
    printf(" Presione [S] para modificar un campo; cualquier otra tecla lo mantiene.\n");
    printf("--------------------------------------------------\n");

    strcpy(nuevoNumero, emp.numero_Empleado);
    if (Preguntar_Modificar_O_Mantener("Numero de empleado", emp.numero_Empleado))
    {
        char numeroIngresado[20];
        Leer_Texto_Simple("  Nuevo numero de empleado: ", numeroIngresado, sizeof(numeroIngresado));

        if (strcmp(numeroIngresado, emp.numero_Empleado) != 0 &&
            Empleado_CTRL_Existe_Numero(numeroIngresado))
        {
            printf("  [Advertencia] El numero ya pertenece a otro empleado. Se conserva el anterior.\n");
            Sleep(1800);
        }
        else
        {
            strcpy(nuevoNumero, numeroIngresado);
        }
    }

    strcpy(nombre, emp.nombre);
    if (Preguntar_Modificar_O_Mantener("Nombre", emp.nombre))
        Leer_Cadena_Segura("  Nuevo nombre: ", nombre, sizeof(nombre));

    strcpy(cargo, emp.cargo);
    if (Preguntar_Modificar_O_Mantener("Cargo", emp.cargo))
        Leer_Cadena_Segura("  Nuevo cargo: ", cargo, sizeof(cargo));

    strcpy(telefono, emp.telefono);
    if (Preguntar_Modificar_O_Mantener("Telefono", emp.telefono))
        leerTelefonoValidado("  Nuevo telefono: ", telefono, sizeof(telefono));

    strcpy(correo, emp.correo);
    if (Preguntar_Modificar_O_Mantener("Correo", emp.correo))
        leerCorreoValidado("  Nuevo correo: ", correo, sizeof(correo));

    strcpy(fecha, emp.fecha_Nacimiento);
    if (Preguntar_Modificar_O_Mantener("Fecha de nacimiento", emp.fecha_Nacimiento))
        leerFechaValidada("  Nueva fecha (DD/MM/AAAA): ", fecha, sizeof(fecha));

    salario = emp.salario_Diario;
    if (Preguntar_Modificar_O_Mantener("Salario diario", "ver ficha"))
        Leer_Flotante_Seguro("  Nuevo salario diario: ", &salario, 0.01f, 1000000.0f);

    jornada = emp.jornada_Horas;
    if (Preguntar_Modificar_O_Mantener("Jornada laboral", "ver ficha"))
        Leer_Flotante_Seguro("  Nueva jornada (horas por dia): ", &jornada, 1.0f, 24.0f);

    printf("\n--------------------------------------------------\n");
    printf(" Resumen de actualizacion:\n");
    printf("  ID           : %s\n", emp.id);
    printf("  N. Empleado  : %s\n", nuevoNumero);
    printf("  Nombre       : %s\n", nombre);
    printf("  Cargo        : %s\n", cargo);
    printf("  Telefono     : %s\n", telefono);
    printf("  Correo       : %s\n", correo);
    printf("  F.Nac.       : %s\n", fecha);
    printf("  Jornada      : %.2f horas por dia\n", jornada);
    printf("  Salario      : $%.2f diarios / $%.2f mensuales\n",
           salario, salario * 30.0f);
    printf("--------------------------------------------------\n");
    printf(" Desea guardar los cambios? (s/n): ");

    if (fgets(confirmacion, sizeof(confirmacion), stdin) == NULL ||
        (confirmacion[0] != 's' && confirmacion[0] != 'S'))
    {
        printf("\nActualizacion cancelada.\n");
        Sleep(2000);
        return;
    }

    resultado = Empleado_CTRL_Actualizar(emp.id, nuevoNumero, nombre, cargo,
                                         telefono, correo, fecha, salario, jornada);

    if (resultado == 1)
        printf("\nEmpleado actualizado correctamente.\n");
    else if (resultado == -3)
        printf("\n[Acceso denegado] Su rol no puede actualizar empleados.\n");
    else
        printf("\nNo se pudo actualizar el empleado.\n");

    Sleep(2000);
}

static void eliminarEmpleado(void)
{
    char criterio[100];
    char confirmacion[32];
    Empleado emp;
    int resultado;

    clearScreen();
    printf("=== ELIMINAR EMPLEADO ===\n\n");
    Leer_Texto_Simple("ID, Numero de empleado, Nombre o Correo: ", criterio, sizeof(criterio));

    resultado = Empleado_CTRL_Obtener(criterio, &emp);

    if (resultado == 0)
    {
        printf("\nEmpleado no encontrado.\n");
        Sleep(2000);
        return;
    }

    if (resultado == -3)
    {
        printf("\n[Acceso denegado] Su rol no puede eliminar empleados.\n");
        Sleep(2000);
        return;
    }

    printf("\n--------------------------------------------------\n");
    printf(" Empleado a eliminar:\n");
    printf("  ID         : %s\n", emp.id);
    printf("  N.Empleado : %s\n", emp.numero_Empleado);
    printf("  Nombre     : %s\n", emp.nombre);
    printf("  Cargo      : %s\n", emp.cargo);
    printf("  Salario    : $%.2f diarios\n", emp.salario_Diario);
    printf("--------------------------------------------------\n");
    printf(" Esta seguro de eliminarlo? (s/n): ");

    if (fgets(confirmacion, sizeof(confirmacion), stdin) == NULL ||
        (confirmacion[0] != 's' && confirmacion[0] != 'S'))
    {
        printf("\nOperacion cancelada.\n");
        Sleep(2000);
        return;
    }

    resultado = Empleado_CTRL_Eliminar(emp.id);

    if (resultado == 1)
        printf("\nEmpleado eliminado correctamente.\n");
    else if (resultado == -3)
        printf("\n[Acceso denegado] Su rol no puede eliminar empleados.\n");
    else
        printf("\nNo se pudo eliminar el empleado.\n");

    Sleep(2000);
}

static void registrarAsistencia(void)
{
    char criterio[20];
    char tipo[12];
    Empleado emp;
    int resultado;

    clearScreen();
    printf("=== REGISTRO DE ASISTENCIA FACIAL ===\n\n");

    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        strncpy(criterio, Sesion_IdEmpleado(), sizeof(criterio) - 1);
        criterio[sizeof(criterio) - 1] = '\0';
        printf("Empleado vinculado: %s\n", criterio);
    }
    else
    {
        Leer_Texto_Simple("ID o numero de empleado: ", criterio, sizeof(criterio));
    }

    resultado = Empleado_CTRL_Obtener(criterio, &emp);

    if (resultado == 0)
    {
        printf("\nEmpleado no encontrado.\n");
        Sleep(2000);
        return;
    }

    if (resultado == -3)
    {
        printf("\n[Acceso denegado] No puede registrar la asistencia de este empleado.\n");
        Sleep(2000);
        return;
    }

    printf("Empleado: %s (%s)\n", emp.nombre, emp.id);
    printf("Iniciando verificacion facial...\n");
    Sleep(1200);

    if (!Facial_Disponible())
    {
        printf("\n[Facial] El binario se compilo sin OpenCV.\n");
        printf("         Recompile con OpenCV para usar la asistencia biometrica.\n");
        Sleep(2500);
        return;
    }

    if (!Facial_Verificar(emp.id))
    {
        printf("\n[Asistencia] Verificacion facial rechazada. No se registro asistencia.\n");
        Sleep(2500);
        return;
    }

    resultado = Asistencia_CTRL_Registrar(emp.id, tipo, sizeof(tipo));

    if (resultado == 1)
        printf("\nAsistencia registrada: %s para %s.\n", tipo, emp.nombre);
    else if (resultado == -2)
        printf("\n[Acceso denegado] No puede registrar esta asistencia.\n");
    else if (resultado == -1)
        printf("\nNo se encontro el empleado en la base de datos.\n");
    else
        printf("\nNo se pudo registrar la asistencia.\n");

    Sleep(2500);
}

static void registrarRostro(void)
{
    char criterio[20];
    Empleado emp;
    int resultado;

    clearScreen();
    printf("=== REGISTRO / ACTUALIZACION DE MUESTRAS FACIALES ===\n\n");

    if (Sesion_Rol() == ROL_EMPLEADO)
    {
        strncpy(criterio, Sesion_IdEmpleado(), sizeof(criterio) - 1);
        criterio[sizeof(criterio) - 1] = '\0';
        printf("Empleado vinculado: %s\n", criterio);
    }
    else
    {
        Leer_Texto_Simple("ID o numero de empleado: ", criterio, sizeof(criterio));
    }

    resultado = Empleado_CTRL_Obtener(criterio, &emp);

    if (resultado == 0)
    {
        printf("\nEmpleado no encontrado.\n");
        Sleep(2000);
        return;
    }

    if (resultado == -3)
    {
        printf("\n[Acceso denegado] No puede registrar muestras para este empleado.\n");
        Sleep(2000);
        return;
    }

    if (Sesion_Rol() == ROL_EMPLEADO && !Permisos_PuedeRegistrarRostro(Sesion_Rol()))
    {
        /* El empleado puede actualizar su propio rostro; el resto requiere supervisor. */
        printf("Registrando muestras para su propia ficha: %s\n", emp.id);
    }

    if (!Facial_Disponible())
    {
        printf("\n[Facial] El binario se compilo sin OpenCV.\n");
        printf("         Recompile con OpenCV para usar la biometria.\n");
        Sleep(2500);
        return;
    }

    if (Facial_Registrar(emp.id))
        printf("\nMuestras faciales registradas correctamente para %s.\n", emp.nombre);
    else
        printf("\nNo se pudieron registrar las muestras faciales.\n");

    Sleep(2000);
}

static void listarAsistencias(void)
{
    clearScreen();

    if (Asistencia_CTRL_Listar() == -3)
        printf("\n[Acceso denegado] Su rol no puede consultar asistencias.\n");

    printf("\n");
    system("pause");
}

int Empleado_Mostrar_Menu(void)
{
    int opcion;
    int rol = Sesion_Rol();
    char lineaMenu[64];

    do
    {
        clearScreen();

        printf("\n");
        printf("==================================================\n");
        printf("                AXIS - RECURSOS HUMANOS           \n");
        printf("==================================================\n");
        printf("  Usuario: %-20.20s Rol: %s\n",
               Sesion_UsuarioNombre(), Sesion_RolNombre());
        printf("  ");
        Mostrar_Fecha_Hora();
        printf("--------------------------------------------------\n");

        if (rol == ROL_EMPLEADO)
        {
            printf("  [1]  Consultar mi ficha\n");
            printf("  [2]  Registrar mi asistencia (facial)\n");
            printf("  [3]  Actualizar mis muestras faciales\n");
        }
        else
        {
            printf("  [1]  Registrar empleado\n");
            printf("  [2]  Consultar empleado\n");
            printf("  [3]  Listar empleados\n");
            printf("  [4]  Actualizar empleado\n");

            if (Permisos_PuedeEliminarEmpleado(rol))
                printf("  [5]  Eliminar empleado\n");

            printf("  [6]  Registrar asistencia (facial)\n");
            printf("  [7]  Actualizar muestras faciales\n");
            printf("  [8]  Listar asistencias\n");
        }

        printf("--------------------------------------------------\n");
        printf("  [9]  Regresar al menu principal\n");
        printf("  [0]  Cerrar AXIS\n");
        printf("==================================================\n");
        printf("  %s\n", Permisos_DescripcionRol(rol));
        printf("==================================================\n");
        printf("  Seleccione una opcion: ");

        if (fgets(lineaMenu, sizeof(lineaMenu), stdin) == NULL)
            opcion = -1;
        else if (sscanf(lineaMenu, "%d", &opcion) != 1)
            opcion = -1;

        if (rol == ROL_EMPLEADO)
        {
            switch (opcion)
            {
                case 1: consultarEmpleado(); break;
                case 2: registrarAsistencia(); break;
                case 3: registrarRostro(); break;
                case 9: return 1;
                case 0:
                    clearScreen();
                    printf("Cerrando AXIS...\n");
                    Sleep(2000);
                    return 0;
                default:
                    printf("\nOpcion no valida.\n");
                    Sleep(1500);
                    break;
            }
        }
        else
        {
            switch (opcion)
            {
                case 1: registrarEmpleado(); break;
                case 2: consultarEmpleado(); break;
                case 3: listarEmpleados(); break;
                case 4: actualizarEmpleado(); break;
                case 5: eliminarEmpleado(); break;
                case 6: registrarAsistencia(); break;
                case 7: registrarRostro(); break;
                case 8: listarAsistencias(); break;
                case 9: return 1;
                case 0:
                    clearScreen();
                    printf("Cerrando AXIS...\n");
                    Sleep(2000);
                    return 0;
                default:
                    printf("\nOpcion no valida.\n");
                    Sleep(1500);
                    break;
            }
        }

    } while (opcion != 0);

    return 0;
}

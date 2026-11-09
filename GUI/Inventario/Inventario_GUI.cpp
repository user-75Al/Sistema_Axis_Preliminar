#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Inventario_GUI.h"
#include "../../CTRL/Inventario/Inventario_CTRL.h"
#include "../../BO/Validaciones_BO.h"

int Inventario_Mostrar_Menu(void)
{
    int opcion;
    int codigo;
    int cantidad;
    float precio;
    char nombre[50];
    char id[20];
    int resultado;
    char lineaMenu[64];

    do
    {
        clearScreen();

        printf("\n");
        printf("==================================================\n");
        printf("                      AXIS                        \n");
        printf("==================================================\n");
        printf("  ");
        Mostrar_Fecha_Hora();
        printf("--------------------------------------------------\n");
        printf("                   INVENTARIO                     \n");
        printf("--------------------------------------------------\n");
        printf("  [1]  Registrar producto\n");
        printf("  [2]  Consultar producto (ID, Codigo o Nombre)\n");
        printf("  [3]  Listar productos\n");
        printf("  [4]  Actualizar producto (ID, Codigo o Nombre)\n");
        printf("  [5]  Eliminar producto (ID, Codigo o Nombre)\n");
        printf("  [6]  Entrada de existencias\n");
        printf("  [7]  Salida de existencias\n");
        printf("--------------------------------------------------\n");
        printf("  [9]  Regresar al menu principal\n");
        printf("  [0]  Cerrar AXIS\n");
        printf("==================================================\n");
        printf("  Seleccione una opcion: ");

        if (fgets(lineaMenu, sizeof(lineaMenu), stdin) == NULL)
        {
            opcion = -1;
        }
        else if (sscanf(lineaMenu, "%d", &opcion) != 1)
        {
            opcion = -1;
        }

        switch (opcion)
        {
            case 1:
                clearScreen();
                printf("=== REGISTRAR PRODUCTO ===\n\n");
                Leer_Entero_Seguro("Codigo del producto: ", &codigo, 1, 999999999);

                if (Inventario_CTRL_Existe_Codigo(codigo))
                {
                    printf("\n[Error] El codigo %d ya pertenece a otro producto registrado.\n", codigo);
                    Sleep(2500);
                    break;
                }

                Leer_Cadena_Segura("Nombre del producto: ", nombre, sizeof(nombre));
                Leer_Entero_Seguro("Cantidad inicial: ", &cantidad, 0, 1000000);
                Leer_Flotante_Seguro("Precio: ", &precio, 0.01f, 10000000.0f);

                resultado = Inventario_CTRL_Registrar(codigo, nombre, cantidad, precio);
                if (resultado == 1)
                {
                    printf("\nProducto registrado correctamente.\n");
                }
                else if (resultado == -2)
                {
                    printf("\n[Error] El codigo ya se encuentra registrado.\n");
                }
                else
                {
                    printf("\nNo se pudo registrar el producto.\n");
                }
                Sleep(2000);
                break;

            case 2:
                clearScreen();
                printf("=== CONSULTAR PRODUCTO ===\n\n");
                Leer_Texto_Simple("Ingrese el ID, Codigo o Nombre del producto: ", id, sizeof(id));

                if (Inventario_CTRL_Consultar(id) == 0)
                {
                    printf("\nProducto no encontrado.\n");
                }
                printf("\n");
                system("pause");
                break;

            case 3:
                clearScreen();
                Inventario_CTRL_Listar();
                printf("\n");
                system("pause");
                break;

            case 4:
            {
                clearScreen();
                printf("=== ACTUALIZAR PRODUCTO ===\n\n");
                Leer_Texto_Simple("ID, Codigo o Nombre del producto que desea actualizar: ", id, sizeof(id));

                Producto prodActual;
                if (Inventario_CTRL_Obtener(id, &prodActual) == 0)
                {
                    printf("\nProducto no encontrado.\n");
                    Sleep(2000);
                    break;
                }

                printf("\n--------------------------------------------------\n");
                printf(" Producto encontrado:\n");
                printf("  ID       : %s\n", prodActual.id);
                printf("  Codigo   : %d\n", prodActual.codigo);
                printf("  Nombre   : %s\n", prodActual.nombre);
                printf("  Cantidad : %d unidades\n", prodActual.cantidad);
                printf("  Precio   : $%.2f\n", prodActual.precio);
                printf("--------------------------------------------------\n");
                printf(" Para cada campo: presione [S] para modificarlo,\n");
                printf(" o presione CUALQUIER OTRA TECLA para mantener el dato actual.\n");
                printf("--------------------------------------------------\n");

                if (Preguntar_Modificar_O_Mantener("Nombre", prodActual.nombre))
                {
                    Leer_Cadena_Segura("  Nuevo nombre: ", nombre, sizeof(nombre));
                }
                else
                {
                    strcpy(nombre, prodActual.nombre);
                }

                char strCant[32];
                sprintf(strCant, "%d unidades", prodActual.cantidad);
                if (Preguntar_Modificar_O_Mantener("Cantidad", strCant))
                {
                    Leer_Entero_Seguro("  Nueva cantidad: ", &cantidad, 0, 1000000);
                }
                else
                {
                    cantidad = prodActual.cantidad;
                }

                char strPrecio[32];
                sprintf(strPrecio, "$%.2f", prodActual.precio);
                if (Preguntar_Modificar_O_Mantener("Precio", strPrecio))
                {
                    Leer_Flotante_Seguro("  Nuevo precio: ", &precio, 0.01f, 10000000.0f);
                }
                else
                {
                    precio = prodActual.precio;
                }

                printf("\n--------------------------------------------------\n");
                printf(" Resumen de actualizacion:\n");
                printf("  ID       : %s\n", prodActual.id);
                printf("  Nombre   : %s\n", nombre);
                printf("  Cantidad : %d unidades\n", cantidad);
                printf("  Precio   : $%.2f\n", precio);
                printf("--------------------------------------------------\n");
                printf(" Desea guardar los cambios? (s/n): ");

                char confirmarActualizacion[32];
                if (fgets(confirmarActualizacion, sizeof(confirmarActualizacion), stdin) != NULL &&
                    (confirmarActualizacion[0] == 's' || confirmarActualizacion[0] == 'S'))
                {
                    if (Inventario_CTRL_Actualizar(prodActual.id, nombre, cantidad, precio) == 1)
                    {
                        printf("\nProducto actualizado correctamente.\n");
                    }
                    else
                    {
                        printf("\nNo se pudo actualizar el producto.\n");
                    }
                }
                else
                {
                    printf("\nActualizacion cancelada. Se conservaron los datos originales.\n");
                }

                Sleep(2000);
                break;
            }

            case 5:
            {
                clearScreen();
                printf("=== ELIMINAR PRODUCTO ===\n\n");
                Leer_Texto_Simple("ID, Codigo o Nombre del producto que desea eliminar: ", id, sizeof(id));

                Producto prodEliminar;
                if (Inventario_CTRL_Obtener(id, &prodEliminar) == 0)
                {
                    printf("\nProducto no encontrado.\n");
                    Sleep(2000);
                    break;
                }

                printf("\n--------------------------------------------------\n");
                printf(" Producto a eliminar:\n");
                printf("  ID       : %s\n", prodEliminar.id);
                printf("  Codigo   : %d\n", prodEliminar.codigo);
                printf("  Nombre   : %s\n", prodEliminar.nombre);
                printf("  Cantidad : %d unidades\n", prodEliminar.cantidad);
                printf("--------------------------------------------------\n");
                printf(" Esta seguro de eliminar este producto? (s/n): ");

                char confirmarEliminar[32];
                if (fgets(confirmarEliminar, sizeof(confirmarEliminar), stdin) != NULL &&
                    (confirmarEliminar[0] == 's' || confirmarEliminar[0] == 'S'))
                {
                    if (Inventario_CTRL_Eliminar(prodEliminar.id) == 1)
                    {
                        printf("\nProducto eliminado correctamente.\n");
                    }
                    else
                    {
                        printf("\nNo se pudo eliminar el producto.\n");
                    }
                }
                else
                {
                    printf("\nOperacion cancelada. El producto no fue eliminado.\n");
                }

                Sleep(2000);
                break;
            }

            case 6:
                clearScreen();
                printf("=== ENTRADA DE EXISTENCIAS ===\n\n");
                Leer_Texto_Simple("ID, Codigo o Nombre del producto: ", id, sizeof(id));
                Leer_Entero_Seguro("Cantidad que ingresa: ", &cantidad, 1, 1000000);

                resultado = Inventario_CTRL_Mover_Stock(id, cantidad, 1);
                if (resultado == 1)
                {
                    printf("\nEntrada registrada correctamente.\n");
                }
                else
                {
                    printf("\nProducto no encontrado o error al guardar.\n");
                }
                Sleep(2000);
                break;

            case 7:
                clearScreen();
                printf("=== SALIDA DE EXISTENCIAS ===\n\n");
                Leer_Texto_Simple("ID, Codigo o Nombre del producto: ", id, sizeof(id));
                Leer_Entero_Seguro("Cantidad que sale: ", &cantidad, 1, 1000000);

                resultado = Inventario_CTRL_Mover_Stock(id, cantidad, 0);
                if (resultado == 1)
                {
                    printf("\nSalida registrada correctamente.\n");
                }
                else if (resultado == -1)
                {
                    printf("\nNo hay suficientes existencias.\n");
                }
                else
                {
                    printf("\nProducto no encontrado o error al guardar.\n");
                }
                Sleep(2000);
                break;

            case 9:
                return 1;

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

    } while (opcion != 0);

    return 0;
}

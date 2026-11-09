#ifndef AUDITORIA_DAO_H
#define AUDITORIA_DAO_H

#define RUTA_HISTORIAL "Data/Historial.txt"

int Auditoria_DAO_Contar(void);
int Auditoria_DAO_Listar(void);
int Auditoria_DAO_Buscar(const char criterio[]);
int Auditoria_DAO_FiltrarPorFecha(const char desde[], const char hasta[]);
void Auditoria_DAO_Resumen(void);

#endif // AUDITORIA_DAO_H

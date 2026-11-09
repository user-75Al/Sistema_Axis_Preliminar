#ifndef AUDITORIA_CTRL_H
#define AUDITORIA_CTRL_H

int Auditoria_CTRL_Listar(void);
int Auditoria_CTRL_Buscar(const char criterio[]);
int Auditoria_CTRL_FiltrarPorFecha(const char desde[], const char hasta[]);
int Auditoria_CTRL_Resumen(void);
int Auditoria_CTRL_Contar(void);

#endif // AUDITORIA_CTRL_H

#ifndef COMPATIBILIDAD_H
#define COMPATIBILIDAD_H

#include <cstddef>
#include <cctype>

/*
 * Comparaciones de texto sin distinguir mayusculas/minusculas.
 * Se implementan aqui para que el codigo funcione igual con -std=c++14
 * y -std=gnu++14, sin depender de stricmp/strnicmp del compilador.
 */
static inline const char *axis_saltar_bom(const char *texto)
{
    if (texto == NULL)
        return NULL;

    if ((unsigned char)texto[0] == 0xEF &&
        (unsigned char)texto[1] == 0xBB &&
        (unsigned char)texto[2] == 0xBF)
    {
        return texto + 3;
    }

    return texto;
}

static inline int axis_stricmp(const char *a, const char *b)
{
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;

    while (*a != '\0' && *b != '\0')
    {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);

        if (ca != cb)
            return ca - cb;

        a++;
        b++;
    }

    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static inline int axis_strnicmp(const char *a, const char *b, size_t cantidad)
{
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;

    for (size_t i = 0; i < cantidad; i++)
    {
        int ca = tolower((unsigned char)a[i]);
        int cb = tolower((unsigned char)b[i]);

        if (ca != cb)
            return ca - cb;

        if (ca == '\0')
            return 0;
    }

    return 0;
}

#endif // COMPATIBILIDAD_H

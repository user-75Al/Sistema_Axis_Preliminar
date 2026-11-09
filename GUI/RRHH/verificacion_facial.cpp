#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "verificacion_facial.h"
#include "../../BO/Compatibilidad.h"

#ifdef AXIS_HAVE_OPENCV

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

#define CARPETA_ROSTROS   "Data\\Rostros"
#define TAMANO_ROSTRO     200
#define UMBRAL_LBPH       70.0

// ---------------------------------------------------------------------------
// Utilidades de sistema de archivos (Windows / MinGW, sin std::filesystem)
// ---------------------------------------------------------------------------
static bool existeArchivo(const std::string &ruta)
{
    FILE *archivo = fopen(ruta.c_str(), "rb");
    if (archivo != NULL)
    {
        fclose(archivo);
        return true;
    }
    return false;
}

static bool existeDirectorio(const std::string &ruta)
{
    DWORD atributos = GetFileAttributesA(ruta.c_str());
    return atributos != INVALID_FILE_ATTRIBUTES &&
           (atributos & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static void crearDirectorio(const std::string &ruta)
{
    if (!existeDirectorio(ruta))
        CreateDirectoryA(ruta.c_str(), NULL);
}

static std::string aMinusculas(const std::string &texto)
{
    std::string salida = texto;
    for (size_t i = 0; i < salida.size(); i++)
        salida[i] = (char)tolower((unsigned char)salida[i]);
    return salida;
}

static bool esImagen(const std::string &nombre)
{
    std::string minusculas = aMinusculas(nombre);
    size_t punto = minusculas.find_last_of('.');

    if (punto == std::string::npos)
        return false;

    std::string extension = minusculas.substr(punto);
    return extension == ".png" || extension == ".jpg" || extension == ".jpeg";
}

static std::vector<std::string> listarCarpetas(const std::string &rutaBase)
{
    std::vector<std::string> carpetas;
    WIN32_FIND_DATAA datos;
    HANDLE busqueda;

    busqueda = FindFirstFileA((rutaBase + "\\*").c_str(), &datos);
    if (busqueda == INVALID_HANDLE_VALUE)
        return carpetas;

    do
    {
        if ((datos.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
            strcmp(datos.cFileName, ".") != 0 &&
            strcmp(datos.cFileName, "..") != 0)
        {
            carpetas.push_back(datos.cFileName);
        }
    } while (FindNextFileA(busqueda, &datos) != 0);

    FindClose(busqueda);
    std::sort(carpetas.begin(), carpetas.end());
    return carpetas;
}

static std::vector<std::string> listarImagenes(const std::string &rutaBase)
{
    std::vector<std::string> imagenes;
    WIN32_FIND_DATAA datos;
    HANDLE busqueda;

    busqueda = FindFirstFileA((rutaBase + "\\*").c_str(), &datos);
    if (busqueda == INVALID_HANDLE_VALUE)
        return imagenes;

    do
    {
        if ((datos.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
            esImagen(datos.cFileName))
        {
            imagenes.push_back(datos.cFileName);
        }
    } while (FindNextFileA(busqueda, &datos) != 0);

    FindClose(busqueda);
    std::sort(imagenes.begin(), imagenes.end());
    return imagenes;
}

static bool identificadorValido(const char *idEmpleado)
{
    size_t longitud;

    if (idEmpleado == NULL)
        return false;

    longitud = strlen(idEmpleado);
    if (longitud == 0 || longitud > 19)
        return false;

    for (size_t i = 0; i < longitud; i++)
    {
        if (!isalnum((unsigned char)idEmpleado[i]) && idEmpleado[i] != '-')
            return false;
    }

    return true;
}

static std::string buscarCascada(void)
{
    const char *candidatos[] = {
        "Data\\haarcascade_frontalface_default.xml",
        "haarcascade_frontalface_default.xml",
        "C:\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_default.xml",
        "C:\\opencv\\data\\haarcascades\\haarcascade_frontalface_default.xml",
        "C:\\msys64\\mingw64\\share\\opencv4\\haarcascades\\haarcascade_frontalface_default.xml",
        "C:\\msys64\\mingw64\\share\\opencv5\\haarcascades\\haarcascade_frontalface_default.xml",
        "C:\\msys64\\mingw64\\share\\opencv\\haarcascades\\haarcascade_frontalface_default.xml",
        "C:\\msys64\\mingw64\\etc\\haarcascades\\haarcascade_frontalface_default.xml"
    };

    for (size_t i = 0; i < sizeof(candidatos) / sizeof(candidatos[0]); i++)
    {
        if (existeArchivo(candidatos[i]))
            return candidatos[i];
    }

    const char *opencvDir = getenv("OPENCV_DIR");
    if (opencvDir != NULL && opencvDir[0] != '\0')
    {
        std::string base(opencvDir);
        std::string opciones[] = {
            base + "\\etc\\haarcascades\\haarcascade_frontalface_default.xml",
            base + "\\..\\etc\\haarcascades\\haarcascade_frontalface_default.xml",
            base + "\\share\\opencv4\\haarcascades\\haarcascade_frontalface_default.xml",
            base + "\\share\\opencv5\\haarcascades\\haarcascade_frontalface_default.xml"
        };

        for (size_t i = 0; i < sizeof(opciones) / sizeof(opciones[0]); i++)
        {
            if (existeArchivo(opciones[i]))
                return opciones[i];
        }
    }

    return "";
}

static cv::Mat prepararRostro(const cv::Mat &rostro)
{
    cv::Mat redimensionado;
    cv::Mat ecualizado;

    cv::resize(rostro, redimensionado, cv::Size(TAMANO_ROSTRO, TAMANO_ROSTRO));
    cv::equalizeHist(redimensionado, ecualizado);
    return ecualizado;
}

static int contarMuestras(const std::string &carpeta)
{
    std::vector<std::string> archivos = listarImagenes(carpeta);
    int maximo = 0;

    for (size_t i = 0; i < archivos.size(); i++)
    {
        const char *nombre = archivos[i].c_str();
        if (strncmp(nombre, "muestra_", 8) == 0)
        {
            int numero = atoi(nombre + 8);
            if (numero > maximo)
                maximo = numero;
        }
    }

    return maximo;
}

struct ModeloFacial
{
    cv::Ptr<cv::face::LBPHFaceRecognizer> reconocedor;
    std::map<int, std::string> empleados;
};

static bool cargarModelo(ModeloFacial &modelo)
{
    std::vector<cv::Mat> imagenes;
    std::vector<int> etiquetas;
    std::vector<std::string> carpetas;

    if (!existeDirectorio(CARPETA_ROSTROS))
    {
        printf("[Facial] No existen muestras faciales registradas.\n");
        return false;
    }

    carpetas = listarCarpetas(CARPETA_ROSTROS);
    int etiqueta = 0;

    for (size_t i = 0; i < carpetas.size(); i++)
    {
        std::string carpeta = std::string(CARPETA_ROSTROS) + "\\" + carpetas[i];
        std::vector<std::string> archivos = listarImagenes(carpeta);
        int muestras = 0;

        for (size_t j = 0; j < archivos.size(); j++)
        {
            std::string ruta = carpeta + "\\" + archivos[j];
            cv::Mat imagen = cv::imread(ruta, cv::IMREAD_GRAYSCALE);

            if (imagen.empty())
                continue;

            imagenes.push_back(prepararRostro(imagen));
            etiquetas.push_back(etiqueta);
            muestras++;
        }

        if (muestras > 0)
        {
            modelo.empleados[etiqueta] = carpetas[i];
            etiqueta++;
        }
    }

    if (imagenes.empty())
    {
        printf("[Facial] No se encontraron muestras faciales validas.\n");
        return false;
    }

#if CV_VERSION_MAJOR >= 4
    modelo.reconocedor = cv::face::LBPHFaceRecognizer::create();
#else
    modelo.reconocedor = cv::face::createLBPHFaceRecognizer();
#endif

    modelo.reconocedor->train(imagenes, etiquetas);
    return true;
}

static bool empleadoTieneMuestras(const ModeloFacial &modelo, const char *idEmpleado)
{
    for (std::map<int, std::string>::const_iterator it = modelo.empleados.begin();
         it != modelo.empleados.end(); ++it)
    {
        if (axis_stricmp(it->second.c_str(), idEmpleado) == 0)
            return true;
    }
    return false;
}

int Facial_Disponible(void)
{
    return 1;
}

int Facial_Registrar(const char idEmpleado[])
{
    try
    {
        if (!identificadorValido(idEmpleado))
        {
            printf("[Facial] Identificador de empleado invalido.\n");
            return 0;
        }

        crearDirectorio("Data");
        crearDirectorio(CARPETA_ROSTROS);

        std::string carpetaEmpleado = std::string(CARPETA_ROSTROS) + "\\" + idEmpleado;
        crearDirectorio(carpetaEmpleado);

        std::string rutaCascada = buscarCascada();
        if (rutaCascada.empty())
        {
            printf("[Facial] No se encontro haarcascade_frontalface_default.xml.\n");
            printf("         Copielo en Data\\ o defina la variable OPENCV_DIR.\n");
            return 0;
        }

        cv::CascadeClassifier detector;
        if (!detector.load(rutaCascada))
        {
            printf("[Facial] No se pudo cargar el detector facial: %s\n", rutaCascada.c_str());
            return 0;
        }

        cv::VideoCapture camara(0);
        if (!camara.isOpened())
        {
            printf("[Facial] No se pudo abrir la camara.\n");
            return 0;
        }

        int contador = contarMuestras(carpetaEmpleado);

        printf("\nRegistro facial de AXIS\n");
        printf("Coloque un solo rostro frente a la camara.\n");
        printf("ESPACIO: guardar muestra | ESC: terminar\n\n");

        while (true)
        {
            cv::Mat imagen;
            camara >> imagen;

            if (imagen.empty())
            {
                printf("[Facial] No se pudo capturar la imagen.\n");
                break;
            }

            cv::Mat gris;
            cv::cvtColor(imagen, gris, cv::COLOR_BGR2GRAY);

            std::vector<cv::Rect> rostros;
            detector.detectMultiScale(gris, rostros, 1.2, 5, 0, cv::Size(80, 80));

            for (size_t i = 0; i < rostros.size(); i++)
            {
                cv::rectangle(imagen, rostros[i], cv::Scalar(0, 255, 0), 2);
            }

            char texto[80];
            sprintf(texto, "Muestras guardadas: %d", contador);
            cv::putText(imagen, texto, cv::Point(10, 30),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

            cv::imshow("Registro facial - AXIS", imagen);
            int tecla = cv::waitKey(1) & 0xFF;

            if (tecla == 27)
                break;

            if (tecla == 32)
            {
                if (rostros.size() != 1)
                {
                    printf("[Facial] Debe aparecer exactamente un rostro.\n");
                    continue;
                }

                cv::Mat rostro = prepararRostro(gris(rostros[0]));
                int siguiente = contador + 1;

                char nombreArchivo[64];
                sprintf(nombreArchivo, "muestra_%02d.png", siguiente);

                std::string ruta = carpetaEmpleado + "\\" + nombreArchivo;

                if (cv::imwrite(ruta, rostro))
                {
                    contador = siguiente;
                    printf("[Facial] Muestra guardada: %s\n", ruta.c_str());
                }
                else
                {
                    printf("[Facial] No se pudo guardar la muestra.\n");
                }
            }
        }

        camara.release();
        cv::destroyAllWindows();

        printf("[Facial] Total de muestras del empleado: %d\n", contador);
        return (contador > 0) ? 1 : 0;
    }
    catch (const cv::Exception &error)
    {
        cv::destroyAllWindows();
        printf("[Facial] Error OpenCV: %s\n", error.what());
        return 0;
    }
}

int Facial_Verificar(const char idEmpleado[])
{
    try
    {
        if (!identificadorValido(idEmpleado))
        {
            printf("[Facial] Identificador de empleado invalido.\n");
            return 0;
        }

        ModeloFacial modelo;
        if (!cargarModelo(modelo))
            return 0;

        if (!empleadoTieneMuestras(modelo, idEmpleado))
        {
            printf("[Facial] El empleado no tiene muestras faciales registradas.\n");
            return 0;
        }

        std::string rutaCascada = buscarCascada();
        if (rutaCascada.empty())
        {
            printf("[Facial] No se encontro haarcascade_frontalface_default.xml.\n");
            return 0;
        }

        cv::CascadeClassifier detector;
        if (!detector.load(rutaCascada))
        {
            printf("[Facial] No se pudo cargar el detector facial.\n");
            return 0;
        }

        cv::VideoCapture camara(0);
        if (!camara.isOpened())
        {
            printf("[Facial] No se pudo abrir la camara.\n");
            return 0;
        }

        bool autorizado = false;

        printf("\nVerificacion facial de AXIS\n");
        printf("Coloque su rostro frente a la camara.\n");
        printf("ESPACIO: verificar | ESC: cancelar\n\n");

        while (true)
        {
            cv::Mat imagen;
            camara >> imagen;

            if (imagen.empty())
            {
                printf("[Facial] No se pudo capturar la imagen.\n");
                break;
            }

            cv::Mat gris;
            cv::cvtColor(imagen, gris, cv::COLOR_BGR2GRAY);

            std::vector<cv::Rect> rostros;
            detector.detectMultiScale(gris, rostros, 1.2, 5, 0, cv::Size(80, 80));

            for (size_t i = 0; i < rostros.size(); i++)
            {
                cv::rectangle(imagen, rostros[i], cv::Scalar(0, 255, 0), 2);
            }

            cv::imshow("Verificacion facial - AXIS", imagen);
            int tecla = cv::waitKey(1) & 0xFF;

            if (tecla == 27)
            {
                printf("[Facial] Verificacion cancelada.\n");
                break;
            }

            if (tecla == 32)
            {
                if (rostros.size() != 1)
                {
                    printf("[Facial] Debe aparecer exactamente un rostro.\n");
                    continue;
                }

                cv::Mat rostro = prepararRostro(gris(rostros[0]));
                int etiqueta = -1;
                double distancia = 0.0;

                modelo.reconocedor->predict(rostro, etiqueta, distancia);

                std::map<int, std::string>::const_iterator encontrado =
                    modelo.empleados.find(etiqueta);

                if (encontrado == modelo.empleados.end())
                {
                    printf("[Facial] Rostro no reconocido.\n");
                    break;
                }

                printf("[Facial] Distancia de reconocimiento: %.2f\n", distancia);

                if (axis_stricmp(encontrado->second.c_str(), idEmpleado) == 0 &&
                    distancia <= UMBRAL_LBPH)
                {
                    printf("[Facial] Rostro reconocido para el empleado %s.\n", idEmpleado);
                    autorizado = true;
                }
                else
                {
                    printf("[Facial] Verificacion rechazada.\n");
                }

                break;
            }
        }

        camara.release();
        cv::destroyAllWindows();

        return autorizado ? 1 : 0;
    }
    catch (const cv::Exception &error)
    {
        cv::destroyAllWindows();
        printf("[Facial] Error OpenCV: %s\n", error.what());
        return 0;
    }
}

#else // AXIS_HAVE_OPENCV

int Facial_Disponible(void)
{
    return 0;
}

int Facial_Registrar(const char idEmpleado[])
{
    (void)idEmpleado;
    printf("\n[Facial] Este binario se compilo sin OpenCV.\n");
    printf("         Instale OpenCV y recompile con compilar.bat para usar la biometria.\n");
    return 0;
}

int Facial_Verificar(const char idEmpleado[])
{
    (void)idEmpleado;
    printf("\n[Facial] Este binario se compilo sin OpenCV.\n");
    printf("         Instale OpenCV y recompile con compilar.bat para usar la biometria.\n");
    return 0;
}

#endif // AXIS_HAVE_OPENCV

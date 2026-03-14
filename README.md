## Documentación: Creación y Manejo de Hilos (Threads)

**Conceptos Clave Investigados**

- **Creación de Hilos:** En lugar de crear procesos completamente aislados, en este laboratorio usamos múltiples threads para procesar partes de un archivo de registro en paralelo. Esto lo implementamos en el archivo `main_linux.c` utilizando la biblioteca de subprocesos POSIX (pthreads). Cada hilo se encarga de procesar un fragmento específico del archivo de los logs, lo que permite aprovechar mejor los recursos del sistema y reducir bastante el tiempo de ejecución.

- **Sincronización y Fusión de Resultados:** Como los hilos trabajan de manera concurrente leyendo chunks del archivo, necesitamos unificar la información al finalizar. El programa requiere sincronización para mezclar y combinar los resultados obtenidos por cada uno de los hilos de manera segura. Usamos un `pthread_mutex_t` que protege todas las actualizaciones a los hashmaps compartidos y a los contadores globales.

- **Estructuras de Datos:** Debido a que un servidor web puede generar archivos con millones de registros, el programa debe optimizar su rendimiento utilizando estructuras de datos eficientes, como las hash tables, ya que esto permite contar rápidamente las direcciones IP, las URLs y los errores.

---

**Estructura del Proyecto**

```
├── main_linux.c       (Es el programa principal para Linux usando pthreads)
├── log_processor.h    (Header con prototipos y struct LogEntry)
├── log_processor.c    (Parseo de cada línea del log con sscanf)
├── hashmap.h          (Header del hashmap)
├── hashmap.c          (Implementación del hashmap con encadenamiento)
├── Makefile            (Compilación para Linux)
├── run.sh             (Script para compilar y ejecutar en Linux)
├── access.log         (Archivo de log grande: 5000 registros)
└── access_short.log   (Archivo de log pequeño: 8 registros)
```

---

**Output esperado**

Para el archivo `access_short.log`:

```
Total Unique IPs: 5
Most Visited URL: /login (3 times)
HTTP Errors: 7
```

Para el archivo `access.log` (5000 registros):

```
Total Unique IPs: 5
Most Visited URL: /login (873 times)
HTTP Errors: 3708
```

---

**Preguntas de la práctica:**

- **Cómo funciona el procesamiento multithread:** El programa divide el archivo en `N` fragmentos de bytes iguales (uno por hilo). Cada hilo abre su propia instancia del archivo y hace `fseek` a su posición de inicio. Si el inicio cae en mitad de una línea, el hilo descarta esa línea incompleta (ya procesada por el hilo anterior). Cada hilo lee y procesa sus líneas de forma independiente, y registra los resultados en las estructuras compartidas usando un mutex. Una vez que todos los hilos terminan (`pthread_join`), el hilo principal imprime los resultados.

- **Métricas a extraer:** A partir de los registros de peticiones HTTP, extrae tres resultados:
    1. El número total de direcciones IP únicas.
    2. La URL visitada con mayor frecuencia (y su conteo).
    3. El número total de errores HTTP (códigos de estado entre 400 y 599).

- **Rendimiento:** Comparamos el tiempo de ejecución entre una versión de un solo hilo (`THREADS_COUNT = 1`) y la versión de 4 hilos (`THREADS_COUNT = 4`) sobre el archivo `access.log` que tiene 5000 registros:

| Versión       | Tiempo real |
|---------------|-------------|
| 1 hilo        | ~0.21 s     |
| 4 hilos       | ~0.01 s     |
| **Speedup**   | **~21x**    |

Por lo que la verrsion multithread es mucho más rápida, ya que aprovecha múltiples núcleos del CPU 

---

**Links que usamos para investigar:**
- [_POSIX Threads (pthreads) tutorial in C_](https://stackoverflow.com/questions/74211121/c-pthreads-creating-and-joining-specific-threads-without-waiting)
- [_Windows API CreateThread documentation_](https://www.geeksforgeeks.org/linux-unix/mutex-lock-for-linux-thread-synchronization/)
- [_Mutex locks and synchronization in C_](https://www.reddit.com/r/C_Programming/comments/6cak4v/trying_to_readwrite_to_file_using_multithreading/)
- [_How to implement Hash Tables in C_](https://andreipall.github.io/c/how-to-read-a-large-file-in-c-on-linux/)
## Documentación: Creación y Manejo de Hilos (Threads)

**Conceptos Clave Investigados**

- **Creación de Hilos:** En lugar de crear procesos completamente aislados, este laboratorio utiliza múltiples hilos de ejecución para procesar partes de un archivo de registro en paralelo. En Linux, esto se implementa en el archivo `main_linux.c` utilizando la biblioteca de subprocesos POSIX (pthreads). Para Windows, se utiliza la API de Windows en el archivo `main_windows.c`.
    
- **Sincronización y Fusión de Resultados:** Como los hilos trabajan de manera concurrente leyendo fragmentos (chunks) del archivo, es necesario unificar la información al finalizar. El programa requiere sincronización para mezclar y combinar los resultados obtenidos por cada uno de los hilos de manera segura.
    
- **Estructuras de Datos Eficientes:** Debido a que un servidor web puede generar archivos con millones de registros , el programa debe optimizar su rendimiento utilizando estructuras de datos eficientes, como las tablas hash (hash tables). Esto permite contar rápidamente las direcciones IP, las URLs y los errores.
    

**Links que usamos para investigar:**
- _POSIX Threads (pthreads) tutorial in C_
- _Windows API CreateThread documentation_
- _Mutex locks and synchronization in C_
- _How to implement Hash Tables in C_
- _Reading large text files in chunks efficiently_
    

**Preguntas del documento (instrucciones de la práctica):**

- **Cómo funciona el procesamiento multihilo:** El objetivo principal de la práctica es enseñar cómo utilizar múltiples hilos para procesar de manera eficiente archivos de texto grandes. El programa divide el trabajo de forma que cada hilo se encarga de procesar una porción del archivo de registro en paralelo. Al paralelizar este cálculo, se busca reducir drásticamente el tiempo de ejecución.
    
- **Cuáles son las métricas a extraer:** El analizador procesa los registros de peticiones HTTP y debe extraer tres resultados finales:
    1. El recuento total de peticiones realizadas por cada dirección IP única.
    2. La identificación de la URL que ha sido visitada con mayor frecuencia.
    3. El número total de errores HTTP, que corresponden específicamente a los códigos de estado entre 400 y 599.
        
- **Cómo se evalúa el rendimiento:** Para entender los beneficios reales del procesamiento multihilo, el programa final debe probarse suministrando archivos de registro tanto pequeños como grandes. Como paso final, se debe realizar una comparación directa del tiempo de ejecución entre la versión del programa que utiliza un solo hilo (single-threaded) frente a la que utiliza múltiples hilos (multi-threaded).
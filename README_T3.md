# Informe Tarea 3

**Autor**: Diego Troncoso Bustamante

---

## Introducción

En esta tarea se implementaron las funciones `mprotect` y `munprotect` en xv6, para gestionar la protección de memoria en el sistema operativo. Estas funciones permiten modificar permisos de escritura en regiones de memoria específicas, implementando un control de solo lectura en las páginas de memoria y restaurando los permisos de escritura cuando es necesario.

## Errores Iniciales

Durante el desarrollo, se encontró un problema relacionado con `spinlock.h`, el cual generaba errores en la compilación. La solución fue agregar la inclusión de `spinlock.h` en otros archivos de kernel, como `proc.h`, para asegurar que `struct spinlock` se definiera correctamente antes de su uso en varias partes del sistema.

## Modificaciones Realizadas

### Carpeta `Kernel`

#### Archivo `proc.h`

Se añadieron las siguientes líneas para declarar las nuevas funciones y manejar el `spinlock`:

```c
#include "spinlock.h"

int mprotect(void *addr, int len);
int munprotect(void *addr, int len);

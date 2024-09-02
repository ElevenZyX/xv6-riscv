
# Informe Tarea 1

**Diego Troncoso Bustamante**

## Introducción

La tarea consistió en implementar dos funciones: `getppid()` y `getancestor()`. Para ello, fue necesario hacer modificaciones en varios archivos del sistema operativo educativo xv6. Al principio, cada vez que ejecutaba el comando `make qemu`, se producían algunos errores durante la inicialización de xv6. Sin embargo, a medida que corregía los parámetros, los errores iban desapareciendo. A continuación, se detallan los cambios realizados y su finalidad.

## Modificaciones realizadas

### Carpeta `kernel`

#### Archivo `syscall.c`

Se añadieron las siguientes líneas de código:

```c
extern uint64 sys_getppid(void);      
extern uint64 sys_getancestor(void); 
[SYS_getppid] sys_getppid,       
[SYS_getancestor] sys_getancestor,
```

Estas líneas son cruciales para el funcionamiento del sistema de llamadas al sistema en xv6. Las declaraciones externas (`extern`) aseguran que las funciones que implementan las llamadas al sistema estén disponibles para ser vinculadas durante la compilación. Por otro lado, las asignaciones en el array de punteros a funciones conectan el número de la llamada al sistema con su implementación específica, permitiendo que el kernel ejecute el código correcto cuando se invoca una llamada al sistema desde un proceso.

#### Archivo `syscall.h`

Se añadieron las siguientes definiciones:

```c
#define SYS_getppid 22 
#define SYS_getancestor 23  
```

Las definiciones `#define SYS_getppid 22` y `#define SYS_getancestor 23` son esenciales para el funcionamiento del sistema de llamadas al sistema en xv6. Asocian un número único a cada llamada al sistema, que el kernel utiliza para identificar la función que debe ejecutar. Estas definiciones permiten que el kernel maneje de manera eficiente múltiples llamadas al sistema al utilizar un simple número como referencia para cada una de ellas.

#### Archivo `sysproc.c`

Se añadieron las siguientes funciones:

```c
int
sys_getppid(void)
{
  struct proc *p = myproc();
  if (p->parent)
      return p->parent->pid;
  return -1;  // En caso de que no haya un proceso padre
}

int
sys_getancestor(void)
{
    int n;
    struct proc *p = myproc();

    // Obtener el valor de n pasado como argumento
    argint(0, &n);

    for (int i = 0; i < n; i++) {
        if (p->parent != 0)
            p = p->parent;
        else
            return -1;  // No hay más ancestros
    }

    return p->pid;
}
```

- **`sys_getppid()`**: Implementa la llamada al sistema `getppid()` y devuelve el PID del proceso padre del proceso actual. Si no hay un proceso padre, devuelve `-1`.
- **`sys_getancestor(int n)`**: Implementa la llamada al sistema `getancestor(int)` y devuelve el PID del ancestro en el nivel especificado por `n`. Si `n` es mayor que el número de ancestros disponibles, devuelve `-1`.

Ambas funciones son esenciales para gestionar y explorar la jerarquía de procesos en xv6, proporcionando a los programas la capacidad de consultar su relación con otros procesos en el sistema.

### Carpeta `user`

#### Archivo `user.h`

Se añadieron las siguientes declaraciones:

```c
int getppid(void);        
int getancestor(int n);  
```

Las declaraciones `int getppid(void);` y `int getancestor(int n);` en `user.h` exponen las llamadas al sistema `getppid` y `getancestor` a los programas de usuario en xv6. Esto permite que los programas de usuario consulten el PID de su proceso padre y de otros ancestros en la jerarquía de procesos, proporcionando una interfaz simple y directa para interactuar con la estructura de procesos del sistema operativo.

### Creación de `yosoytupadre.c`

Se creó el siguiente código:

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int n;

    printf("Identificación del proceso: %d
", getpid());
    printf("Identificación del proceso padre: %d
", getppid());

    printf("Ingresa el nivel de ancestro que deseas (0 para ti mismo, 1 para el padre, etc.): ");
    char buf[10];
    gets(buf, sizeof(buf));
    n = atoi(buf);

    if (n < 0) {
        printf("El nivel %d no es válido.
", n);
        exit(1);
    }

    int ancestor = getancestor(n);
    if (ancestor < 0) {
        printf("(-1) No hay más ancestros en el nivel %d
", n);
    } else {
        printf("Ancestro en nivel %d: Identificación del ancestro: %d
", n, ancestor);
    }

    exit(0);
}
```

Este código cumple con lo solicitado en la tarea 1, retornando el ID del padre cada vez que se ejecuta y solicitando al usuario el nivel del ancestro que desea ver. Si el ancestro no existe, retorna `-1` y muestra un mensaje indicando que no existe un ancestro para ese nivel.

#### Modificación del `Makefile`

Se añadió la siguiente línea para compilar el programa `yosoytupadre.c`:

```makefile
$U/_yosoytupadre
```

### Ejemplos de ejecución:

#### Ejemplo 1:

```bash
$ yosoytupadre
Identificación del proceso: 3
Identificación del proceso padre: 2
Ingresa el nivel de ancestro que deseas (0 para ti mismo, 1 para el padre, etc.): 5
(-1) No hay más ancestros en el nivel 5
```

#### Ejemplo 2:

```bash
$ yosoytupadre
Identificación del proceso: 3
Identificación del proceso padre: 2
Ingresa el nivel de ancestro que deseas (0 para ti mismo, 1 para el padre, etc.): 1
Ancestro en nivel 1: Identificación del ancestro: 2
```

## Conclusiones

La implementación del programa fue un éxito y representó un desafío interesante. Cada vez que ejecutaba `make qemu`, era necesario verificar que todo estuviera en orden e investigar qué archivos debían ser modificados para que `yosoytupadre.c` funcionara correctamente. Esta tarea me permitió comprender mejor el funcionamiento del sistema operativo xv6, especialmente en lo que respecta a la implementación y gestión de llamadas al sistema. La experiencia adquirida al manipular y modificar el kernel de xv6 es invaluable para el entendimiento profundo de cómo operan los sistemas operativos a bajo nivel.

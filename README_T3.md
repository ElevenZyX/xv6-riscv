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
```
>Estas líneas declaran las funciones `mprotect` y `munprotect`, que serán responsables de manejar la protección de memoria para el proceso.

#### Archivo `spinlock.h`


Se realizó la siguiente modificación para definir `spinlock` con los guards necesarios, y se añadieron las funciones de manejo de locks:

```c
#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

// Mutual exclusion lock.
struct spinlock {
  uint locked;        // Indica si el lock está activo.

  // Para depuración:
  char *name;         // Nombre del lock.
  struct cpu *cpu;    // La CPU que tiene el lock actualmente.
};

// Declaraciones de funciones para manipular spinlocks
void initlock(struct spinlock *lock, char *name);
void acquire(struct spinlock *lock);
void release(struct spinlock *lock);
int holding(struct spinlock *lock);

#endif // SPINLOCK_H
```

Esta estructura permite la creación y manipulación de locks de exclusión mutua en xv6.

#### Archivo `vm.c`

En el archivo `vm.c`, se agregó `#include "proc.h"` al inicio para poder utilizar la estructura de proceso. También se implementaron las funciones `mprotect` y `munprotect` para manejar la protección de las páginas. Las funciones son las siguientes:

```c
int mprotect(void *addr, int len) {
    struct proc *p = myproc();  // Proceso actual
    uint64 start = PGROUNDDOWN((uint64)addr);  // Redondear dirección inicial
    uint64 end = start + len;

    // Verificación 1: Dirección o longitud inválida
    if (len <= 0 || start >= p->sz || end > p->sz) {
        return -1; // Error si la longitud es no positiva o si las direcciones no son válidas
    }

    for (uint64 a = start; a < end; a += PGSIZE) {
        pte_t *pte = walk(p->pagetable, a, 0); // Buscar la entrada PTE
        if (pte == 0) {
            return -1; // Error si la página no está mapeada
        }
        *pte &= ~PTE_W; // Desactivar el bit de escritura (solo lectura)
    }
    return 0;
}

int munprotect(void *addr, int len) {
    struct proc *p = myproc();  // Proceso actual
    uint64 start = PGROUNDDOWN((uint64)addr);  // Redondear dirección inicial
    uint64 end = start + len;

    // Verificación 1: Dirección o longitud inválida
    if (len <= 0 || start >= p->sz || end > p->sz) {
        return -1; // Error si la longitud es no positiva o si las direcciones no son válidas
    }

    for (uint64 a = start; a < end; a += PGSIZE) {
        pte_t *pte = walk(p->pagetable, a, 0); // Buscar la entrada PTE
        if (pte == 0) {
            return -1; // Error si la página no está mapeada
        }
        *pte |= PTE_W; // Activar el bit de escritura (lectura-escritura)
    }
    return 0;
}
```

Estas funciones manipulan el bit `PTE_W` en la tabla de páginas para permitir o bloquear la escritura en una región de memoria específica.

#### Archivo `syscall.h`

Se agregaron los códigos de sistema para las nuevas llamadas:

```c
#define SYS_mprotect  22
#define SYS_munprotect  23
```

#### Archivo `syscall.c`

Se declararon las funciones y se registraron en el array de llamadas al sistema:

```c
uint64 sys_mprotect(void);
uint64 sys_munprotect(void);

[SYS_mprotect]  sys_mprotect,
[SYS_munprotect] sys_munprotect,
```

#### Archivo `sysproc.c`

Se implementaron las funciones `sys_mprotect` y `sys_munprotect`, que invocan las funciones de kernel `mprotect` y `munprotect`:

```c
uint64 sys_mprotect(void) {
    uint64 addr;
    int len;
    argaddr(0, &addr);
    argint(1, &len);
    return mprotect((void *)addr, len);
}

uint64 sys_munprotect(void) {
    uint64 addr;
    int len;
    argaddr(0, &addr);
    argint(1, &len);
    return munprotect((void *)addr, len);
}
```

### Carpeta `User`

#### Archivo `usys.pl`

Se añadieron las entradas de `mprotect` y `munprotect` para que puedan ser llamadas desde el espacio de usuario:

```plaintext
entry("mprotect");  
entry("munprotect");
```

#### Archivo `user.h`

Se declararon las funciones `mprotect` y `munprotect` para su uso en los programas de usuario:

```c
int mprotect(void *addr, int len);
int munprotect(void *addr, int len);
```

## Código de Prueba

Se creó el siguiente programa para probar `mprotect` y `munprotect`:

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    // Alocar memoria con sbrk
    char *mem = sbrk(4096);  // Alocar una página

    // Verificar si la memoria es escribible
    printf("Escribiendo en la memoria antes de mprotect...\n");
    mem[0] = 'A';
    printf("Escribió correctamente en memoria antes de mprotect: %c\n", mem[0]);

    // Llamar a mprotect para hacer la página de solo lectura
    if (mprotect(mem, 4096) < 0) {
        printf("Error: mprotect falló\n");
        exit(1);
    }

    // Intentar escribir en la memoria protegida (esto debería fallar)
    printf("Intentando escribir en la memoria después de mprotect (debería fallar)...\n");
    if (fork() == 0) {  // Hacemos un fork para que el proceso hijo falle y no afecte al padre
        mem[0] = 'B';  // Esto debería causar una falla de segmentación
        printf("Error: se escribió en memoria protegida (esto no debería imprimirse)\n");
        exit(1);
    } else {
        wait(0);  // Espera al proceso hijo
    }

    // Llamar a munprotect para restaurar el permiso de escritura
    if (munprotect(mem, 4096) < 0) {
        printf("Error: munprotect falló\n");
        exit(1);
    }

    // Intentar escribir en la memoria nuevamente después de munprotect
    printf("Intentando escribir en la memoria después de munprotect...\n");
    mem[0] = 'C';
    printf("Escribió correctamente en memoria después de munprotect: %c\n", mem[0]);

    printf("Prueba completada.\n");
    exit(0);
}
```

Este programa aloca una página de memoria y la protege con `mprotect`, verificando que escribir en ella causa un error. Después, llama a `munprotect` y confirma que se puede escribir nuevamente en la página.

## Resultados de la Ejecución

Al ejecutar el código de prueba `test_protect`, se obtuvo el siguiente resultado:

```plaintext
Escribiendo en la memoria antes de mprotect...
Escribió correctamente en memoria antes de mprotect: A
Intentando escribir en la memoria después de mprotect (debería fallar)...
usertrap(): unexpected scause 0xf pid=4
            sepc=0x58 stval=0x4000
Intentando escribir en la memoria después de munprotect...
Escribió correctamente en memoria después de munprotect: C
Prueba completada.
```

## Conclusión

En esta tarea, se implementaron las funciones de protección de memoria `mprotect` y `munprotect`, lo cual permitió aprender sobre la manipulación de permisos en la tabla de páginas de xv6. Aunque se presentaron errores iniciales, se solucionaron correctamente mediante modificaciones en archivos clave del kernel. Esta experiencia permitió fortalecer los conocimientos en gestión de memoria y control de permisos en un sistema operativo.

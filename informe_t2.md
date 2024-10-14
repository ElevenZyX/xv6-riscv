
# Informe Tarea 2

**Autor**: Diego Troncoso Bustamante

## Introducción

La tarea consistió en modificar el sistema operativo educativo **Xv6-riscv** para incorporar un mecanismo de **Prioridad** y **Boost** en la planificación de procesos. En este sistema, los procesos con un valor numérico más bajo de prioridad son ejecutados antes. El **Boost** ajusta dinámicamente la prioridad: cuando un proceso alcanza una prioridad de 9, el boost cambia a -1; y cuando alcanza una prioridad de 0, el boost vuelve a 1.

## Errores Iniciales

Uno de los principales errores al comenzar fue la confusión entre **xv6** y la versión **xv6-riscv**. Al investigar, muchas fuentes sugerían implementar el código usando la variable `ptable`, la cual no está presente en la versión **xv6-riscv**. Esto generó dificultades al principio.

Otro desafío importante fue el error **"panic: kerneltrap"**. Inicialmente intenté reiniciar el trabajo desde cero, pero la solución fue investigar en profundidad la versión exacta de **xv6-riscv** que estaba utilizando. Esto me permitió entender las diferencias y resolver el problema sin recurrir a recomendaciones que no aplicaban.

Una vez comprendidas estas diferencias, la tarea se volvió mucho más clara, permitiéndome avanzar sin enfrentar errores relacionados con `ptable` o `sti()`.

## Modificaciones Realizadas

### Carpeta `kernel`

#### Archivo `proc.h`

Se añadieron dos nuevos campos a la estructura de proceso en `proc.h`:

```c
int priority;  // Campo para la prioridad del proceso
int boost;     // Campo para el valor de boost
```

> Estos campos permiten controlar la prioridad del proceso. Un valor numérico menor en `priority` indica una mayor prioridad. El campo `boost` ajusta dinámicamente la prioridad incrementando o reduciendo su valor dentro de un rango.

#### Archivo `proc.c`

En la función `static struct proc* allocproc(void)`, se añadieron las siguientes líneas para inicializar la prioridad y el boost de cada proceso:

```c
p->priority = 0;  // Inicializar prioridad en 0
p->boost = 1;     // Inicializar boost en 1
```

> Esto establece que cada proceso comienza con la mayor prioridad posible (0) y un `boost` positivo (1), que aumentará su prioridad a lo largo del tiempo.

En la función `scheduler()`, se realizó la siguiente modificación para verificar el funcionamiento básico del sistema antes de implementar la lógica completa:

```c
void scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();  // Obtener la CPU actual

  c->proc = 0;

  for(;;){
    intr_on();  // Habilitar interrupciones

    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state != RUNNABLE) {
        release(&p->lock);
        continue;
      }

      // Asignar el proceso a la CPU
      p->state = RUNNING;
      c->proc = p;
      
      swtch(&c->context, &p->context);  // Cambiar el contexto de CPU al proceso

      // Proceso ha terminado de correr
      c->proc = 0;
      release(&p->lock);
    }
  }
}
```

> Este código inicial simplemente valida que el sistema operativo funcione correctamente con el planificador básico, antes de implementar la versión completa con lógica de prioridades.

### Carpeta `user`

#### Creación de `test_prioridad.c`

Se creó un programa de prueba para generar múltiples procesos y observar su comportamiento:

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
  int pid;
  int num_procesos = 20;

  for (int i = 0; i < num_procesos; i++) {
    pid = fork();

    if (pid < 0) {
      printf("Error al crear el proceso %d\n", i);
      exit(1);
    } else if (pid == 0) {
      printf("Ejecutando proceso %d\n", getpid());
      sleep(2);  // Dormir el proceso por 2 segundos
      exit(0);
    } else {
      wait(0);  // El proceso padre espera a que cada hijo termine
    }
  }

  printf("Todos los procesos han terminado.\n");
  exit(0);
}
```

> Este programa crea 20 procesos hijos utilizando `fork()`. Cada proceso hijo imprime su PID, duerme por 2 segundos y luego termina. El padre espera a que cada proceso hijo termine antes de continuar.

### Problemas con `test_prioridad.c`

Fue necesario modificar el `Makefile` para incluir el programa de prueba `test_prioridad` en la compilación:

```makefile
UPROGS=\  
  $U/_test_prioridad\
  ...
```

Además, se agregó una regla en el `Makefile` para compilar y generar los archivos necesarios:

```makefile
$U/_test_prioridad: $U/test_prioridad.o $(ULIB)
	$(LD) $(LDFLAGS) -T $U/user.ld -o $U/_test_prioridad $U/test_prioridad.o $(ULIB)
	$(OBJDUMP) -S $U/_test_prioridad > $U/test_prioridad.asm
	$(OBJDUMP) -t $U/_test_prioridad | sed '1/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $U/test_prioridad.sym
```

Finalmente, por recomendación del profesor, se cambió el número de CPUs de 3 a 1 en el `Makefile`:

```makefile
CPUS := 3  -->  CPUS := 1
```

### Modificación del `scheduler()`

La versión final del `scheduler` incluye la lógica completa para manejar la prioridad y el boost de cada proceso:

```c
void scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();  // Obtener la CPU actual

  c->proc = 0;

  for(;;){
    intr_on();  // Habilitar interrupciones

    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state != RUNNABLE) {
        release(&p->lock);
        continue;
      }

      // Ajustar la prioridad del proceso usando el boost
      p->priority += p->boost;

      // Verificar y ajustar los límites de la prioridad
      if(p->priority >= 9) {
        p->priority = 9;
        p->boost = -1;
      } else if(p->priority <= 0) {
        p->priority = 0;
        p->boost = 1;
      }

      // Ejecutar el proceso
      p->state = RUNNING;
      c->proc = p;
      swtch(&c->context, &p->context);

      c->proc = 0;
      release(&p->lock);
    }
  }
}
```

> Este código implementa la lógica de prioridad y boost solicitada en la tarea. La prioridad de cada proceso se ajusta dinámicamente utilizando `boost`, y se limita entre 0 y 9.


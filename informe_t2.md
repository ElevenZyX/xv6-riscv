


# Informe Tarea 2

**Autor**: Diego Troncoso Bustamante

## Introducción:

La tarea consistió en incorporar Prioridad y Boost al sistema operativo educativo **Xv6-riscv**, haciendo que el menor número correspondiera a una mayor prioridad. La regla general era que, al llegar a prioridad 9, el boost cambiara a -1, y si llegaba a 0, el boost cambiara a 1.

## Errores Iniciales:

Mi principal error inicial fue confundir **xv6** con la versión **xv6-riscv**, ya que, al investigar, la recomendación era crear un código dentro de `proc.c` con la variable `ptable`. No obstante, la versión más reciente (**xv6-riscv**) no cuenta con esa variable. Por lo tanto, este fue un error común al principio.

Otro problema con el que pasé mucho tiempo intentando solucionar fue el mensaje de error **"panic: kerneltrap"**. Mi solución inicial fue reiniciar desde 0, pero la verdadera solución fue investigar con la misma versión de **xv6** que tenía, es decir, **xv6-riscv**.

Una vez resuelto este punto, la tarea se hizo mucho más sencilla, ya que investigar cómo generar lo solicitado para mi versión no arrojaba errores de variables como `sti()` o `ptable()`.

## Modificaciones Realizadas:

### Carpeta Kernel:

#### Archivo `proc.h`:

En la sección de `struct proc`, se añadió el siguiente código:

```c
int priority;
int boost;
```

> Estas líneas añaden dos campos nuevos en la estructura de procesos:
> - `priority`: Define la prioridad del proceso (cuanto menor es el número, mayor es la prioridad).
> - `boost`: Controla cómo se ajusta la prioridad, incrementándola o reduciéndola en función de sus límites (0 a 9).

#### Archivo `proc.c`:

En la sección de `static struct proc* allocproc(void)`, se añadió lo siguiente:

```c
p->priority = 0;  // Inicializar prioridad en 0
p->boost = 1;     // Inicializar boost en 1
```

> Estas líneas inicializan cada proceso con una prioridad de 0 (mayor prioridad) y un boost de 1 (incremento positivo).

Además, en la función `void scheduler(void)`, se modificó de la siguiente manera:

```c
void scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();  // Obtener la CPU actual

  c->proc = 0;

  for(;;){
    // Enable interrupts on this processor.
    intr_on();

    // Loop sobre la tabla de procesos.
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state != RUNNABLE) {
        release(&p->lock);
        continue;
      }

      // Cambiar a este proceso
      p->state = RUNNING;
      c->proc = p;
      
      // Cambiar el contexto del CPU al proceso
      swtch(&c->context, &p->context);

      // El proceso ha terminado de correr
      c->proc = 0;
      release(&p->lock);
    }
  }
}
```

> Este código se utilizó momentáneamente para verificar que **xv6** estaba funcionando correctamente, antes de implementar la versión definitiva del **scheduler** que incluía la lógica de Prioridad y Boost.

### Carpeta User:

#### Creación de `test_prioridad.c`:

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
  int pid;
  int num_procesos = 20;

  // Crear 20 procesos usando fork()
  for (int i = 0; i < num_procesos; i++) {
    pid = fork();

    if (pid < 0) {
      // Error al crear el proceso
      printf("Error al crear el proceso %d\n", i);
      exit(1);
    } else if (pid == 0) {
      // Código ejecutado por los procesos hijos
      printf("Ejecutando proceso %d\n", getpid());

      // Dormir el proceso por 2 segundos
      sleep(2);

      // Terminar el proceso hijo
      exit(0);
    } else {
      // El proceso padre espera a que el hijo termine antes de seguir creando más procesos
      wait(0);
    }
  }

  printf("Todos los procesos han terminado.\n");
  exit(0);
}
```

> Este código crea 20 procesos hijos usando `fork()`. Cada proceso hijo imprime su PID, duerme por 2 segundos y luego termina. El proceso padre espera a que cada proceso hijo termine antes de crear el siguiente.

## Errores del `test_prioridad.c`:

Se tuvo que modificar el `Makefile` añadiendo lo siguiente para que `test_prioridad` funcionase:

```makefile
UPROGS=\
  $U/_cat\
  $U/_test_prioridad\
  $U/_echo\
  $U/_forktest\
  $U/_grep\
  $U/_init\
  $U/_kill\
  $U/_ln\
  $U/_ls\
  $U/_mkdir\
  $U/_rm\
  $U/_sh\
  $U/_stressfs\
  $U/_usertests\
  $U/_grind\
  $U/_wc\
  $U/_zombie

# Regla para compilar test_prioridad
$U/_test_prioridad: $U/test_prioridad.o $(ULIB)
	$(LD) $(LDFLAGS) -T $U/user.ld -o $U/_test_prioridad $U/test_prioridad.o $(ULIB)
	$(OBJDUMP) -S $U/_test_prioridad > $U/test_prioridad.asm
	$(OBJDUMP) -t $U/_test_prioridad | sed '1/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $U/test_prioridad.sym
```

> Esta regla en el `Makefile` compila el archivo `test_prioridad.c` y genera el binario ejecutable, además de crear los archivos ASM y SYM que contienen el volcado de ensamblado y la tabla de símbolos, respectivamente.

Además, por recomendación del profesor, se cambió dentro del `Makefile` el número de CPUs de 3 a 1:

```makefile
CPUS := 3  -->  CPUS := 1
```

Posteriormente, en `user/user.h` y `kernel/stat.h` se añadió:

```c
#include "kernel/types.h"   // user/user.h
#include "types.h"          // kernel/stat.h
```

> La inclusión de estos archivos de cabecera fue necesaria para resolver referencias de tipos de datos y funciones compartidas entre el kernel y los programas de usuario.

### Modificación del Scheduler:

Finalmente, después de generar `make clean` y `make qemu`, y verificar que todo estuviera en orden, se modificó el `void scheduler(void)` de tal manera que la tarea lo solicitaba:

```c
void scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();  // Obtener la CPU actual

  c->proc = 0;

  for(;;){
    // Habilitar interrupciones en este procesador
    intr_on();

    // Iterar sobre la tabla de procesos
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state != RUNNABLE) {
        release(&p->lock);
        continue;
      }

      // Incrementar la prioridad con el boost
      p->priority += p->boost;

      // Verificar los límites de prioridad y ajustar el boost
      if(p->priority >= 9) {
        p->priority = 9;
        p->boost = -1;
      } else if(p->priority <= 0) {
        p->priority = 0;
        p->boost = 1;
      }

      // Cambiar a este proceso
      p->state = RUNNING;
      c->proc = p;
      swtch(&c->context, &p->context);

      // Proceso ha terminado de correr por ahora
      c->proc = 0;
      release(&p->lock);
    }
  }
}
```

> Este código implementa la lógica solicitada para ajustar la **prioridad** de cada proceso usando el **boost**. La prioridad se incrementa o decrementa según el valor de `boost`, y se ajusta cuando alcanza los límites de 0 o 9.

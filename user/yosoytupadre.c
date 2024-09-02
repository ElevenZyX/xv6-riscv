#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int n;


    printf("Identificación del proceso padre: %d\n", getppid());

    printf("Ingresa el nivel de ancestro que deseas (0 para ti mismo, 1 para el padre, etc.): ");
    char buf[10];
    gets(buf, sizeof(buf));
    n = atoi(buf);

    if (n < 0) {
        printf("El nivel %d no es válido.\n", n);
        exit(1);
    }

    int ancestor = getancestor(n);
    if (ancestor < 0) {
        printf("(-1) No hay más ancestros en el nivel %d\n", n);
    } else {
        printf("Ancestro en nivel %d: Identificación del ancestro: %d\n", n, ancestor);
    }

    exit(0);
}

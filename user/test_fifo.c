#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int pid;

    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            // Child process
            sleep(i * 10);  // Dormir diferente tiempo para evitar colisiones en la salida
            printf("Child %d: PID %d\n", i + 1, getpid());
            sleep(10);  // Dormir para simular trabajo
            exit(0);
        }
        // Parent process continúa para crear más hijos
    }

    // Esperar a que todos los hijos terminen
    for (int i = 0; i < 5; i++) {
        wait(0);
    }

    exit(0);
}

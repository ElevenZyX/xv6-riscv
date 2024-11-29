#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

int main() {
    char *filename = "testfile.txt";
    int fd;

    // Creación del Archivo con permisos de lectura y escritura
    fd = open(filename, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf("Error: no se pudo crear el archivo %s\n", filename);
        exit(1);
    }
    printf("Archivo %s creado con permisos de lectura y escritura.\n", filename);

    // Escritura Inicial
    if (write(fd, "Hello, xv6!\n", 13) != 13) {
        printf("Error: no se pudo escribir en el archivo %s durante la escritura inicial.\n", filename);
        close(fd);
        exit(1);
    }
    printf("Escritura inicial en el archivo completada.\n");
    close(fd); // Asegurarse de cerrar después de escribir

    // Cambio de Permisos a Solo Lectura
    if (chmod(filename, 1) < 0) {
        printf("Error: no se pudieron cambiar los permisos a solo lectura para el archivo %s.\n", filename);
        exit(1);
    }
    printf("Permisos cambiados a solo lectura para el archivo %s.\n", filename);

    // Prueba de Escritura con Solo Lectura
    fd = open(filename, O_WRONLY); // Intentar abrir en modo escritura
    if (fd >= 0) {
        printf("Error: se pudo abrir el archivo %s en modo escritura cuando está en solo lectura.\n", filename);
        close(fd); // Cerrar si se abre por error
        exit(1);
    }
    printf("Intento de abrir en modo escritura falló como era esperado.\n");

    // Cambio de Permisos a Lectura/Escritura
    if (chmod(filename, 3) < 0) {
        printf("Error: no se pudieron cambiar los permisos a lectura/escritura para el archivo %s.\n", filename);
        exit(1);
    }
    printf("Permisos cambiados a lectura/escritura para el archivo %s.\n", filename);

    // Escritura Final
    fd = open(filename, O_RDWR); // Abrir nuevamente en modo lectura/escritura
    if (fd < 0) {
        printf("Error: no se pudo abrir el archivo %s en modo lectura/escritura.\n", filename);
        exit(1);
    }
    if (write(fd, "Permisos restaurados.\n", 22) != 22) {
        printf("Error: no se pudo escribir en el archivo %s después de restaurar los permisos.\n", filename);
        close(fd);
        exit(1);
    }
    printf("Escritura final en el archivo completada.\n");
    close(fd); // Cerrar después de escribir

    printf("Prueba de chmod completada exitosamente.\n");
    exit(0);
}

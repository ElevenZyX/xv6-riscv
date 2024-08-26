# Informe Tarea 0



Primero que todo, mi PC es un MacBook, el cual tiene instalado Windows 10 mediante BootCamp. Sin embargo, no tengo activada la virtualización, por lo que no puedo instalar Ubuntu mediante WSL. Por lo tanto, instalé VirtualBox y descargué Ubuntu 24.04 para instalarlo en VirtualBox.

Al instalarlo, seguí los pasos de un tutorial en YouTube, y la instalación fue exitosa. Posteriormente, hice un fork del repositorio del profesor y cloné mi repositorio en Ubuntu. Luego, creé una rama llamada Diego_Troncoso_t0 y le hice push. El terminal me solicitó mi usuario y contraseña de GitHub, por lo que tuve que crear un token de identificación en GitHub para iniciar sesión en la terminal. Después de esto, comencé con la instalación de xv6.

**Primer comando utilizado:**
```
sudo apt-get update
```

**Segundo comando:**
```
sudo apt-get install gcc-riscv64-linux-gnu
```

**Tercer comando:**
```
sudo apt-get install g++-riscv64-linux-gnu
```

Después de ejecutar estos tres comandos, ejecuté `make qemu` y me dio error, por lo que instalé el siguiente comando:
```
sudo apt-get install gcc
```

Volví a ejecutar `make qemu`, pero el error persistió (no tenía instalado `qemu`). Entonces utilicé el siguiente comando:
```
sudo apt-get install qemu qemu-system-misc
```

Obtuve el siguiente error:
```
E: El paquete «qemu» no tiene un candidato para la instalación
```

Para solucionarlo, habilité más repositorios con los siguientes comandos:
```
sudo add-apt-repository universe
sudo add-apt-repository multiverse
```

Después de habilitar los repositorios, ejecuté:
```
sudo apt-get install qemu-system-misc
```

Una vez instalado `qemu`, ejecuté `make qemu` nuevamente, obteniendo un inicio exitoso.

Cada vez que ingreso el comando `make qemu`, aparece lo siguiente:

```
make qemu
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -global virtio-mmio.force-legacy=false -drive file=fs.imgif=noneformat=rawid=x0 -device virtio-blk-devicedrive=x0bus=virtio-mmio-bus.0
xv6 kernel is booting
hart 1 starting
hart 2 starting
init: starting sh
```

Adjunto capturas de pantalla del xv6 funcionando:


![xv6 Funcionando - Imagen 1](https://i.postimg.cc/zBScfP1R/qemu.jpg)

![xv6 Funcionando - Imagen 2](https://i.postimg.cc/bJj5pf4h/quemu2.jpg)
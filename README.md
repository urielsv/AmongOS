### TP2 - Sistemas Operativos
>Para correr el docker en macos (con silicon chip):
```
docker run -v "${PWD}:/root" --platform linux/amd64 --privileged -ti agodio/itba-so-multi-platform:3.0
```
> Pare sobre el directorio del proyecto y ejecute reemplazando <container_id> por el id del contenedor de docker (puede averiguarlo con el comando `docker ps`):
```
docker exec -it <container_id> /bin/bash
```
> Dirigirse a la carpeta del proyecto:
```
cd root
```
> Generar los artefactos:
```
make all
```
> Limpiar los artefactos:
```
make clean
```




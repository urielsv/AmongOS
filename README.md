### TP2 - Sistemas Operativos
>Para correr el docker en macos (con silicon chip):
```
docker run -v "${PWD}:/root" --platform linux/amd64 --privileged -ti agodio/itba-so-multi-platform:3.0
```
> Pare sobre el directorio del proyecto y ejecute reemplazando <container_id> por el id del contenedor de docker (puede averiguarlo con el comando `docker ps`):
```
docker exec -ti <container_id> /bin/bash
```
> Dirigirse a la carpeta del proyecto:
```
cd root
```
> Instruccion para compilar:
```
make all
```
> Instruccion para limpiar los artefactos:
```
make clean
```
> Instruccion de atajo (hace clean, all) para compilar con memoria buddy:
```
./compile.sh BUDDY
```
> Instruccion de atajo (hace clean, all) para compilar sin memoria buddy:
```
./compile.sh
```
> Instruccion para ejecutar la shell. Pararse sobre el directorio del proyecto (fuera del contenedor) y ejecutar:
```
./run.sh
```




# Juego de Referencias en C con Sockets

Proyecto académico: implementación de un juego educativo basado en referencias en lenguaje C, con comunicación cliente-servidor mediante sockets TCP/IP.

## Funcionalidades
- Juego interactivo por consola
- Comunicación cliente-servidor mediante sockets TCP
- Gestión de puntuaciones y registro de usuarios
- Validación de entradas y control de errores
- Uso de punteros y estructuras dinámicas en C

## Estructura del proyecto
- `server3.c` — Código del servidor
- `cliente1.c` — Código del cliente
- `funciones.h` — Cabecera con funciones compartidas
- `BDClientes.c` — Gestión de datos de usuarios
- `refranes.txt` — Archivo de datos para el juego

## Tecnologías
- Lenguaje C
- Sockets TCP/IP
- Linux / Ubuntu

## Ejecución
```bash
# Compilar el servidor y cliente
gcc server3.c -o server3
gcc cliente1.c -o cliente1

# Ejecutar el servidor
./server3

# En otro terminal, ejecutar el cliente
./cliente1

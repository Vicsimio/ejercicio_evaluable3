#ifndef OPERACIONES_H
#define OPERACIONES_H

/*códigos de operación compartidos entre proxy y servidor */
#define OP_SET    1
#define OP_GET    2
#define OP_MODIFY 3
#define OP_DELETE 4
#define OP_EXIST  5
#define OP_DESTROY 6

#endif
/*esta vez no definimos nuestra estructuras paquete y peticion porque utilizamos sockets*/
/*incompatibilidad de la arquitectura de nuestros ordenadores*/
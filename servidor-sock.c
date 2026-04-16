#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "claves.h"
#include "operaciones.h"

/*funcion auxiliar que se va a encargar de crear el socket y dejarlo listo*/
static int conectar_servidor(){
    int sock;
    struct sockaddr_in dir_serv;
    /*leemos la ip y puerto destino */
    char *ip = getenv("IP_TUPLAS");
    char *puerto = getenv("PORT_TUPLAS");
    if (ip == NULL || puerto == NULL) return -1;

    /*creamos el socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error al crear el socket");
        return -1;}

    dir_serv.sin_family = AF_INET;
    dir_serv.sin_port = htons(atoi(puerto));
    /*traducimos la direccón de envio*/
    if (inet_pton(AF_INET, ip, &dir_serv.sin_addr) <= 0) {
    close(sock);
    return -1;
    } 
    /*conectamos con el servidor*/
    if (connect(sock, (struct sockaddr *)&dir_serv, sizeof(dir_serv)) < 0) {
        perror("Error al conectar el servidor");
        close(sock);
        return -1;
    }
    return sock;

}
/*funcion auxliar para enviar un entero*/
static int enviar_entero(int sock, uint32_t valor, const char *mensaje) {
    uint32_t valor_red = htonl(valor);
    if (send(sock, &valor_red, sizeof(uint32_t), 0) < 0) {
        perror(mensaje); 
        return -1;
    }
    return 0;
}
/*funcion auxiliar para enviar una cadena*/
static int enviar_cadena(int sock, char *cadena, const char *mensaje) {
    uint32_t tam_cadena = strlen(cadena) + 1;
    /*enviamos el tamaño de la cadena */
    if (enviar_entero(sock, tam_cadena, "Error al enviar el tamaño de la cadena") < 0) return -1; 
    if (send(sock, cadena, tam_cadena, 0) < 0) {
        perror(mensaje);
        return -1;
    }
    return 0;
}
/*función auxiliar para recibir un entero*/
static int recibir_entero(int sock, uint32_t *valor, const char *mensaje) {
    uint32_t valor_red;
    if (recv(sock, &valor_red, sizeof(uint32_t), 0) <= 0) {
        perror(mensaje);
        return -1;
    }
    *valor = ntohl(valor_red);
    return 0;
}
/*funcion auxiliar para recibir una cadena*/
static int recibir_cadena(int sock, char *cadena, const char *mensaje) {
    uint32_t tam_cadena;
    if (recibir_entero(sock, &tam_cadena, "Error al recibir el tamaño de la cadena") < 0) return -1;
    if (recv(sock, cadena, tam_cadena, 0) <= 0) {
        perror(mensaje);
        return -1;
    }
    return 0;}

int exist(char *key) {
    int sock;
    uint32_t respuesta;
    /*comprobamos la validación de nuestros parámetros*/
    if (key == NULL) 
    return -1;
    if(strlen(key) > 255)return -1;
    /*creamos el socket */
    sock = conectar_servidor();
    if (sock < 0) 
        return -1;
    /*preparamos el codigo de opeeracion y enviamos*/
    if (enviar_entero(sock, OP_EXIST, "Error al enviar el codigo de operación") < 0) {
        close(sock);
        return -1;
    }
    /*enviamos tamño de la clave y la clave */
    if (enviar_cadena(sock, key, "Error al enviar la clave") < 0) {
        close(sock);
        return -1;
    }
    /*esperamos respuesta del servidor*/
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {
        close(sock); 
        return -1;
    }
    close(sock);
    return respuesta;}

int destroy(){
    int sock;
    uint32_t respuesta;
    sock = conectar_servidor();
    if (sock < 0) return -1;

    /*transformamos el codigo de operacion y enviamos*/
    if (enviar_entero(sock, OP_DESTROY, "Error al enviar el codigo de operación") < 0) {
        close(sock);
        return -1;
    }
    /*esperamos la respuesta del servidor*/
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {
        close(sock); return -1;
    }
    close(sock);
    return respuesta;

}
int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){
    int sock;
    uint32_t respuesta;
    uint32_t n_val2_red;
    uint32_t x_red, y_red, z_red;
    if (key == NULL || value1 == NULL || N_value2== NULL|| V_value2== NULL|| value3 == NULL) 
        return -1;
    if(strlen(key) > 255){
        return -1;
    }
    sock = conectar_servidor();
    if (sock < 0) return -1;
    /*comprobamos que existe una llave y el resto de valores para evitar una violacion de segmento*/
    if (enviar_entero(sock, OP_GET, "Error al enviar el codigo de operación") < 0) {
        close(sock);
        return -1;
    }
    /*enviamos tamño de la clave y la clave */
    if (enviar_cadena(sock, key, "Error al enviar la clave") < 0) {
        close(sock);
        return -1;
    }
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {
        close(sock); 
        return -1;
    }
    /*si la respuesta es un error, abortamos*/
    if (respuesta != 0) { 
        close(sock);
        return -1; 
    }
    /*Recibimos tamaño y contenido de value1*/
    if (recibir_cadena(sock, value1, "Error al recibir value1") < 0) { 
        close(sock); 
        return -1; 
    }
    /*recibimos N_value2 */
    if (recibir_entero(sock, &n_val2_red, "Error al recibir el valor2") < 0) { 
        close(sock);
        return -1; }
    *N_value2 = n_val2_red; /* Lo guardamos directamente en el puntero que nos pasaron */
    /*recibimos el Array de floats */
    if (recv(sock, V_value2, sizeof(float) * (*N_value2), 0) <= 0) { 
        close(sock); 
        return -1; }

    /*recibimos el entero x*/
    if (recibir_entero(sock, &x_red, "Error al recibir x") < 0) { close(sock);return -1;}
    value3->x = x_red;
    /*recibimos el entero y*/
    if (recibir_entero(sock, &y_red, "Error al recibir y") < 0) { close(sock);return -1;}
    value3->y = y_red;
    /*recibimos el entero z*/
    if (recibir_entero(sock, &z_red, "Error al recibir z") < 0) { close(sock);return -1;}
    value3->z = z_red;
    close(sock);
    return respuesta; 
}

int delete_key(char *key){
    int sock;
    uint32_t respuesta;
    if (key == NULL) return -1;
    if(strlen(key) > 255)return -1;
    /*conectamos servidor*/
    sock = conectar_servidor();
    if (sock < 0) return -1;
    /*comprobamos parámetro válido*/
    if (enviar_entero(sock, OP_DELETE, "Error al enviar el codigo de operación") < 0) {
        close(sock);
        return -1;
    }
    /*enviamos tamño de la clave y la clave */
    if (enviar_cadena(sock, key, "Error al enviar la clave") < 0) {
        close(sock);
        return -1;
    }
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {close(sock); return -1;}
    close(sock);
    return respuesta;}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    int sock;
    uint32_t respuesta;
    /*comprobacion de parametros*/
    if(key == NULL || value1 == NULL || V_value2 == NULL)return -1;
    if(strlen(key) > 255 || strlen(value1) > 255) return -1;
    if(N_value2 < 1 || N_value2 > 32)return -1;
    if(strlen(key) == 0 || strlen(value1) == 0) return -1;
    /*conectamos servidor*/
    sock = conectar_servidor();
    if (sock <0 )return -1;
    /*comprobamos que existe una llave y el resto de valores para evitar una violacion de segmento*/
    if (enviar_entero(sock, OP_SET, "Error al enviar el codigo de operación") < 0) {
        close(sock);
        return -1;
    }
    /*enviamos tamño de la clave y la clave */
    if (enviar_cadena(sock, key, "Error al enviar la clave") < 0) {
        close(sock);
        return -1;
    }
    /*enviamso el valor1*/
    if (enviar_cadena(sock, value1, "Error al enviar el valor1") < 0) {
        close(sock);
        return -1;
    }
    /*enviamos el value2*/
    if (enviar_entero(sock, N_value2, "Error al enviar el N_value2") < 0) {
        close(sock);
        return -1;
    }
    if(send(sock, V_value2, sizeof(float) * N_value2, 0) < 0) {
        perror("Error al enviar el array V_value2"); close(sock);
        return -1;
    }
    /*enviamos la estructura value3*/
    if (enviar_entero(sock, value3.x, "Error al enviar x") <0) {
        close(sock);
        return -1;
    }
    if (enviar_entero(sock, value3.y, "Error al enviar y") <0) {
        close(sock);
        return -1;
    }
    if (enviar_entero(sock, value3.z, "Error al enviar z") < 0) {
        close(sock);
        return -1;
    }
    /*recibimos la respuesta del envio*/
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {
        close(sock); return -1;
    }
    close(sock);
    return respuesta;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    int sock;
    uint32_t respuesta;
    /*comprobamos parametros*/
    if(key == NULL || value1 == NULL || V_value2 == NULL)return -1;
    if(strlen(key) > 255 || strlen(value1) > 255) return -1;
    if(N_value2 < 1 || N_value2 > 32) return -1;
    if(strlen(key) == 0 || strlen(value1) == 0) return -1;
    /*conectamos servidor*/
    sock = conectar_servidor();
    if (sock < 0)return -1;
    /*preparamos los datos a enviar*/
    if (enviar_entero(sock, OP_MODIFY, "Error al enviar el codigo de operación") < 0){
        close(sock);
        return -1;
    }
    /*enviamos tamño de la clave y la clave */
    if (enviar_cadena(sock, key, "Error al enviar la clave") < 0) {
        close(sock);
        return -1;
    }
    if (enviar_cadena(sock, value1, "Error al enviar el valor1") < 0){
        close(sock);
        return -1;
    }
    /*enviamos el value2*/
    if (enviar_entero(sock, N_value2, "Error al enviar el N_value2") <0) {
        close(sock);
        return -1;
    }
    if(send(sock, V_value2, sizeof(float) * N_value2, 0) <0) {
        perror("Error al enviar el array V_value2"); 
        close(sock); 
        return -1;
    }
    /*enviamos la estructura value3*/
    if (enviar_entero(sock, value3.x, "Error al enviar x") <0) {close(sock);return -1;}
    if (enviar_entero(sock, value3.y, "Error al enviar y") <0) {close(sock);return -1;}
    if (enviar_entero(sock, value3.z, "Error al enviar z") <0) {close(sock);return -1;}
    /*recibimos si la operación se ha realizado con exito*/
    if (recibir_entero(sock, &respuesta, "Error al esperar la respuesta del servidor") < 0) {
        close(sock); return -1;
    }
    close(sock);
    return respuesta;
}
#include <stdio.h>
#include <string.h>
#include "claves.h"

void sacar_datos(char *key) {
    char value1[256];
    int n;
    float v2[32];
    struct Paquete v3;
    int resultado;
    int i;

    resultado = get_value(key, value1, &n, v2, &v3);

    if (resultado == -1) {
        printf("get_value('%s') -> ERROR\n", key);
    } else {
        printf("get_value('%s') -> value1='%s', N=%d, V2=[", key, value1, n);

        for (i = 0; i < n; i++) {
            printf("%.2f", v2[i]);
            if (i < n - 1) {
                printf(", ");
            }
        }

        printf("], value3=(%d, %d, %d)\n", v3.x, v3.y, v3.z);
    }
}


int main() {
    struct Paquete p1;
    struct Paquete p2;
    struct Paquete p3;

    float vec1[] = {1.0, 0.33, 30.14};
    float vec2[] = {2.3, 0.5};
    float vec3[] = {0.02, 99.99, 7.7, 28.28};

    char key_larga[257];
    char value1_largo[257];

    char key_limite[256];
    char value1_limite[256];
    float vec_32[32] = {1.1};

    p1.x = 5;
    p1.y = 10;
    p1.z = 0;

    p2.x = 7;
    p2.y = 8;
    p2.z = 9;

    p3.x = 150;
    p3.y = 200;
    p3.z = 1200;

    memset(key_larga, 'k', 256);
    key_larga[256] = '\0';

    memset(value1_largo, 'v', 256);
    value1_largo[256] = '\0';

    memset(key_limite, 'l', 255);
    key_limite[255] = '\0';

    memset(value1_limite, 'm', 255);
    value1_limite[255] = '\0';

    printf("destroy() -> %d\n", destroy());

    printf("set_value('clave1') -> %d\n", set_value("clave1", "hola", 3, vec1, p1));
    printf("set_value('clave2') -> %d\n", set_value("clave2", "adios", 2, vec2, p2));
    printf("set_value duplicada('clave1') -> %d\n", set_value("clave1", "duplicada", 2, vec2, p2));

    printf("exist('clave1') -> %d\n", exist("clave1"));
    printf("exist('claveX') -> %d\n", exist("claveX"));

    sacar_datos("clave1");
    sacar_datos("clave2");

    printf("modify_value('clave1') -> %d\n", modify_value("clave1", "modificado", 4, vec3, p3));

    sacar_datos("clave1");

    /* pruebas de error*/
    printf("set_value(key >255) -> %d\n", set_value(key_larga, "hola", 2, vec2, p1));
    printf("set_value(value1 >255) -> %d\n", set_value("clave3", value1_largo, 2, vec2, p1));
    printf("set_value(N_value2=0) -> %d\n", set_value("clave4", "hola", 0, vec2, p1));
    printf("set_value(N_value2=33) -> %d\n", set_value("clave5", "hola", 33, vec2, p1));

    /*pruebas con valores limite*/
    printf("set_value(limite exacto 255) -> %d\n", set_value(key_limite, value1_limite, 2, vec2, p1));
    /*claves inexistente*/
    printf("modify_value('inventada') -> %d\n", modify_value("inventada", "nada", 2, vec2, p1));
    sacar_datos("inventada");
    /*cadena vacía key & value1*/
    printf("set_value(key vacia) -> %d\n", set_value("", "hola", 2, vec2, p1));
    printf("set_value(value1 vacio) -> %d\n", set_value("clave_vacia", "", 2, vec2, p1));
    /*limites validos para  N*/
    printf("set_value(N_value2=1) -> %d\n", set_value("clave_n1", "uno", 1, vec1, p1));
    printf("set_value(N_value2=32) -> %d\n", set_value("clave_n32", "tope", 32, vec_32, p1));

    printf("delete_key('clave2') -> %d\n", delete_key("clave2"));
    printf("exist('clave2') tras borrar -> %d\n", exist("clave2"));
    printf("delete_key('clave2') otra vez -> %d\n", delete_key("clave2"));

    printf("destroy() final -> %d\n", destroy());

    return 0;
}

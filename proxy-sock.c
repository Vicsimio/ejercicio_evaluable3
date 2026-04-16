#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "claves.h"

typedef struct Nodo {
    char *key;
    char value1[256];
    int N_value2;
    float V_value2[32];
    struct Paquete value3;
    struct Nodo *sig;
} Nodo;

static Nodo *lista = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int destroy(void) {
    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    while (actual != NULL) {
        Nodo *tmp = actual;
        actual = actual->sig;
        free(tmp->key);
        free(tmp);
    }

    lista = NULL;

    pthread_mutex_unlock(&mutex);
    return 0;
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL) {
        return -1;
    }

    if (strlen(value1) > 255) {
        return -1;
    }

    if (N_value2 < 1 || N_value2 > 32) {
        return -1;
    }

    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
        actual = actual->sig;
    }

    Nodo *nuevo = malloc(sizeof(Nodo));
    if (nuevo == NULL) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    nuevo->key = malloc(strlen(key) + 1);
    if (nuevo->key == NULL) {
        free(nuevo);
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    strcpy(nuevo->key, key);
    strcpy(nuevo->value1, value1);
    nuevo->N_value2 = N_value2;

    for (int i = 0; i < N_value2; i++) {
        nuevo->V_value2[i] = V_value2[i];
    }

    nuevo->value3 = value3;
    nuevo->sig = lista;
    lista = nuevo;

    pthread_mutex_unlock(&mutex);
    return 0;
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3) {
    if (key == NULL || value1 == NULL || N_value2 == NULL || V_value2 == NULL || value3 == NULL) {
        return -1;
    }

    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            strcpy(value1, actual->value1);
            *N_value2 = actual->N_value2;

            for (int i = 0; i < actual->N_value2; i++) {
                V_value2[i] = actual->V_value2[i];
            }

            *value3 = actual->value3;

            pthread_mutex_unlock(&mutex);
            return 0;
        }
        actual = actual->sig;
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    if (key == NULL || value1 == NULL || V_value2 == NULL) {
        return -1;
    }

    if (strlen(value1) > 255) {
        return -1;
    }

    if (N_value2 < 1 || N_value2 > 32) {
        return -1;
    }

    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            strcpy(actual->value1, value1);
            actual->N_value2 = N_value2;

            for (int i = 0; i < N_value2; i++) {
                actual->V_value2[i] = V_value2[i];
            }

            actual->value3 = value3;

            pthread_mutex_unlock(&mutex);
            return 0;
        }
        actual = actual->sig;
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}

int delete_key(char *key) {
    if (key == NULL) {
        return -1;
    }

    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    Nodo *anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            if (anterior == NULL) {
                lista = actual->sig;
            } else {
                anterior->sig = actual->sig;
            }

            free(actual->key);
            free(actual);

            pthread_mutex_unlock(&mutex);
            return 0;
        }

        anterior = actual;
        actual = actual->sig;
    }

    pthread_mutex_unlock(&mutex);
    return -1;
}

int exist(char *key) {
    if (key == NULL) {
        return -1;
    }

    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    Nodo *actual = lista;
    while (actual != NULL) {
        if (strcmp(actual->key, key) == 0) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }
        actual = actual->sig;
    }

    pthread_mutex_unlock(&mutex);
    return 0;
}
#include <stdio.h>
#include <pthread.h>

#define N_FILOSOFOS 5
pthread_mutex_t tenedores[N_FILOSOFOS] = PTHREAD_MUTEX_INITIALIZER;

void *filosofo(void *arg) {
    int id = *(int *)arg;
    int tenedor_izq = id;
    int tenedor_der = (id + 1) % N_FILOSOFOS;
  
    while (1) {
        // Esperar turno para adquirir los tenedores
        pthread_mutex_lock(&tenedores[tenedor_izq]);
        pthread_mutex_lock(&tenedores[tenedor_der]);

        // Filósofo come

        // Liberar los tenedores
        pthread_mutex_unlock(&tenedores[tenedor_der]);
        pthread_mutex_unlock(&tenedores[tenedor_izq]);

        // Filósofo piensa
    }
  
    pthread_exit(NULL);
}

int main() {
    pthread_t filosofos[N_FILOSOFOS];
    int ids[N_FILOSOFOS];

    // Inicializar los candados para los tenedores
    for (int i = 0; i < N_FILOSOFOS; i++) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    // Crear hilos para representar a los filósofos
    for (int i = 0; i < N_FILOSOFOS; i++) {
        ids[i] = i;
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);
    }

    // Olvidaste destruir los candados, ¿Recuerdas que estan guardados en memoria? xD

    return 0;
}

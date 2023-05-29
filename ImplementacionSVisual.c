#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h> // Se añade la librería de semáforos

#define N_FILOSOFOS 5

int tiempo_accion = 8; // Tiempo que tardan las acciones de comer y pensar

sem_t tenedores[N_FILOSOFOS]; // Arreglo de semáforos para representar los tenedores
pthread_mutex_t imprimir_mutex = PTHREAD_MUTEX_INITIALIZER; // Un candado para imprimir resultados coherentes.

int estados_filosofos[N_FILOSOFOS];
int estado_programa = 1; // 0: Inactivo; Cualquier otro: Activo.

void imprimirTabla() {
    pthread_mutex_lock(&imprimir_mutex);

    printf("+----------+----------+\n");
    printf("| Filósofo |  Estado  |\n");
    printf("+----------+----------+\n");

    for (int i = 0; i < N_FILOSOFOS; i++) {
        if (estados_filosofos[i] == 0) {
            printf("|    %d     |    P    |\n", i); // P: Pensando
        } else {
            printf("|    %d     |    C    |\n", i); // C: Comiendo
        }

        printf("+----------+----------+\n");
    }

    printf("\n");

    pthread_mutex_unlock(&imprimir_mutex);
}

void *filosofo(void *arg) {
    int id = *(int *)arg; // Id del filósofo
    int tenedor_izq = id; // Id del tenedor izquierdo
    int tenedor_der = (id + 1) % N_FILOSOFOS;

    int filosofo_izquierda = estados_filosofos[(id + N_FILOSOFOS - 1) % N_FILOSOFOS];
    int filosofo_derecha = estados_filosofos[(id + 1) % N_FILOSOFOS];

    while (estado_programa) {
        estados_filosofos[id] = 0; 
        imprimirTabla(); 

        // Esperar un tiempo antes de intentar comer nuevamente
        sleep(tiempo_accion);

        if (filosofo_izquierda == 0 && filosofo_derecha == 0) {
            // Esperar turno para adquirir los tenedores
            sem_wait(&tenedores[tenedor_izq]);
            sem_wait(&tenedores[tenedor_der]);

            estados_filosofos[id] = 1;
            imprimirTabla();

            // Esperar un tiempo, simula comer
            sleep(tiempo_accion);

            // Liberar los tenedores
            sem_post(&tenedores[tenedor_der]);
            sem_post(&tenedores[tenedor_izq]);
        }
    }

    estados_filosofos[id] = 0;

    pthread_exit(NULL);
}

int main() {
    pthread_t filosofos[N_FILOSOFOS];
    int ids[N_FILOSOFOS];

    // Inicializar los tenedores (semáforos)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        sem_init(&tenedores[i], 0, 1); // Inicializar los semáforos con un recurso disponible
    }

    // Crear filósofos (hilos)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        ids[i] = i;
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);
    }

    getchar(); // Esperar la entrada de un caracter
    printf("\nDeteniendo programa...\n");    
    estado_programa = 0;
    tiempo_accion = 0;

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < N_FILOSOFOS; i++) {
        pthread_join(filosofos[i], NULL);
    }

    // Destruir los tenedores (semáforos)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        sem_destroy(&tenedores[i]);
        printf("Se ha destruido el tenedor #%d\n", i);
    }

    pthread_mutex_destroy(&imprimir_mutex); // Eliminar el candado de la tabla
    printf("\nSe ha destruido el candado de la tabla\n");

    return 0;
}
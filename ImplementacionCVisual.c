#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>

#define N_FILOSOFOS 5

int tiempo_accion = 8; // Tiempo que tardan las acciones de comer y pensar

pthread_mutex_t tenedores[N_FILOSOFOS] = PTHREAD_MUTEX_INITIALIZER; // Que el compilador inicialice los tenedores. 
pthread_mutex_t imprimir_mutex = PTHREAD_MUTEX_INITIALIZER; // Un candado para imprimir resultados coherentes.

int estados_filosofos[N_FILOSOFOS];
int estado_programa = 1; // 0: Inactivo; Cualquier otro: Activo.

void imprimirTabla(){

    pthread_mutex_lock(&imprimir_mutex); // El propio candado de la tabla para que no se impriman cosas raras

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

    int id = *(int *)arg; // Id del filosofo
    int tenedor_izq = id; // Id del tenedor izquierdo
    int tenedor_der = (id + 1) % N_FILOSOFOS;

    int filosofo_izquierda = estados_filosofos[(id + N_FILOSOFOS - 1) % N_FILOSOFOS];
    int filosofo_derecha = estados_filosofos[(id + 1) % N_FILOSOFOS];

    while (estado_programa) {

        estados_filosofos[id] = 0; 
        imprimirTabla(); 

        // Esperar un tiempo antes de intentar comer nuevamente
        sleep(tiempo_accion);

        if( filosofo_izquierda == 0 && filosofo_derecha == 0){
            // Esperar turno para adquirir los tenedores
            pthread_mutex_lock(&tenedores[tenedor_izq]);
            pthread_mutex_lock(&tenedores[tenedor_der]);

            estados_filosofos[id] = 1;
            imprimirTabla();

            // Esperar un tiempo, simula comer
            sleep(tiempo_accion);

            // Liberar los tenedores
            pthread_mutex_unlock(&tenedores[tenedor_der]);
            pthread_mutex_unlock(&tenedores[tenedor_izq]);
        }
    }

    estados_filosofos[id] = 0;

    pthread_exit(NULL);
}

int main() {
    pthread_t filosofos[N_FILOSOFOS];
    int ids[N_FILOSOFOS];

    // Inicializar los tenedores(candados)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    // Crear filosofos(hilos)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        ids[i] = i;
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);
    }

    getchar(); // Espera la entrada de un caracter
    printf("\nDeteniendo programa...\n");    
    estado_programa = 0;
    tiempo_accion = 0;

    // Esperar a que todos los hilos terminen y liberen los tenedores(candados)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        pthread_join(filosofos[i], NULL);
    }

    // Destruir los tenedores(candados)
    for (int i = 0; i < N_FILOSOFOS; i++) {
        pthread_mutex_destroy(&tenedores[i]);
        printf("Se ha destruido el tenedor #%d\n", i);
    }

    pthread_mutex_destroy(&imprimir_mutex); // Eliminando el candado de la lista
    printf("\nSe ha destruido el candado de la tabla\n");

    return 0;
}
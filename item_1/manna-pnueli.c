#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

int request = 0, response = 0, sum = 0;

#define IS_CRITICAL_SECTION 1
#define NUM_THREADS 4

void criticalSection(int thread) {
    int local_sum = sum;
    usleep(rand() % 250 * 1000);
    sum = local_sum + 1;
    printf("Thread [%d] Result: [%d]\n ", thread, sum);
}

void serverSide() {
    while (1) {
        // Executa espera ocupada pela chegada de uma request
        while (request == 0);

        // Processa a request
        response = request;

        // Espera ocupada até que a resposta seja consumida caso seja necessario
        while (response != 0);
        request = 0;
    }
}

void clientSide(int thread) {
    while (1) {
        // Executa espera ocupada ate que a secao critica esteja disponivel
        if (IS_CRITICAL_SECTION) 
            while (response != thread) 
                request = thread;

        // Entra na secão critica
        criticalSection(thread);

         // Notifica que a secao critica foi concluida
        response = 0;
    }
}

int main() {
    omp_set_num_threads(NUM_THREADS + 1);

    #pragma omp parallel
    {
        int thread = omp_get_thread_num();
        if (thread == 0) serverSide();
        else clientSide(thread);
    }

    return 0;
}
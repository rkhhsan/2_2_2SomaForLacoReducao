#include <omp.h>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

#define NumThreads 8
int n = 100000000;
int s;
char caracter;
int M1[NumThreads];
int M2[NumThreads];

int tid;
int nt;
int ini;
int fim;
int id;
int st;

int main(int argc, char *argv[]) {

    omp_set_num_threads(NumThreads);
    s = 0;
    // inicializa o vetor
    for (int i = 0; i < NumThreads; i++)
        M1[i] = 0;
/**/
#pragma omp parallel default(none) shared(nt, M1, M2, n, s) private(tid, ini, fim)
    {
        tid = omp_get_thread_num();
        nt = omp_get_num_threads();
        ini = tid * (n / nt);
        fim = (tid + 1) * (n / nt);
        // Nesta parte do código tid,nt,ini e fim são variáveis privadas.
        if (tid == nt - 1)
            fim = n;

        /* Cada thread tem o seu for e em cada um destes for o i varia
         * de ini a fim, mas cada ini e fim é diferente para cada thread. */
        for (int i = ini; i < fim; i++) {
            s += 1;
            int id = omp_get_thread_num();
            M1[id] = M1[id] + 1;
            M2[id] = i;

        }
    }
    /*
     * O erro do código está na condição de corrida (race condition) na linha 28.
     * Esta é uma operação que ao ser iniciada por um thread, precisa ser terminada pela
     * mesma thread, antes que outro possa iniciá-la.
     */
    printf("\n Soma(errada por causa do race condition na variavel shared s) = %d", s);
    for (int i = 0; i < NumThreads; i++)
        printf("\n M1[%d]=%d    M2[%d]=%d", i, M1[i], i, M2[i]);

    /*
     * Solução 1:
     Podemos fazer adicionando o construtor
          #pragma omp critical
     imediatamente antes da linha de código s += st;
     Observação 2.2.3. A utilização do construtor #pragma omp critical
     reduz a performance do código e só deve ser usada quando realmente necessária.
     * */

    s = 0;
    // inicializa o vetor
    for (int i = 0; i < NumThreads; i++)
        M1[i] = 0;

#pragma omp parallel default(none) shared(nt, M1, M2, n, s) private(tid, ini, fim)
    {
        tid = omp_get_thread_num();
        nt = omp_get_num_threads();
        ini = n / nt * tid;
        fim = n / nt * (tid + 1);
        if (tid == nt - 1)
            fim = n;

        int st = 0; // st é private
        /* Cada thread tem o seu for e em cada um destes for o i varia
         * de ini a fim, mas cada ini e fim é diferente para cada thread. */
        for (int i = ini; i < fim; i++) {// Cada thread faz o seu pedaço
            st += 1;
            int id = omp_get_thread_num();
            M1[id] = M1[id] + 1;
            M2[id] = i;
        }
        // Nesta parte do código tid, nt,ini,fim,i e st são variáveis privadas.
#pragma omp critical
        s += st; //soma o valor de st de cada thread a s
    }
    printf("\n\n Soma usando critical = %d", s);
    for (int i = 0; i < NumThreads; i++)
        printf("\n M1[%d]=%d    M2[%d]=%d", i, M1[i], i, M2[i]);


    /*
    Solução 2:
    Além do construtor  #pragma omp critical
    imediatamente antes da linha de código s += st;
    Usamos o construtor  #pragma omp for
    Com este construtor, o laço do somatório pode ser automaticamente distribuindo entre os threads.
    */

    s = 0;
    // inicializa o vetor
    for (int i = 0; i < NumThreads; i++)
        M1[i] = 0;

#pragma omp parallel default(none) shared(M1, M2, n, s) private(st,id)
    {
        st = 0; // st é variável private(cada thread, possui a sua)

#pragma omp for
        for (int i = 0; i < n; i++) {
            id = omp_get_thread_num();
            M1[id] = M1[id] + 1;
            M2[id] = i;
            st += 1;
        }

#pragma omp critical
        s += st; //Soma o st de cada thread, s=s+st
    }

    printf("\n\n Soma usando critical e for = %d", s);
    for (int i = 0; i < NumThreads; i++)
        printf("\n M1[%d]=%d    M2[%d]=%d", i, M1[i], i, M2[i]);

    /*
    Solução 3:
    Mais simples e otimizado, é automatizar a operação de redução (no caso, a soma das somas parciais)
    adicionado reduction(+: s) ao construtor que inicializa a região paralela.
    Observação 2.2.4. A instrução de redução pode ser usada com qualquer operação binária
    aritmética (+, -, /, *), lógica (&, |) ou procedimentos intrínsecos (max, min).
    */

    s = 0;
#pragma omp parallel for reduction(+: s)
    for (int i = 0; i < n; i++)
        s += 1;

    printf("\n\n Soma usando for reduction(+:s) = %d", s);

    cout << "\n\n Tecle uma tecla e apos Enter para finalizar...\n";
    cin >> caracter;

    return 0;
}


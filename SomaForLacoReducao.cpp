#include <omp.h>
#include <cstdio>
#include <math.h>
#include <iomanip>
#include <iostream>

#define NumThreads 8

int n = 100000000;
int s = 0;
char caracter;
int M1[NumThreads];
int M2[NumThreads];
using namespace std;

int main(int argc, char *argv[]) {

    omp_set_num_threads(NumThreads);
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nt = omp_get_num_threads();
        int ini = n / nt * tid;
        int fim = n / nt * (tid + 1);
        // Nesta parte do código tid,nt,ini e fim são variáveis privadas.
        if (tid == nt - 1)
            fim = n;
        for (int i = ini; i < fim; i++)
            s += 1;
    }
    /*
     * O erro do código está na condição de corrida (race condition) na linha 28.
     * Esta é uma operação que ao ser iniciada por um thread, precisa ser terminada pela
     * mesma thread, antes que outro possa iniciá-la.
     */
    printf("\n Soma(errada por causa do race condition na variavel shared s) = %d", s);

    /*
     * Solução 1:
     Podemos fazer adicionando o construtor
          #pragma omp critical
     imediatamente antes da linha de código s += st;
     Observação 2.2.3. A utilização do construtor #pragma omp critical
     reduz a performance do código e só deve ser usada quando realmente necessária.
     * */
    s = 0;
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nt = omp_get_num_threads();
        int ini = n / nt * tid;
        int fim = n / nt * (tid + 1);
        if (tid == nt - 1)
            fim = n;

        int st = 0;
        for (int i = ini; i < fim; i++)
            st += 1;
        // Nesta parte do código tid, nt,ini,fim,i e st são variáveis privadas.
#pragma omp critical
        s += st;
    }
    printf("\n Soma usando critical = %d", s);

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

#pragma omp parallel
    {
        int st = 0; // st é variável private(cada thread, possui a sua)

#pragma omp for
        for (int i = 1; i <= n; i++) {
            int id = omp_get_thread_num();
            M1[id] = M1[id] + 1;
            M2[id] = i;
            st += 1;
        }

#pragma omp critical
        s += st; //Soma o st de cada thread, s=s+st
    }
    printf("\n Soma usando critical e for = %d", s);

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


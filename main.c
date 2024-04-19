#include <stdio.h>
#include <stdlib.h>

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}


int main(void) {
    /*
     * toDo:
        Roteiro
        1. Implemente uma versão concorrente para esse problema, dividindo a tarefa entre
        as threads. O número de elementos (N) e o número de threads devem ser informados na linha de comando.
        IMPORTANTE: Use o padrão de  bolsa de tarefa para distribuição da tarefa entre as threads: cada thread pega o próximo número
        da serie para avaliar a sua primalidade (veja Aula 4, de 16 de abril).
        ATENÇÃO:  os valores da série NÃO devem ser colocados em um vetor, devem  ser obtidos incrementalmente em tempo de execução.
        2. OBS.: defina a variável N do tipo long long int e use a função atoll() para converter o valor recebido do usuário (string) para long long int.
        3. Certifique-se que a sua solução está correta.
        4. Inclua tomadas de tempo no código (parte de processamento da sequência).
        5. Execute o programa várias vezes, variando o valor de N (experimente 10ˆ3
        e 10ˆ6);
        e para cada valor de N, varie a quantidade de threads (1, 2 e 4).
        6. Para cada configuração, execute o programa ao menos 5 vezes e registre o tempo médio dessas execuções.
        7. Calcule a aceleração e a eficiência  alcançada em cada configuração, tomando como base (execução sequencial) a execução com 1 thread.

     */
}
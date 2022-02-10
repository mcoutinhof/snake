# Snake
Inspirado no vídeo ["Algoritmo INVENCÍVEL dominando o jogo da cobrinha (SNAKE)"](https://youtu.be/Vii9XiQ8bec) do canal [Universo Programado](https://www.youtube.com/c/UniversoProgramado), implementei o jogo [Snake](https://pt.wikipedia.org/wiki/Serpente_(jogo_eletrônico)) na linguagem [C++11](https://pt.wikipedia.org/wiki/C++11) usando a biblioteca gráfica [SDL2](https://www.libsdl.org/), e apliquei o algoritmo apresentado. A ideia dele é proibir alguns movimentos da *cobrinha* de modo a garantir que sempre haja um caminho até a cauda para que ela nunca se prenda e nunca perca o jogo.

O vídeo também apresenta as seguintes otimizações:
- [x] Escolher entre duas direções usando a [distância euclidiana](https://pt.wikipedia.org/wiki/Distância_euclidiana) até o alimento.
- [ ] Escolher entre duas direções usando [Dijkstra](https://pt.wikipedia.org/wiki/Algoritmo_de_Dijkstra) (algoritmo de [pathfinding](https://pt.wikipedia.org/wiki/Problema_do_caminho_mínimo)).
- [ ] Obrigar a *cobrinha* a não deixar lacunas no tabuleiro.

Esse algoritmo, apenas com a primeira otimização, conseguiu *zerar* o jogo em 100% das vez com 25 mil passos, em média. Compararado ao [algoritmo de Monte Carlo](https://youtu.be/S6p7NJUxnOo), esse algoritmo consegue terminar o jogo em menos passos e consegue jogar em tabuleiros maiores sem necessitar de mais processamento. O ponto fraco desse algoritmo é que ele não funciona em tabuleiros de tamanho ímpar (9x5 por exemplo).

### Compilando
É necessário que você tenha a biblioteca [SDL2](https://www.libsdl.org/) instalada. No Ubuntu (e derivados), você pode usar o seguinte comando:  
```sudo apt install libsdl2-dev -y```

Para compilar com o **g++**, use o seguinte comando:  
```g++ -std=c++11 -o snake main.cpp -lSDL2```

### Comandos
<kbd>Espaço</kbd> para acelerar o jogo.  
<kbd>Enter</kbd> para reiniciar o jogo.  
<kbd>Esc</kbd> para perder o jogo.  

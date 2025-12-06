# The_Boys
Trabalho de finalização da disciplina de Programação I

<img width="1500" height="597" alt="image" src="https://github.com/user-attachments/assets/e49bdbaa-fca2-4dbe-9edd-5e25102851f1" />

O problema

Este projeto consiste em simular um mundo com heróis, bases e missões. Cada herói possui um conjunto de habilidades (voo, invisibilidade, visão de raios X, superaudição, etc.), um nível de experiência e outros atributos.

As bases estão espalhadas pelo mundo e os heróis se deslocam periodicamente entre elas. Cada base possui uma lotação máxima e uma fila de espera; ao chegar na base, o herói pode decidir entre esperar na fila ou desistir, viajando para outra base. Cada base possui um porteiro que gerencia a entrada/saída dos heróis e a fila de espera.

Esporadicamente surgem missões em locais aleatórios desse mundo; cada missão exige um conjunto de habilidades específicas. A missão pode ser atendida por uma equipe de heróis que se encontre em uma base, desde que os heróis da equipe tenham, em conjunto, as habilidades requeridas. Ao ser escolhido para participar de uma missão, cada herói recebe pontos de experiência.

Este projeto consiste em simular a dinâmica desse mundo durante um ano (525.600 minutos). Ao final, a simulação deve apresentar o número de missões cumpridas (pontos ganhos) por cada herói e outras estatísticas.

# Composição

Para o desenvolvimento da simulação foram usados 8 arquivos (junto do Makefile):
  Desenvolvidos:
    Fila de prioridades - fprio.c;
    Lista encadeada - lista.c;
    A simulação do mundo - theboys.c;
    Makefile;

  Disponibilizados sem possibilidade de alteração:
    Conjuntos - conjunto.h e .o;

  Bibliotecas:
    fprio.h;
    lista.h;
    conjunto.h;

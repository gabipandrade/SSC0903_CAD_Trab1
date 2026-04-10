# SSC0903_CAD_Trab1
Projeto do primeiro trabalho da disciplina de Computação de Alto Desempenho da USP São Carlos (1º semestre de 2026).

## Execução
Para compilar todos os códigos    : make all

Para executar o código sequêncial : make run_seq

Para executar o código paralelo   : make run_par

## Explicações

### Sequêncial 
O código sequencial segue a mesma lógica do código do Rafael. Fiz modificações somente para torná-lo mais legível e optimizado: alocação dinâmica de memória, comentários, encurtei alguns loops, etc. Creio que é possível entendê-lo somente através dos comentários.

### Paralelo
É uma cópia do código sequencial, e nas partes em que tem algum loop, coloquei as funções do OMP.
Como existem várias formas de paralelisar, se olharem o código, tem blocos de código com: "Método 1" e logo em sequência um bloco comentado com "Método 2". Ambos blocos desempenham a mesma função de diferentes formas. Posteriormente podemos testar e comparar para ver qual a melhor.

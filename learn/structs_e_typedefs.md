### Typedef:
Serve para criação de alias para qualquer coisa

```C
typedef int inteiro;
```

Agora inteiro também irá instanciar int 8bytes

typedef é apenas uma notação para o compilar atribuir a classe do objeto original outro nome para chamá-lo

### Struct: 

Tipo de dados COMPOSTO, ou seja, ele não tem lenght de bytes fixo, ele é apenas um template de objetos novos, que tu podes usar para inserir outros tipos dentro e nomeâ-lo como novo tipo de dado, ao compilar o compilador irá calcular o layout de bytes deste novo tipo com o padding e otimizações adequadas, salva em memória a declaração e a pelo nome da struct temos sua label e definição.

```C
struct Ponto{
  int x;
  int y;
};

struct Ponto ponto;
ponto.x = 2;
ponto.y = 3;
```

Em memória ele organiza os campos sequencialmente(com os paddings devidos) mas com uma lógica de offset.

OFFSET: é a distância em bytes a partir de um ponto de referência. Serve para aritmética. Então se a referência for:
0x10000 e offset for 4 bytes então o endereço final fica 0x10004. Então no struct o endereço referência é o endereço base do struct e o offset é o que precisa pular para chegar no campo

```C
typedef struct {
    int  id;       // 4 bytes
    char name[8];  // 8 bytes
    int  score;    // 4 bytes
} Task;
```

id    → offset 0   (começa no byte 0)
name  → offset 4   (começa depois dos 4 bytes de id)
score → offset 12  (começa depois dos 4 bytes de id + 8 bytes de name)

Esses números de offset são determinados em tempo de compilação.

PORTANTO, quando tu fazes t.score o compilador não vai atrás de nenhum hash do nome numa hash table para verificar o nome, mas sim ele usa justamente o offset que já sabe para acessar o dado, ai é bem mais rápido, aritmética de ponteiro pura.

Um exemplo de aritmética como o struct faz com a expressão aberta:
```C
&t                    // endereço base da struct — ex: 0x1000
(char *)&t            // cast para char* para fazer aritmética byte a byte
(char *)&t + 12       // pula 12 bytes — chega em 0x100C, onde score começa
(int *)((char*)&t+12) // interpreta esses bytes como int*
*( ... ) = 99         // escreve 99 nesse endereço
```

### Typedef + Struct:

Forma anônima
```C
typedef struct{
  int x;
  int y;
} Ponto;
```
Basicamente defino o objeto no campo de tipos comuns pelo alias aonde vivem int, char etc

Forma com Tag
```C
typedef struct Ponto{
  int x;
  int y;
  Ponto* next;
} Ponto;
```

"struct Ponto" define o Ponto no namespace de tags do struct, local separado dos tipos comuns, necessário fazer essa declaração do namespace antecipada em casos aonde usaremos o namespace já dentro da própria definição que é o caso de ponteiro do tipo do próprio objeto.

Existe função **offsetof** do stddef que diz em runtime o offset que o compilador atribui.

### Forward declaration:

```C
typedef struct List List;

typedef struct {
  List *lista;
} Node;
```

Que basicamente diz ao compilador para registrar o tipo struct List, serve para podermos utilizar esse tipo em definições sem nem estar definido ele, basicamente registra o nome para uso.

OBS: para caso de forward declaration o uso de ponteiro nos usos do tipo é obrigatório pois o compilador não sabe o tamanho do nome por não ter sido definido

### Organização dos Campos
O struct instrui o compilador a criação de um espaço composto em memória para nosso novo tipo, contudo para fins de otimizações para caches de CPU o compilador adicionad paddings de bytes para tornar o espaço ocupado em memória múltiplo de 4.

```C
typedef struct{
  char flag; // 1 byte
  // 7 bytes de padding
  double value; // 8 bytes
  char name[4]; // 4 bytes
  // 4 bytes de padding
} Ruim; // Total: 24 bytes

```

Reordenando para os maiores primeiro:

```C
typedef struct{
  double value; // 8 bytes
  char name[4]; // 4 bytes
  char flag; // 1 byte
  // 3 bytes de padding
} Bom; // Total: 16 bytes

```


### Enum:
Enquanto o struct é define um campo de tipos compostos com padding, o enum é basicamente um **int** com campos de nomes que tem valores, como se fosse um vocabulário.

```C
enum Status{
  CLOSED,
  OPEN,
  UNKNOW
};
```
Closed é 0, open é 1 e unknow é 2

Outra funcionalidade interessante é que você pode definir o valor dos enums, desde que não quebre a ordem de menor para maior
```C
enum Status{
  PENDING = 1,
  DONE = 2,
  DELETED = 4
};
```




### Union:
Diferentemente do struct que calcula offsets, padding e organiza os campos num campo de memória composto, os campos do union compartilham o mesmo endereço de memória, o tamanho é do maior campo. Basicamente uma forma de permitir leitura diferente tipos de dados em runtime sem problema de tipo.

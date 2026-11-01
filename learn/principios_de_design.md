### Information Hiding
Regra de Design que LIMITA o que é tornado público no header file para ser acessado por outros arquivos, keyword **static** pode ser usado para cortar o acesso de um objeto a outros arquivos. O que é interno de módulo deve ficar no .c do módulo.

db      → esconde a decisão: "usamos SQLite"
          se trocar por arquivo flat, nenhum outro módulo muda

filter  → esconde a decisão: "como buscamos tarefas"
          se trocar busca linear por índice, nenhum outro módulo muda

task    → esconde a decisão: "regras de validação de uma Task"
          se mudar o que torna uma Task válida, nenhum outro módulo muda

tui     → esconde a decisão: "usamos ncurses"
          se trocar por notcurses, cli e db não sabem

O teste prático é: se você mudar completamente a implementação de um módulo mantendo o .h idêntico, quantos outros arquivos precisam mudar? Se a resposta for zero, o information hiding está correto.

### Principio da interface mínima
Evitar colocar detalhes demais da implementação nas interfaces.

Exemplo ruim:
```C
void task_init
void task_set_title
void task_set_tag
void ...
```

Exemplo bom:
```C
void task_create
```

### Cohesion e Coupling
Basicamente métricas de qualidade de módulo.

Cohesion: quantifica o quanto os componentes do módulo são usadas e dependem entre si, coesão alta é bom pois indica pouco despercício e boa modularização.

Coupling: é o acoplamento de um módulo pelos componentes internos de outro, baixo acoplamento é bom pois garante independência e modularização de qualidade.

### Design by Contract 
Basicamente toda função tem 3 contratos implícitos para controle de qualidade:

Pre-Condition: o que deve ser verdade quando a função é chamada
Post-Condition: o que é garantido verdade no retorno da função
Invariant: o que é verdade do móudlo independente de qual função foi chamada.

Na implementação em C isso reflete em asserts:

```C
Task *task_create(const char *title, Priority priority) {
    assert(title != NULL);
    assert(strlen(title) > 0);
    assert(strlen(title) < 256);
``` 

### Law of Demeter
Basicamente se você tá encadeando mais de um . ou -> está errado a sua interface pode ser refatorada.


### Opaque Pointer
Basicamente declara os objetos nas interfaces com forward declartion sem abrir a definição do objeto com os campos.
```C
typedef struct Db Db;    // forward declaration — interior invisível

Db  *db_open(const char *path);
int  db_task_insert(Db *db, const Task *t);
void db_close(Db *db);
```
ENCAPSULAMENTO real em C, você limite o que está acessível do objeto a quem for usá-lo


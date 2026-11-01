Entendido — você quer os idioms gerais de C, conhecidos e usados amplamente. Esses têm nome próprio na literatura de C e aparecem em SQLite, curl, OpenSSL, Redis, GLib e muitos outros.

---

## A categoria correta se chama C Idioms

Não são Design Patterns do GoF, não são estratégias do Linux. São **idioms de C** — soluções consolidadas para problemas recorrentes de estruturação de código em C, documentados em livros como *Expert C Programming* de Peter van der Linden e *C Interfaces and Implementations* de David Hanson. Esse segundo livro é a referência mais direta para o que você está fazendo.

---

## 1. Abstract Data Type (ADT)

O idiom mais fundamental. Você define um tipo pelo que ele **faz**, não pelo que ele **é** por dentro. A interface pública descreve operações, a implementação é completamente escondida.

```c
// o que o mundo vê — operações, não estrutura
typedef struct Task Task;

Task *task_create(const char *title, Priority p);
void  task_set_due(Task *t, time_t due);
bool  task_is_overdue(const Task *t);
void  task_free(Task *t);
```

A struct `Task` é opaca — ninguém acessa campos diretamente. Só as operações existem publicamente. Isso é o que David Hanson chama de *interface* no livro — um header que descreve o que um tipo faz, não como funciona.

Onde aparece: a `FILE` da stdlib é um ADT clássico. Você nunca acessa `FILE->buffer` diretamente — só usa `fread`, `fwrite`, `fclose`. SQLite faz o mesmo com `sqlite3 *`.

---

## 2. Vtable — polimorfismo por tabela de funções

Quando diferentes tipos precisam responder à mesma operação de formas diferentes, você coleta os ponteiros de função numa struct separada. Cada tipo fornece sua própria tabela.

```c
// a interface — o que qualquer "item" sabe fazer
typedef struct {
    void (*render)(void *self, WINDOW *win);
    int  (*serialize)(void *self, char *buf, size_t len);
    void (*destroy)(void *self);
} ItemVtable;

// cada tipo tem seus dados + ponteiro para sua vtable
typedef struct {
    const ItemVtable *vtable;   // qual implementação usar
    int   id;
    char  title[256];
    time_t due_at;
} Task;

typedef struct {
    const ItemVtable *vtable;   // mesma posição, mesmo padrão
    int   id;
    char  title[256];
    int   frequency_days;
} Habit;
```

```c
// vtables concretas — cada tipo preenche a sua
static const ItemVtable task_vtable = {
    .render    = task_render,
    .serialize = task_serialize,
    .destroy   = task_free,
};

static const ItemVtable habit_vtable = {
    .render    = habit_render,
    .serialize = habit_serialize,
    .destroy   = habit_free,
};
```

```c
// código genérico que funciona com qualquer tipo
void render_all(void **items, int count) {
    for (int i = 0; i < count; i++) {
        ItemVtable *v = *(ItemVtable **)items[i];  // primeiro campo é sempre a vtable
        v->render(items[i], win);
    }
}
```

Onde aparece: GLib usa isso extensivamente com `GTypeInstance`. OpenSSL usa com `EVP_CIPHER`. curl usa com backends de SSL intercambiáveis. É polimorfismo manual — o que C++ faz automaticamente com virtual, você faz explicitamente.

---

## 3. Struct Embedding — composição e herança de dados

Quando tipos compartilham campos comuns, você embute a struct comum como **primeiro membro**. O endereço da struct filha e da struct mãe é o mesmo — cast direto é válido.

```c
// campos comuns a todas as entidades
typedef struct {
    int    id;
    char   title[256];
    time_t created_at;
} Item;

// Task "herda" Item embutindo como primeiro campo
typedef struct {
    Item   base;          // DEVE ser primeiro
    time_t due_at;
    int    project_id;
} Task;

typedef struct {
    Item   base;          // mesmo primeiro campo
    int    frequency_days;
    time_t last_completed;
} Habit;
```

```c
Task t = { .base.id = 1, .base.title = "Estudar", .due_at = ... };

// cast seguro — mesmo endereço
Item *item = (Item *)&t;
printf("%d %s\n", item->id, item->title);

// funções que operam em Item funcionam com Task e Habit
void item_print(const Item *item) {
    printf("[%d] %s\n", item->id, item->title);
}

item_print((Item *)&t);      // Task passando como Item
item_print((Item *)&habit);  // Habit passando como Item
```

Onde aparece: GObject da GLib inteiro é baseado nisso. O padrão é tão conhecido que tem nome: **first-member inheritance** ou **struct embedding**.

---

## 4. Discriminated Union — tipo soma

Quando você precisa de um valor que pode ser de tipos diferentes, você combina `union` com um campo `enum` que identifica qual membro está ativo. Já vimos isso antes, mas o nome do idiom é importante.

```c
typedef enum { VAL_INT, VAL_BOOL, VAL_STR } ValType;

typedef struct {
    ValType type;       // discriminante — qual membro é válido
    union {
        int   i;
        bool  b;
        char  s[256];
    };
} Value;
```

```c
// uso seguro — sempre verifica o discriminante antes de acessar
void value_print(const Value *v) {
    switch (v->type) {
        case VAL_INT:  printf("%d\n",  v->i); break;
        case VAL_BOOL: printf("%s\n",  v->b ? "true" : "false"); break;
        case VAL_STR:  printf("%s\n",  v->s); break;
    }
}
```

Onde aparece: SQLite usa internamente para representar valores de colunas. Interpretadores e parsers usam para representar tokens e nós de AST. JSON parsers em C usam para representar qualquer valor JSON.

---

## 5. Handle Pattern

Uma variação do ADT onde o "objeto" é representado por um inteiro ou ponteiro opaco — o chamador passa esse handle para todas as operações, nunca acessa o interior diretamente. O módulo mantém uma tabela interna de handles para objetos reais.

```c
// o handle é só um int — sem struct exposta
typedef int TaskHandle;

TaskHandle task_create(const char *title, Priority p);
int        task_done(TaskHandle h);
void       task_free(TaskHandle h);
```

```c
// internamente — tabela de objetos reais
// mt_task.c
static Task task_table[MAX_TASKS];
static int  task_count = 0;

TaskHandle task_create(const char *title, Priority p) {
    int h = task_count++;
    task_table[h].id = h;
    strncpy(task_table[h].title, title, 255);
    return h;   // retorna o índice como handle
}
```

Onde aparece: a API POSIX inteira usa isso — `open()` retorna um `int` (file descriptor) que é um handle. `pthread_t`, `FILE *`, handles de Windows — todos são variações desse idiom. A vantagem é que você pode mudar completamente a representação interna sem mudar nenhuma linha de código do chamador.

---

## 6. Callback com contexto — closure manual

C não tem closures, mas o padrão de callback com `void *userdata` é o substituto universal. Você passa uma função e um ponteiro opaco de contexto junto. A função recebe esse contexto de volta quando chamada.

```c
// assinatura com contexto
typedef void (*OnTaskDone)(const Task *t, void *ctx);

// registra callback + contexto
void task_on_done(Task *t, OnTaskDone fn, void *ctx);
```

```c
// o chamador passa seu próprio estado como contexto
typedef struct {
    WINDOW *win;
    int     row;
} TuiCtx;

void tui_on_task_done(const Task *t, void *ctx) {
    TuiCtx *tui = ctx;
    mvwprintw(tui->win, tui->row++, 0, "[done] %s", t->title);
}

TuiCtx ctx = { .win = main_win, .row = 0 };
task_on_done(t, tui_on_task_done, &ctx);
```

Onde aparece: qsort da stdlib usa `void *` genérico. GLib usa em toda API de callbacks. libuv usa em todo event loop. É o idiom de extensibilidade mais universal de C.

---

## Como esses idioms se relacionam

```
Problema                       Idiom
──────────────────────────────────────────────────────────
esconder implementação       → ADT / Handle Pattern
comportamento diferente       → Vtable
por tipo
campos comuns entre tipos     → Struct Embedding
valor que pode ser N tipos    → Discriminated Union
passar comportamento          → Callback com contexto
como parâmetro
```

Para o seu projeto especificamente, os que aparecem naturalmente são struct embedding para `Task`, `Project` e `Habit` compartilharem campos de `Item`, vtable se você quiser que a TUI renderize qualquer item sem saber o tipo, e callback com contexto em todo lugar que a TUI precisa reagir a eventos. Os outros aparecem se o projeto crescer até precisar.

Include Guard:

```c
#ifndef MT_TASK_H
#define MT_TASK_H

// conteúdo do header

#endif
```


O guard ifndef garante que o compilador defina apenas uma vez, em caso de múltiplos .c importarem o mesmo .h isso garante atomicidade das definições 

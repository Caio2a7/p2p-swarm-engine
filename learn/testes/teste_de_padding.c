#include <stdio.h>

struct Ruim{
  char flag; 
  double value; 
  char name[4]; 
};

struct Bom{
  double value; 
  char name[4]; 
  char flag; 
}; 

enum Teste{
  teste = 1,
  teste2 = 2,
  teste3 = 4
};

int main(){
  printf("Tamanho do RUIM: %zu bytes\n", sizeof(struct Ruim));

  printf("Tamanho do BOM: %zu bytes\n", sizeof(struct Bom));


  printf("Tamanho do Enum: %zu bytes\n", sizeof(enum Teste));

  return 0;
}

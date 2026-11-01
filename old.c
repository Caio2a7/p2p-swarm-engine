#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define SERVER_PORT 8080

int main(){
  int serverfd;
  serverfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr addr;
  
  addr.sa_family = AF_INET;
  addr.sa_data[0] = 0x1F;
  addr.sa_data[1] = 0x90;

  
  addr.sa_data[2] = 127;
  addr.sa_data[3] = 0;
  addr.sa_data[4] = 0;
  addr.sa_data[5] = 1;

  for(int i = 6; i <= 12; i++){
    addr.sa_data[i] = 0;
  }

  socklen_t addr_len = sizeof(addr);

  bind(serverfd, (struct sockaddr*)&addr, addr_len);

  char buf[1024];
  while(1){
    listen(serverfd, 1);
  }


  return 0;
}


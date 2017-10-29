#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

#define INET_NUM 1122
#define IP_ADDRESS "127.0.0.1"

#define TRUE 1

#define BUFF_LEN 1028

int main(void)
{
  int sockfd;
  sockfd = network_init(TCP_SERVER, IP_ADDRESS, INET_NUM);
  if(sockfd == -1)
  {
  perror("Network init error!");
  exit(EXIT_FAILURE);
  }
  printf("LISTEN-ing...\n");

  char RCV_BUFF[BUFF_LEN];
  int cmd_result;
  int connectfd;
  int readlen;
  while(TRUE)
  {
  if((connectfd = accept(sockfd, NULL, NULL)) == -1)
  {
  perror("Connect error!\n");
  break;
  }
  printf("Connect success!\n");
  while(TRUE)
  {
  readlen = read(connectfd, RCV_BUFF, sizeof(RCV_BUFF)); //Ω” ’√¸¡Ó
  if(readlen <0)
  {
  perror("Read error!\n");
  break;
  }
  if(readlen == 0)
  {
  printf("Welcome to use again!\nQUIT...\n");
  break;
  }

  printf("**************************\n");
  printf("RECV:%s\n",RCV_BUFF);
  cmd_result = ftp_cmd_analyse(RCV_BUFF);
  switch(cmd_result)
  {
  case CMD_ERROR:
  printf("CMD_ERROR!\n");
  break;
  case CMD_SERVER_LS:
  if(ftp_putlist(connectfd) == -1)
  {
  printf("List files error!\n");
  }
  else
  {
  printf("List files success!\n");
  }
  break;
  case CMD_DOWNLOAD:
  printf("Put files:%s\n", RCV_BUFF+9);
  if(ftp_putfile(connectfd, RCV_BUFF+9) == -1)
  {
  printf("Put files error!\n");
  }
  else
  {
  printf("Put files success!\n");
  }
  break;
  case CMD_UPLOAD:
  printf("Get files:%s\n", RCV_BUFF+7);
  if(ftp_getfile(connectfd, RCV_BUFF+7) == -1)
  {

  printf("Get files error!\n");
  }
  else
  {
  printf("Get files success!\n");
  }
  break;
  default:
  break;
  }
  }
  close(connectfd);
  }
  close(sockfd);
  return 0;
}
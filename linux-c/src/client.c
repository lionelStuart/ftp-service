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
#include <dirent.h>

#include "common.h"

#define INET_NUM 1122 //端口号
#define IP_ADDRESS "127.0.0.1" //服务器IP

#ifndef BUFF_LEN
#define BUFF_LEN 1028
#endif

#define TRUE 1

/*
#define CMD_LS 11//列出客户端所有文件
#define CMD_SERVER_LS 22//列出服务器所有文件
#define CMD_DOWNLOAD 33//下载文件
#define CMD_UPLOAD 44//上传文件
#define CMD_QUIT 55//退出
#define CMD_ERROR -1//错误
*/

int main(void)
{
  int sockfd;
  DIR* dp;
  struct dirent *ep;
  sockfd = network_init(TCP_CLIENT, IP_ADDRESS, INET_NUM); //初始化网络连接
  if(sockfd == -1)
  {
  perror("client::main::Network init error!\n");
  exit(EXIT_FAILURE);
  }
  else
  {
  printf("client::main::Connect success!\n");
  }

  ftp_print_help();

  char SEND_BUFF[BUFF_LEN];
  int cmd_result;
  while(TRUE)
  {
  fgets(SEND_BUFF, sizeof(SEND_BUFF), stdin);
  SEND_BUFF[strlen(SEND_BUFF)-1] = '\0';
  cmd_result = ftp_cmd_analyse(SEND_BUFF);

  switch(cmd_result)
  {
  case CMD_ERROR:
  printf("client::main::CMD_ERROR!\n");
  break;
  case CMD_LS:
  dp=opendir (STR_LOCAL_FILE_PATH);
  printf("*********File List of Client*********\n");
  while((ep = readdir(dp)))
  {
  if(ep->d_name[0] != '.')
  {
  printf("%s\n",ep->d_name);
  }
  }
  printf("*************************************\n");
  printf("List file success!\n");
  closedir(dp);
  break;
  case CMD_SERVER_LS:
  printf("*********File List of Server*********\n");
  if(ftp_getlist(sockfd) == -1)
  {
  printf("client::main::List file error!\n");
  }
  else
  {
  printf("*************************************\n");
  printf("client::main::List file success!\n");
  }
  break;
  case CMD_DOWNLOAD:
  if(write(sockfd, SEND_BUFF, BUFF_LEN) == -1)
  {
  perror("client::main::Send cmd error!");
  break;
  }
  if(ftp_getfile(sockfd, SEND_BUFF+9) == -1)
  {
  printf("client::main::Download error!\n");
  }
  else
  {
  printf("client::main::Download The File Success!!\n");
  }
  break;
  case CMD_UPLOAD:
  if(write(sockfd, SEND_BUFF, BUFF_LEN) == -1)
  {
  perror("client::main::Send cmd error!");
  break;
  }
  if(ftp_putfile(sockfd, SEND_BUFF+7) == -1)
  {
  perror("client::main::Upload error!\n");
  }
  else
  {
  printf("client::main::Upload The File Success!!\n");
  }
  break;
  case CMD_QUIT: //断开连接
  printf("client::main::Welcome to use again!\nQUIT!\n");
  close(sockfd);
  exit(EXIT_SUCCESS);
  break;
  default:
  break;
  }
  }
  close(sockfd);
  return 0;
}
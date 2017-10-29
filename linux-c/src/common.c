#include "common.h"

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
#include <stddef.h>
#include <dirent.h>

#define BACKLOG 10

int ftp_write_end_string(int putsockfd);

int network_init(int tcp_type, const char* ip_addr, short ip_port)
{
int sockfd = -1;
if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0)))
{
perror("common::network_init::Create socket error!\n");
return -1;
}

struct sockaddr_in sockadd;
memset(&sockadd, 0, sizeof(sockadd));
sockadd.sin_family = AF_INET;
sockadd.sin_port = htons(ip_port);
sockadd.sin_addr.s_addr = inet_addr(ip_addr);

switch (tcp_type)
{
case TCP_CLIENT:
if (-1== connect(sockfd, (struct sockaddr*) (&sockadd),sizeof(sockadd)))
{
perror("common::network_init::Connect error!\n");
return -1;
}
break;
case TCP_SERVER:
if (-1== bind(sockfd, (struct sockaddr*) (&sockadd),sizeof(sockadd)))
{
perror("common::network_init::Bind error!\n");
return -1;
}
if (-1 == listen(sockfd, BACKLOG))
{
perror("common::network_init::Listen error!\n");
return -1;
}
break;
default:
return -1;
}
return sockfd;
}

void ftp_print_help(void) {
printf("+========FTP HELP====================+\n");
printf("1.显示本地文件列表: ls \n");
printf("2.显示服务器文件列表: server ls \n");
printf("3.实现xxx文件读取与上传: upload xxx \n");
printf("4.实现xxx文件下载与存储: download xxx \n");
printf("5.断开socket链接: quit \n");
printf("+=====================================+\n\n");
}

int ftp_cmd_analyse(const char* cmd)
{
if (NULL == cmd)
{
return CMD_ERROR;
}
if (0 == strncmp(cmd,STR_CMD_QUIT,strlen(STR_CMD_QUIT)))
{
return CMD_QUIT;
}
if (0 == strncmp(cmd, STR_CMD_LS, strlen(STR_CMD_LS)))
{
return CMD_LS;
}
if (0 == strncmp(cmd, STR_CMD_SERVER_LS, strlen(STR_CMD_SERVER_LS)))
{
return CMD_SERVER_LS;
}
if (0 == strncmp(cmd, STR_CMD_DOWNLOAD, strlen(STR_CMD_DOWNLOAD)))
{
return CMD_DOWNLOAD;
}
if (0 == strncmp(cmd, STR_CMD_UPLOAD, strlen(STR_CMD_UPLOAD)))
{
return CMD_UPLOAD;
}
return CMD_ERROR;
}

int ftp_getlist(int getsockfd)
{
char GET_BUFF[BUFF_LEN];
int readsize;
sprintf(GET_BUFF, STR_CMD_SERVER_LS);
if (-1 == write(getsockfd, GET_BUFF, BUFF_LEN))
{
perror("common::ftp_getlist::Send cmd error!\n");
return -1;
}
while (TRUE)
{
readsize = read(getsockfd, GET_BUFF, BUFF_LEN);
if (readsize <= 0)
{
perror("common::ftp_getlist::Get list error!\n");
return -1;
}
else if (0 == strncmp(GET_BUFF, STR_GET_LIST_END, sizeof(STR_GET_LIST_END)))
{
break;
}
else
{
printf("%s\n", GET_BUFF);
}
}
return getsockfd;
}

int ftp_putlist(int putsockfd)
{
const char* LIST_PATH = STR_PUT_LIST_PATH;
char PUT_BUFF[BUFF_LEN];
DIR* dp=NULL;
struct dirent *ep=NULL;

dp = opendir(LIST_PATH);
if (NULL == dp)
{
perror("common::ftp_putlist::Can't open the directory!\n");
ftp_write_end_string(putsockfd);
return -1;
}
const char hiden_path='.';
while ((ep = readdir(dp)))
{
if (ep->d_name[0] != hiden_path)
{
sprintf(PUT_BUFF, "%s", ep->d_name);
write(putsockfd, PUT_BUFF, BUFF_LEN);
}
}
if(-1==ftp_write_end_string(putsockfd))
{
return -1;
}
closedir(dp);
return putsockfd;
}
int ftp_write_end_string(int putsockfd)
{
int ret_val= write(putsockfd, STR_GET_LIST_END, BUFF_LEN);
if(-1==ret_val)
{
perror("common::ftp_putlist::Write end string error!\n");
}
return ret_val;
}

int ftp_getfile(int getsockfd, const char* file_name)
{
int getfilefd;
int getfilesize;
char GET_BUFF[BUFF_LEN];
char file_path[100];
memset(file_path,0,sizeof(file_path));
strcpy(file_path,STR_PUT_LIST_PATH);
strcat(file_path,file_name);
if (-1== (getfilefd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC,0666)))
{
perror("common::ftp_getfile::Can't open or create file!\n");
return -1;
}
while ((getfilesize = read(getsockfd, GET_BUFF, BUFF_LEN)) > 0)
{
if (0 == strncmp(GET_BUFF, STR_ERROR_PREFIX, sizeof(STR_ERROR_PREFIX)))
{
printf("%s\n", GET_BUFF);
return -1;
}
memcpy(&getfilesize, GET_BUFF, 4);
if (-1 == write(getfilefd, GET_BUFF + 4, getfilesize))
{
perror("common::ftp_getfile::Download error!\n");
close(getfilefd);
return -1;
}
if (getfilesize < (BUFF_LEN - 4))
{
break;
}

}
close(getfilefd);
return getfilefd;
}

int ftp_putfile(int putsockfd, const char* file_name)
{
int putfilefd;
int putfilesize;
char PUT_BUFF[BUFF_LEN];
char file_path[100];
memset(file_path,0,sizeof(file_path));
strcpy(file_path,STR_LOCAL_FILE_PATH);
strcat(file_path,file_name);
if (-1 == (putfilefd = open(file_path, O_RDONLY)))
{
perror("common::ftp_putfile::Open error!\n");
write(putsockfd, STR_ERR_NOFILE, BUFF_LEN);
return -1;
}
while ((putfilesize = read(putfilefd, PUT_BUFF + 4, (BUFF_LEN - 4))) > 0)
{
memcpy(PUT_BUFF, &putfilesize, 4);
if (-1 == write(putsockfd, PUT_BUFF, BUFF_LEN))
{
perror("common::ftp_putfile::Put file error!\n");
close(putfilefd);
return -1;
}
memset(PUT_BUFF, 0, BUFF_LEN);
}
close(putfilefd);
return putfilefd;
}
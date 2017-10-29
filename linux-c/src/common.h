#ifndef COMMON_H_
#define COMMON_H_

#define TCP_SERVER (1)
#define TCP_CLIENT (2)

#define BUFF_LEN 1028

#define TRUE 1
#define FALSE 0

#define CMD_LS (1)//列出客户端所有文件
#define CMD_SERVER_LS (2)//列出服务器所有文件
#define CMD_DOWNLOAD (3)//下载文件
#define CMD_UPLOAD (4)//上传文件
#define CMD_QUIT (5)//退出
#define CMD_ERROR (-1)//错误

#define STR_CMD_LS ("ls")
#define STR_CMD_SERVER_LS ("server ls")
#define STR_CMD_DOWNLOAD ("download ")
#define STR_CMD_UPLOAD ("upload ")
#define STR_CMD_QUIT ("quit")

#define STR_ERR_NOFILE ("ERROR:No such file or directory!\n")
#define STR_ERR_DOWNLOAD ("ERROR:Download error!\n")
#define STR_ERR_UPLOAD ("ERROR:Upload error!\n")
#define STR_GET_LIST_END ("SUCCESS:GET LIST SUCCESS!\n")

#define STR_PUT_LIST_PATH ("./file/")
#define STR_LOCAL_FILE_PATH ("./file/")

#define STR_ERROR_PREFIX ("ERROR:")

int network_init(int tcp_type, const char* ip_addr, short ip_port);

void ftp_print_help(void);
int ftp_cmd_analyse(const char* cmd);
int ftp_getlist(int getsockfd);
int ftp_putlist(int putsockfd);
int ftp_getfile(int getsockfd, const char* file_name);
int ftp_putfile(int putsockfd, const char* file_name);

#endif /* COMMON_H_ */
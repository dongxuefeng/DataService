#ifndef _FTP_CLIENT_H_
#define _FTP_CLIENT_H_

#define FTP_LOGIN_SUCCESS       0
#define FTP_LOGIN_TIMEOUT       1
#define FTP_LOGIN_NO_MEMORY     2
#define FTP_LOGIN_RECONNECT     3

#define FTP_GET_SUCCESS   	    0
#define FTP_GET_ERR             1

#define FTP_SEND_SUCCESS        0
#define FTP_SEND_TIMEOUT        1
#define FTP_SEND_NO_LOGIN       2
#define FTP_SEND_NO_DATACONNECT 3

#define FTP_ACCESS_SUCCESS      0
#define FTP_ACCESS_ERR          1

#define FTP_NOLOGIN				5

typedef struct {
    /*int     m_FtpAddr;*/
	char    m_FtpAddr[32];
    int     m_FtpPort;
    char    m_User[16];
    char    m_Psd[16];
}FTP_SERVER_INTERFACE;


void ftp_client_init(void);
int  ftp_client_access(char *path, char *name);
int  ftp_client_access2(char *path);
int  ftp_client_send(unsigned char *data, int len);
int  ftp_client_putImg(const char * srcpath, const char * dstpath);
int  ftp_client_get(char *remotepath, char *localpath);
void ftp_client_close(void);
int  ftp_client_mkdir(char* path);
int  ftp_client_delete(char* path);

int  ftp_client_connect(FTP_SERVER_INTERFACE *ftp_interface);

#endif


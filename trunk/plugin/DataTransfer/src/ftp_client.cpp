#include "stdio.h"
#include "string.h"
#include "ftplib.h"
#include "ftp_client.h"

static netbuf       *m_ftp_ctl;
static netbuf       *m_ftp_dat;

void ftp_client_init(void)
{
    m_ftp_ctl = NULL;
    m_ftp_dat = NULL;

    FtpInit();
}


void ftp_client_close(void)
{
    if(m_ftp_ctl != NULL)
    {
        FtpQuit(m_ftp_ctl);
        m_ftp_ctl = NULL;
    }
}


int  ftp_client_connect(FTP_SERVER_INTERFACE *ftp_interface)
{
    int err;
    int ret;
    char temp[128];
//    int a0, b0, c0, d0;
	
    if(m_ftp_ctl != NULL)
    {
        err = FTP_LOGIN_RECONNECT;
        goto FTP_CLIENT_CONNECT_END;
    }

//    a0 = ftp_interface->m_FtpAddr&0xff;
//    b0 = (ftp_interface->m_FtpAddr>>8)&0xff;
//    c0 = (ftp_interface->m_FtpAddr>>16)&0xff;
//    d0 = (ftp_interface->m_FtpAddr>>24)&0xff;
	
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%s:%d", ftp_interface->m_FtpAddr,ftp_interface->m_FtpPort);
    printf("ftp connect %s\n", temp);
	
    ret = FtpConnect(temp, &m_ftp_ctl, 3000);  // timeout 3000 ms, TBD
    if(ret != 1)
    {
        err = FTP_LOGIN_TIMEOUT;    // TBD
        goto FTP_CLIENT_CONNECT_END;
    }
	
    ret = FtpLogin(ftp_interface->m_User, ftp_interface->m_Psd, m_ftp_ctl);
    if(ret != 1)
    {		
        FtpQuit(m_ftp_ctl);
        m_ftp_ctl = NULL;
        err = FTP_LOGIN_TIMEOUT;    // TBD
    }
    else
    {
        err = FTP_LOGIN_SUCCESS;
    }

FTP_CLIENT_CONNECT_END:
    return err;
}


#if 0
// send a whole file
int  ftp_client_send_data(char *path, char *name, unsigned char *data, int len)
{
    int ret, err=FTP_SEND_SUCCESS ;
    int length, offset;
    char remote_name[512];
	
    if(m_ftp_ctl != NULL)
    {
        // TBD, ƴ��path and name
        memset(remote_name, 0, sizeof(remote_name));
        sprintf(remote_name, "%s%s", path, name);
					
        ret = FtpAccess(remote_name, 
                        FTPLIB_FILE_WRITE, 
                        FTPLIB_IMAGE,
                        m_ftp_ctl,
                        &m_ftp_dat);
        if(ret != 1)
        {
            printf("ftpaccess err,ret:%d\n", ret);
            FtpQuit(m_ftp_ctl);
            m_ftp_ctl = NULL;
            err = FTP_SEND_TIMEOUT;         // TBD
            goto FTP_CLIENT_SEND_END;
        }
					
        length = len;
        offset = 0;
        while(length > 0)
        {
            ret = FtpWrite(data+offset, length, m_ftp_dat);
            if(ret <= 0)
            {
                printf("ftpwrite,ret:%d\n", ret);
                FtpClose(m_ftp_dat);
                FtpQuit(m_ftp_ctl);
                m_ftp_ctl = NULL;
                m_ftp_dat = NULL;
                err = FTP_SEND_TIMEOUT;     // TBD
                goto FTP_CLIENT_SEND_END;
            }
            else
            {
                length -= ret;
                offset += ret;
            }
        }	
//TBD!! 		
        FtpClose(m_ftp_dat);
        m_ftp_dat = NULL;				
    }
    else
    {
        printf("ftpaccess err, no login\n");
        err = FTP_SEND_NO_LOGIN;
    }

FTP_CLIENT_SEND_END:
	
    return err;
}
#endif

int  ftp_client_access(char *path, char *name)
{
    int ret, err=FTP_ACCESS_SUCCESS ;
   // int length, offset;
    char remote_name[512];

    if(m_ftp_ctl != NULL)
    {
        // TBD, ƴ��path and name
        memset(remote_name, 0, sizeof(remote_name));
        sprintf(remote_name, "%s%s", path, name);

        ret = FtpAccess(remote_name, 
                        FTPLIB_FILE_WRITE, 
                        FTPLIB_IMAGE,
                        m_ftp_ctl,
                        &m_ftp_dat);
        if(ret != 1)
        {
            //printf("ftpaccess err,ret:%d\n", ret);
            //FtpQuit(m_ftp_ctl);
            //m_ftp_ctl = NULL;
            err = FTP_ACCESS_ERR;         // TBD
            
            goto FTP_CLIENT_ACCESS_END;
        }
        else
        	printf("file exist!\n");
    }					
    else
    {
        //printf("ftpaccess err, no login\n");
        err = FTP_ACCESS_ERR;
    }

FTP_CLIENT_ACCESS_END:
	
    return err;
}

int  ftp_client_access2(char *path)
{
    int ret, err=FTP_ACCESS_SUCCESS ;

    if(m_ftp_ctl != NULL)
    {
        ret = FtpAccess(path,
        				FTPLIB_DIR,
                        FTPLIB_ASCII,
                        m_ftp_ctl,
                        &m_ftp_dat);
        if(ret != 1)
        {
            printf("ftpaccess err,ret:%d\n", ret);
            //FtpQuit(m_ftp_ctl);
            //m_ftp_ctl = NULL;
            err = FTP_ACCESS_ERR;         // TBD

            goto FTP_CLIENT_ACCESS_END;
        }
        else
        {
        	printf("Dir exist!\n");
        }
    }
    else
    {
        printf("ftpaccess err, no login\n");
        err = FTP_NOLOGIN;
    }

FTP_CLIENT_ACCESS_END:

    return err;
}

int  ftp_client_get(char *remotepath, char *localpath)
{
	int ret, err = FTP_GET_SUCCESS;
	if(m_ftp_ctl != NULL)
	{
		ret = FtpGet(localpath,	remotepath, FTPLIB_IMAGE, m_ftp_ctl);
		if(ret != 1)
		{
			printf("ftp get err,ret:%d\n", ret);
			FtpQuit(m_ftp_ctl);
			m_ftp_ctl = NULL;
			err = FTP_GET_ERR;         // TBD

			goto FTP_CLIENT_ACCESS_END;
		}
	}
	else
	{
		printf("ftp get err, no login\n");
		err = FTP_GET_ERR;
	}

FTP_CLIENT_ACCESS_END:
	return err;
}

int  ftp_client_send(unsigned char *data, int len)
{
    int ret, err=FTP_SEND_SUCCESS ;
    int length, offset;
    //char remote_name[512];
	
    if(m_ftp_dat != NULL)
    {					
        length = len;
        offset = 0;
        while(length > 0)
        {
            ret = FtpWrite(data+offset, length, m_ftp_dat);
            if(ret <= 0)
            {
                printf("ftpwrite,ret:%d\n", ret);
                FtpClose(m_ftp_dat);
                FtpQuit(m_ftp_ctl);
                m_ftp_ctl = NULL;
                m_ftp_dat = NULL;
                err = FTP_SEND_TIMEOUT;     // TBD
                goto FTP_CLIENT_SEND_END;
            }
            else
            {
                length -= ret;
                offset += ret;
            }
        }	
/*		
//TBD!! 		
        FtpClose(m_ftp_dat);
        m_ftp_dat = NULL;	
*/        
    }
    else
    {
        err = FTP_SEND_NO_DATACONNECT;
    }

FTP_CLIENT_SEND_END:
	
    return err;
}

void  ftp_client_data_close()
{
    if(m_ftp_dat != NULL)
    {
        FtpClose(m_ftp_dat);
        m_ftp_dat = NULL;    
    }	
}
int ftp_client_mkdir(char* path)
{
    if(m_ftp_ctl != NULL)
    {
       return FtpMkdir(path, m_ftp_ctl);     
    }	
    return -1;
}

int ftp_client_chdir(char* path)
{
    if(m_ftp_ctl != NULL)
    {
       return FtpChdir(path, m_ftp_ctl);
    }	
    return -1;
}

int  ftp_client_putImg(const char * srcpath, const char * dstpath)
{
	if(m_ftp_ctl != NULL)
	{
	   return FtpPut(srcpath, dstpath, FTPLIB_IMAGE, m_ftp_ctl);
	}

	return -1;
}

int  ftp_client_delete(char* path)
{
	if(m_ftp_ctl != NULL)
	{
	   return FtpDelete(path, m_ftp_ctl);
	}

	return -1;
}

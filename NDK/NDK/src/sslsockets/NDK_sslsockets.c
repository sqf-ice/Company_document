#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <memory.h>
#include <pthread.h>

#include "NDK_sslsock.h"
#include "NDK.h"
#include "NDK_debug.h"


typedef struct _sslhandle {
    int CloseFlag;
    SSL *CurrentSsl;
    SSL_CTX *CurrentCtx;
    int CurrentSocketMode;
    int CurrentConnectState;
    int CurrentSocket;
    int TimeOutFlag;
    int timeout;
} SSLHANDLE;

#define DEBUG 1
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "Spire SSL: "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

#define PBE_SHA1_3DES "pbeWithSHA1And3-KeyTripleDES-CBC"
static const char *mon[12]= {
    "Jan","Feb","Mar","Apr","May","Jun",
    "Jul","Aug","Sep","Oct","Nov","Dec"
};
static int MY_ASN1_UTCTIME_print(const ASN1_UTCTIME *tm)
{
    const char *v;
    int gmt=0;
    int i;
    int y=0,M=0,d=0,h=0,m=0,s=0;

    i=tm->length;
    v=(const char *)tm->data;

    if (i < 10) goto err;
    if (v[i-1] == 'Z') gmt=1;
    for (i=0; i<10; i++)
        if ((v[i] > '9') || (v[i] < '0')) goto err;
    y= (v[0]-'0')*10+(v[1]-'0');
    if (y < 50) y+=100;
    M= (v[2]-'0')*10+(v[3]-'0');
    if ((M > 12) || (M < 1)) goto err;
    d= (v[4]-'0')*10+(v[5]-'0');
    h= (v[6]-'0')*10+(v[7]-'0');
    m=  (v[8]-'0')*10+(v[9]-'0');
    if (tm->length >=12 &&
        (v[10] >= '0') && (v[10] <= '9') &&
        (v[11] >= '0') && (v[11] <= '9'))
        s=  (v[10]-'0')*10+(v[11]-'0');

    if (fprintf(stderr,"%s %2d %02d:%02d:%02d %d%s",
                mon[M-1],d,h,m,s,y+1900,(gmt)?" GMT":"") <= 0)
        return(0);
    else
        return(1);
err:
    return(0);
}
int verify_callback(int ok, X509_STORE_CTX *ctx)
{
    char buf[256];
    X509 *err_cert;
    int err,depth,pubkeylen;
    err_cert=X509_STORE_CTX_get_current_cert(ctx);
    err=    X509_STORE_CTX_get_error(ctx);
    depth=  X509_STORE_CTX_get_error_depth(ctx);

    pubkeylen=EVP_PKEY_bits(X509_get_pubkey(ctx->current_cert));
    if(pubkeylen<1024) {
        PDEBUG("%s Certificate with RSA key size(%d) of less than 1024 bits, line:(%d)\n", __func__,pubkeylen,__LINE__);
        ok=0;
        return(ok);
    }
    if (!ok) {

        PDEBUG("%s verify error:num=%d:%s,line:(%d)\n", __func__,err,X509_verify_cert_error_string(err),__LINE__);
        switch(err) {
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
            case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
            case X509_V_ERR_CERT_SIGNATURE_FAILURE:
            case X509_V_ERR_CERT_NOT_YET_VALID:
            case X509_V_ERR_CERT_HAS_EXPIRED:
            case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
            case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
            case X509_V_ERR_OUT_OF_MEM:
            case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
            case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
            case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
            case X509_V_ERR_INVALID_CA:
            case X509_V_ERR_PATH_LENGTH_EXCEEDED:
            case X509_V_ERR_INVALID_PURPOSE:
            case X509_V_ERR_CERT_UNTRUSTED:
            case X509_V_ERR_CERT_REJECTED:
            case X509_V_ERR_SUBJECT_ISSUER_MISMATCH:
            case X509_V_ERR_AKID_SKID_MISMATCH:
            case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH:
            case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
                break;
            default:
                ok=1;
                X509_STORE_CTX_set_error(ctx,X509_V_OK);
                break;
        }
    }
    switch (ctx->error) {
        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
            X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert),buf,256);
            PDEBUG("%s issuer= %s,line:(%d)\n", __func__,buf,__LINE__);
            break;
        case X509_V_ERR_CERT_NOT_YET_VALID:
        case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
            PDEBUG("%s notBefore=", __func__);
            MY_ASN1_UTCTIME_print(X509_get_notBefore(ctx->current_cert));
            fprintf(stderr,"\n");
            break;
        case X509_V_ERR_CERT_HAS_EXPIRED:
        case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
            PDEBUG("%s notAfter=", __func__);
            MY_ASN1_UTCTIME_print(X509_get_notAfter(ctx->current_cert));
            fprintf(stderr,"\n");
            break;
    }
    PDEBUG("%s verify return:%d\n", __func__,ok);
    return(ok);
}
static int my_pem_passwd_cb(char *buf, int size, int rwflag, void *userdata)
{
    strncpy(buf, (char *)(userdata), size);
    buf[size - 1] = '\0';
    return(strlen(buf));
}
static int connection_set_cipher_list(SSL_CTX *ctx,int *cipher)
{
    int *c;
    char buf[1024];
    memset(buf,0,sizeof(buf));

    if (ctx==NULL||cipher == NULL)
        return -1;
    c=cipher;
    while(*c!=0) {
        char *suite;
        switch(*c) {
            case SSL3_CIPHER_RSA_NULL_MD5:
                suite=":NULL-MD5";
                break;
            case SSL3_CIPHER_RSA_NULL_SHA:
                suite=":NULL-SHA";
                break;
            case SSL3_CIPHER_RSA_RC4_40_MD5:
                suite=":EXP-RC4-MD5";
                break;
            case SSL3_CIPHER_RSA_RC4_128_MD5:
                suite=":RC4-MD5";
                break;
            case SSL3_CIPHER_RSA_RC4_128_SHA:
                suite=":RC4-SHA";
                break;
            case SSL3_CIPHER_RSA_RC2_40_MD5:
                suite=":EXP-RC2-CBC-MD5";
                break;
            case SSL3_CIPHER_RSA_IDEA_128_SHA :
                suite=":IDEA-CBC-SHA";
                break;
            case SSL3_CIPHER_RSA_DES_40_CBC_SHA:
                suite=":EXP-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_RSA_DES_64_CBC_SHA :
                suite=":DES-CBC-SHA";
                break;
            case SSL3_CIPHER_RSA_DES_192_CBC3_SHA :
                suite=":DES-CBC3-SHA";
                break;
            case SSL3_CIPHER_DH_RSA_DES_192_CBC3_SHA:
                suite=":!DH-DES-CBC3-SHA";
                break;
            case SSL3_CIPHER_DH_DSS_DES_40_CBC_SHA :
                suite=":!EXP-DH-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_DH_DSS_DES_64_CBC_SHA:
                suite=":!DH-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_DH_DSS_DES_192_CBC3_SHA:
                suite=":!DH-DSS-DES-CBC3-SHA";
                break;
            case SSL3_CIPHER_DH_RSA_DES_40_CBC_SHA:
                suite=":!EXP-DH-RSA-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_DH_RSA_DES_64_CBC_SHA:
                suite=":!DH-RSA-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_EDH_DSS_DES_40_CBC_SHA:
                suite=":EXP-EDH-DSS-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_EDH_DSS_DES_64_CBC_SHA:
                suite=":EDH-DSS-CBC-SHA";
                break;
            case SSL3_CIPHER_EDH_DSS_DES_192_CBC3_SHA:
                suite=":EDH-DSS-DES-CBC3-SHA";
                break;
            case SSL3_CIPHER_EDH_RSA_DES_40_CBC_SHA:
                suite=":EXP-EDH-RSA-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_EDH_RSA_DES_64_CBC_SHA:
                suite=":EDH-RSA-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_EDH_RSA_DES_192_CBC3_SHA:
                suite=":EDH-RSA-DES-CBC3-SHA";
                break;
            case SSL3_CIPHER_ADH_RC4_40_MD5:
                suite=":EXP-ADH-RC4-MD5";
                break;
            case SSL3_CIPHER_ADH_RC4_128_MD5:
                suite=":ADH-RC4-MD5";
                break;
            case SSL3_CIPHER_ADH_DES_40_CBC_SHA:
                suite=":EXP-ADH-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_FZA_DMS_NULL_SHA:
                suite=":!FORTEZZA-NULL-SHA";
                break;
            case SSL3_CIPHER_CK_FZA_DMS_FZA_SHA:
                suite=":!FORTEZZA-SHA";
                break;
            case SSL3_CIPHER_CK_FZA_DMS_RC4_SHA:
                suite=":!FORTEZZA-RC4-SHA";
                break;
            case SSL3_CIPHER_CK_ADH_DES_64_CBC_SHA:
                suite=":ADH-DES-CBC-SHA";
                break;
            case SSL3_CIPHER_CK_ADH_DES_192_CBC_SHA:
                suite=":ADH-DES-CBC3-SHA";
                break;
            default:
                PDEBUG("%s SSL3: Unsupported " "cipher selection:[%d] line:[%d]\n", __func__,*c,__LINE__);
                return -1;
        }
        strcat(buf,suite);
        c++;
    }
    PDEBUG("OpenSSL: Support cipher suites: [%s]\n",buf + 1);
    if(SSL_CTX_set_cipher_list(ctx, buf + 1) != 1) {
        PDEBUG("%s Cipher suite configuration failed(%d)\n", __func__,__LINE__);
        return -1;
    }

    return 0;
}

NEXPORT int NDK_LoadClientCertificate(NDK_HANDLE handle, const char *filename, int format)
{
    SSL_CTX *tmpctx = NULL;
    if(handle==NULL||filename==NULL) {
        PDEBUG("%s handle and certfile file is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    tmpctx=((SSLHANDLE *)handle)->CurrentCtx;
    if(format==SSL_FILE_PEM||format==SSL_FILE_DER) {
        if(format==SSL_FILE_PEM) {
            if(SSL_CTX_use_certificate_file(tmpctx,filename,SSL_FILETYPE_PEM)!=1) {
                PDEBUG("%s Load client certificate file fail(%d)\n", __func__,__LINE__);
                return NDK_ERR;
            } else
                return NDK_OK;
        }
        if(format==SSL_FILE_DER) {
            if(SSL_CTX_use_certificate_file(tmpctx,filename,SSL_FILETYPE_ASN1)!=1) {
                PDEBUG("%s Load client certificate file fail(%d)\n", __func__,__LINE__);
                return NDK_ERR;
            } else
                return NDK_OK;

        }
    } else
        return NDK_ERR_SSL_MODEUNSUPPORTED;
    return NDK_OK;
}
NEXPORT int NDK_LoadClientPrivateKey(NDK_HANDLE handle, const char *filename, int format,char *password)
{
    SSL_CTX *tmpctx = NULL;
    if(handle==NULL||filename==NULL) {
        PDEBUG("%s ssl and keyfile is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    tmpctx=((SSLHANDLE *)handle)->CurrentCtx;

    if(password==NULL)
        goto DONE;
    else {
        SSL_CTX_set_default_passwd_cb(tmpctx, my_pem_passwd_cb);
        SSL_CTX_set_default_passwd_cb_userdata(tmpctx, password);
        goto DONE;
    }
DONE:
    if(format==SSL_FILE_PEM||format==SSL_FILE_DER) {
        if(format==SSL_FILE_PEM) {
            if(SSL_CTX_use_PrivateKey_file(tmpctx,filename,SSL_FILETYPE_PEM)!=1) {
                PDEBUG("%s Load client key file fail(%d),password:%s\n", __func__,__LINE__,password);
                ERR_print_errors_fp(stderr);
                return NDK_ERR;
            } else
                return NDK_OK;
        } else {
            if(SSL_CTX_use_PrivateKey_file(tmpctx,filename,SSL_FILETYPE_ASN1)!=1) {
                PDEBUG("%s Load client key file fail(%d)\n", __func__,__LINE__);
                ERR_print_errors_fp(stderr);
                return NDK_ERR;
            } else
                return NDK_OK;
        }
    } else
        return NDK_ERR_SSL_MODEUNSUPPORTED;
    return NDK_OK;
}
static int load_ca_der_or_pem(SSL_CTX *ctx, const char *ca_cert,int type)
{
    X509_LOOKUP *lookup=NULL;
    int ret = 0;
    lookup = X509_STORE_add_lookup(ctx->cert_store, X509_LOOKUP_file());
    if (lookup == NULL) {
        PDEBUG("%s Failed add lookup for X509 store(%d)\n", __func__,__LINE__);
        return -1;
    }
    if(type==SSL_FILE_DER) {

        if (!X509_LOOKUP_load_file(lookup, ca_cert, X509_FILETYPE_ASN1)) {
            unsigned long err = ERR_peek_error();
            PDEBUG("%s Load CA in DER format\n", __func__);
            if (ERR_GET_LIB(err) == ERR_LIB_X509 &&ERR_GET_REASON(err) == X509_R_CERT_ALREADY_IN_HASH_TABLE) {
                PDEBUG("%s ignoring cert already in hash table error(%d)\n", __func__,__LINE__);
            } else
                ret = -1;
        }
    } else {
        if (!X509_LOOKUP_load_file(lookup, ca_cert, X509_FILETYPE_PEM)) {
            unsigned long err = ERR_peek_error();
            PDEBUG("%s Load CA in PEM format\n", __func__);
            if (ERR_GET_LIB(err) == ERR_LIB_X509 &&ERR_GET_REASON(err) == X509_R_CERT_ALREADY_IN_HASH_TABLE) {
                PDEBUG("%s ignoring cert already in hash table error(%d)\n", __func__,__LINE__);
            } else
                ret = -1;
        }
    }

    return ret;
}

NEXPORT int NDK_LoadServerCertificate(NDK_HANDLE handle, const char *filename, int format)
{
    int ret=0;
    if(handle==NULL||filename==NULL) {
        PDEBUG("%s handle or server cafile is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    if(format==SSL_FILE_PEM||format==SSL_FILE_DER) {
        ret=load_ca_der_or_pem(((SSLHANDLE *)handle)->CurrentSsl->ctx,filename,format);
        if(ret==-1)
            return NDK_ERR;
        else
            return NDK_OK;
    } else
        return NDK_ERR_SSL_MODEUNSUPPORTED;
    return NDK_OK;
}

NEXPORT NDK_HANDLE NDK_OpenSSLSocket(int type,int auth_opt,int* cipher)
{
    //SSL库初始化
    SSL_library_init();
    //载入所有SSL算法
    OpenSSL_add_ssl_algorithms();
    //载入所有SSL错误消息
    SSL_load_error_strings();

    SSLHANDLE *myhandle=(SSLHANDLE *)malloc(sizeof(SSLHANDLE));

    SSL_METHOD *meth=NULL;

    SSL_CTX *ctx=NULL;
    switch(type) {
        case HANDSHAKE_SSLv2:
            meth=(SSL_METHOD *)SSLv2_method();
            break;
        case HANDSHAKE_SSLv3:
            meth=(SSL_METHOD *)SSLv3_method();
            break;
        case HANDSHAKE_SSLv23:
            meth=(SSL_METHOD *)SSLv23_method();
            break;
        case HANDSHAKE_TLSv1:
            meth=(SSL_METHOD *)TLSv1_method();
            break;
        default:
            PDEBUG("%s type is nonsupport(%d)\n", __func__,__LINE__);
            return NULL;
    }
    //利用meth产生一个SSL_CTX
    ctx = SSL_CTX_new(meth);
    if (!ctx) {
        PDEBUG("%s SSL_CTX_new failed(%d)\n", __func__,__LINE__);
        return NULL;
    }
    SSL_CTX_set_verify_depth(ctx,4);
    SSL_CTX_set_options(ctx,SSL_OP_ALL);
    switch(auth_opt) {
        case SSL_AUTH_NONE:/*服务器模式:服务器不会给一个客户端证书请求客户机,那么客户端将不会发送一个证书。
    客户机模式:如果不使用一个匿名的密码(默认情况下禁用),服务器就会发送一个证书,然后将被检查。*/
            SSL_CTX_set_verify(ctx,SSL_VERIFY_NONE,NULL);
            PDEBUG("MODE is SSL_VERIFY_NONE\n");
            break;
        case SSL_AUTH_CLIENT:/*服务器模式:服务器发送一个客户端证书请求的客户机。返回的证书(如果有的话)是检查。
    客户机模式:验证的服务器证书。*/
            SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,verify_callback);
            PDEBUG("MODE is SSL_VERIFY_PEER\n");
            break;
        default:
            PDEBUG("%s MODE is NONSUPPORT(%d)\n", __func__,__LINE__);
            return NULL;
    }
    if(SSL_CTX_set_default_verify_paths(ctx)==0)
        PDEBUG("%s SSL_CTX_set_default_verify_paths failed(%d)\n", __func__,__LINE__);

    if(connection_set_cipher_list(ctx,cipher)!=0) {
        PDEBUG("%s Cipher suite configuration failed(%d)\n", __func__,__LINE__);
        return NULL;
    }
    myhandle->CurrentSsl=SSL_new(ctx);
    myhandle->CurrentCtx=ctx;
    myhandle->CloseFlag=0;
    myhandle->CurrentSocketMode=NDK_SUSPEND;
    myhandle->TimeOutFlag=0;
    myhandle->CurrentConnectState=SSL_IS_DISCONNECTED;

    return myhandle;
}

NEXPORT int NDK_CloseSSLSocket(NDK_HANDLE handle)
{
    int flag=0;
    SSL *tmpssl = NULL;
    SSL_CTX *tmpctx = NULL;
    if(handle==NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    } else {
        tmpssl=((SSLHANDLE *)handle)->CurrentSsl;
        tmpctx=((SSLHANDLE *)handle)->CurrentCtx;
        if(((SSLHANDLE *)handle)->CloseFlag==1) {
            PDEBUG("%s Current handle already close(%d)\n", __func__,__LINE__);
            return NDK_ERR_SSL_ALREADCLOSE;
        }
        if(tmpssl!=NULL)
            NDK_SSLDisconnect(handle);
    }
    if(tmpssl!=NULL) {
        close(SSL_get_fd(((SSLHANDLE *)handle)->CurrentSsl));
        SSL_clear(((SSLHANDLE *)handle)->CurrentSsl);
        SSL_free(((SSLHANDLE *)handle)->CurrentSsl);
    }
    if(tmpctx!=NULL)
        SSL_CTX_free(((SSLHANDLE *)handle)->CurrentCtx);
    ERR_free_strings();
    ((SSLHANDLE *)handle)->CurrentCtx=NULL;
    ((SSLHANDLE *)handle)->CurrentSsl=NULL;
    ((SSLHANDLE *)handle)->CloseFlag=1;
    ((SSLHANDLE *)handle)->CurrentSocketMode=NDK_SUSPEND;
    return NDK_OK;
}
NEXPORT int NDK_SSLDisconnect(NDK_HANDLE handle)
{

    int ret,flag=0,sd;
    int state=0;
    SSL *tmpssl=NULL;
    if(handle==NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    } else {
        tmpssl=((SSLHANDLE *)handle)->CurrentSsl;
        ((SSLHANDLE *)handle)->TimeOutFlag=0;
        NDK_GetSSLConnectStatus(handle,&state);
        if(state==SSL_IS_DISCONNECTED) {
            if(((SSLHANDLE *)handle)->CloseFlag==1) {
                PDEBUG("%s Current handle already closeed(%d)\n", __func__,__LINE__);
                return  NDK_ERR_SSL_ALREADCLOSE;
            }
            SSL_clear(((SSLHANDLE *)handle)->CurrentSsl);
            return NDK_OK;
        } else {
            sd=SSL_get_fd(tmpssl);
            if(sd>0)
                ret=shutdown(sd,1);
            SSL_set_shutdown(tmpssl,SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN);
            ret=SSL_shutdown(tmpssl);
            if(!ret) {
                shutdown(sd,1);
                ret=SSL_shutdown(tmpssl);
                switch(ret) {
                    case 1:
                        flag=1;
                        break;
                    case 0:
                    case -1:
                    default:
                        flag=0;
                        break;
                }
            } else
                flag=1;
        }

    }
    if(flag==1) {
        ret=close(sd);
        SSL_clear(((SSLHANDLE *)handle)->CurrentSsl);
        PDEBUG("%s succ(%d)\n", __func__,__LINE__);
        return NDK_OK;
    } else {
        PDEBUG("%s fail(%d)\n", __func__,__LINE__);
        return NDK_ERR;
    }

}
static void *pthreadcreate(void *arg)
{
    int code,tmpouttime = 0;
    time_t oldtime,changetime;
    SSLHANDLE *temphandle;
    SSL *tmpssl;
    temphandle=(SSLHANDLE *)arg;
    tmpssl=((SSLHANDLE *)temphandle)->CurrentSsl;
    tmpouttime=((SSLHANDLE *)temphandle)->timeout/1000;

    oldtime=time(NULL);
    while(1) {
        if(((SSLHANDLE *)temphandle)->CloseFlag==1)
            return (void *)0;
        code=SSL_connect(tmpssl);
        if(code<=0) {
            switch(SSL_get_error(tmpssl,code)) {
                case SSL_ERROR_WANT_READ:
                    break;
                default:
                    return (void *)0;
            }
        } else {
            ((SSLHANDLE *)temphandle)->TimeOutFlag=0;
            return (void *)0;
        }
        usleep(10);
        changetime=time(NULL)-oldtime;
        if(changetime>45) {
            ((SSLHANDLE *)temphandle)->TimeOutFlag=1;
            return (void *)0;
        }
    }

}
static void GPNDK_sslpthread(NDK_HANDLE handle)
{
    pthread_t tidp;
    int error;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    error=pthread_create(&tidp,&attr,pthreadcreate,(void *)handle);
    if(error!=0) {
        PDEBUG("%s pthread_create fail(%d)\n", __func__,__LINE__);
        return;
    }
}
NEXPORT int NDK_SSLConnect(NDK_HANDLE handle, ST_SOCKET_ADDR *pServer, int timeout)
{
    int ret,sd,res,len,code=0,arg,val;
    int server_addr_len;
    struct sockaddr_in serveraddr;
    struct timeval time_out, starttv,endtv;
    int ms;
    fd_set s;
    SSL *tmpssl = NULL;

    if(handle==NULL||pServer==NULL||timeout<0) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Can not connect a closed handle(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    tmpssl=SSL_new(((SSLHANDLE *)handle)->CurrentCtx);
    if(tmpssl==NULL) {
        PDEBUG("%s SSL_new failed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALLOC;
    }

    sd= socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0) {
        perror("socket");
        return NDK_ERR_SSL_ALLOC;
    }

    if(SSL_set_fd(tmpssl,sd)==0) {
        PDEBUG("%s SSL_set_fd failed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALLOC;
    }


    //设置地址重用
    int opt=1;
    res=setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    if(res<0) {
        PDEBUG("%s setsockopt failed(%d)\n", __func__,__LINE__);
        perror("setsockopt");
        return NDK_ERR;
    }

    /**<设置非阻塞*/
    if( (arg = fcntl(sd, F_GETFL, NULL)) < 0) {
        PDEBUG("%s fcntl failed(%d)\n", __func__,__LINE__);
        return NDK_ERR;
    }
    //arg &= ~O_NONBLOCK;
    arg |= O_NONBLOCK;
    if( fcntl(sd, F_SETFL, arg) < 0) {
        PDEBUG("%s fcntl failed(%d)\n", __func__,__LINE__);
        return NDK_ERR;
    }
    //绑定服务器地址
    memset(&serveraddr,'\0',sizeof(serveraddr));
    if(pServer->unAddrType==SSL_ADDR_IPV4)
        serveraddr.sin_family=AF_INET;
    else
        serveraddr.sin_family=AF_INET6;

    serveraddr.sin_port=htons(pServer->usPort);
    if(inet_aton((char *)(pServer->psAddr),(struct in_addr *)&serveraddr.sin_addr.s_addr)==0) {
        PDEBUG("%s inet_aton error(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_INVADDR;
    }

    server_addr_len=sizeof(serveraddr);
    ret = connect(sd,(struct sockaddr*)&serveraddr, server_addr_len);
    if(ret<0) {
        if(errno==EINPROGRESS) {
            do {
                len=sizeof(int);
                FD_ZERO(&s);
                FD_SET(sd,&s);
                time_out.tv_sec=timeout/1000;
                time_out.tv_usec=(timeout%1000)*1000;
                res=select(sd+1, NULL, &s, NULL, &time_out);               
                if(res>0) {
                    if (getsockopt(sd, SOL_SOCKET, SO_ERROR, (void*)(&val), &len) < 0) {
                        shutdown(sd, SHUT_RDWR);
                        goto ERR;
                    }
                    if (val) {
                        shutdown(sd, SHUT_RDWR);
                        goto ERR;
                    }
                    goto CONNECT;
                } else if(res<0) {
                    if (EINTR == errno)
                        continue;
                    shutdown(sd, SHUT_RDWR);
                    goto ERR;
                } else {
                    goto TIMEOUTERR;
                }
            } while(1);
        } else {
            shutdown(sd, SHUT_RDWR);
            goto ERR;
        }
    }
CONNECT:
    gettimeofday(&starttv,NULL);
    while(1) {
        code=SSL_connect(tmpssl);
        if(code<=0) {
            if(((SSLHANDLE *)handle)->CurrentSocketMode==NDK_NOWAIT) {
                ((SSLHANDLE *)handle)->CurrentSsl=tmpssl;
                ((SSLHANDLE *)handle)->timeout=timeout;
                GPNDK_sslpthread((SSLHANDLE *)handle);
                return NDK_OK;
            } else {
                switch(SSL_get_error(tmpssl,code)) {
                    case SSL_ERROR_WANT_READ:
                        usleep(100);
                        break;
                    default:
                        goto ERR;
                }
            }
        } else {
            ((SSLHANDLE *)handle)->CurrentSsl=tmpssl;
            goto SUCC;
        }
        gettimeofday(&endtv,NULL);
        ms=(endtv.tv_sec * 1000 + endtv.tv_usec/1000)-(starttv.tv_sec * 1000 + starttv.tv_usec/1000);
        if(ms>timeout)
            goto TIMEOUTERR;
    }
SUCC:
    PDEBUG("%s succ,%d\n",__func__,__LINE__);
    return NDK_OK;
ERR:
    PDEBUG("%s err,%d\n",__func__,__LINE__);
    perror("connect");
    return NDK_ERR;
TIMEOUTERR:
    PDEBUG("%s timeout,%d\n",__func__,__LINE__);
    perror("connect");
    return NDK_ERR_SSL_TIMEOUT;
}

NEXPORT int NDK_GetSSLBlockingMode(NDK_HANDLE handle)
{
    if(handle==NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }

    return ((SSLHANDLE *)handle)->CurrentSocketMode;
}
NEXPORT int NDK_SetSSLBlockingMode(NDK_HANDLE handle,int mode)
{
    if(handle==NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    if(mode==NDK_NOWAIT) {
        ((SSLHANDLE *)handle)->CurrentSocketMode=NDK_NOWAIT;
    } else if(mode==NDK_SUSPEND) {
        ((SSLHANDLE *)handle)->CurrentSocketMode=NDK_SUSPEND;
        ((SSLHANDLE *)handle)->TimeOutFlag=0;
    } else {
        PDEBUG("%s mode is unsupported(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_MODEUNSUPPORTED;
    }
    return NDK_OK;
}
NEXPORT int NDK_SSLSend(NDK_HANDLE handle, const char *pBuffer, size_t SizeOfBuffer, size_t *sent_data)
{
    int state=0;
    if (handle == NULL) {
        PDEBUG("%s SSL is failed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if ((pBuffer==NULL)||(sent_data==NULL)) {
        PDEBUG("%s pBuffer,sent_data is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    NDK_GetSSLConnectStatus(handle,&state);
    if(state==SSL_IS_CONNECTED) {
        if (((*sent_data)=SSL_write(((SSLHANDLE *)handle)->CurrentSsl, pBuffer, SizeOfBuffer)) <= 0) {
            PDEBUG("%s SSL_write failed(%d)\n", __func__,__LINE__);
            return NDK_ERR_SSL_SEND;
        }
    } else
        return NDK_ERR;
    return NDK_OK;
}
NEXPORT int NDK_SSLReceive(NDK_HANDLE handle, void *pBuffer, const size_t SizeOfBuffer, size_t *recv_data)
{
    int state=0;
    int len = 0,totallen = 0;
    SSL *tmpssl = NULL;

    if (handle == NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }

    if ((pBuffer==NULL)||(recv_data==NULL)) {
        PDEBUG("%s pBuffer,recv_data is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    tmpssl=((SSLHANDLE *)handle)->CurrentSsl;
    NDK_GetSSLConnectStatus(handle,&state);
    if(state==SSL_IS_CONNECTED) {
        while(1) {
            len=SSL_read(tmpssl, pBuffer+totallen, SizeOfBuffer-totallen);
            if(len<=0) {
                switch(SSL_get_error(tmpssl, len)) {
                    case SSL_ERROR_WANT_READ:
                        usleep(10);
                        break;
                    default:
                        goto ERR;
                }
            } else {
                totallen+=len;
                if(totallen<SizeOfBuffer&&(((SSLHANDLE *)handle)->CurrentSocketMode==NDK_SUSPEND))
                    continue;
                else
                    goto SUCC;
            }
        }
    } else {
        *recv_data=0;
        return NDK_ERR;
    }
SUCC:
    *recv_data=totallen;
    return NDK_OK;
ERR:
    *recv_data=totallen;
    return NDK_ERR;
}

NEXPORT int NDK_SSLBind(NDK_HANDLE handle, ST_SOCKET_ADDR *Address)
{
    int res,sd;
    int server_addr_len;
    struct sockaddr_in serveraddr;
    if(handle==NULL||Address==NULL) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Can not bind a closed handle(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    sd= socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0) {
        perror("socket");
        return NDK_ERR_SSL_ALLOC;
    }

    //设置地址重用
    int opt=1;
    res=setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    if(res<0) {
        PDEBUG("%s setsockopt fail(%d)\n", __func__,__LINE__);
        return NDK_ERR;
    }

    //绑定服务器地址
    memset(&serveraddr,'\0',sizeof(serveraddr));
    if(Address->unAddrType==SSL_ADDR_IPV4)
        serveraddr.sin_family=AF_INET;
    else
        serveraddr.sin_family=AF_INET6;
    serveraddr.sin_port=htons(Address->usPort);
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    server_addr_len=sizeof(serveraddr);
    if(bind(sd,(struct sockaddr*)&serveraddr, server_addr_len)==-1) {
        PDEBUG("%s bind fail(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_INVADDR;
    }
    ((SSLHANDLE *)handle)->CurrentSocket=sd;
    return NDK_OK;
}

NEXPORT int NDK_GetSSLConnectStatus(NDK_HANDLE handle, int *state)
{
    int flag=0;
    if(handle==NULL||state==NULL) {
        PDEBUG("%s ssl or state is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    if(((SSLHANDLE *)handle)->TimeOutFlag==1)
        return NDK_ERR_SSL_TIMEOUT;
    SSL *tempSSl=((SSLHANDLE *)handle)->CurrentSsl;
    if(((tempSSl->state)&SSL_ST_BEFORE)==SSL_ST_BEFORE) {
        (*state)=SSL_IS_DISCONNECTED;
        flag=1;
    } else if(((tempSSl->state)&SSL_ST_OK)==SSL_ST_OK) {
        (*state)=SSL_IS_CONNECTED;
        flag=1;
    } else {
        (*state)=SSL_CONNECTION_IN_PROGRESS;
        flag=1;

    }
    if(flag==1)
        return NDK_OK;
    else
        return NDK_ERR;
}

NEXPORT int NDK_SSLDataAvailable(NDK_HANDLE handle, uint timeout)
{
    int state=0;
    if(handle==NULL||timeout<0) {
        PDEBUG("%s ssl is NULL(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_PARAM;
    }
    ((SSLHANDLE *)handle)->TimeOutFlag=0;
    NDK_GetSSLConnectStatus(handle,&state);
    if(((SSLHANDLE *)handle)->CloseFlag==1) {
        PDEBUG("%s Current handle already closed(%d)\n", __func__,__LINE__);
        return NDK_ERR_SSL_ALREADCLOSE;
    }
    if(state!=SSL_IS_CONNECTED)
        return NDK_ERR_SSL_CONNECT;
    int fd=SSL_get_fd(((SSLHANDLE *)handle)->CurrentSsl);
    int maxfd,nread;
    maxfd=fd;
    fd_set s;
    FD_ZERO(&s);
    FD_SET(fd,&s);
    struct  timeval tv;
    tv.tv_sec=timeout/100;
    tv.tv_usec=((timeout*10)%1000)*1000;
    while(1) {
        nread=select(maxfd+1,&s,NULL,NULL,&tv);
        if(nread==-1) {
            if(errno==EINTR)
                continue;
            else
                return NDK_ERR_SSL_TIMEOUT;
        } else if(nread==0) {
            //PDEBUG("time out\n");
            return NDK_ERR_SSL_TIMEOUT;
        } else
            return NDK_OK;
    }
}
void *NDK_SSLGetPeerCerificate(NDK_HANDLE pvHandle)
{
    if(pvHandle==NULL)
        return NULL;
    return SSL_get_peer_certificate(((SSLHANDLE *)pvHandle)->CurrentSsl);
}


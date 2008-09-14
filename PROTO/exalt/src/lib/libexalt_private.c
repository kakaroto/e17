/*
 * =====================================================================================
 *
 *       Filename:  libexalt_private.c
 *
 *    Description:  private functions
 *
 *        Version:  1.0
 *        Created:  09/03/2007 08:59:47 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "libexalt_private.h"


Exalt_Ioctl_Key exalt_ioctl_key[] =
{
    {SIOCSIFFLAGS, "SIOCSIFFLAGS"},
    {SIOCDELRT, "SIOCDELRT"},
    {SIOCSIFADDR, "SIOCSIFADDR"},
    {SIOCSIFNETMASK, "SIOCSIFNETMASK"},
    {SIOCADDRT, "SIOCADDRT"},
    {SIOCETHTOOL, "SIOCETHTOOL"},
    {SIOCGIWNAME, "SIOCGIWNAME"},
    {SIOCGIWESSID, "SIOCGIWESSID"},
    {SIOCGIWNAME, "SIOCGIWNAME"},
    {SIOCGIFFLAGS, "SIOCGIFFLAGS"},
    {SIOCGIFADDR, "SIOCGIFADDR"},
    {SIOCGIFNETMASK, "SIOCGIFNETMASK"},
    {SIOCGIFHWADDR, "SIOCGIFHWADDR"}
};



/**
 * @brief execute a ioctl call
 * @param argp the strucuture with data (struct ifreq, rtentry, iwreq)
 * @param request the request key (SIOCGIWNAME ...)
 * @return Return 1 if ok, else 0
 */
short exalt_ioctl(void* argp, int request)
{
    int fd;
    short busy = 0;
    char* ioctl_msg_buf[1024];
    char* ioctl_msg;

    snprintf(ioctl_msg_buf,1024,"%d",request);
    ioctl_msg = ioctl_msg_buf;

int i;
    for (i = 0; i < sizeof (exalt_ioctl_key) / sizeof (Exalt_Ioctl_Key); ++i)
        if (request == exalt_ioctl_key[i].key)
        {
            ioctl_msg = exalt_ioctl_key[i].value;
            break;
        }


    //edit param: SIOCSIFFLAGS SIOCSIFFLAGS SIOCDELRT SIOCSIFADDR SIOCSIFNETMASK SIOCADDRT SIOCETHTOOL
    //read param: SIOCGIWNAME SIOCGIWESSID SIOCGIWNAME SIOCGIFFLAGS SIOCGIFADDR SIOCGIFNETMASK SIOCGIFHWADDR

    EXALT_ASSERT_RETURN(!
            (!exalt_is_admin() &&
            ( request == SIOCSIFFLAGS || request == SIOCSIFFLAGS
              || request == SIOCDELRT || request == SIOCSIFADDR
              || request == SIOCSIFNETMASK || request == SIOCADDRT
              || request== SIOCETHTOOL )));

    EXALT_ASSERT_RETURN(argp);

    fd=iw_sockets_open();
    EXALT_ASSERT_RETURN(fd>=0);

    //Sometimes the ressource is busy, we can wait a little time and retry (only 3 times)
    do{
        busy++;
        if(ioctl(fd, request, argp) ==-1)
        {
            if( (busy==1 || busy==3 || busy==5 ) && errno==11)//ressource not available
            {
                busy++;
                EXALT_ASSERT_ADV(0,,"ioctl(%s): %s (%d) (will retry)",ioctl_msg,strerror(errno),errno);
                usleep(50000);
            }
            else
            {
                close(fd);
                EXALT_ASSERT_ADV(0,,"ioctl(%s): %s (%d)",ioctl_msg,strerror(errno),errno);
                return 0;
            }
        }
    }while(busy == 2 || busy==4 || busy==6);

    close(fd);
    return 1;
}

/**
 * @brief convert a hexadecimal address to a decimal address (xxx.xxx.xxx.xxx)
 * @param addr the address
 * @return Return the address in decimal format
 */
char* exalt_addr_hexa_to_dec(const char* addr)
{
    char *res;
    char buf[4];
    char* end;
    int i;
    int n;
    EXALT_ASSERT_RETURN(strlen(addr)==8);

    res = (char*)malloc((unsigned int)sizeof(char)*16);
    res[0] = '\0';
    for(i=0;i<8;i+=2)
    {
        buf[0] = addr[7-i-1];
        buf[1] = addr[7-i];
        buf[2] = '\0';
        n = strtoul(buf,&end,16);
        sprintf(buf,"%d",n);
        strcat(res,buf);
        if(i<6)
            strcat(res,".");
    }
    return res;
}

/**
 * @brief remove a substring in a string
 * @param s1 the string
 * @param ct the substring
 * @return Return the new string
 */
char *str_remove (const char *s, const char *ct)
{
    char *new_s = NULL;
    char* start = strstr(s,ct);
    int l = strlen(s);
    const char* end_s = s + l +1;
    char* c_new_s;
    if (s && ct && start)
    {
        size_t size = strlen (s) - strlen(ct);

        new_s = malloc (sizeof (char) * (size + 1));
        c_new_s = new_s;
        if (new_s)
        {
            while(s<end_s)
            {
                if(s<start || s>=start + strlen(ct))
                {
                    *c_new_s = *s;
                    c_new_s++;
                }
                s++;
            }
        }
    }
    return new_s;
}








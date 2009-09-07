/*
 * =====================================================================================
 *
 *       Filename:  iwlib_custom.c
 *
 *    Description: Here are the change made for Exalt to iwlib.c
 *                      - The original code comes from wireless_tools (iwlib and iwlist)
 *                      - new/modifies functions
 *                      - new enumerations
 *                      - add more informations (wpa, tkip ...), the code comes from iwlist.c
 *                      - The code has the same aspect as the original, the aim is to get something than we can update easily when wireless_tools is update.
 *
 *        Version:  1.0
 *        Created:  15/02/09 19:57:53 CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "iwlib.c"

/*********************** SCANNING SUBROUTINES ***********************/
/*
 * The Wireless Extension API 14 and greater define Wireless Scanning.
 * The normal API is complex, this is an easy API that return
 * a subset of the scanning results. This should be enough for most
 * applications that want to use Scanning.
 * If you want to have use the full/normal API, check iwlist.c...
 *
 * Precaution when using scanning :
 * The scanning operation disable normal network traffic, and therefore
 * you should not abuse of scan.
 * The scan need to check the presence of network on other frequencies.
 * While you are checking those other frequencies, you can *NOT* be on
 * your normal frequency to listen to normal traffic in the cell.
 * You need typically in the order of one second to actively probe all
 * 802.11b channels (do the maths). Some cards may do that in background,
 * to reply to scan commands faster, but they still have to do it.
 * Leaving the cell for such an extended period of time is pretty bad.
 * Any kind of streaming/low latency traffic will be impacted, and the
 * user will perceive it (easily checked with telnet). People trying to
 * send traffic to you will retry packets and waste bandwidth. Some
 * applications may be sensitive to those packet losses in weird ways,
 * and tracing those weird behavior back to scanning may take time.
 * If you are in ad-hoc mode, if two nodes scan approx at the same
 * time, they won't see each other, which may create associations issues.
 * For those reasons, the scanning activity should be limited to
 * what's really needed, and continuous scanning is a bad idea.
 * Jean II
 */

/* Functions add by Watchwolf for Exalt
 * These functions add the detection of the encryption type (WPA, TKIP, CCMP ...)
 * This code is inspired by the code of iwlist.c from the wireless_tools apps
 * Use the exalt type Exalt_Wireless_Network to store a wireless network
 */
#define IW_ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

/* Values for the IW_IE_KEY_MGMT_* in GENIE */
static const Exalt_Wireless_Network_Auth_Suites	iw_ie_key_mgmt_name[] = {
    AUTH_SUITES_NONE,
    AUTH_SUITES_8021X,
    AUTH_SUITES_PSK,
};
#define	IW_IE_KEY_MGMT_NUM	IW_ARRAY_LEN(iw_ie_key_mgmt_name)

static const Exalt_Wireless_Network_Cypher_Name	iw_ie_cypher_name[] = {
    CYPHER_NAME_NONE,
    CYPHER_NAME_WEP40,
    CYPHER_NAME_TKIP,
    CYPHER_NAME_WRAP,
    CYPHER_NAME_CCMP,
    CYPHER_NAME_WEP104,
};
#define	IW_IE_CYPHER_NUM	IW_ARRAY_LEN(iw_ie_cypher_name)

    static Exalt_Wireless_Network_Cypher_Name
iw_process_cypher_name(unsigned int		    value,
        const Exalt_Wireless_Network_Cypher_Name    names[],
        const unsigned int		            num_names)
{
    if(value >= num_names)
        return CYPHER_NAME_UNKNOWN;
    else
        return names[value];
}

    static Exalt_Wireless_Network_Auth_Suites
iw_process_auth_name(unsigned int		    value,
        const Exalt_Wireless_Network_Auth_Suites    names[],
        const unsigned int		            num_names)
{
    if(value >= num_names)
        return AUTH_SUITES_UNKNOWN;
    else
        return names[value];
}

    static inline void
iw_process_ie_wpa(unsigned char* iebuf,
        int buflen,
        Exalt_Wireless_Network_IE* ie)
{
    int ielen = iebuf[1] +2;
    int offset = 2;
    unsigned char wpa1_oui[3] = {0x00, 0x50, 0xf2};
    unsigned char wpa2_oui[3] = {0x00, 0x0f, 0xac};
    unsigned char* wpa_oui;
    int i;
    uint16_t ver = 0;
    uint16_t cnt =0;

    if(ielen > buflen)
        ielen = buflen;

    switch(iebuf[0])
    {
        case 0x30:  /*WPA2*/
            /* Check if we have enough data */
            if(ielen < 4)
            {
                exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_UNKNOWN);
                return;
            }

            wpa_oui = wpa2_oui;
            break;
        case 0xdd: /* WPA or others */
            wpa_oui = wpa1_oui;

            /* Not all IEs that start with 0xdd are WPA
             * So check that the OUI is valid. Note : offset = 2 */
            if((ielen<8)
                    || (memcmp(&iebuf[offset], wpa_oui, 3) !=0)
                    || (iebuf[offset+3]!= 0x01))
            {
                exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_UNKNOWN);
                return ;
            }

            /* Skip the OUI type */
            offset += 4;
            break;
        default:
            return;
    }

    /* Pick version numer (little endian) */
    ver = iebuf[offset] | (iebuf[offset+1] << 8);
    offset += 2;

    if(iebuf[0] == 0xdd)
        exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_WPA);
    if(iebuf[0] == 0x30)
        exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_WPA2);
    exalt_wireless_network_ie_wpa_version_set(ie,ver);

    /* Check if we are done */
    if(ielen < (offset + 4))
    {
        /* We have a short UE, So we should assume TKIP/TKIP */
        exalt_wireless_network_ie_group_cypher_set(ie,CYPHER_NAME_TKIP);
        exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_TKIP,0);
        exalt_wireless_network_ie_pairwise_cypher_number_set(ie,1);
        return ;
    }

    /* Next we have our group cipher */
    if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
        exalt_wireless_network_ie_group_cypher_set(ie,CYPHER_NAME_PROPRIETARY);
    else
        exalt_wireless_network_ie_group_cypher_set(ie,iw_process_cypher_name(iebuf[offset+3], iw_ie_cypher_name, IW_IE_CYPHER_NUM));

    offset += 4;

    /* Check if we are done */
    if(ielen < (offset + 2))
    {
        /* We don't have a pairwise cipher, or auth method. Assume TKIP */
        exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_TKIP,0);
        exalt_wireless_network_ie_pairwise_cypher_number_set(ie,1);
        return;
    }

    /* otherwise, we have some number of pairwise ciphers */
    cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
    offset += 2;
    exalt_wireless_network_ie_pairwise_cypher_number_set(ie,cnt);

    if(ielen < (offset + 4*cnt))
        return;

    for(i = 0; i < cnt; i++)
    {
        if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
        {
            exalt_wireless_network_ie_pairwise_cypher_set(ie,
                    CYPHER_NAME_PROPRIETARY,
                    i);
        }
        else
        {
            exalt_wireless_network_ie_pairwise_cypher_set(ie,
                    iw_process_cypher_name(iebuf[offset+3],
                        iw_ie_cypher_name, IW_IE_CYPHER_NUM),
                    i);
        }
        offset+=4;
    }

    /* Check if we are done */
    if(ielen < (offset + 2))
        return;

    /* Now, we have authentication suites. */
    cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
    offset += 2;
    exalt_wireless_network_ie_auth_suites_number_set(ie,cnt);

    if(ielen < (offset + 4*cnt))
        return;

    for(i = 0; i < cnt; i++)
    {
        if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
        {
            exalt_wireless_network_ie_auth_suites_set(ie,
                    AUTH_SUITES_UNKNOWN,
                    i);
        }
        else
        {
            exalt_wireless_network_ie_auth_suites_set(ie,
                    iw_process_auth_name(iebuf[offset+3],
                        iw_ie_key_mgmt_name, IW_IE_KEY_MGMT_NUM),
                    i);
        }
        offset+=4;
    }

    /* Check if we are done */
    if(ielen < (offset + 1))
        return;

    /* Otherwise, we have capabilities bytes.
     * For now, we only care about preauth which is in bit position 1 of the
     * first byte.  (But, preauth with WPA version 1 isn't supposed to be
     * allowed.) 8-) */
    if(iebuf[offset] & 0x01)
        exalt_wireless_network_ie_preauth_supported_set(ie,1);
}

    static inline void
iw_process_gen_ie(unsigned char* buffer,
        int buflen,
        Exalt_Wireless_Network* wscan)
{
    int offset =  0;

    /* Loop on each IE, each IE is minimum 2 bytes */
    while(offset <= (buflen-2))
    {
        /* Check IE type */
        switch(buffer[offset])
        {
            case 0xdd: /* WPA1 (and other) */
            case 0x30: /* WPA2 */
            {
                Exalt_Wireless_Network_IE* ie =
                    exalt_wireless_network_ie_new();
                Eina_List* l = exalt_wireless_network_ie_get(wscan);
                l = eina_list_append(l,ie);
                exalt_wireless_network_ie_set(wscan,l);

                iw_process_ie_wpa(buffer+offset, buflen, ie);
                break;
            }
            default:
                //iw_process_ie_unknown(buffer+offset , bufflen);
                ;
        }
        /* Skip over this IE to the next one in the list */
        offset += buffer[offset+1] + 2;
    }
}

/*------------------------------------------------------------------*/
/*
 * Process/store one element from the scanning results in wireless_scan
 */
    static inline Exalt_Wireless_Network*
iw_process_scanning_token(struct iw_event *	    event,
        Exalt_Wireless_Network *	            wscan,
        Eina_List**                                 networks,
        Exalt_Wireless*                             w)
{
    /* Now, let's decode the event */
    switch(event->cmd)
    {
        case SIOCGIWAP:
            /* New cell description. Allocate new cell descriptor, zero it. */
            wscan = exalt_wireless_network_new(w);
            if(wscan == NULL)
                return(wscan);
            (*networks) = eina_list_append(*networks,wscan);

            char buf[18];
            iw_ether_ntop((struct ether_addr* )event->u.ap_addr.sa_data,buf);

            exalt_wireless_network_address_set(wscan,buf);
            break;
        case SIOCGIWNWID:
            break;
        case SIOCGIWFREQ:
            break;
        case SIOCGIWMODE:
            if((event->u.mode < IW_NUM_OPER_MODE)
                    && (event->u.mode >= 0))
            {
                exalt_wireless_network_mode_set(wscan,exalt_wireless_network_mode_from_mode_id(event->u.mode));
            }
            break;
        case SIOCGIWESSID:
            if((event->u.essid.pointer) && (event->u.essid.length))
            {
                char essid[IW_ESSID_MAX_SIZE+1];
                memset(essid,'\0', sizeof(essid));
                memcpy(essid,event->u.essid.pointer, event->u.essid.length);
                exalt_wireless_network_essid_set(wscan,essid);
            }
            break;
        case SIOCGIWENCODE:
            if(!event->u.data.pointer)
                event->u.data.flags |= IW_ENCODE_NOKEY;
            if(event->u.data.flags & IW_ENCODE_DISABLED)
                exalt_wireless_network_encryption_set(wscan,0);
            else
            {
                exalt_wireless_network_encryption_set(wscan,1);
                if(event->u.data.flags & IW_ENCODE_RESTRICTED)
                    exalt_wireless_network_security_mode_set(wscan,SECURITY_RESTRICTED);
                if(event->u.data.flags & IW_ENCODE_OPEN)
                    exalt_wireless_network_security_mode_set(wscan,SECURITY_OPEN);
            }
            break;
        case IWEVQUAL:
            exalt_wireless_network_quality_set(wscan,event->u.qual.qual);
            break;
        case SIOCGIWRATE:
            break;
        case IWEVGENIE:
                iw_process_gen_ie(event->u.data.pointer, event->u.data.length, wscan);
            break;
        case IWEVCUSTOM:
        default:
            break;
    }

    return wscan;
}

/*------------------------------------------------------------------*/
/*
 * Initiate the scan procedure, and process results.
 * This is a non-blocking procedure and it will return each time
 * it would block, returning the amount of time the caller should wait
 * before calling again.
 * Return -1 for error, delay to wait for (in ms), or 0 for success.
 * Error code is in errno
 */
    int
iw_process_scan(int		skfd,
        char *			ifname,
        int			we_version,
        Eina_List**             networks,
        Exalt_Wireless*         w,
        int*                    retry)
{
    struct iwreq		wrq;
    unsigned char *	buffer = NULL;		/* Results */
    int			buflen = IW_SCAN_MAX_DATA; /* Min for compat WE<17 */
    unsigned char *	newbuf;

    /* Don't waste too much time on interfaces (150 * 100 = 15s) */
    (*retry)++;
    if(*retry > 150)
    {
        errno = ETIME;
        return(-1);
    }

    /* If we have not yet initiated scanning on the interface */
    if(*retry == 1)
    {
        /* Initiate Scan */
        wrq.u.data.pointer = NULL;		/* Later */
        wrq.u.data.flags = 0;
        wrq.u.data.length = 0;
        /* Remember that as non-root, we will get an EPERM here */
        if((iw_set_ext(skfd, ifname, SIOCSIWSCAN, &wrq) < 0)
                && (errno != EPERM))
            return(-1);
        /* Success : now, just wait for event or results */
        return(250);	/* Wait 250 ms */
    }

realloc:
    /* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
    newbuf = realloc(buffer, buflen);
    if(newbuf == NULL)
    {
        /* man says : If realloc() fails the original block is left untouched */
        if(buffer)
            free(buffer);
        errno = ENOMEM;
        return(-1);
    }
    buffer = newbuf;

    /* Try to read the results */
    wrq.u.data.pointer = buffer;
    wrq.u.data.flags = 0;
    wrq.u.data.length = buflen;
    if(iw_get_ext(skfd, ifname, SIOCGIWSCAN, &wrq) < 0)
    {
        /* Check if buffer was too small (WE-17 only) */
        if((errno == E2BIG) && (we_version > 16))
        {
            /* Some driver may return very large scan results, either
             * because there are many cells, or because they have many
             * large elements in cells (like IWEVCUSTOM). Most will
             * only need the regular sized buffer. We now use a dynamic
             * allocation of the buffer to satisfy everybody. Of course,
             * as we don't know in advance the size of the array, we try
             * various increasing sizes. Jean II */

            /* Check if the driver gave us any hints. */
            if(wrq.u.data.length > buflen)
                buflen = wrq.u.data.length;
            else
                buflen *= 2;

            /* Try again */
            goto realloc;
        }

        /* Check if results not available yet */
        if(errno == EAGAIN)
        {
            free(buffer);
            /* Wait for only 100ms from now on */
            return(100);	/* Wait 100 ms */
        }

        free(buffer);
        /* Bad error, please don't come back... */
        return(-1);
    }

    /* We have the results, process them */
    if(wrq.u.data.length)
    {
        struct iw_event		iwe;
        struct stream_descr	stream;
        int			ret;
        Exalt_Wireless_Network* wscan = NULL;

#ifdef DEBUG
        /* Debugging code. In theory useless, because it's debugged ;-) */
        int	i;
        printf("Scan result [%02X", buffer[0]);
        for(i = 1; i < wrq.u.data.length; i++)
            printf(":%02X", buffer[i]);
        printf("]\n");
#endif

        /* Init */
        iw_init_event_stream(&stream, (char *) buffer, wrq.u.data.length);

        /* Look every token */
        do
        {
            /* Extract an event and print it */
            ret = iw_extract_event_stream(&stream, &iwe, we_version);
            if(ret > 0)
            {
                /* Convert to wireless_scan struct */
                wscan = iw_process_scanning_token(&iwe, wscan, networks, w);
                /* Check problems */
                if(wscan == NULL)
                {
                    free(buffer);
                    errno = ENOMEM;
                    return(-1);
                }
            }
        }
        while(ret > 0);
    }

    /* Done with this interface - return success */
    free(buffer);
    return(0);
}


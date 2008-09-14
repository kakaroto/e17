/** @file exalt_wireless_network.c */
#include "exalt_wireless_network.h"
#include "libexalt_private.h"

/**
 * @addtogroup Exalt_Wireless_Network
 * @{
 */

struct Exalt_Wireless_Network
{
    Exalt_Wireless* w;

    char* address;
    char* essid;
    int encryption;
    int quality;
    int signal_lvl;
    int noise_lvl;
    char* mode;

    //no more use
    char* protocol;
    char* channel;
    char* bit_rates;
    //


    short scan_ok; //0 if the network is not in the scan result
    short known;   //1 if the network is known, in the config file.

    Exalt_Connection* default_conn;
};


/*
 * Set/get functions for Exalt_Wireless_Network
 */


/**
 * @brief Create a new exalt_Wireless_Network structure
 * @param w the interfacewhich be associated to the new network
 * @return Returns the new network
 */
Exalt_Wireless_Network* exalt_wirelessnetwork_create(Exalt_Wireless* w)
{
    Exalt_Wireless_Network* wi = (Exalt_Wireless_Network*)malloc((unsigned int)sizeof(Exalt_Wireless_Network));

    EXALT_ASSERT_RETURN(wi!=NULL);

    wi->w = w;
    wi->address = NULL;
    wi->essid= NULL;
    wi->encryption = 0;

    wi->protocol = NULL;
    wi->mode = NULL;
    wi->channel = NULL;
    wi->bit_rates= NULL;
    wi->quality = 0;
    wi->signal_lvl = 0;
    wi->noise_lvl = 0;
    wi->scan_ok = 0;
    wi->known = 0;

    return wi;
}



/**
 * @brief free Exalt_Wireless_Network
 * @param data the Exalt_Wireless_Network
 */
void exalt_wirelessnetwork_free(void* data)
{
    Exalt_Wireless_Network* wi = Exalt_Wireless_Network(data);
    EXALT_FREE(wi->address);
    EXALT_FREE(wi->essid);
    EXALT_FREE(wi->protocol);
    EXALT_FREE(wi->mode);
    EXALT_FREE(wi->channel);
    EXALT_FREE(wi->bit_rates);
}



/**
 * @brief set the mac address
 * @param w the Exalt_Wireless_Network
 * @param address the new mac address
 */
void exalt_wirelessnetwork_set_address(Exalt_Wireless_Network* w, const char* address)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(address!=NULL);
    EXALT_FREE(w->address);
    w->address = strdup(address);
}




/**
 * @brief set the essid
 * @param w the Exalt_Wireless_Network
 * @param essid the new essid
 */
void exalt_wirelessnetwork_set_essid(Exalt_Wireless_Network* w, const char* essid)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(essid!=NULL);
    EXALT_FREE(w->essid);
    w->essid = strdup(essid);
}



/**
 * @brief set the mode
 * @param w the Exalt_Wireless_Network
 * @param mode the new mode
 */
void exalt_wirelessnetwork_set_mode(Exalt_Wireless_Network* w, const char* mode)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(mode!=NULL);
    EXALT_FREE(w->mode);
    w->mode = strdup(mode);
}



/**
 * @brief set the protocol
 * @param w the Exalt_Wireless_Network
 * @param protocol the new protocol
 */
void exalt_wirelessnetwork_set_protocol(Exalt_Wireless_Network* w, const char* protocol)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(protocol!=NULL);
    EXALT_FREE(w->protocol);
    w->protocol = strdup(protocol);
}



/**
 * @brief set the channel
 * @param w the Exalt_Wireless_Network
 * @param channel the new channel
 */
void exalt_wirelessnetwork_set_channel(Exalt_Wireless_Network* w, const char* channel)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(channel!=NULL);
    EXALT_FREE(w->channel);
    w->channel = strdup(channel);
}



/**
 * @brief set if ecryption state
 * @param w the Exalt_Wireless_Network
 * @param encryption the new encryption
 */
void exalt_wirelessnetwork_set_encryption(Exalt_Wireless_Network* w, int  encryption)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->encryption = encryption;
}



/**
 * @brief set the bit rates
 * @param w the Exalt_Wireless_Network
 * @param bit_rates the new bit rates
 */
void exalt_wirelessnetwork_set_bitrates(Exalt_Wireless_Network* w, const char* bit_rates)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    EXALT_ASSERT_RETURN_VOID(bit_rates!=NULL);
    EXALT_FREE(w->bit_rates);
    w->bit_rates = strdup(bit_rates);
}



/**
 * @brief set the quality
 * @param w the Exalt_Wireless_Network
 * @param quality the new quality
 */
void exalt_wirelessnetwork_set_quality(Exalt_Wireless_Network* w, int quality)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->quality = quality;
}



/**
 * @brief set the signal level
 * @param w the Exalt_Wireless_Network
 * @param signal_lvl the new signal level
 */
void exalt_wirelessnetwork_set_signalvl(Exalt_Wireless_Network* w, int signal_lvl)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->signal_lvl = signal_lvl;
}



/**
 * @brief set the noise level
 * @param w the Exalt_Wireless_Network
 * @param noise_lvl the new noise level
 */
void exalt_wirelessnetwork_set_noiselvl(Exalt_Wireless_Network* w, int noise_lvl)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->noise_lvl = noise_lvl;
}



/**
 * @brief set if the network is in the scan result (1 or 0)
 * @param w the Exalt_Wireless_Network
 * @param ok 1 if the network is in the scan result, else 0
 */

void exalt_wirelessnetwork_set_scanok(Exalt_Wireless_Network *w,short ok)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->scan_ok = ok;
}



/**
 * @brief set 1 if the network is know, if he is a in the configuration file
 * @param w the Exalt_Wireless_Network
 * @param known 1 if the network is know, else 0
 */

void exalt_wirelessnetwork_set_known(Exalt_Wireless_Network* w,short known)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    w->known = known;
}



/**
 * @brief get the mac address
 * @param w the Exalt_Wireless_Network
 * @return Return the mac address
 */
const char* exalt_wirelessnetwork_get_addr(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->address;
}

/**
 * @brief get the wireless struct
 * @param w the Exalt_Wireless_Network
 * @return Return the wireless struct of the card which provide the scan
 */
Exalt_Wireless* exalt_wirelessnetwork_get_wireless(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->w;
}

/**
 * @brief get the essid
 * @param w the Exalt_Wireless_Network
 * @return Return the essid
 */
const char* exalt_wirelessnetwork_get_essid(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->essid;
}



/**
 * @brief get the protocol
 * @param w the Exalt_Wireless_Network
 * @return Return the protocol
 */
const char* exalt_wirelessnetwork_get_protocol(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->protocol;
}



/**
 * @brief get the mode
 * @param w the Exalt_Wireless_Network
 * @return Return the mode
 */
const char* exalt_wirelessnetwork_get_mode(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->mode;
}



/**
 * @brief get the channel
 * @param w the Exalt_Wireless_Network
 * @return Returns the channel
 */
const char* exalt_wirelessnetwork_get_channel(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->channel;
}



/**
 * @brief get if the network is encrypted or not
 * @param w the Exalt_Wireless_Network
 * @return Returns the encryption state (1 or 0)
 */
int exalt_wirelessnetwork_get_encryption(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->encryption;
}



/**
 * @brief get the bit rates
 * @param w the Exalt_Wireless_Network
 * @return Returns the bit rates
 */
const char* exalt_wirelessnetwork_get_bitrates(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->bit_rates;
}



/**
 * @brief get the quality
 * @param w the Exalt_Wireless_Network
 * @return Returns the quality
 */
int exalt_wirelessnetwork_get_quality(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->quality;
}



/**
 * @brief get the signal level
 * @param w the Exalt_Wireless_Network
 * @return Returns the signal level
 */
int exalt_wirelessnetwork_get_signallvl(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->signal_lvl;
}



/**
 * @brief get the noise level
 * @param w the Exalt_Wireless_Network
 * @return Returns the noise level
 */
int exalt_wirelessnetwork_get_noiselvl(Exalt_Wireless_Network* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->noise_lvl;
}



/**
 * @brief check if a wireless network is known
 * @param wi the network
 * @return Returns 1 if the network is known, else 0
 */
int exalt_wirelessnetwork_is_known(Exalt_Wireless_Network* wi)
{
    EXALT_ASSERT_RETURN(wi!=NULL);

    return wi->known;
}


/**
 * @brief check if a wireless network is known
 * @param wi the network
 * @return Returns 1 if the network is the result of a scan, else 0
 */
int exalt_wirelessnetwork_is_scan(Exalt_Wireless_Network* wi)
{
    EXALT_ASSERT_RETURN(wi!=NULL);

    return wi->scan_ok;
}
/** @} */


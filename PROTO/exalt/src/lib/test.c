#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commande.h"
#include "regex.h"

#include "ethernet.h"




int main(int argc, char** argv)
{
	eth_init();
	eth_load();
	eth_printf();
	eth_free();
	dns_printf();	
	return 1;

}







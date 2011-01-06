#ifndef ELSA_H_
#define ELSA_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>

#include "elsa_session.h"
#include "elsa_pam.h"
#include "elsa_config.h"
#include "elsa_xserver.h"
#include "elsa_server.h"

void elsa_close_log();
int elsa_main();

#endif /* ELSA_H_ */

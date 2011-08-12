#ifndef ELSA_H_
#define ELSA_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_File.h>

#include "elsa_session.h"
#include "elsa_pam.h"
#include "elsa_config.h"
#include "elsa_xserver.h"
#include "elsa_server.h"
#include "elsa_history.h"
#include "elsa_action.h"
#include "../event/elsa_event.h"

void elsa_close_log();

#endif /* ELSA_H_ */

#include "eclipse.h"
#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <limits.h>
#ifdef HAVE_CURL_CURL_H
#include <curl/curl.h>
#endif
#include "images.h"
#include "keys.h"
#include "menu.h"

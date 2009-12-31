/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _EUPNP_LOG_H
#define _EUPNP_LOG_H

#include <Eina.h>

EAPI extern int EUPNP_LOGGING_DOM_GLOBAL;

#define WARN(...) EINA_LOG_DOM_WARN(EUPNP_LOGGING_DOM_GLOBAL, __VA_ARGS__)
#define DEBUG(...) EINA_LOG_DOM_DBG(EUPNP_LOGGING_DOM_GLOBAL, __VA_ARGS__)
#define INFO(...) EINA_LOG_DOM_INFO(EUPNP_LOGGING_DOM_GLOBAL, __VA_ARGS__)
#define ERROR(...) EINA_LOG_DOM_ERR(EUPNP_LOGGING_DOM_GLOBAL, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(EUPNP_LOGGING_DOM_GLOBAL, __VA_ARGS__)


#define WARN_D(DOM, ...) EINA_LOG_DOM_WARN(DOM, __VA_ARGS__)
#define DEBUG_D(DOM, ...) EINA_LOG_DOM_DBG(DOM, __VA_ARGS__)
#define INFO_D(DOM, ...) EINA_LOG_DOM_INFO(DOM, __VA_ARGS__)
#define ERROR_D(DOM, ...) EINA_LOG_DOM_ERR(DOM, __VA_ARGS__)
#define CRIT_D(DOM, ...) EINA_LOG_DOM_CRIT(DOM, __VA_ARGS__)

#endif /* _EUPNP_LOG_H */

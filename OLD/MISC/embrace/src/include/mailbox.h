/*
 * $Id$
 *
 * Copyright (C) 2004 Embrace project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __MAILBOX_H
#define __MAILBOX_H

#include <Evas.h>
#include <Edb.h>

#ifndef __MAILBOX_C
typedef void MailBox;
#endif

MailBox *mailbox_new (Evas *evas, const char *theme);
void mailbox_free (MailBox *mb);

bool mailbox_load_config (MailBox *mb, E_DB_File *edb, char *key);
void mailbox_emit_add (MailBox *mb);
bool mailbox_check (MailBox *mb);

const char *mailbox_title_get (MailBox *mb);
void mailbox_title_set (MailBox *mb, const char *title);

int mailbox_unseen_get (MailBox *mb);
void mailbox_unseen_set (MailBox *mb, int unseen);

int mailbox_total_get (MailBox *mb);
void mailbox_total_set (MailBox *mb, int total);

void *mailbox_plugin_get (MailBox *mb);
void mailbox_plugin_set (MailBox *mb, void *data);

void *mailbox_property_get (MailBox *mb, char *key);
void mailbox_property_set (MailBox *mb, char *key, void *data);

int mailbox_poll_interval_get (MailBox *mb);
void mailbox_poll_interval_set (MailBox *mb, int interval);

void mailbox_is_checking_set (MailBox *mb, bool checking);

Evas_Object *mailbox_edje_get (MailBox *mb);
int mailbox_width_get (MailBox *mb);
int mailbox_height_get (MailBox *mb);

#endif

/* vim:tabstop=4
 * Copyright © 2009 Rui Miguel Silva Seabra <rms@1407.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef TWITTER_H

#define TWITTER_H

int ed_twitter_post(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg);
void ed_twitter_timeline_get(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline);

void ed_twitter_init_friends(void);

void ed_twitter_friends_startDocument(void *user_data);
void ed_twitter_friends_endDocument(void *user_data);

void ed_twitter_friends_startElement(void *user_data, const xmlChar * name, const xmlChar ** attrs);
void ed_twitter_friends_endElement(void *user_data, const xmlChar * name);
void ed_twitter_friends_characters(void *user_data, const xmlChar * ch, int len);


#endif

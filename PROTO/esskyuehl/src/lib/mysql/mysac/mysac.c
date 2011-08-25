/*
 * Copyright (c) 2009 Thierry FOURNIER
 *
 * This file is part of MySAC.
 *
 * MySAC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License
 *
 * MySAC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MySAC.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef HAVE_MYSQL_H
#include <mysql.h>
#include <my_global.h>
#else
#include <mysql/mysql.h>
#include <mysql/my_global.h>
#endif
#include "mysac_decode_field.h"
#include "mysac_encode_values.h"
#include "mysac_decode_row.h"
#include "mysac.h"
#include "mysac_net.h"
#include "mysac_utils.h"

enum my_response_t {
	MYSAC_RET_EOF = 1000,
	MYSAC_RET_OK,
	MYSAC_RET_ERROR,
	MYSAC_RET_DATA
};

const char *mysac_type[] = {
	[MYSQL_TYPE_DECIMAL]     = "MYSQL_TYPE_DECIMAL",
	[MYSQL_TYPE_TINY]        = "MYSQL_TYPE_TINY",
	[MYSQL_TYPE_SHORT]       = "MYSQL_TYPE_SHORT",
	[MYSQL_TYPE_LONG]        = "MYSQL_TYPE_LONG",
	[MYSQL_TYPE_FLOAT]       = "MYSQL_TYPE_FLOAT",
	[MYSQL_TYPE_DOUBLE]      = "MYSQL_TYPE_DOUBLE",
	[MYSQL_TYPE_NULL]        = "MYSQL_TYPE_NULL",
	[MYSQL_TYPE_TIMESTAMP]   = "MYSQL_TYPE_TIMESTAMP",
	[MYSQL_TYPE_LONGLONG]    = "MYSQL_TYPE_LONGLONG",
	[MYSQL_TYPE_INT24]       = "MYSQL_TYPE_INT24",
	[MYSQL_TYPE_DATE]        = "MYSQL_TYPE_DATE",
	[MYSQL_TYPE_TIME]        = "MYSQL_TYPE_TIME",
	[MYSQL_TYPE_DATETIME]    = "MYSQL_TYPE_DATETIME",
	[MYSQL_TYPE_YEAR]        = "MYSQL_TYPE_YEAR",
	[MYSQL_TYPE_NEWDATE]     = "MYSQL_TYPE_NEWDATE",
	[MYSQL_TYPE_VARCHAR]     = "MYSQL_TYPE_VARCHAR",
	[MYSQL_TYPE_BIT]         = "MYSQL_TYPE_BIT",
	[MYSQL_TYPE_NEWDECIMAL]  = "MYSQL_TYPE_NEWDECIMAL",
	[MYSQL_TYPE_ENUM]        = "MYSQL_TYPE_ENUM",
	[MYSQL_TYPE_SET]         = "MYSQL_TYPE_SET",
	[MYSQL_TYPE_TINY_BLOB]   = "MYSQL_TYPE_TINY_BLOB",
	[MYSQL_TYPE_MEDIUM_BLOB] = "MYSQL_TYPE_MEDIUM_BLOB",
	[MYSQL_TYPE_LONG_BLOB]   = "MYSQL_TYPE_LONG_BLOB",
	[MYSQL_TYPE_BLOB]        = "MYSQL_TYPE_BLOB",
	[MYSQL_TYPE_VAR_STRING]  = "MYSQL_TYPE_VAR_STRING",
	[MYSQL_TYPE_STRING]      = "MYSQL_TYPE_STRING",
	[MYSQL_TYPE_GEOMETRY]    = "MYSQL_TYPE_GEOMETRY"
};

enum read_state {
	RDST_INIT = 0,
	RDST_READ_LEN,
	RDST_READ_DATA,
	RDST_DECODE_DATA
};

static inline
int mysac_extend_res(MYSAC *m)
{
	MYSAC_RES *res = m->res;
	MYSAC_ROWS *row;
	struct mysac_list_head *stop;
	struct mysac_list_head *run;
	struct mysac_list_head *next;
	struct mysac_list_head *prev;
	long long int offset;
	int i;

	if (res->extend_bloc_size == 0) {
		m->errorcode = MYERR_BUFFER_OVERSIZE;
		return MYSAC_RET_ERROR;
	}

	res = realloc(m->res, res->max_len + res->extend_bloc_size);
	if (res == NULL) {
		m->errorcode = MYERR_SYSTEM;
		return MYSAC_RET_ERROR;
	}

	/* clear new memory */
	memset((char *)res + res->max_len, 0, res->extend_bloc_size);

	mysac_print_audit(m, "mysac realloc memory: from_ptr=%p, to_ptr=%p, from=%d to=%d",
	                  m->res, res, res->max_len, res->max_len + res->extend_bloc_size);

	/* update lengths */
	res->buffer_len += res->extend_bloc_size;
	res->max_len += res->extend_bloc_size;
	m->read_len += res->extend_bloc_size;

	/* if the pointer of the dat bloc changed, update links */
	if (res != m->res) {

		/* compute offset between old and new memory bloc */
		offset = (long long int)(*(unsigned long int *)&res) -
		         (long long int)(*(unsigned long int *)&m->res);

		/* update pointers */
		res->buffer = (char *)((char *)res->buffer + offset);
		res->cr = (MYSAC_ROWS *)((char *)res->cr + offset);
		m->read = (char *)((char *)m->read + offset);

		/* update first cell */
		next = (struct mysac_list_head *)((char *)res->data.next + offset);
		prev = (struct mysac_list_head *)((char *)res->data.prev + offset);
		res->data.next = next;
		res->data.prev = prev;
		stop = &res->data;

		/* update row names */
		res->cols = (MYSQL_FIELD *)((char *)res->cols + offset);
		for (i=0; i<res->nb_cols; i++) {
			if (res->cols[i].name != NULL)
				res->cols[i].name = res->cols[i].name + offset;
			if (res->cols[i].org_name != NULL)
				res->cols[i].org_name = res->cols[i].org_name + offset;
			if (res->cols[i].table != NULL)
				res->cols[i].table = res->cols[i].table + offset;
			if (res->cols[i].org_table != NULL)
				res->cols[i].org_table = res->cols[i].org_table + offset;
			if (res->cols[i].db != NULL)
				res->cols[i].db = res->cols[i].db + offset;
			if (res->cols[i].catalog != NULL)
				res->cols[i].catalog = res->cols[i].catalog + offset;
			if (res->cols[i].def != NULL)
				res->cols[i].def = res->cols[i].def + offset;
		}

		/* parcours la liste */
		run = res->data.next;
		while (1) {
			if (run == stop)
				break;
			next = (struct mysac_list_head *)((char *)run->next + offset);
			prev = (struct mysac_list_head *)((char *)run->prev + offset);
			row = mysac_container_of(run, MYSAC_ROWS, link);

			/* upadate data pointer */
			row->lengths = (unsigned long *)((char *)row->lengths + offset);
			row->data = (MYSAC_ROW *)((char *)row->data + offset);

			/* struct tm */
			for (i=0; i<res->nb_cols; i++) {
				switch(res->cols[i].type) {

				/* apply offset on data pointer */
				case MYSQL_TYPE_TIME:
				case MYSQL_TYPE_YEAR:
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_DATETIME:
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_VARCHAR:
				case MYSQL_TYPE_VAR_STRING:
				case MYSQL_TYPE_TINY_BLOB:
				case MYSQL_TYPE_MEDIUM_BLOB:
				case MYSQL_TYPE_LONG_BLOB:
				case MYSQL_TYPE_BLOB:
					if (row->data[i].string != NULL)
						row->data[i].string = row->data[i].string + offset;
					break;

				/* do nothing for other types */
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
				case MYSQL_TYPE_NULL:
				case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_INT24:
				case MYSQL_TYPE_NEWDATE:
				case MYSQL_TYPE_BIT:
				case MYSQL_TYPE_NEWDECIMAL:
				case MYSQL_TYPE_ENUM:
				case MYSQL_TYPE_SET:
				case MYSQL_TYPE_GEOMETRY:
				default:
					break;
				}
			}

			run->next = next;
			run->prev = prev;
			run = run->next;
		}

		/* update resource pointer */
		m->res = res;
	}

	return 0;
}

enum my_expected_response_t check_action(const char *request, int len) {

	const char *parse;

	/* jump blank char '\r', '\n', '\t' and ' ' */
	parse = request;
	while (1) {
		if (!isspace(*parse))
			break;

		/* if no more chars in string */
		len--;
		if (len <= 0)
			return MYSAC_EXPECT_OK;

		parse++;
	}

	/* check request type */
	if ( (len > 6) && ( strncasecmp(parse, "SELECT", 5) == 0) )
		return MYSAC_EXPECT_DATA;

	return MYSAC_EXPECT_OK;
}

static int my_response(MYSAC *m, enum my_expected_response_t expect) {
	int i;
	int err;
	int errcode;
	char *read;
	unsigned long len;
	unsigned long rlen;
	char nul;

	switch ((enum read_state)m->readst) {

	case RDST_INIT:
		m->len = 0;
		m->readst = RDST_READ_LEN;

	/* read length */
	case RDST_READ_LEN:

		/* check for avalaible size in buffer */
		while (m->read_len < 4)
			if (mysac_extend_res(m) != 0)
				return MYSAC_RET_ERROR;

		err = mysac_read(m->fd, m->read + m->len,
		                 4 - m->len, &errcode);
		if (err == -1) {
			m->errorcode = errcode;
			return errcode;
		}

		m->len += err;
		if (m->len < 4) {
			m->errorcode = MYERR_WANT_READ;
			return MYERR_WANT_READ;
		}

		/* decode */
		m->packet_length = uint3korr(&m->read[0]);

		/* packet number */
		m->packet_number = m->read[3];

		/* update read state */
		m->readst = RDST_READ_DATA;
		m->len = 0;

	/* read data */
	case RDST_READ_DATA:

		/* check for avalaible size in buffer */
		while ((unsigned int)m->read_len < m->packet_length)
			if (mysac_extend_res(m) != 0)
				return MYSAC_RET_ERROR;

		err = mysac_read(m->fd, m->read + m->len,
		                 m->packet_length - m->len, &errcode);
		if (err == -1)
			return errcode;

		m->len += err;
		if ((unsigned int)m->len < m->packet_length) {
			m->errorcode = MYERR_WANT_READ;
			return MYERR_WANT_READ;
		}
		m->read_len -= m->packet_length;

		/* re-init eof */
		m->readst = RDST_DECODE_DATA;
		m->eof = 1;

	/* decode data */
	case RDST_DECODE_DATA:

		/* error */
		if ((unsigned char)m->read[0] == 255) {

			/* defined mysql error */
			if (m->packet_length > 3) {

				/* read error code */
				// TODO: voir quoi foutre de ca plus tard
				// m->errorcode = uint2korr(&m->read[1]);

				/* read mysal code and message */
				for (i=3; i<3+5; i++)
					m->read[i] = m->read[i+1];
				m->read[8] = ' ';
				m->mysql_error = &m->read[3];
				m->read[m->packet_length] = '\0';
				m->errorcode = MYERR_MYSQL_ERROR;
			}

			/* unknown error */
			else
				m->errorcode = MYERR_PROTOCOL_ERROR;

			return MYSAC_RET_ERROR;
		}

		/* reponse is expectig sucess and onmly success */
		if (expect == MYSAC_EXPECT_OK || expect == MYSAC_EXPECT_BOTH) {

			/* not a sucess code */
			if ((unsigned char)m->read[0] == 0xff) {
				m->errorcode = MYERR_PROTOCOL_ERROR;
				return MYSAC_RET_ERROR;
			}

			/* is sucess */
			if ((unsigned char)m->read[0] == 0) {

				read = &m->read[1];
				rlen = m->packet_length - 1;

				/* affected rows */
				len = my_lcb(read, &m->affected_rows, &nul, rlen);
				rlen -= len;
				read += len;
				/* m->affected_rows = uint2korr(&m->read[1]); */

				/* insert id */
				len = my_lcb(read, &m->insert_id, &nul, rlen);
				rlen -= len;
				read += len;

				/* server status */
				m->status = uint2korr(read);
				read += 2;

				/* server warnings */
				m->warnings = uint2korr(read);

				return MYSAC_RET_OK;
			}
		}

		/* response is expecting data. Maybe contain an EOF */
		if (expect == MYSAC_EXPECT_DATA || expect == MYSAC_EXPECT_BOTH) {

			/* EOF marker: marque la fin d'une serie
				(la fin des headers dans une requete) */
			if ((unsigned char)m->read[0] == 254) {
				m->warnings = uint2korr(&m->read[1]);
				m->status = uint2korr(&m->read[3]);
				m->eof = 1;
				return MYSAC_RET_EOF;
			}

			else
				return MYSAC_RET_DATA;
		}

		/* the expect code is not valid */
		m->errorcode = MYERR_UNKNOWN_ERROR;
		return MYSAC_RET_ERROR;

	default:
		m->errorcode = MYERR_UNEXPECT_R_STATE;
		return MYSAC_RET_ERROR;
	}

	m->errorcode = MYERR_PACKET_CORRUPT;
	return MYSAC_RET_ERROR;
}

void mysac_init(MYSAC *mysac, char *buffer, unsigned int buffsize) {

	/* init */
	memset(mysac, 0, sizeof(MYSAC));
	mysac->qst = MYSAC_START;
	mysac->buf = buffer;
	mysac->bufsize = buffsize;
}

MYSAC *mysac_new(int buffsize) {
	MYSAC *m;
	char *buf;

	/* struct memory */
	m = calloc(1, sizeof(MYSAC));
	if (m == NULL)
		return NULL;

	/* buff memory */
	buf = calloc(1, buffsize);
	if (buf == NULL) {
		free(m);
		return NULL;
	}

	/* init */
	m->free_it = 1;
	m->qst = MYSAC_START;
	m->buf = buf;
	m->bufsize = buffsize;

	return m;
}

void mysac_setup(MYSAC *mysac, const char *my_addr, const char *user,
                 const char *passwd, const char *db,
                 unsigned long client_flag) {
	mysac->addr     = my_addr;
	mysac->login    = user;
	mysac->password = passwd;
	mysac->database = db;
	mysac->flags    = client_flag;
	mysac->call_it  = mysac_connect;
}

int mysac_connect(MYSAC *mysac) {
	int err;
	int errcode;
	int i;
	int len;

	switch (mysac->qst) {

	/***********************************************
	 network connexion
	***********************************************/
	case MYSAC_START:
		err = mysac_socket_connect(mysac->addr, &mysac->fd);
		if (err != 0) {
			mysac->qst = MYSAC_START;
			mysac->errorcode = err;
			return err;
		}
		mysac->qst = MYSAC_CONN_CHECK;
		return MYERR_WANT_READ;

	/***********************************************
	 check network connexion
	***********************************************/
	case MYSAC_CONN_CHECK:
		err = mysac_socket_connect_check(mysac->fd);
		if (err != 0) {
			close(mysac->fd);
			mysac->qst = MYSAC_START;
			mysac->errorcode = err;
			return err;
		}
		mysac->qst = MYSAC_READ_GREATINGS;
		mysac->len = 0;
		mysac->readst = 0;
		mysac->read = mysac->buf;
		mysac->read_len = mysac->bufsize;

	/***********************************************
	 read greatings
	***********************************************/
	case MYSAC_READ_GREATINGS:

		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		else if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* ok */
		else if (err != MYSAC_RET_DATA) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* decode greatings */
		i = 0;

		/* protocol */
		mysac->protocol = mysac->buf[i];
		i++;

		/* version */
		mysac->version = &mysac->buf[i];

		/* search \0 */
		while (mysac->buf[i] != 0)
			i++;
		i++;

		/* thread id */
		mysac->threadid = uint4korr(&mysac->buf[i]);

		/* first part of salt */
		strncpy(mysac->salt, &mysac->buf[i+4], SCRAMBLE_LENGTH_323);
		i += 4 + SCRAMBLE_LENGTH_323 + 1;

		/* options */
		mysac->options = uint2korr(&mysac->buf[i]);

		/* charset */
		mysac->charset = mysac->buf[i+2];

		/* server status */
		mysac->status = uint2korr(&mysac->buf[i+3]);

		/* salt part 2 */
		strncpy(mysac->salt + SCRAMBLE_LENGTH_323, &mysac->buf[i+5+13],
		        SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323);
		mysac->salt[SCRAMBLE_LENGTH] = '\0';

		/* checks */
		if (mysac->protocol != PROTOCOL_VERSION)
			return CR_VERSION_ERROR;

		/********************************
		  prepare auth packet
		********************************/

		/* set m->buf number */
		mysac->packet_number++;
		mysac->buf[3] = mysac->packet_number;

		/* set options */
		if (mysac->options & CLIENT_LONG_PASSWORD)
			mysac->flags |= CLIENT_LONG_PASSWORD;
		mysac->flags |= CLIENT_LONG_FLAG   |
		                CLIENT_PROTOCOL_41 |
		                CLIENT_SECURE_CONNECTION;
		to_my_2(mysac->flags, &mysac->buf[4]);

		/* set extended options */
		to_my_2(0, &mysac->buf[6]);

		/* max m->bufs */
		to_my_4(0x40000000, &mysac->buf[8]);

		/* charset */
		/* 8: swedish */
		mysac->buf[12] = 8;

		/* 24 unused */
		memset(&mysac->buf[13], 0, 24);

		/* username */
		strcpy(&mysac->buf[36], mysac->login);
		i = 36 + strlen(mysac->login) + 1;

		/* password CLIENT_SECURE_CONNECTION */
		if (mysac->options & CLIENT_SECURE_CONNECTION) {

			/* the password hash len */
			mysac->buf[i] = SCRAMBLE_LENGTH;
			i++;
			scramble(&mysac->buf[i], mysac->salt, mysac->password);
			i += SCRAMBLE_LENGTH;
		}

		/* password ! CLIENT_SECURE_CONNECTION */
		else {
			scramble_323(&mysac->buf[i], mysac->salt, mysac->password);
			i += SCRAMBLE_LENGTH_323 + 1;
		}

		/* Add database if needed */
		if ((mysac->options & CLIENT_CONNECT_WITH_DB) &&
		    (mysac->database != NULL)) {
			/* TODO : debordement de buffer */
			len = strlen(mysac->database);
			memcpy(&mysac->buf[i], mysac->database, len);
			i += len;
			mysac->buf[i] = '\0';
		}

		/* len */
		to_my_3(i-4, &mysac->buf[0]);
		mysac->len = i;
		mysac->send = mysac->buf;
		mysac->qst = MYSAC_SEND_AUTH_1;

	/***********************************************
	 send paquet
	***********************************************/
	case MYSAC_SEND_AUTH_1:
		err = mysac_write(mysac->fd, mysac->send, mysac->len, &errcode);

		if (err == -1)
			return errcode;

		mysac->len -= err;
		mysac->send += err;
		if (mysac->len > 0)
			return MYERR_WANT_WRITE;

		mysac->qst = MYSAC_RECV_AUTH_1;
		mysac->readst = 0;
		mysac->read = mysac->buf;
		mysac->read_len = mysac->bufsize;

	/***********************************************
	 read response 1
	***********************************************/
	case_MYSAC_RECV_AUTH_1:
	case MYSAC_RECV_AUTH_1:
	/*
		MYSAC_RET_EOF,
		MYSAC_RET_OK,
		MYSAC_RET_ERROR,
		MYSAC_RET_DATA
	*/
		err = my_response(mysac, MYSAC_EXPECT_BOTH);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* ok */
		else if (err == MYSAC_RET_OK)
			return 0;

		/*
		   By sending this very specific reply server asks us to send scrambled
		   password in old format.
		*/
		else if (mysac->packet_length == 1 && err == MYSAC_RET_EOF &&
		         mysac->options & CLIENT_SECURE_CONNECTION) {
			/* continue special paquet after conditions */
		}

		/* protocol error */
		else {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* send scrambled password in old format */

		/* set packet number */
		mysac->packet_number++;
		mysac->buf[3] = mysac->packet_number;

		/* send scrambled password in old format. */
		scramble_323(&mysac->buf[4], mysac->salt, mysac->password);
		mysac->buf[4+SCRAMBLE_LENGTH_323] = '\0';

		/* len */
		to_my_3(SCRAMBLE_LENGTH_323+1, &mysac->buf[0]);
		mysac->qst = MYSAC_SEND_AUTH_2;
		mysac->len = SCRAMBLE_LENGTH_323 + 1 + 4;
		mysac->send = mysac->buf;

	/* send scrambled password in old format */
	case MYSAC_SEND_AUTH_2:
		err = mysac_write(mysac->fd, mysac->send, mysac->len, &errcode);

		if (err == -1)
			return errcode;

		mysac->len -= err;
		mysac->send += err;
		if (mysac->len > 0)
			return MYERR_WANT_WRITE;

		mysac->qst = MYSAC_RECV_AUTH_1;
		mysac->readst = 0;
		mysac->read = mysac->buf;
		mysac->read_len = mysac->bufsize;
		goto case_MYSAC_RECV_AUTH_1;

	case MYSAC_SEND_QUERY:
	case MYSAC_RECV_QUERY_COLNUM:
	case MYSAC_RECV_QUERY_COLDESC1:
	case MYSAC_RECV_QUERY_COLDESC2:
	case MYSAC_RECV_QUERY_EOF1:
	case MYSAC_RECV_QUERY_EOF2:
	case MYSAC_RECV_QUERY_DATA:
	case MYSAC_SEND_INIT_DB:
	case MYSAC_RECV_INIT_DB:
	case MYSAC_SEND_STMT_QUERY:
	case MYSAC_RECV_STMT_QUERY:
	case MYSAC_SEND_STMT_EXECUTE:
	case MYSAC_RECV_STMT_EXECUTE:
	case MYSAC_READ_NUM:
	case MYSAC_READ_HEADER:
	case MYSAC_READ_LINE:
		mysac->errorcode = MYERR_BAD_STATE;
		return MYERR_BAD_STATE;

	}

	return 0;
}

int mysac_set_database(MYSAC *mysac, const char *database) {
	int i;

	/* set packet number */
	mysac->buf[3] = 0;

	/* set mysql command */
	mysac->buf[4] = COM_INIT_DB;

	/* build sql query */
	i = strlen(database);
	memcpy(&mysac->buf[5], database, i);

	/* len */
	to_my_3(i + 1, &mysac->buf[0]);

	/* send params */
	mysac->send = mysac->buf;
	mysac->len = i + 5;
	mysac->qst = MYSAC_SEND_INIT_DB;
	mysac->call_it = mysac_send_database;

	return 0;
}

int mysac_send_database(MYSAC *mysac) {
	int err;
	int errcode;

	switch (mysac->qst) {

	/**********************************************************
	*
	* send query on network
	*
	**********************************************************/
	case MYSAC_SEND_INIT_DB:
		err = mysac_write(mysac->fd, mysac->send, mysac->len, &errcode);

		if (err == -1)
			return errcode;

		mysac->len -= err;
		mysac->send += err;
		if (mysac->len > 0)
			return MYERR_WANT_WRITE;
		mysac->qst = MYSAC_RECV_INIT_DB;
		mysac->readst = 0;
		mysac->read = mysac->buf;

	/**********************************************************
	*
	* receive
	*
	**********************************************************/
	case MYSAC_RECV_INIT_DB:
		err = my_response(mysac, MYSAC_EXPECT_OK);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err == MYSAC_RET_OK)
			return 0;

		else {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

	case MYSAC_START:
	case MYSAC_CONN_CHECK:
	case MYSAC_READ_GREATINGS:
	case MYSAC_SEND_AUTH_1:
	case MYSAC_RECV_AUTH_1:
	case MYSAC_SEND_AUTH_2:
	case MYSAC_SEND_QUERY:
	case MYSAC_RECV_QUERY_COLNUM:
	case MYSAC_RECV_QUERY_COLDESC1:
	case MYSAC_RECV_QUERY_COLDESC2:
	case MYSAC_RECV_QUERY_EOF1:
	case MYSAC_RECV_QUERY_EOF2:
	case MYSAC_RECV_QUERY_DATA:
	case MYSAC_SEND_STMT_QUERY:
	case MYSAC_RECV_STMT_QUERY:
	case MYSAC_SEND_STMT_EXECUTE:
	case MYSAC_RECV_STMT_EXECUTE:
	case MYSAC_READ_NUM:
	case MYSAC_READ_HEADER:
	case MYSAC_READ_LINE:
		mysac->errorcode = MYERR_BAD_STATE;
		return MYERR_BAD_STATE;

	}

	mysac->errorcode = MYERR_BAD_STATE;
	return MYERR_BAD_STATE;
}

int mysac_b_set_stmt_prepare(MYSAC *mysac, unsigned int *stmt_id,
                             const char *request, int len) {

	/* set packet number */
	mysac->buf[3] = 0;

	/* set mysql command */
	mysac->buf[4] = COM_STMT_PREPARE;

	/* check len */
	if (mysac->bufsize - 5 < (unsigned int)len)
		return -1;

	/* build sql query */
	memcpy(&mysac->buf[5], request, len);

	/* request type */
	mysac->expect = check_action(&mysac->buf[5], len);

	/* l */
	to_my_3(len + 1, &mysac->buf[0]);

	/* send params */
	mysac->send = mysac->buf;
	mysac->len = len + 5;
	mysac->qst = MYSAC_SEND_STMT_QUERY;
	mysac->call_it = mysac_send_stmt_prepare;
	mysac->stmt_id = stmt_id;

	return 0;
}

int mysac_s_set_stmt_prepare(MYSAC *mysac, unsigned int *stmt_id,
                             const char *request) {
	return mysac_b_set_stmt_prepare(mysac, stmt_id, request, strlen(request));
}

int mysac_v_set_stmt_prepare(MYSAC *mysac, unsigned int *stmt_id,
                             const char *fmt, va_list ap) {
	int len;

	/* set packet number */
	mysac->buf[3] = 0;

	/* set mysql command */
	mysac->buf[4] = COM_STMT_PREPARE;

	/* build sql query */
	len = vsnprintf(&mysac->buf[5], mysac->bufsize - 5, fmt, ap);
	if ((unsigned int)len >= mysac->bufsize - 5)
		return -1;

	/* request type */
	mysac->expect = check_action(&mysac->buf[5], len);

	/* len */
	to_my_3(len + 1, &mysac->buf[0]);

	/* send params */
	mysac->send = mysac->buf;
	mysac->len = len + 5;
	mysac->qst = MYSAC_SEND_STMT_QUERY;
	mysac->call_it = mysac_send_stmt_prepare;
	mysac->stmt_id = stmt_id;

	return 0;
}

int mysac_set_stmt_prepare(MYSAC *mysac, unsigned int *stmt_id,
                           const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	return mysac_v_set_stmt_prepare(mysac, stmt_id, fmt, ap);
}

int mysac_send_stmt_prepare(MYSAC *mysac) {
	int err;
	int errcode;

	switch (mysac->qst) {

	/**********************************************************
	*
	* send query on network
	*
	**********************************************************/
	case MYSAC_SEND_STMT_QUERY:
		err = mysac_write(mysac->fd, mysac->send, mysac->len, &errcode);

		if (err == -1)
			return errcode;

		mysac->len -= err;
		mysac->send += err;
		if (mysac->len > 0)
			return MYERR_WANT_WRITE;
		mysac->qst = MYSAC_RECV_STMT_QUERY;
		mysac->readst = 0;
		mysac->read = mysac->buf;

	/**********************************************************
	*
	* receive
	*
	**********************************************************/
	case MYSAC_RECV_STMT_QUERY:
		err = my_response(mysac, MYSAC_EXPECT_OK);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		if (err != MYSAC_RET_OK) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* 0: don't care */

		/* 1-4: get statement id */
		*mysac->stmt_id = uint4korr(&mysac->buf[1]);

		/* if data expected, set MSB */
		if (mysac->expect == MYSAC_EXPECT_DATA)
			(*mysac->stmt_id) |= 0x80000000;

		/* 5-6: get nb of columns */
		mysac->nb_cols = uint2korr(&mysac->buf[5]);

		/* 7-8: Number of placeholders in the statement */
		mysac->nb_plhold = uint2korr(&mysac->buf[7]);

		/* 9-.. don't care ! */

		/* if no data expected */
		if (mysac->expect == MYSAC_EXPECT_OK)
			return 0;

		if (mysac->nb_plhold > 0)
			mysac->qst = MYSAC_RECV_QUERY_COLDESC1;
		else {
			mysac->qst = MYSAC_RECV_QUERY_COLDESC2;
			goto case_MYSAC_RECV_QUERY_COLDESC2;
		}

	/**********************************************************
	*
	* receive place holder description
	*
	**********************************************************/
	case_MYSAC_RECV_QUERY_COLDESC1:
	mysac->readst = 0;
	mysac->read = mysac->buf;

	case MYSAC_RECV_QUERY_COLDESC1:

		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_DATA) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* XXX for a moment, dont decode columns
		 * names and types
		 */
		mysac->nb_plhold--;
		if (mysac->nb_plhold != 0)
			goto case_MYSAC_RECV_QUERY_COLDESC1;

		mysac->readst = 0;
		mysac->qst = MYSAC_RECV_QUERY_EOF1;
		mysac->read = mysac->buf;

	/**********************************************************
	*
	* receive EOF
	*
	**********************************************************/
	case MYSAC_RECV_QUERY_EOF1:
		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_EOF) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		mysac->qst = MYSAC_RECV_QUERY_COLDESC2;

	/**********************************************************
	*
	* receive column description
	*
	**********************************************************/
	case_MYSAC_RECV_QUERY_COLDESC2:
	mysac->readst = 0;
	mysac->read = mysac->buf;

	case MYSAC_RECV_QUERY_COLDESC2:

		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_DATA) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* XXX for a moment, dont decode columns
		 * names and types
		 */
		mysac->nb_cols--;
		if (mysac->nb_cols != 0)
			goto case_MYSAC_RECV_QUERY_COLDESC2;

		mysac->readst = 0;
		mysac->qst = MYSAC_RECV_QUERY_EOF2;
		mysac->read = mysac->buf;

	/**********************************************************
	*
	* receive EOF
	*
	**********************************************************/
	case MYSAC_RECV_QUERY_EOF2:
		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_EOF) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		return 0;

	case MYSAC_START:
	case MYSAC_CONN_CHECK:
	case MYSAC_READ_GREATINGS:
	case MYSAC_SEND_AUTH_1:
	case MYSAC_RECV_AUTH_1:
	case MYSAC_SEND_AUTH_2:
	case MYSAC_SEND_QUERY:
	case MYSAC_RECV_QUERY_COLNUM:
	case MYSAC_RECV_QUERY_DATA:
	case MYSAC_SEND_INIT_DB:
	case MYSAC_RECV_INIT_DB:
	case MYSAC_SEND_STMT_EXECUTE:
	case MYSAC_RECV_STMT_EXECUTE:
	case MYSAC_READ_NUM:
	case MYSAC_READ_HEADER:
	case MYSAC_READ_LINE:
		mysac->errorcode = MYERR_BAD_STATE;
		return MYERR_BAD_STATE;
	}

	mysac->errorcode = MYERR_BAD_STATE;
	return MYERR_BAD_STATE;
}



int mysac_set_stmt_execute(MYSAC *mysac, MYSAC_RES *res, unsigned int stmt_id,
                           MYSAC_BIND *values, int nb) {
	int i;
	int nb_bf;
	int desc_off;
	unsigned int vals_off;
	unsigned int len = 3 + 1 + 1 + 4 + 1 + 4;
	int ret;

	/* check len */
	if (mysac->bufsize < len) {
		mysac->errorcode = MYERR_BUFFER_TOO_SMALL;
		mysac->len = 0;
		return -1;
	}

	/* 3 : set packet number */
	mysac->buf[3] = 0;

	/* 4 : set mysql command */
	mysac->buf[4] = COM_STMT_EXECUTE;

	/* dat aexpected */
	if ((stmt_id & 0x80000000) == 0)
		mysac->expect = MYSAC_EXPECT_OK;
	else {
		stmt_id &= 0x7fffffff;
		mysac->expect = MYSAC_EXPECT_DATA;
	}
	mysac->stmt_id = (void *)1;

	/* 5-8 : build sql query */
	to_my_4(stmt_id, &mysac->buf[5]);

	/* 9 : flags (unused) */
	mysac->buf[9] = 0;

	/* 10-13 : iterations (unused) */
	to_my_4(1, &mysac->buf[10]);

	/* number of bytes for the NULL values bitfield */
	nb_bf = ( nb / 8 ) + 1;
	desc_off = len + nb_bf + 1;
	vals_off = desc_off + ( nb * 2 );

	/* check len */
	if (mysac->bufsize < vals_off) {
		mysac->errorcode = MYERR_BUFFER_TOO_SMALL;
		mysac->len = 0;
		return -1;
	}

	/* init bitfield: set 0 */
	memset(&mysac->buf[len], 0, nb_bf);

	/* build NULL bitfield and values type */
	for (i=0; i<nb; i++) {

		/***********************
		 *
		 * NULL bitfield
		 *
		 ***********************/
		if (values[i].is_null != 0)
			mysac->buf[len + (i << 3)] |= 1 << (i & 0xf);

		/***********************
		 *
		 * Value type
		 *
		 ***********************/
		mysac->buf[desc_off + ( i * 2 )] =  values[i].type;
		mysac->buf[desc_off + ( i * 2 ) + 1] = 0x00; /* ???? */

		/***********************
		 *
		 * set values data
		 *
		 ***********************/
		ret = mysac_encode_value(&values[i], &mysac->buf[vals_off],
		                         mysac->bufsize - vals_off);
		if (ret < 0) {
			mysac->errorcode = MYERR_BUFFER_TOO_SMALL;
			mysac->len = 0;
			return -1;
		}
		vals_off += ret;
	}

	/* 01 byte ??? */
	mysac->buf[len + nb_bf] = 0x01;

	/* 0-2 : len
	 * 4 = packet_len + packet_id
	 */
	to_my_3(vals_off - 4, &mysac->buf[0]);

	/* send params */
	mysac->res = res;
	mysac->send = mysac->buf;
	mysac->len = vals_off;
	mysac->qst = MYSAC_SEND_QUERY;
	mysac->call_it = mysac_send_stmt_execute;

	return 0;
}

inline
int mysac_b_set_query(MYSAC *mysac, MYSAC_RES *res, const char *query, unsigned int len) {

	/* set packet number */
	mysac->buf[3] = 0;

	/* set mysql command */
	mysac->buf[4] = COM_QUERY;

	/* build sql query */
	if (mysac->bufsize - 5 < len) {
		mysac->errorcode = MYERR_BUFFER_TOO_SMALL;
		mysac->len = 0;
		return -1;
	}
	memcpy(&mysac->buf[5], query, len);

	/* request type */
	mysac->expect = check_action(&mysac->buf[5], len);

	/* unset statement result */
	mysac->stmt_id = (void *)0;

	/* len */
	to_my_3(len + 1, &mysac->buf[0]);

	/* send params */
	mysac->res = res;
	mysac->send = mysac->buf;
	mysac->len = len + 5;
	mysac->qst = MYSAC_SEND_QUERY;
	mysac->call_it = mysac_send_query;

	return 0;
}

int mysac_s_set_query(MYSAC *mysac, MYSAC_RES *res, const char *query) {
	return mysac_b_set_query(mysac, res, query, strlen(query));
}

inline
int mysac_v_set_query(MYSAC *mysac, MYSAC_RES *res, const char *fmt, va_list ap) {
	unsigned int len;

	/* set packet number */
	mysac->buf[3] = 0;

	/* set mysql command */
	mysac->buf[4] = COM_QUERY;

	/* build sql query */
	len = vsnprintf(&mysac->buf[5], mysac->bufsize - 5, fmt, ap);
	if (len >= mysac->bufsize - 5) {
		mysac->errorcode = MYERR_BUFFER_TOO_SMALL;
		mysac->len = 0;
		return -1;
	}

	/* request type */
	mysac->expect = check_action(&mysac->buf[5], len);

	/* unset statement result */
	mysac->stmt_id = (void *)0;

	/* len */
	to_my_3(len + 1, &mysac->buf[0]);

	/* send params */
	mysac->res = res;
	mysac->send = mysac->buf;
	mysac->len = len + 5;
	mysac->qst = MYSAC_SEND_QUERY;
	mysac->call_it = mysac_send_query;

	return 0;
}

int mysac_set_query(MYSAC *mysac, MYSAC_RES *res, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	return mysac_v_set_query(mysac, res, fmt, ap);
}

int mysac_send_query(MYSAC *mysac) {
	int err;
	int errcode;
	int i;
	int len;
	int nb_cols;

	switch (mysac->qst) {

	/**********************************************************
	*
	* send query on network
	*
	**********************************************************/
	case MYSAC_SEND_QUERY:
		err = mysac_write(mysac->fd, mysac->send, mysac->len, &errcode);

		if (err == -1)
			return errcode;

		mysac->len -= err;
		mysac->send += err;
		if (mysac->len > 0)
			return MYERR_WANT_WRITE;
		mysac->qst = MYSAC_RECV_QUERY_COLNUM;
		mysac->readst = 0;

	/**********************************************************
	*
	* receive
	*
	**********************************************************/

	/* prepare struct

	 +---------------+-----------------+
	 | MYSQL_FIELD[] | char[]          |
	 | resp->nb_cols | all fields name |
	 +---------------+-----------------+

	 */

	mysac->res->nb_lines = 0;
	mysac->res->cols = (MYSQL_FIELD *)(mysac->res->buffer + sizeof(MYSAC_RES));
	INIT_LIST_HEAD(&mysac->res->data);
	mysac->read = mysac->res->buffer;
	mysac->read_len = mysac->res->buffer_len;

	case MYSAC_RECV_QUERY_COLNUM:
		err = my_response(mysac, mysac->expect);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* ok ( for insert or no return data command ) */
		if (mysac->expect == MYSAC_EXPECT_OK) {
			if (err == MYSAC_RET_OK)
				return 0;
			else {
				mysac->errorcode = MYERR_PROTOCOL_ERROR;
				return mysac->errorcode;
			}
		}

		/* invalide expect */
		else if (mysac->expect != MYSAC_EXPECT_DATA) {
			mysac->errorcode = MYERR_INVALID_EXPECT;
			return mysac->errorcode;
		}

		/* protocol error */
		if (err != MYSAC_RET_DATA) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* get nb col TODO: pas sur que ce soit un byte */
		nb_cols = mysac->read[0];
		mysac->read_id = 0;
		mysac->qst = MYSAC_RECV_QUERY_COLDESC1;

		/* prepare cols space */

		/* check for avalaible size in buffer */
		while ((unsigned int)mysac->read_len < sizeof(MYSQL_FIELD) * nb_cols)
			if (mysac_extend_res(mysac) != 0)
				return mysac->errorcode;

		mysac->res->nb_cols = nb_cols;
		mysac->res->cols = (MYSQL_FIELD *)mysac->read;
		memset((char *)mysac->res->cols, 0, sizeof(MYSQL_FIELD) * mysac->res->nb_cols);
		mysac->read += sizeof(MYSQL_FIELD) * mysac->res->nb_cols;
		mysac->read_len -= sizeof(MYSQL_FIELD) * mysac->res->nb_cols;

	/**********************************************************
	*
	* receive column description
	*
	**********************************************************/

	case_MYSAC_RECV_QUERY_COLDESC1:
	mysac->readst = 0;

	case MYSAC_RECV_QUERY_COLDESC1:

		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_DATA) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* decode mysql packet with field desc, use packet buffer for storing
		   mysql data (field name) */
#if 0
		for (i=0; i<mysac->packet_length; i++) {
			fprintf(stderr, "%02x ", (unsigned char)mysac->read[i]);
		}
		fprintf(stderr, "\n");
#endif
		len = mysac_decode_field(mysac->read, mysac->packet_length,
		                         &mysac->res->cols[mysac->read_id]);

		if (len < 0) {
			mysac->errorcode = len * -1;
			return mysac->errorcode;
		}
		mysac->read += len;
		mysac->read_len += mysac->packet_length - len;

		mysac->read_id++;
		if (mysac->read_id < mysac->res->nb_cols)
			goto case_MYSAC_RECV_QUERY_COLDESC1;

		mysac->readst = 0;
		mysac->qst = MYSAC_RECV_QUERY_EOF1;

	/**********************************************************
	*
	* receive EOF
	*
	**********************************************************/
	case MYSAC_RECV_QUERY_EOF1:
		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* protocol error */
		else if (err != MYSAC_RET_EOF) {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		mysac->qst = MYSAC_RECV_QUERY_DATA;

	/**********************************************************
	*
	* read data lines
	*
	**********************************************************/
	case_MYSAC_RECV_QUERY_DATA:

	/*
	   +-------------------+----------------+-----------------+----------------+
	   | struct mysac_rows | MYSAC_ROW[]    | unsigned long[] | struct tm[]    |
	   |                   | mysac->nb_cols | mysac->nb_cols  | mysac->nb_time |
	   +-------------------+----------------+-----------------+----------------+
	 */

	/* check for avalaible size in buffer */
	while ((unsigned int)mysac->read_len < sizeof(MYSAC_ROWS) + ( mysac->res->nb_cols * (
	                         sizeof(MYSAC_ROW) + sizeof(unsigned long) ) ) )
		if (mysac_extend_res(mysac) != 0)
			return mysac->errorcode;

	mysac->read_len -= sizeof(MYSAC_ROWS) + ( mysac->res->nb_cols * (
	                   sizeof(MYSAC_ROW) + sizeof(unsigned long) ) );

	/* reserve space for MYSAC_ROWS and add it into chained list */
	mysac->res->cr = (MYSAC_ROWS *)mysac->read;
	list_add_tail(&mysac->res->cr->link, &mysac->res->data);
	mysac->read += sizeof(MYSAC_ROWS);

	/* space for each field definition into row */
	mysac->res->cr->data = (MYSAC_ROW *)mysac->read;
	mysac->read += sizeof(MYSAC_ROW) * mysac->res->nb_cols;

	/* space for length table */
	mysac->res->cr->lengths = (unsigned long *)mysac->read;
	mysac->read += sizeof(unsigned long) * mysac->res->nb_cols;

	/* struct tm */
	for (i=0; i<mysac->res->nb_cols; i++) {
		switch(mysac->res->cols[i].type) {

		/* date type */
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_DATE:
			while ((unsigned int)mysac->read_len < sizeof(struct tm))
				if (mysac_extend_res(mysac) != 0)
					return mysac->errorcode;

			mysac->res->cr->data[i].tm = (struct tm *)mysac->read;
			mysac->read += sizeof(struct tm);
			mysac->read_len -= sizeof(struct tm);
			memset(mysac->res->cr->data[i].tm, 0, sizeof(struct tm));
			break;

		/* do nothing for other types */
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_NULL:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_NEWDATE:
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_GEOMETRY:
			break;
		}
	}

	/* set state at 0 */
	mysac->readst = 0;

	case MYSAC_RECV_QUERY_DATA:
		err = my_response(mysac, MYSAC_EXPECT_DATA);

		if (err == MYERR_WANT_READ)
			return MYERR_WANT_READ;

		/* error */
		else if (err == MYSAC_RET_ERROR)
			return mysac->errorcode;

		/* EOF */
		else if (err == MYSAC_RET_EOF) {
			list_del(&mysac->res->cr->link);
			mysac->res->cr = NULL;
			return 0;
		}

		/* read data in string type */
		if (mysac->stmt_id == (void *)0) {
#if 0
			for (i=0; i<mysac->packet_length; i+=20) {
				int j;

				for(j=i;j<i+20;j++)
					fprintf(stderr, "%02x ", (unsigned char)mysac->read[j]);

				for(j=i;j<i+20;j++)
					if (isprint(mysac->read[j]))
						fprintf(stderr, "%c", (unsigned char)mysac->read[j]);
					else
						fprintf(stderr, ".");

				fprintf(stderr, "\n");
			}
			fprintf(stderr, "\n\n");
#endif
			len = mysac_decode_string_row(mysac->read, mysac->packet_length,
			                              mysac->res, mysac->res->cr);
			if (len < 0) {
				mysac->errorcode = len * -1;
				return mysac->errorcode;
			}
			mysac->read += len;
			mysac->read_len += mysac->packet_length - len;
		}

		/* read data in binary type */
		else if (mysac->stmt_id == (void *)1) {
#if 0
			for (i=0; i<mysac->packet_length; i++) {
				fprintf(stderr, "%02x ", (unsigned char)mysac->read[i]);
			}
			fprintf(stderr, "\n");
#endif
			len = mysac_decode_binary_row(mysac->read, mysac->packet_length,
			                              mysac->res, mysac->res->cr);
			if (len == -1) {
				mysac->errorcode = MYERR_BINFIELD_CORRUPT;
				return mysac->errorcode;
			}
			mysac->read += len;
			mysac->read_len += mysac->packet_length - len;
		}

		/* protocol error */
		else {
			mysac->errorcode = MYERR_PROTOCOL_ERROR;
			return mysac->errorcode;
		}

		/* next line */
		mysac->res->nb_lines++;
		goto case_MYSAC_RECV_QUERY_DATA;

	case MYSAC_START:
	case MYSAC_CONN_CHECK:
	case MYSAC_READ_GREATINGS:
	case MYSAC_SEND_AUTH_1:
	case MYSAC_RECV_AUTH_1:
	case MYSAC_SEND_AUTH_2:
	case MYSAC_SEND_INIT_DB:
	case MYSAC_RECV_INIT_DB:
	case MYSAC_SEND_STMT_QUERY:
	case MYSAC_RECV_STMT_QUERY:
	case MYSAC_SEND_STMT_EXECUTE:
	case MYSAC_RECV_STMT_EXECUTE:
	case MYSAC_READ_NUM:
	case MYSAC_READ_HEADER:
	case MYSAC_READ_LINE:
	case MYSAC_RECV_QUERY_COLDESC2:
	case MYSAC_RECV_QUERY_EOF2:
		mysac->errorcode = MYERR_BAD_STATE;
		return MYERR_BAD_STATE;
	}

	mysac->errorcode = MYERR_BAD_STATE;
	return MYERR_BAD_STATE;
}

void mysac_close(MYSAC *mysac) {
	if (mysac->free_it == 1)
		free(mysac);
}

int mysac_get_fd(MYSAC *mysac) {
	return mysac->fd;
}

int mysac_io(MYSAC *mysac) {
	if (mysac == NULL || mysac->call_it == NULL)
		return MYERR_BAD_STATE;
	return mysac->call_it(mysac);
}

MYSAC_RES *mysac_init_res(char *buffer, int len) {
	MYSAC_RES *res;

	/* check minimu length */
	if ((unsigned int)len < sizeof(MYSAC_RES))
		return NULL;

	res = (MYSAC_RES *)buffer;
	res->nb_cols = 0;
	res->nb_lines = 0;
	res->extend_bloc_size = 0;
	res->max_len = len;
	res->do_free = 0;
	res->buffer = buffer + sizeof(MYSAC_RES);
	res->buffer_len = len - sizeof(MYSAC_RES);

	return res;
}

MYSAC_RES *mysac_new_res(int chunk_size, int extend)
{
	MYSAC_RES *res;

	res = calloc(1, chunk_size);
	if (res == NULL)
		return NULL;

	if (mysac_init_res((char *)res, chunk_size) == NULL)
		return NULL;
	if (extend)
		res->extend_bloc_size = chunk_size;
	res->do_free = 1;

	return res;
}

void mysac_free_res(MYSAC_RES *r)
{
	if (r && r->do_free == 1)
		free(r);
}

MYSAC_RES *mysac_get_res(MYSAC *mysac) {
	return mysac->res;
}

int mysac_send_stmt_execute(MYSAC *mysac) {
	return mysac_send_query(mysac);
}

unsigned int mysac_field_count(MYSAC_RES *res) {
	return res->nb_cols;
}

unsigned long mysac_num_rows(MYSAC_RES *res) {
	return res->nb_lines;
}

MYSAC_ROW *mysac_fetch_row(MYSAC_RES *res) {
	if (res->cr == NULL)
		res->cr = mysac_list_first_entry(&res->data, MYSAC_ROWS, link);
	else
		res->cr = mysac_list_next_entry(&res->cr->link, MYSAC_ROWS, link);
	if (&res->data == &res->cr->link) {
		res->cr = NULL;
		return NULL;
	}
	return res->cr->data;
}

void mysac_first_row(MYSAC_RES *res) {
	res->cr = NULL;
}

MYSAC_ROW *mysac_cur_row(MYSAC_RES *res) {
	if (res->cr == NULL)
		return NULL;
	return res->cr->data;
}

unsigned long mysac_insert_id(MYSAC *m) {
	return m->insert_id;
}

unsigned int mysac_errno(MYSAC *mysac) {
	return mysac->errorcode;
}

const char *mysac_error(MYSAC *mysac) {
	return mysac_errors[mysac->errorcode];
}

const char *mysac_advance_error(MYSAC *mysac) {
	if (mysac->errorcode == MYERR_MYSQL_ERROR)
		return mysac->mysql_error;
	else if (mysac->errorcode == MYERR_SYSTEM)
		return strerror(errno);
	else
		return mysac_errors[mysac->errorcode];
}

void mysac_set_audit_fcn(MYSAC *mysac, void *arg, mysac_audit ma) {
	mysac->ma = ma;
	mysac->ma_arg = arg;
}

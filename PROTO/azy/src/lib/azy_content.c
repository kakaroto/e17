/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "azy_private.h"

/**
 * @defgroup Azy_Content Content Object Functions
 * @brief Functions which affect #Azy_Content objects
 * @{
 */

/* internal helper function to set buffer and free existing buffer if present */
Eina_Bool
azy_content_buffer_set_(Azy_Content   *content,
                        unsigned char *buffer,
                        int            length)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }
   if ((!buffer) || (length < 1))
     return EINA_FALSE;

   free(content->buffer);

   content->buffer = buffer;
   content->length = length;
   return EINA_TRUE;
}

/*
 * @brief Convert the xml/json parameters to #Azy_Value
 *
 * This function converts a block of xml/json (based on @p type)
 */
Eina_Bool
azy_content_deserialize(Azy_Content *content,
                        Azy_Net     *net)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(net->buffer, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!net->buffer[0], EINA_FALSE);
   DBG("(content=%p, net=%p, net->type=%u)", content, net, net->transport);
   if (net->transport == AZY_NET_TRANSPORT_JSON)
     return azy_content_deserialize_json(content, (char *)net->buffer, net->size);

   ERR("UNSUPPORTED TYPE PASSED! FIXME!");
   return EINA_FALSE;
#if 0
   if (type == AZY_NET_TRANSPORT_XML)
     {
#ifdef HAVE_XML
        return azy_content_deserialize_xml(content, buf, len);
#else
        ERR("%s", eina_error_msg_get(AZY_ERROR_XML_UNSUPPORTED));
        return NULL;
#endif
     }
#endif
}

/**
 * @brief Create a new content object
 *
 * This function creates a new content object, stringsharing
 * @p method as its method if specified.
 * @param method The rpc method that the content represents
 * @return The content object, or #NULL on error
 */
Azy_Content *
azy_content_new(const char *method)
{
   Azy_Content *c;

   c = calloc(1, sizeof(Azy_Content));

   EINA_SAFETY_ON_NULL_RETURN_VAL(c, NULL);
   if (method && method[0])
     c->method = eina_stringshare_add(method);

   DBG("(method=%s) = %p", method, c);
   AZY_MAGIC_SET(c, AZY_MAGIC_CONTENT);
   return c;
}

/**
 * @brief Free a content object
 *
 * This function frees a content object and all subobjects within it
 * including parameters added.
 * @param content The content to free (NOT #NULL)
 */
void
azy_content_free(Azy_Content *content)
{
   Azy_Value *v;

   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   AZY_MAGIC_SET(content, AZY_MAGIC_NONE);
   if (content->method && content->method[0])
     eina_stringshare_del(content->method);
   EINA_LIST_FREE(content->params, v)
     azy_value_unref(v);
   if (content->retval)
     azy_value_unref(content->retval);
   if (content->length)
     free(content->buffer);
   if (content->faultmsg)
     eina_stringshare_del(content->faultmsg);
   if (content->recv_net)
     azy_net_free(content->recv_net);
   free(content);
}

/**
 * @brief Associate data with an #Azy_Content object
 *
 * This function associates @p data with @p content to
 * be retrieved with azy_content_data_get.
 * @param content The content object (NOT #NULL)
 * @param data The data to associate
 */
void
azy_content_data_set(Azy_Content *content,
                     const void  *data)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   content->data = (void *)data;
}

/**
 * @brief Return the data associated with a content object
 *
 * This function returns the data associated with @p content
 * previously.
 * @see azy_content_data_set
 * @param content The content object
 * @return The data
 */
void *
azy_content_data_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->data;
}

/**
 * @brief Internally convert the #Azy_Value parameters to xml/json
 *
 * This function converts an rpc method request into xml/json (based on @p type)
 * for transmission to a server.
 * @param content The content containing the method request (NOT #NULL)
 * @param type The rpc type to use
 * @return EINA_TRUE upon success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_content_serialize_request(Azy_Content      *content,
                              Azy_Net_Transport type)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_serialize_request_json(content);

   if (type == AZY_NET_TRANSPORT_XML)
     {
#ifdef HAVE_XML
        return azy_content_serialize_request_xml(content);
#else
        azy_content_error_code_set(content, AZY_ERROR_XML_UNSUPPORTED);
        return EINA_FALSE;
#endif
     }

   ERR("ILLEGAL TYPE PASSED! %i", type);
   return EINA_FALSE;
}

/**
 * @brief Internally convert the xml/json parameters to #Azy_Value
 *
 * This function converts an rpc method request from xml/json (based on @p type)
 * and stores it in @p content for use by a server.
 * @param content The content containing the method request (NOT #NULL)
 * @param type The rpc type that @p buf contains
 * @param buf The rpc call in its native state (xml/json/etc) (NOT #NULL)
 * @param len The length of @p buf (> 0)
 * @return EINA_TRUE upon success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_content_deserialize_request(Azy_Content      *content,
                                Azy_Net_Transport type,
                                char             *buf,
                                ssize_t           len)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(buf, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!buf[0], EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!len, EINA_FALSE);

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_deserialize_request_json(content, buf, len);

   if (type == AZY_NET_TRANSPORT_XML)
     {
#ifdef HAVE_XML
        return azy_content_deserialize_request_xml(content, buf, len);
#else
        azy_content_error_code_set(content, AZY_ERROR_XML_UNSUPPORTED);
        return EINA_FALSE;
#endif
     }

   azy_content_error_faultmsg_set(content, 1, eina_stringshare_add_length(buf, len));
   return EINA_FALSE;
}

/**
 * @brief Internally convert the #Azy_Value parameters to xml/json
 *
 * This function converts an rpc method response into xml/json (based on @p type)
 * for transmission to a client.
 * @param content The content containing the method response (NOT #NULL)
 * @param type The rpc type to use
 * @return EINA_TRUE upon success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_content_serialize_response(Azy_Content      *content,
                               Azy_Net_Transport type)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_serialize_response_json(content);

   if (type == AZY_NET_TRANSPORT_XML)
     {
#ifdef HAVE_XML
        return azy_content_serialize_response_xml(content);
#else
        azy_content_error_code_set(content, AZY_ERROR_XML_UNSUPPORTED);
        return EINA_FALSE;
#endif
     }

   ERR("ILLEGAL TYPE PASSED! %i", type);
   return EINA_FALSE;
}

/**
 * @brief Internally convert the xml/json parameters to #Azy_Value
 *
 * This function converts an rpc method response from xml/json (based on @p type)
 * and stores it in @p content for use by a client.
 * @param content The content containing the method request (NOT #NULL)
 * @param type The rpc type that @p buf contains
 * @param buf The rpc response in its native state (xml/json/etc) (NOT #NULL)
 * @param len The length of @p buf (> 0)
 * @return EINA_TRUE upon success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_content_deserialize_response(Azy_Content      *content,
                                 Azy_Net_Transport type,
                                 char             *buf,
                                 ssize_t           len)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   EINA_SAFETY_ON_NULL_RETURN_VAL(buf, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!buf[0], EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!len, EINA_FALSE);

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_deserialize_response_json(content, buf, len);

   if (type == AZY_NET_TRANSPORT_XML)
     {
#ifdef HAVE_XML
        return azy_content_deserialize_response_xml(content, buf, len);
#else
        azy_content_error_code_set(content, AZY_ERROR_XML_UNSUPPORTED);
        return EINA_FALSE;
#endif
     }

   azy_content_error_faultmsg_set(content, 1, eina_stringshare_add_length(buf, len));
   return EINA_FALSE;
}

/**
 * @brief Return the full method name of a content object
 *
 * This function returns @p content's full method name including
 * the module name.
 * Note that the return value is stringshared but belongs to @p content
 * and should not be freed.
 * Examples:
 * [namespace].{Module.methodname}<==azy_content_method_full_get
 * [namespace].Module.{methodname}<==azy_content_method_get
 * [namespace].{Module}.methodname<==azy_content_module_name_get
 * @param content The content (NOT #NULL)
 * @return The full method name, or #NULL on failure
 */
const char *
azy_content_method_full_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->method;
}

/**
 * @brief Return only the method name of a content object
 *
 * This function returns @p content's method name NOT including
 * the module name.
 * Note that the return value is NOT stringshared, belongs to @p content,
 * and CANNOT not be freed.
 * Examples:
 * {[namespace].Module.methodname}<==azy_content_method_full_get
 * [namespace].Module.{methodname}<==azy_content_method_get
 * [namespace].{Module}.methodname<==azy_content_module_name_get
 * @param content The content (NOT #NULL)
 * @return The method name, or #NULL on failure
 */
const char *
azy_content_method_get(Azy_Content *content)
{
   const char *ret;
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if (!content->method)
     return NULL;

   if ((ret = strrchr(content->method, '.')))
     return ret + 1;

   return content->method;
}

/**
 * @brief Return only the module name of a content object's method
 *
 * This function returns @p content's method's module name NOT including
 * the method name.
 * Note that the return value is stringshared.
 * Examples:
 * {Module.methodname}<==azy_content_method_full_get
 * Module.{methodname}<==azy_content_method_get
 * {Module}.methodname<==azy_content_module_name_get
 * @param content The content (NOT #NULL)
 * @param fallback The fallback string to return if module name is not found
 * @return The module name, or #NULL on failure
 */
const char *
azy_content_module_name_get(Azy_Content *content,
                            const char  *fallback)
{
   const char *ret;
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if ((!content->method) || (!(ret = strrchr(content->method, '.'))))
     return eina_stringshare_add(fallback);

   return eina_stringshare_add_length(content->method, ret - content->method);
}

/**
 * @brief Get the #Azy_Net object of a transmission
 *
 * This function is used by clients to retrieve the network object containing
 * http header information from a server's response.  It will always return #NULL
 * if used outside of a transmission result callback/event.
 * @param content The content object (NOT #NULL)
 * @return The #Azy_Net object, or #NULL on failure
 */
Azy_Net *
azy_content_net_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->recv_net;
}

/**
 * @brief Add a parameter to a content object
 *
 * This function adds @p val parameter to @p content for later serialization
 * and transmission to a client/server.
 * @param content The content object (NOT #NULL)
 * @param val The #Azy_Value parameter object to add (NOT #NULL)
 */
void
azy_content_param_add(Azy_Content *content,
                      Azy_Value   *val)
{
   DBG("(content=%p, val=%p)", content, val);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   content->params = eina_list_append(content->params, val);
}

/**
 * @brief Return the Nth parameter from a content object
 *
 * This function returns the parameter from @p content in position
 * @p pos.
 * @param content The content object (NOT #NULL)
 * @param pos The position to return
 * @return The matching parameter, or #NULL on failure
 */
Azy_Value *
azy_content_param_get(Azy_Content *content,
                      unsigned int pos)
{
   DBG("(content=%p, pos=%u)", content, pos);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return eina_list_nth(content->params, pos);
}

/**
 * @brief Get the list of params
 *
 * This function returns an #Eina_List of #Azy_Value structs which
 * represent the parameters for the method call in @p content.
 * @param content The content object (NOT #NULL)
 * @return The list of params, or #NULL on failure
 */
Eina_List *
azy_content_params_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->params;
}

/* retval manipulation */
/**
 * @brief Set the retval of a content object
 *
 * This function sets the retval of the content, which is
 * what will be serialized and sent back to the client.
 * @param content The content (NOT #NULL)
 * @param val The retval
 */
void
azy_content_retval_set(Azy_Content *content,
                       Azy_Value   *val)
{
   DBG("(content=%p, val=%p)", content, val);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->retval = val;
}

/**
 * @brief Get the retval of a content object
 *
 * This function gets the retval of the content, which is
 * what is serialized and sent to the client.
 * @param content The content (NOT #NULL)
 * @return The retval, or #NULL on failure
 */
Azy_Value *
azy_content_retval_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->retval;
}

/* error manipulation */
/**
 * @brief Set the error/fault code to a pre-set error
 *
 * This function sets @p content's faultcode to @p code
 * which must be a previously defined #Eina_Error with an error string.
 * This error code and string will be used to serialize an error response
 * with faultcode and faultstring members.
 * @param content The content object (NOT #NULL)
 * @param code The error code
 */
void
azy_content_error_code_set(Azy_Content *content,
                           Eina_Error   code)
{
   DBG("(content=%p, code=%d)", content, code);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->error_set = EINA_TRUE;
   content->errcode = code;
   content->faultcode = code;
   eina_stringshare_del(content->faultmsg);
   content->faultmsg = NULL;
}

/**
 * @brief Set the faultcode to an arbitrary number, but use a previously defined faultstring
 *
 * This function sets @p content's faultcode to @p faultcode
 * but uses the error message from @p code which must be a previously defined
 * #Eina_Error with an error string.
 * This faultcode and error string will be used to serialize an error response
 * with faultcode and faultstring members.
 * @param content The content object (NOT #NULL)
 * @param code The error code with a set error message to use
 * @param faultcode The faultcode to use
 */
void
azy_content_error_faultcode_set(Azy_Content *content,
                                Eina_Error   code,
                                int          faultcode)
{
   DBG("(content=%p, code=%d)", content, code);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->error_set = EINA_TRUE;
   content->errcode = code;
   content->faultcode = faultcode;
   eina_stringshare_del(content->faultmsg);
   content->faultmsg = NULL;
}

/**
 * @brief Set the faultcode and faultstring to arbitrary values
 *
 * This function sets @p content's faultcode to @p faultcode
 * and faultstring to the format string defined by @p fmt and its subarguments.
 * This faultcode and format string will be used to serialize an error response
 * with faultcode and faultstring members.
 * @param content The content object (NOT #NULL)
 * @param faultcode The error code
 * @param fmt The format string of the error message (NOT #NULL)
 */
void
azy_content_error_faultmsg_set(Azy_Content *content,
                               int          faultcode,
                               const char  *fmt,
                               ...)
{
   va_list args;
   char *msg;

   DBG("(content=%p, faultcode='%d')", content, faultcode);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   EINA_SAFETY_ON_NULL_RETURN(fmt);

   if (content->faultmsg)
     {
        ERR("already set error : %s", content->faultmsg);
        return;
     }

   content->error_set = EINA_TRUE;

   va_start(args, fmt);
   if (vasprintf(&msg, fmt, args) < 0)
     ERR("Memory error in %s!", __PRETTY_FUNCTION__);
   va_end(args);

   content->faultcode = faultcode;
   content->faultmsg = eina_stringshare_add(msg);
   free(msg);
}

/**
 * @brief Check if a content object contains an rpc error
 *
 * This function will return true if and only if @p content contains
 * an rpc error.
 * @param content The content object (NOT #NULL)
 * @return #EINA_TRUE if an error is set, else #EINA_FALSE
 */
Eina_Bool
azy_content_error_is_set(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   return content->error_set;
}

/**
 * @brief Reset a content's error status
 *
 * This function unsets the error marker in @p content,
 * resets its stored error codes, and frees any previously set
 * fault strings.
 * @param content The content object (NOT #NULL)
 */
void
azy_content_error_reset(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   if (!content->error_set)
     return;

   if (content->faultmsg)
     {
        eina_stringshare_del(content->faultmsg);
        content->faultmsg = NULL;
     }
   content->errcode = 0;
   content->faultcode = 0;

   content->error_set = EINA_FALSE;
   return;
}

/**
 * @brief Copy all error information from one #Azy_Content to another
 *
 * This function propogates error information from one content object to
 * another object, also copying a user-specified faultstring if previously
 * set.
 * @param from The content to copy from
 * @param to The content to copy to
 */
void
azy_content_error_copy(Azy_Content *from,
                       Azy_Content *to)
{
   DBG("(from=%p, to=%p)", from, to);

   if (!AZY_MAGIC_CHECK(from, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(from, AZY_MAGIC_CONTENT);
        return;
     }

   if (!AZY_MAGIC_CHECK(to, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(to, AZY_MAGIC_CONTENT);
        return;
     }

   to->errcode = from->errcode;
   to->faultcode = from->faultcode;
   to->error_set = from->error_set;
   if (from->faultmsg) to->faultmsg = eina_stringshare_ref(from->faultmsg);
}

/**
 * @brief Return the error code of an #Azy_Content object
 *
 * This function returns the error code of @p content object.  The
 * error code returned will be one set with a previously defined
 * #EINA_ERROR or an arbitrary faultcode.
 * @param content The content object (NOT #NULL)
 * @return The error code in the object, or 0 on failure
 */
Eina_Error
azy_content_error_code_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return 0;
     }

   if (content->errcode) return content->errcode;
   return content->faultcode;
}

/**
 * @brief Get the error message from a content object
 *
 * This function returns the error message of an #Azy_Content.
 * Precedence is given to specific fault strings over #Eina_Error
 * strings.
 * Note that the returned value belongs to the content and cannot be freed.
 * @param content The content object (NOT #NULL)
 * @return The error message, or #NULL on failure
 */
const char *
azy_content_error_message_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if (!content->error_set)
     return NULL;

   if (content->faultmsg)
     return content->faultmsg;

   return eina_error_msg_get(content->errcode);
}

/**
 * @brief Dump an #Azy_Content's params to a string with optional indentation
 *
 * This function dumps the params in @p content to a string and returns it,
 * optionally indenting each line @p indent spaces.
 * Note that the returned string must be manually freed.
 * @param content The content object (NOT #NULL)
 * @param indent The number of spaces to indent
 * @return The parameters string, or #NULL on failure
 */
char *
azy_content_dump_string(const Azy_Content *content,
                        unsigned int       indent)
{
   Eina_List *l;
   Eina_Strbuf *string;
   char buf[256];
   char *ret;
   Eina_Bool single_line = EINA_TRUE;
   Azy_Value *v;

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   memset(buf, 0, (indent * 2 < sizeof(buf)) ? sizeof(buf) - (indent * 2) : 1);
   memset(buf, ' ', MIN(indent * 2, sizeof(buf) - 1));
   string = eina_strbuf_new();

   if (eina_list_count(content->params) > 6)
     single_line = EINA_FALSE;
   else
     EINA_LIST_FOREACH(content->params, l, v)
       if (azy_value_multi_line_get_(v, 25))
         single_line = EINA_FALSE;

   eina_strbuf_append_printf(string, "%s%s(", buf, content->method ? content->method : "<anonymous>");

   if (single_line)
     EINA_LIST_FOREACH(content->params, l, v)
       {
          azy_value_dump(v, string, indent);

          if (l->next)
            eina_strbuf_append(string, ", ");
       }
   else
     {
        EINA_LIST_FOREACH(content->params, l, v)
          {
             eina_strbuf_append_printf(string, "\n%s  ", buf);
             azy_value_dump(v, string, indent + 1);

             if (l->next)
               eina_strbuf_append(string, ",");
          }

        eina_strbuf_append_printf(string, "\n%s", buf);
     }

   eina_strbuf_append(string, ")");

   if (content->retval)
     {
        eina_strbuf_append(string, " = ");
        azy_value_dump(content->retval, string, indent);
     }

   if (content->errcode)
     eina_strbuf_append_printf(string, " = { faultCode: %d, faultString: \"%s\" }", content->faultcode, content->faultmsg ? content->faultmsg : "");

   ret = eina_strbuf_string_steal(string);
   eina_strbuf_free(string);

   return ret;
}

/**
 * @brief Get the return value of a transmission
 *
 * This function gets the return value of a method call, and is only
 * functional for clients in return callbacks/events. The type of the
 * return value can be determined by the transport type of the content's
 * #Azy_Net object.
 * @note the returned content is owned by @p content and should not
 * be manually freed.
 * @param content The content object (NOT #NULL)
 * @return The method response return value, or #NULL on failure
 */
void *
azy_content_return_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->ret;
}

/**
 * @brief Get the size of the return value of a transmission
 *
 * This function gets the size of the return value of a method call, and is only
 * functional for clients in return callbacks/events.
 * Note that the returned content is owned by @p content and should not
 * be manually freed.
 * @param content The content object (NOT #NULL)
 * @return The method response return value, or #NULL on failure
 */
uint64_t
azy_content_return_size_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return -1;
     }
   return content->retsize;
}

/**
 * @brief Get the id of a content object
 *
 * This function returns the client transmission id of @p content,
 * allowing a user to determine which call the content object returned from.
 * Note that this function will return 0 unless called in a client return
 * callback/event.
 * @param content The content object (NOT #NULL)
 * @return The transmission id, or 0 on failure
 */
Azy_Client_Call_Id
azy_content_id_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return 0;
     }
   return content->id;
}

/**
 * @brief Set the callback used to serialize a content's retval.
 *
 * This function is called automatically by the azy_parser utility.
 * @param content The content
 * @param cb The serialization callback
 */
void
azy_content_retval_cb_set(Azy_Content *content, Azy_Content_Retval_Cb cb)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   content->retval_cb = cb;
}

/**
 * @brief Get the callback used to serialize a content's retval.
 *
 * This function is used to retrieve the callback used to serialize an RPC
 * message using native data types.
 * @param content The content
 * @return The serialization callback
 */
Azy_Content_Retval_Cb
azy_content_retval_cb_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->retval_cb;

}

/** @} */

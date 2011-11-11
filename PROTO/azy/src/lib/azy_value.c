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
 * @defgroup Azy_Value Low-level serialization
 * @brief Functions which provide a generic struct to represent RPC
 * @{
 */

static Eina_Mempool *value_mempool = NULL;

static Eina_Hash *string_values = NULL;
static Eina_Hash *base64_values = NULL;
static Eina_Hash *int_values = NULL;
static Eina_Hash *bool_values = NULL;
static Eina_Hash *time_values = NULL;

static Azy_Value *azy_value_new_(void);
static Eina_Bool  azy_value_list_multi_line_get_(Azy_Value *v);

/* allocate a new Azy_Value */
static inline Azy_Value *
azy_value_new_(void)
{
   Azy_Value *v;

   v = eina_mempool_malloc(value_mempool, sizeof(Azy_Value));

   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   EINA_REFCOUNT_INIT(v);
   AZY_MAGIC_SET(v, AZY_MAGIC_VALUE);
   return v;
}

/* returns EINA_TRUE if a struct/array object requires multiple lines to print */
static Eina_Bool
azy_value_list_multi_line_get_(Azy_Value *v)
{
   Eina_List *l;
   Azy_Value *val;

   if (!v) return EINA_FALSE;

   if (v->type == AZY_VALUE_ARRAY)
     {
        if (eina_list_count(azy_value_children_items_get(v)) > 8)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
            if (azy_value_multi_line_get_(val, 35))
              return EINA_TRUE;
     }
   else if (v->type == AZY_VALUE_STRUCT)
     {
        if (eina_list_count(azy_value_children_items_get(v)) > 5)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
            if (azy_value_multi_line_get_(azy_value_struct_member_value_get(val), 25))
              return EINA_TRUE;
     }

   return EINA_FALSE;
}

Eina_Bool
azy_value_init(void)
{
   const char *type;

   type = getenv("EINA_MEMPOOL");
   if ((!type) || (!type[0])) type = "chained_mempool";
   value_mempool = eina_mempool_add(type, "Azy_Value", NULL, sizeof(Azy_Value), 64);
   if (!value_mempool)
     {
        if (!strcmp(type, "pass_through")) goto error;
        value_mempool = eina_mempool_add("pass_through", "Azy_Value", NULL, sizeof(Azy_Value), 64);
        if (!value_mempool) goto error;
     }

   string_values = eina_hash_string_superfast_new(NULL);
   base64_values = eina_hash_string_superfast_new(NULL);
   time_values = eina_hash_string_superfast_new(NULL);
   int_values = eina_hash_int64_new(NULL);
   bool_values = eina_hash_int32_new(NULL);

   return EINA_TRUE;
error:
   EINA_LOG_CRIT("Unable to set up mempool!");
   return EINA_FALSE;
}

void
azy_value_shutdown(void)
{
   eina_hash_free(string_values);
   eina_hash_free(base64_values);
   eina_hash_free(time_values);
   eina_hash_free(int_values);
   eina_hash_free(bool_values);
   string_values = base64_values = int_values = time_values = bool_values = NULL;
}

/* returns EINA_TRUE if the line requires multiple lines to print */
Eina_Bool
azy_value_multi_line_get_(Azy_Value *val,
                          int        max_strlen)
{
   switch (val->type)
     {
      case AZY_VALUE_STRUCT:
      case AZY_VALUE_ARRAY:
        if (azy_value_children_items_get(val))
          return EINA_TRUE;
        break;

      case AZY_VALUE_STRING:
        if (val->str_val && (eina_stringshare_strlen(val->str_val) > max_strlen))
          return EINA_TRUE;
        break;

      default:
        break;
     }
   return EINA_FALSE;
}

/**
 * @brief Increase the refcount of an existing #Azy_Value
 *
 * This function can be used to increase the reference count of @p val,
 * effectively copying the struct.  Note that only the struct itself has
 * its refcount increased, not the members.
 * @param val The #Azy_Value to ref (NOT NULL)
 * @return The same variable, or #NULL on error
 */
Azy_Value *
azy_value_ref(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   EINA_REFCOUNT_REF(val);
   return val;
}

/**
 * @brief Decrease the refcount of an existing #Azy_Value
 *
 * This function can be used to decrease the reference count of @p val,
 * effectively deleting an instance of the struct.  Note that only the
 * struct itself has its refcount decreased, not the members.  If the refcount
 * of an object reaches 0, it and its data are automatically freed.
 * @param val The #Azy_Value to ref (NOT NULL)
 */
void
azy_value_unref(Azy_Value *val)
{
   Azy_Value *v;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   EINA_REFCOUNT_UNREF(val)
     {
        AZY_MAGIC_SET(val, AZY_MAGIC_NONE);

        if (val->str_val) eina_stringshare_del(val->str_val);
        if (val->member_name) eina_stringshare_del(val->member_name);
        if (val->member_value)
          azy_value_unref(val->member_value);
        EINA_LIST_FREE(val->children, v)
          azy_value_unref(v);
        eina_mempool_free(value_mempool, val);
     }
}

/* base types */
/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_STRING
 *
 * This function allocates memory around and stringshares a string into
 * an #Azy_Value struct.
 * @param val The string
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_string_new(const char *val)
{
   Azy_Value *v;

   v = eina_hash_find(string_values, val ? val : "");
   if (v)
     {
        EINA_REFCOUNT_REF(v);
        return v;
     }

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRING;
   v->str_val = eina_stringshare_add(val ? val : "");
   eina_hash_add(string_values, v->str_val, v);
   return v;
}

/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_INT
 *
 * This function allocates memory around an int into
 * an #Azy_Value struct.
 * @param val The int
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_int_new(int val)
{
   Azy_Value *v;

   v = eina_hash_find(int_values, &val);
   if (v)
     {
        EINA_REFCOUNT_REF(v);
        return v;
     }

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_INT;
   v->int_val = val;
   eina_hash_add(int_values, &v->int_val, v);
   return v;
}

/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_BOOL
 *
 * This function allocates memory around a boolean value into
 * an #Azy_Value struct.
 * @param val The boolean value
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_bool_new(Eina_Bool val)
{
   Azy_Value *v;

   val = !!val;

   v = eina_hash_find(bool_values, &val);
   if (v)
     {
        EINA_REFCOUNT_REF(v);
        return v;
     }

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_BOOL;
   v->int_val = val;
   eina_hash_add(bool_values, &v->int_val, v);
   return v;
}

/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_DOUBLE
 *
 * This function allocates memory around a double into
 * an #Azy_Value struct.
 * @param val The double
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_double_new(double val)
{
   Azy_Value *v;

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_DOUBLE;
   v->dbl_val = val;
   return v;
}

/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_TIME
 *
 * This function allocates memory around and stringshares a time string into
 * an #Azy_Value struct.
 * @param val The time string
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_time_new(const char *val)
{
   Azy_Value *v;

   v = eina_hash_find(time_values, val ? val : "");
   if (v)
     {
        EINA_REFCOUNT_REF(v);
        return v;
     }

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_TIME;
   v->str_val = eina_stringshare_add(val ? val : "");
   eina_hash_add(time_values, v->str_val, v);
   return v;
}

/**
 * @brief Create an #Azy_Value of type #AZY_VALUE_BASE64
 *
 * This function allocates memory around a base64 encoded
 * string into an #Azy_Value struct.
 * @param base64 The base64 string
 * @return The allocated struct, or #NULL on error
 */
Azy_Value *
azy_value_base64_new(const char *base64)
{
   Azy_Value *val;

   if (!base64) return NULL;

   val = eina_hash_find(base64_values, base64);
   if (val)
     {
        EINA_REFCOUNT_REF(val);
        return val;
     }

   val = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(val, NULL);
   val->type = AZY_VALUE_BASE64;
   val->str_val = eina_stringshare_add(base64);
   eina_hash_add(base64_values, val->str_val, val);
   return val;
}

/* conversion functions */
/**
 * @brief Retrieve the int value from an #Azy_Value struct of type #AZY_VALUE_INT
 *
 * This function gets the int value previously stored in the struct,
 * returning success or failure.
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the int in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_int_get(Azy_Value *val,
                  int       *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_INT))
     return EINA_FALSE;

   *nval = val->int_val;
   return EINA_TRUE;
}

/**
 * @brief Retrieve the string value from an #Azy_Value struct of type
 *
 * #AZY_VALUE_STRING, #AZY_VALUE_TIME, or #AZY_VALUE_BASE64
 * This function stringshare_refs the string value previously stored in the struct,
 * returning success or failure.  It accepts all string type values, and
 * will automatically decode base64 strings.
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the value in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_string_get(Azy_Value   *val,
                     const char **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if (!nval) return EINA_FALSE;

   switch (val->type)
     {
      case AZY_VALUE_STRING:
      case AZY_VALUE_TIME:
        *nval = eina_stringshare_ref(val->str_val);
        break;

      case AZY_VALUE_BASE64:
      {
         char *buf;
         buf = azy_base64_decode(val->str_val, eina_stringshare_strlen(val->str_val));
         EINA_SAFETY_ON_NULL_RETURN_VAL(buf, EINA_FALSE);
         *nval = eina_stringshare_add(buf);
         free(buf);
         break;
      }

      default:
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

/**
 * @brief Retrieve the string value from an #Azy_Value struct of type #AZY_VALUE_BASE64
 *
 * This function stringshare_refs the base64 encoded string previously stored in
 * the struct into a new pointer.
 * To retrieve the value and decode it in one call, @see azy_value_string_get
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the value in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_base64_get(Azy_Value   *val,
                     const char **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_BASE64)) return EINA_FALSE;

   *nval = eina_stringshare_ref(val->str_val);
   return EINA_TRUE;
}

/**
 * @brief Retrieve the int value from an #Azy_Value struct of type #AZY_VALUE_BOOL
 *
 * This function gets the bool value previously stored in the struct,
 * returning success or failure.
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the bool in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_bool_get(Azy_Value *val,
                   Eina_Bool *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_BOOL))
     return EINA_FALSE;

   *nval = !!val->int_val;
   return EINA_TRUE;
}

/**
 * @brief Retrieve the double value from an #Azy_Value struct of type
 *
 * #AZY_VALUE_DOUBLE, or #AZY_TYPE_INT
 * This function gets the double value previously stored in the struct,
 * returning success or failure.
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the value in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_double_get(Azy_Value *val,
                     double    *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || ((val->type != AZY_VALUE_DOUBLE) && (val->type != AZY_VALUE_INT)))
     return EINA_FALSE;

   if (val->type == AZY_VALUE_DOUBLE)
     *nval = val->dbl_val;
   else
     *nval = (double)val->int_val;
   return EINA_TRUE;
}

/**
 * @brief Increases the refcount of an #Azy_Value
 *
 * This function increases the refcount of an #Azy_Value, returning
 * success or failure.
 * @param val The #Azy_Value (NOT NULL)
 * @param nval The pointer to store the value in (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_value_value_get(Azy_Value  *val,
                    Azy_Value **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }

   if (!nval)
     return EINA_FALSE;

   *nval = azy_value_ref(val);
   return EINA_TRUE;
}

/**
 * @brief Return the type of an #Azy_Value
 *
 * This function is used to return the type of value in
 * an #Azy_Value.
 * @param val The #Azy_Value struct (NOT NULL)
 * @return The #Azy_Value_Type, or -1 on error
 */
Azy_Value_Type
azy_value_type_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return -1;
     }

   return val->type;
}

/**
 * @brief Set the type of an #Azy_Value
 *
 * This function is used to set the type of value in
 * an #Azy_Value.
 * @param val The #Azy_Value struct (NOT NULL)
 * @param type The #Azy_Value_Type
 */
void
azy_value_type_set(Azy_Value *val, Azy_Value_Type type)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   val->type = type;
}

/**
 * @brief Returns the name of the #Azy_Value struct member
 *
 * This function returns the name of the struct member that the #Azy_Value
 * contains.  Note that the name is still owned by the #Azy_Value, but is
 * guaranteed to be stringshared.
 * @param val The #Azy_Value of type #AZY_VALUE_MEMBER (NOT NULL)
 * @return The struct member's name, or #NULL on failure
 */
const char *
azy_value_struct_member_name_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if (val->type != AZY_VALUE_MEMBER)
     return NULL;

   return val->member_name;
}

/**
 * @brief Returns the value of the #Azy_Value struct member
 *
 * This function returns the value of the struct member that the specified
 * #Azy_Value contains.  Note that the returned value is still owned by the
 * specified #Azy_Value.
 * @param val The #Azy_Value of type #AZY_VALUE_MEMBER (NOT NULL)
 * @return The struct member's value, or #NULL on failure
 */
Azy_Value *
azy_value_struct_member_value_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if (val->type != AZY_VALUE_MEMBER)
     return NULL;

   return val->member_value;
}

/**
 * @brief Returns the #Azy_Value struct member that matches a name
 *
 * This function returns the struct member that the specified
 * #Azy_Value contains which matches @p name.
 * Note that the returned value is still owned by the
 * specified #Azy_Value.
 * @param val The #Azy_Value of type #AZY_VALUE_STRUCT (NOT NULL)
 * @param name The name of the member to return (NOT NULL)
 * @return The #Azy_Value of the named struct member, or #NULL on failure
 */
Azy_Value *
azy_value_struct_member_get(Azy_Value  *val,
                            const char *name)
{
   Eina_List *l;
   Azy_Value *m;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   if (val->type != AZY_VALUE_STRUCT)
     return NULL;

   EINA_LIST_FOREACH(val->children, l, m)

     if (!strcmp(azy_value_struct_member_name_get(m), name))
       return azy_value_struct_member_value_get(m);

   return NULL;
}

/**
 * @brief Returns a list of the child members of an #Azy_Value
 *
 * This function returns the list of struct member that the specified
 * #Azy_Value contains.
 * Note that the returned values (including the list) are still owned by the
 * specified #Azy_Value.
 * @param val The #Azy_Value of type #AZY_VALUE_ARRAY or #AZY_VALUE_STRUCT (NOT NULL)
 * @return An #Eina_List of #Azy_Value members, or #NULL on failure
 */
Eina_List *
azy_value_children_items_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if ((val->type != AZY_VALUE_ARRAY) && (val->type != AZY_VALUE_STRUCT))
     return NULL;

   return val->children;
}

/* composite types */
/**
 * @brief This function allocates and returns a new #Azy_Value of type AZY_VALUE_STRUCT
 * @return The new #Azy_Value struct, or #NULL on failure
 */
Azy_Value *
azy_value_struct_new(void)
{
   Azy_Value *v;

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRUCT;
   return v;
}

/**
 * @brief This function allocates and returns a new #Azy_Value of type AZY_VALUE_ARRAY
 * @return The new #Azy_Value array struct, or #NULL on failure
 */
Azy_Value *
azy_value_array_new(void)
{
   Azy_Value *v;

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_ARRAY;
   return v;
}

/**
 * @brief Set a struct member in an #Azy_Value to use a name and a value
 *
 * This function sets member with name @p name to @p val.  If a previous member
 * with @p name exists, unref it and use @p val instead.
 * @param struc The #Azy_Value struct to set the member in (NOT NULL)
 * @param name The member name (NOT NULL)
 * @param val The struct member value
 */
void
azy_value_struct_member_set(Azy_Value  *struc,
                            const char *name,
                            Azy_Value  *val)
{
   Eina_List *l;
   Azy_Value *m, *v;

   if (!AZY_MAGIC_CHECK(struc, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(struc, AZY_MAGIC_VALUE);
        return;
     }

   EINA_SAFETY_ON_NULL_RETURN(name);

   if (struc->type != AZY_VALUE_STRUCT)
     return;

   EINA_LIST_FOREACH(struc->children, l, m)
     if (!strcmp(m->member_name, name))
       {
          azy_value_unref(m->member_value);
          m->member_value = val;
          return;
       }

   v = azy_value_new_();
   EINA_SAFETY_ON_NULL_RETURN(v);
   v->type = AZY_VALUE_MEMBER;
   v->member_name = eina_stringshare_add(name);
   v->member_value = val;
   struc->children = eina_list_append(struc->children, v);
}

/**
 * @brief Add a value to an array
 *
 * This function adds @p val to array @p arr.
 * @param arr The array (NOT NULL)
 * @param val The value to add (NOT NULL)
 */
void
azy_value_array_push(Azy_Value *arr,
                     Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }
   if (!AZY_MAGIC_CHECK(arr, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(arr, AZY_MAGIC_VALUE);
        return;
     }

   arr->children = eina_list_append(arr->children, val);
}

/**
 * @brief Check if an #Azy_Value is an RPC error
 *
 * This function checks to see if @p val is an RPC error
 * with a faultcode and faultstring, returning both if
 * it is.
 * Note that the faultstring returned is still owned by @p val.
 * @param val The #Azy_Value to check
 * @param errcode Pointer to store a faultcode in
 * @param errmsg Pointer to store a faultmsg in
 * @return EINA_FALSE if @p val is not an error value, else EINA_TRUE
 */
Eina_Bool
azy_value_retval_is_error(Azy_Value   *val,
                          int         *errcode,
                          const char **errmsg)
{
   Azy_Value *c, *s;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((val->type != AZY_VALUE_STRUCT) || (!errcode) || (!errmsg))
     return EINA_FALSE;

   c = azy_value_struct_member_get(val, "faultCode");
   s = azy_value_struct_member_get(val, "faultString");

   if ((!c) && (!s))
     return EINA_FALSE;

   if (s)
     *errmsg = s->str_val;
   if (c)
     *errcode = c->int_val;

   return EINA_TRUE;
}

/**
 * @brief Dump a value's contents into a string
 *
 * This function appends the values in @p v into #Eina_Strbuf @p string,
 * indenting @p indent spaces.  It calls itself recursively, dumping all sub-values
 * into @p string as well.
 * Note that base64 values are NOT decoded when dumping.
 * @param v The value to dump (NOT NULL)
 * @param string The #Eina_Strbuf to append to (NOT NULL)
 * @param indent The number of spaces to indent
 */
void
azy_value_dump(Azy_Value   *v,
               Eina_Strbuf *string,
               unsigned int indent)
{
   Eina_List *l;
   Azy_Value *val;
   char buf[256];

   EINA_SAFETY_ON_NULL_RETURN(v);
   EINA_SAFETY_ON_NULL_RETURN(string);

   memset(buf, ' ', MIN(indent * 2, sizeof(buf) - 1));

   switch (azy_value_type_get(v))
     {
      case AZY_VALUE_ARRAY:
      {
         if (!azy_value_children_items_get(v))
           eina_strbuf_append(string, "[]");
         else if (!azy_value_list_multi_line_get_(v))
           {
              eina_strbuf_append(string, "[ ");
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent + 1);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, ']');
           }
         else
           {
              eina_strbuf_append_char(string, '[');
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   eina_strbuf_append_printf(string, "\n%s  ", buf);
                   azy_value_dump(val, string, indent + 1);

                   if (l->next)
                     eina_strbuf_append_char(string, ',');
                }
              eina_strbuf_append_printf(string, "\n%s]", buf);
           }

         break;
      }

      case AZY_VALUE_STRUCT:
      {
         if (!azy_value_children_items_get(v))
           eina_strbuf_append(string, "{}");
         else if (!azy_value_list_multi_line_get_(v))
           {
              eina_strbuf_append(string, "{ ");
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, '}');
           }
         else
           {
              eina_strbuf_append_char(string, '{');
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   eina_strbuf_append_printf(string, "\n%s  ", buf);
                   azy_value_dump(val, string, indent);

                   if (l->next)
                     eina_strbuf_append_char(string, ',');
                }
              eina_strbuf_append_printf(string, "\n%s}", buf);
           }

         break;
      }

      case AZY_VALUE_MEMBER:
      {
         eina_strbuf_append_printf(string, "%s: ", azy_value_struct_member_name_get(v));
         azy_value_dump(azy_value_struct_member_value_get(v), string, indent + 1);
         break;
      }

      case AZY_VALUE_INT:
      {
         eina_strbuf_append_printf(string, "%d", v->int_val);
         break;
      }

      case AZY_VALUE_STRING:
      case AZY_VALUE_TIME:
      case AZY_VALUE_BASE64:
      {
         eina_strbuf_append_printf(string, "\"%s\"", v->str_val);
         break;
      }

      case AZY_VALUE_BOOL:
      {
         eina_strbuf_append_printf(string, "%s", v->int_val ? "true" : "false");
         break;
      }

      case AZY_VALUE_DOUBLE:
      {
         eina_strbuf_append_printf(string, "%g", v->dbl_val);
         break;
      }
     }
}

/** @} */

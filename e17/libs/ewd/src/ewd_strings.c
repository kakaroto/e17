#include <Ewd.h>

static Ewd_Hash *ewd_strings = NULL;

/**
 * ewd_stringinstance - retrieve an instance of a string for use in an ewd
 * @string: the string to retrieve an instance
 *
 * Returns a pointer to a the string on success, NULL on failure.
 */
char *ewd_string_instance(char *string)
{
	Ewd_String *str;

	CHECK_PARAM_POINTER_RETURN("string", string, NULL);

	/*
	 * No strings have been loaded at this point, so create the hash
	 * table for storing string info for later.
	 */
	if (!ewd_strings)
		ewd_strings = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	/*
	 * Check for a previous instance of the string, if not found, create
	 * it.
	 */
	str = ewd_hash_get(ewd_strings, string);
	if (!str) {

		/*
		 * Allocate and initialize a new string reference.
		 */
		str = (Ewd_String *)malloc(sizeof(Ewd_String));

		str->string = strdup(string);
		str->references = 0;

		ewd_hash_set(ewd_strings, string, str);
	}

	str->references++;

	return str->string;
}

/**
 * ewd_string_release - release an instance of a string
 * @string: the string to release an instance
 *
 * Returns no value. Marks the string as losing an instance, will free the
 * string if no other instances are present.
 */
void ewd_string_release(char *string)
{
	Ewd_String *str;

	CHECK_PARAM_POINTER("string", string);

	str = ewd_hash_get(ewd_strings, string);
	if (!str)
		return;

	str->references--;
	if (str->references < 1) {
		ewd_hash_remove(ewd_strings, string);
		FREE(str->string);
	}
}

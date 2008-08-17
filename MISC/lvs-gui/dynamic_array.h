/**********************************************************************
 * dynamic_array.h                                        November 1999
 *
 * Dynamic array, to store all your flims in
 * Includes macros required to create an array of strings but
 * as the primitive type for the array is void * providing your
 * own duplicate_primitive and destroy_primitive functions will
 * allow you to use the dynamic_array API to have a dynamic
 * array containing any primitive
 *
 * Copyright (C) 1999 Carsten Haitzler and Simon Horman
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *   
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 *    
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#ifndef DYNAMIC_ARRAY_FLIM
#define DYNAMIC_ARRAY_FLIM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/*
 * Default blocking size for dynamic array
 * can be overriden when array is created
 */
#define DEFAULT_DYNAMIC_ARRAY_BLOCK_SIZE (size_t)7


/* #defines to destroy and dupilcate strings */
#define DESTROY_STR (void (*)(void *s))free
#define DUP_STR (void *(*)(void *s))strdup
#define DISPLAY_STR (void (*)(char *d, void *s))strcpy
#define LEN_STR (size_t (*)(void *s))strlen


typedef struct {
  void   **vector;
  size_t count;
  size_t allocated_size;
  size_t block_size;
} dynamic_array_t;


/**********************************************************************
 * dynamic_array_create
 * Create a dynamic array
 * pre: block_size: blocking size to use.
 *                  DEFAULT_DYNAMIC_ARRAY_BLOCK_SIZE is used if 
 *                  block_size is 0
 *                  Block size refers to how many elements are prealocated
 *                  each time the array is grown. 
 * return: An empty dynamic array 
 *         NULL on error
 **********************************************************************/

dynamic_array_t *dynamic_array_create(size_t block_size);


/**********************************************************************
 * dynamic_array_destroy
 * Free an array an all the elements held within
 * pre: a: array to destroy
 *      destroy_element: pointer to funtion to destroy array elements
 *                       Function should take an argument of a pointer 
 *                       and free the memory allocated to the structure
 *                       pointed to.
 * post: array is freed and destroy_element is called for all elements
 *       of the array.
 *       Nothing if a is NULL
 **********************************************************************/

void dynamic_array_destroy(dynamic_array_t *a, void (*destroy_element)(void *));


/**********************************************************************
 * dynamic_array_add_element
 * Add an element to a dynamic array
 * pre: a: dynamic array to add element to
 *      e: element to add
 *      destroy_element: pointer to a function to destroy an element
 *                       passed to dynamic_array_destroy on error
 *      duplicate_element: pointer to a function to duplicate an 
 *                         element should take a pointer to an element 
 *                         to duplicate as the only element and return 
 *                         a copy of the element Any memory allocation 
 *                         required should be done by this function.
 * post: element in inserted in the first unused position in the array
 *       array size is incresaed by a->block_size if there is 
 *       insufficient room in the array to add the element.
 *       Nothing is done if e is NULL
 * return: a on success
 *         NULL if a is NULL or an error occurs
 **********************************************************************/

dynamic_array_t *dynamic_array_add_element(
  dynamic_array_t *a, 
  void *e,
  void (*destroy_element)(void *s),
  void *(*duplicate_element)(void *s)
);


/**********************************************************************
 * dynamic_array_display
 * Print the contents of a dynamic array to a string
 * pre: a: dynamic array to display
 *      delimiter: character to place between elements of the array
 *      display_element: pointer to a function to display an element
 *      element_length:  pointer to a function to return the
 *                       length of an element
 * post: If a is NULL or there are no elements in a then nothing is done
 *       Else a character buffer is alocated and the contents
 *       of each array element, separated by delimiter is placed
 *       in the '\0' termintated buffer returned. It is up to the
 *       user to free this buffer.
 * return: Allocated buffer as above
 *         NULL on error, NULL a or empty a
 **********************************************************************/

char *dynamic_array_display(
  dynamic_array_t *a,
  char delimiter,
  void (*display_element)(char *, void *),
  size_t (*element_length)(void *)
);


/**********************************************************************
 * dynamic_array_get_element
 * Get an element from an array
 * pre: a: array to retrieve element from
 *      elementno: index element in array to retrieve
 * post: no change is made to a
 * return: element requested
 *         NULL if element is beyond the number of elements in the arary
 **********************************************************************/

void *dynamic_array_get_element(dynamic_array_t *a, size_t elementno);


/**********************************************************************
 * dynamic_array_get_count
 * Get the number of elements in the array
 * pre: array to find the number of elements in
 * return: number of elements in the array
 *         -1 if a is NULL
 **********************************************************************/

size_t dynamic_array_get_count(dynamic_array_t *a);

/**********************************************************************
 * dynamic_array_get_vector
 * Get the array contained in the dynamic array
 * pre: array to find the vector of
 * return: vector
 *         NULL if a is NULL
 **********************************************************************/

void *dynamic_array_get_vector(dynamic_array_t *a);


/**********************************************************************
 * dynamic_array_split_str
 * Split a string into substrings on a delimiter
 * pre: str: string to split
 *      delimiter: character to split string on
 * post: string is split. 
 *       Note: The string is modified.
 * return: dynamic array containing sub_strings
 *         NULL on error
 *         string being NULL is an error state
 **********************************************************************/

dynamic_array_t *dynamic_array_split_str(char *string, const char delimiter);

#endif

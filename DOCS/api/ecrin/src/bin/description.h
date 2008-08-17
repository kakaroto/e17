#ifndef __DESCRIPTION_H__
#define __DESCRIPTION_H__



/* 
 * Description of an element
 * Brief and detailed one. 
 */
typedef struct _Ecrin_Description Ecrin_Description;
struct _Ecrin_Description
{
  char *brief;
  char *detailed;
};

Ecrin_Description *ecrin_description_new (char *brief,
					  char *detailed);
void ecrin_description_free (Ecrin_Description *description);


#endif /* __DESCRIPTION_H__ */

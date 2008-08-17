#ifndef __EDVI_PROPERTY_H__
#define __EDVI_PROPERTY_H__


#include "edvi_forward.h"


/**
 * @file edvi_property.h
 * @defgroup Edvi_Property Edvi Property
 * @brief Functions that manage DVI properties
 * @ingroup Edvi
 *
 * Functions that manage DVI properties
 */


/**
 * Return a newly created Edvi_Property object
 *
 * @return A pointer to a newly created Edvi_Property
 *
 * Return a newly created Edvi_Property object built from the
 * DVI file whose name is @p filename. The result must be freed with
 * edvi_property_delete().
 *
 * @ingroup Edvi_Property
 */
Edvi_Property *edvi_property_new ();

/**
 * Delete an Edvi_Property object
 *
 * @param property The property to delete
 *
 * Delete the Edvi_Property @p property that has been created
 * with edvi_property_new()
 *
 * @ingroup Edvi_Document
 */
void edvi_property_delete (Edvi_Property *property);

/**
 * Set an identifier in a given property.
 *
 * @param property The property
 * @param id The property identifier.
 *
 * Set the property identifier @p id in @p property.
 *
 * @ingroup Edvi_Document
 */
void edvi_property_property_set (Edvi_Property   *property,
                                 Edvi_Property_Id id);

/**
 * Unset an identifier in a given property.
 *
 * @param property The property
 * @param id The property identifier.
 *
 * Unset the property identifier @p id in @p property.
 *
 * @ingroup Edvi_Document
 */
void edvi_property_property_unset (Edvi_Property   *property,
                                   Edvi_Property_Id id);

/**
 * Check if an identifier is set in a given property.
 *
 * @param property The property
 * @param id The property identifier.
 * @return 1 if the identifier is set, 0 otherwise.
 *
 * Check if the property identifier @p id is set in @p property.
 *
 * @ingroup Edvi_Document
 */
int edvi_property_property_isset (Edvi_Property   *property,
                                  Edvi_Property_Id id);


#endif /* __EDVI_PROPERTY_H__ */

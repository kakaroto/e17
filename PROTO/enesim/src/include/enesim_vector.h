/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ENESIM_VECTOR_H_
#define ENESIM_VECTOR_H_


#if 0

/** @} */ //End of Enesim_Raster_Group

/**
 * @defgroup Enesim_Vector_Group Vector
 * @{
 * @defgroup Enesim_Component_Group Component
 * @{
 */
typedef struct _Enesim_Component Enesim_Component; /**< Component Handler */
typedef struct _Enesim_Component_Reader Enesim_Component_Reader; /**< Reader Handler */
EAPI void enesim_component_delete(Enesim_Component *c);
EAPI int enesim_component_source_set(Enesim_Component *c, Enesim_Component *src);
enum
{
	ENESIM_CMD_MOVE_TO,
	ENESIM_CMD_LINE_TO,
	ENESIM_CMD_CURVE3,
	ENESIM_CMD_CURVE4,
	ENESIM_CMD_END,
	ENESIM_CMDS
};

/**
 * @defgroup Enesim_Component_Transform_Group Transform Component
 * @brief This Component applies a transformation matrix.
 * Reads vertex values from another component that is linked with the XXX
 * function and applies a transformation matrix to them.
 * @{
 */
EAPI Enesim_Component * enesim_transform_new(void);
EAPI void enesim_transform_matrix_set(Enesim_Component *c, float *matrix);
/** @} //End of Enesim_Component_Transform_Group */
/**
 * @defgroup Enesim_Component_Csv_Source_Group CSV Source Component
 * @brief This Component reads data from a csv (Comma Separated Values) file.
 * @{
 */
EAPI Enesim_Component * enesim_source_csv_new(void);
EAPI int enesim_source_csv_file_set(Enesim_Component *c, const char *path);
/** @} //End of Enesim_Component_Csv_Source_Group */

/**
 * @defgroup Enesim_Component_Path_Group Path Source Component
 * @{
 */
EAPI Enesim_Component * enesim_path_new(int num_vertices);
EAPI void enesim_path_move_to(Enesim_Component *p, int x, int y);
EAPI void enesim_path_line_to(Enesim_Component *p, int x, int y);
EAPI void enesim_path_curve3(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3);
EAPI void enesim_path_curve4(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3, float x4, float y4);
EAPI void enesim_path_close(Enesim_Component *p);
/** @} //End of Enesim_Component_Path_Group */
/** @} //End of Enesim_Component_Group */
/** 
 * @defgroup Enesim_Component_Reader_Group Reader
 * @{
 */
EAPI Enesim_Component_Reader * enesim_reader_new(Enesim_Component *c);
EAPI void enesim_reader_rewind(Enesim_Component_Reader *r);
EAPI int enesim_reader_vertex_get(Enesim_Component_Reader *r, float *x, float *y);
/** @} */ //End of Enesim_Component_Reader_Group
/** @} */ //End of Enesim_Vector_Group
/** @} */ //End of Enesim_Group

#endif

#endif /*ENESIM_VECTOR_H_*/

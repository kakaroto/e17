/******************************************************************************
**  Copyright (c) 2006-2010, Calaos. All Rights Reserved.
**
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  This is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/

#ifndef PHYSIC_OBJECT_H
#define PHYSIC_OBJECT_H

#include "common.h"
#include <chipmunk/chipmunk.h>

// struct _PhysicObject;
// typedef struct _Constraint
// {
//         struct _PhysicObject *obj1;
//         struct _PhysicObject *obj2;
// 
//         DrawPoint pivotjoint;
// 
//         cpConstraint *constraint;
// } Constraint;

typedef struct _PhysicObject
{
        cpBody *body; //chipmunk body
        Eina_List *shapes; //list of all chipmunk shapes

        Evas_Object *obj; //associated evas object

        Eina_Bool static_body; //true if it's a static body (don't move with physics)

        /* Joints are stored here */
        Eina_List pivot_joints;

        //list containing evas rectangles for drawing bounding boxes for
        //all shapes of the body
        Eina_List *bouding_box;
        Eina_Bool show_bbox;

        //Callbacks func
        void (*move_callback)(struct _PhysicObject *obj, DrawPoint new_pos, DrawPoint center, double rot);
        void *move_data;

        void (*collision_callback)(struct _PhysicObject *object1, struct _PhysicObject *object2, void *data);
        void *collision_data;
        struct _PhysicObject *collision_pobj;

} PhysicObject;

typedef void (*Physic_Move_Cb)(PhysicObject *obj, DrawPoint new_pos, DrawPoint center, double rot);
typedef void (*Physic_Collision_Cb)(PhysicObject *object1, PhysicObject *object2, void *data);

void            physic_init(void);
void            physic_uninit(void);

PhysicObject   *physic_object_drawing_add(Evas_Object *obj, DrawPoint center, Eina_List *path, Eina_Bool static_body);
PhysicObject   *physic_object_circle_add(Evas_Object *obj, DrawPoint center, double radius, Eina_Bool static_body);
PhysicObject   *physic_object_segment_add(Evas_Object *obj, DrawPoint center, Eina_List *path, double thickness, Eina_Bool static_body);

void            physic_object_move_callback_set(PhysicObject *pobj, Physic_Move_Cb cb, void *data);
void            physic_object_move_callback_unset(PhysicObject *pobj);

void            physic_object_collision_callback_set(PhysicObject *pobj, PhysicObject *pobj2, Physic_Collision_Cb cb, void *data);
void            physic_object_collision_callback_unset(PhysicObject *pobj);

// void            physic_object_constraint_joint_pivot_add(PhysicObject *pobj1, PhysicObject *pobj2, DrawPoint point);
// void            physic_object_joint_pivot_del(PhysicObject *pobj1, PhysicObject *pobj2, DrawPoint point);

void            physic_object_bounding_box_show(PhysicObject *pobj);
void            physic_object_bounding_box_hide(PhysicObject *pobj);

void            physic_object_del(PhysicObject *pobj);

void            physic_simulation_clear();

void            physic_simulation_step_set(double step);
double          physic_simulation_step_get(void);

void            physic_simulation_play_set(Eina_Bool play);
Eina_Bool       physic_simulation_play_get(void);

#endif

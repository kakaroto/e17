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
#include "physic_object.h"

#define CHECK_INITSPACE                 if (!space) { ERR("Physic is not initialised !"); return; }
#define CHECK_INITSPACE_RET(ret)        if (!space) { ERR("Physic is not initialised !"); return ret; }

static cpSpace *space = NULL;
static Eina_List *physic_objects = NULL; //list of physic object
static Ecore_Timer *timer = NULL;
static Eina_Bool play_status = EINA_FALSE;
static double simulation_step = 1.0 / 60.0;

static Eina_Bool _main_tick(void *data);
static void _physic_simulation_step(double step);
static int _collision_begin_cb(cpArbiter *arb, cpSpace *space, void *unused);

/**
 * @brief init physic
 */
void physic_init(void)
{
        cpInitChipmunk();

        space = cpSpaceNew();
        space->gravity = cpv(0, -1000);
        space->elasticIterations = 10;
}

/**
 * @brief uninit physic
 */
void physic_uninit(void)
{
        CHECK_INITSPACE;

        physic_simulation_play_set(EINA_FALSE);

        PhysicObject *pobj;
        Eina_List *l, *lnext;
        EINA_LIST_FOREACH_SAFE(physic_objects, l, lnext, pobj)
                physic_object_del(pobj);

        cpSpaceFree(space);
        space = NULL;
}

/**
 * @brief add a physic object with a list of point for creating the collision shapes
 * @param obj The evas object that should react to physics
 * @param center Center of gravity of object
 * @param path List of DrawPoint that correspond to the drawn path
 * @param static_body if the object is static (they don't move), it should be EINA_TRUE
 * @return The created physic object
 */
PhysicObject *physic_object_drawing_add(Evas_Object *obj, DrawPoint center, Eina_List *path, Eina_Bool static_body)
{
        CHECK_INITSPACE_RET(NULL);
        if (!path || eina_list_count(path) <= 0)
        {
                ERR("No points given for object.");
                return NULL;
        }
        if (!obj)
        {
                ERR("No evas object given");
                return NULL;
        }

        //Get canvas size. We need that to calculate chipmunk coordinates.
        //Chipmunk coordinates starts at bottom/left instead of evas that starts at top/left.
        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(obj)), NULL, NULL, &bg_width, &bg_height);

        PhysicObject *pobj = calloc(1, sizeof(PhysicObject));

        pobj->obj =  obj;
        pobj->static_body = static_body;
        pobj->show_bbox = EINA_FALSE;

        double mass = 4.0f; //default mass for a shape
        cpVect center_of_gravity = cpv(center.x, bg_height - center.y);

        //calculate moment of inertia and mass of object
        cpFloat moment_of_inertia = 0.0;
        cpFloat object_mass = 0.0;

        Eina_List *l;
        DrawPoint *point;
        if (!pobj->static_body)
        {
                EINA_LIST_FOREACH(path, l, point)
                {
                        DrawPoint p = { point->x, point->y };
                        p.y = bg_height - p.y;

                        p.x = p.x - center_of_gravity.x;
                        p.y = p.y - center_of_gravity.y;

                        object_mass += mass;

                        //TOFIX: Don't use static value for shape radius !
                        moment_of_inertia += cpMomentForCircle(mass, 0.0f, 4.0f, cpv(p.x, p.y));
                }
        }

        //create body
        if (!pobj->static_body)
        {
                INF("Creating body with mass: %g and moment of inertia: %g", object_mass, moment_of_inertia);

                pobj->body = cpBodyNew(object_mass, moment_of_inertia);
                pobj->body->p = center_of_gravity;
        //         pobj->body->w = 1.0f;
                cpSpaceAddBody(space, pobj->body);
        }
        else
        {
                INF("Creating static body");

                pobj->body = cpBodyNew(INFINITY, INFINITY);
                pobj->body->p = center_of_gravity;
        }

        //the body is created, now add all shapes to it
        EINA_LIST_FOREACH(path, l, point)
        {
                DrawPoint p = { point->x, point->y };
                p.y = bg_height - p.y;

                p.x = p.x - center_of_gravity.x;
                p.y = p.y - center_of_gravity.y;

                //TOFIX: Don't use static value for shape radius !
                cpShape *shape;
                if (pobj->static_body)
                        shape = cpSpaceAddStaticShape(space, cpCircleShapeNew(pobj->body, 4.0f, cpv(p.x, p.y)));
                else
                        shape = cpSpaceAddShape(space, cpCircleShapeNew(pobj->body, 4.0f, cpv(p.x, p.y)));
                shape->e = 0.6f; //Elasticity
                shape->u = 0.6f; //Friction
//                shape->group = number_of_element;
                shape->data = pobj;
                shape->collision_type = (int)pobj;

                pobj->shapes = eina_list_append(pobj->shapes, shape);
        }

        physic_objects = eina_list_append(physic_objects, pobj);

        return pobj;
}

/**
 * @brief add a circle physic object
 * @param obj The evas object that should react to physics
 * @param center Center of gravity of object
 * @param radius Radius of the circle
 * @param static_body if the object is static (they don't move), it should be EINA_TRUE
 * @return The created physic object
 */
PhysicObject *physic_object_circle_add(Evas_Object *obj, DrawPoint center, double radius, Eina_Bool static_body)
{
        CHECK_INITSPACE_RET(NULL);
        if (radius <= 0)
        {
                ERR("radius not valid (%g).", radius);
                return NULL;
        }
        if (!obj)
        {
                ERR("No evas object given");
                return NULL;
        }

        //Get canvas size. We need that to calculate chipmunk coordinates.
        //Chipmunk coordinates starts at bottom/left instead of evas that starts at top/left.
        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(obj)), NULL, NULL, &bg_width, &bg_height);

        PhysicObject *pobj = calloc(1, sizeof(PhysicObject));

        pobj->obj =  obj;
        pobj->static_body = static_body;
        pobj->show_bbox = EINA_FALSE;

        cpVect center_of_gravity = cpv(center.x, bg_height - center.y);

        //create body
        if (!pobj->static_body)
        {
                //calculate moment of inertia and mass of object
                cpFloat object_mass = 5.0f * radius;
                cpFloat moment_of_inertia = cpMomentForCircle(1.0, 0.0f, radius, center_of_gravity);;

                INF("Creating body with mass: %g and moment of inertia: %g", object_mass, moment_of_inertia);

                pobj->body = cpBodyNew(object_mass, moment_of_inertia);
                pobj->body->p = center_of_gravity;
                //pobj->body->w = 1.0f;
                cpSpaceAddBody(space, pobj->body);
        }
        else
        {
                INF("Creating static body");

                pobj->body = cpBodyNew(INFINITY, INFINITY);
                pobj->body->p = center_of_gravity;
        }

        cpShape *shape;
        if (pobj->static_body)
                shape = cpSpaceAddStaticShape(space, cpCircleShapeNew(pobj->body, radius, cpvzero));
        else
                shape = cpSpaceAddShape(space, cpCircleShapeNew(pobj->body, radius, cpvzero));
        shape->e = 0.6f; //Elasticity
        shape->u = 0.6f; //Friction
//        shape->group = number_of_element;
        shape->data = pobj;
        shape->collision_type = (int)pobj;

        pobj->shapes = eina_list_append(pobj->shapes, shape);

        physic_objects = eina_list_append(physic_objects, pobj);

        return pobj;
}


/**
 * @brief add a physic object with a list of point for creating the collision shapes
 * @param obj The evas object that should react to physics
 * @param center Center of gravity of object
 * @param path List of DrawPoint that correspond to the segments path
 * @param thickness thickness of segments
 * @param static_body if the object is static (they don't move), it should be EINA_TRUE
 * @return The created physic object
 * @note Warning! for now chipmunk does not do collision detection for 2 segments...
 */
PhysicObject *physic_object_segment_add(Evas_Object *obj, DrawPoint center, Eina_List *path, double thickness, Eina_Bool static_body)
{
        CHECK_INITSPACE_RET(NULL);
        if (!path || eina_list_count(path) <= 0)
        {
                ERR("No points given for object.");
                return NULL;
        }
        if (!obj)
        {
                ERR("No evas object given");
                return NULL;
        }

        DrawPoint *first_point = (DrawPoint *)eina_list_data_get(path);

        //Get canvas size. We need that to calculate chipmunk coordinates.
        //Chipmunk coordinates starts at bottom/left instead of evas that starts at top/left.
        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(obj)), NULL, NULL, &bg_width, &bg_height);

        first_point->y = bg_height - first_point->y;

        PhysicObject *pobj = calloc(1, sizeof(PhysicObject));

        pobj->obj =  obj;
        pobj->static_body = static_body;
        pobj->show_bbox = EINA_FALSE;

        double mass = 1.0f; //default mass for a shape
        cpVect center_of_gravity = cpv(center.x, bg_height - center.y);

        //calculate moment of inertia and mass of object
        cpFloat moment_of_inertia = 0.0;
        cpFloat object_mass = 0.0;

        cpVect begin = cpv(first_point->x - center_of_gravity.x, first_point->y - center_of_gravity.y);
        cpVect end = cpvzero;

        Eina_List *l;
        DrawPoint *point;

        if (!pobj->static_body)
        {
                EINA_LIST_FOREACH(path, l, point)
                {
                        if (point == first_point) continue;

                        DrawPoint p = { point->x, point->y };

                        p.y = bg_height - p.y;

                        end.x = p.x - center_of_gravity.x;
                        end.y = p.y - center_of_gravity.y;

                        object_mass += mass;

                        moment_of_inertia += cpMomentForSegment(mass, begin, end);

                        begin = end;
                }
        }

        //moment_of_inertia /= (cpFloat)eina_list_count(path);

        //create body
        if (!pobj->static_body)
        {
                INF("Creating body with mass: %g and moment of inertia: %g", object_mass, moment_of_inertia);

                pobj->body = cpBodyNew(object_mass, moment_of_inertia);
                pobj->body->p = center_of_gravity;
                //pobj->body->w = 1.0f;
                cpSpaceAddBody(space, pobj->body);
        }
        else
        {
                INF("Creating static body");

                pobj->body = cpBodyNew(INFINITY, INFINITY);
                pobj->body->p = center_of_gravity;
        }

        //the body is created, now add all shapes to it
        begin = cpv(first_point->x - center_of_gravity.x, first_point->y - center_of_gravity.y);
        end = cpvzero;
        EINA_LIST_FOREACH(path, l, point)
        {
                if (point == first_point) continue;

                DrawPoint p = { point->x, point->y };

                p.y = bg_height - p.y;

                end.x = p.x - center_of_gravity.x;
                end.y = p.y - center_of_gravity.y;

                //TOFIX: Don't use static value for shape radius !
                cpShape *shape;
                if (pobj->static_body)
                        shape = cpSpaceAddStaticShape(space, cpSegmentShapeNew(pobj->body, begin, end, thickness));
                else
                        shape = cpSpaceAddShape(space, cpSegmentShapeNew(pobj->body, begin, end, thickness));
                shape->e = 0.8f; //Elasticity
                shape->u = 0.8f; //Friction
//                shape->group = number_of_element;
                shape->data = pobj;
                shape->collision_type = (int)pobj;

                pobj->shapes = eina_list_append(pobj->shapes, shape);

                begin = end;
        }

        physic_objects = eina_list_append(physic_objects, pobj);

        return pobj;
}

/**
 * @brief set the callback when position/roatino changes for the object
 * @param pobj The Physic object
 * @param cb The callback function
 * @param data The data pointer to be passed to @a cb
 */
void physic_object_move_callback_set(PhysicObject *pobj, Physic_Move_Cb cb, void *data)
{
        CHECK_INITSPACE;
        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        pobj->move_callback = cb;
        pobj->move_data = data;
}

/**
 * @brief Unset the callback
 * @param pobj The Physic object
 */
void physic_object_move_callback_unset(PhysicObject *pobj)
{
        CHECK_INITSPACE;

        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        pobj->move_callback = NULL;
        pobj->move_data = NULL;
}

/**
 * @brief set the callback when collision occured with object
 * @param pobj The Physic object
 * @param cb The callback function
 * @param data The data pointer to be passed to @a cb
 */
void physic_object_collision_callback_set(PhysicObject *pobj, PhysicObject *pobj2, Physic_Collision_Cb cb, void *data)
{
        CHECK_INITSPACE;
        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        cpSpaceAddCollisionHandler(space, (int)pobj, (int)pobj2, _collision_begin_cb, NULL, NULL, NULL, pobj);

        pobj->collision_callback = cb;
        pobj->collision_data = data;
        pobj->collision_pobj = pobj2;
}

/**
 * @brief Unset the callback
 * @param pobj The Physic object
 */
void physic_object_collision_callback_unset(PhysicObject *pobj)
{
        CHECK_INITSPACE;

        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        pobj->collision_callback = NULL;
        pobj->collision_data = NULL;
}

/**
 * @brief Show all bounding box for a physic object
 * @param pobj The Physic object
 */
void physic_object_bounding_box_show(PhysicObject *pobj)
{
        CHECK_INITSPACE;

        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        pobj->show_bbox = EINA_TRUE;
}

/**
 * @brief Hide all bounding box for a physic object
 * @param pobj The Physic object
 */
void physic_object_bounding_box_hide(PhysicObject *pobj)
{
        CHECK_INITSPACE;

        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        pobj->show_bbox = EINA_FALSE;

        Evas_Object *o;
        EINA_LIST_FREE(pobj->bouding_box, o)
                evas_object_del(o);
}

/**
 * @brief Delete and free a physic object
 * @param pobj The Physic object
 * @note The function will also delete the evas object
 */
void physic_object_del(PhysicObject *pobj)
{
        CHECK_INITSPACE;

        if (!pobj)
        {
                ERR("pobj is NULL !");
                return;
        }

        evas_object_del(pobj->obj);

        cpShape *shape;
        EINA_LIST_FREE(pobj->shapes, shape)
        {
                if (pobj->static_body)
                        cpSpaceRemoveStaticShape(space, shape);
                else
                        cpSpaceRemoveShape(space, shape);

                cpShapeFree(shape);
        }

        Evas_Object *obj;
        EINA_LIST_FREE(pobj->bouding_box, obj)
                evas_object_del(obj);

        //remove body
        cpSpaceRemoveBody(space, pobj->body);
        cpBodyFree(pobj->body);

        physic_objects = eina_list_remove(physic_objects, pobj);

        free(pobj);
}

/**
 * @brief removes all objects from the simulation
 */
void physic_simulation_clear()
{
        PhysicObject *pobj;
        Eina_List *l, *lnext;
        EINA_LIST_FOREACH_SAFE(physic_objects, l, lnext, pobj)
                physic_object_del(pobj);
}

/**
 * @brief Set the time step for simulation
 * @param step The time step
 */
void physic_simulation_step_set(double step)
{
        simulation_step = step;
}

/**
 * @brief Get the time step for simulation
 * @return The time step
 */
double physic_simulation_step_get(void)
{
        return simulation_step;
}

/**
 * @brief Set the status of the simulation
 * @param play EINA_TRUE if simulation should go on
 */
void physic_simulation_play_set(Eina_Bool play)
{
        if (play == play_status)
                return;

        play_status = play;

        if (play_status)
        {
                timer = ecore_timer_add(simulation_step, _main_tick, NULL);
                INF("Simulation is running...");
        }
        else
        {
                if (timer)
                {
                        ecore_timer_del(timer);
                        timer = NULL;
                }

                INF("Simulation is stopped.");
        }
}

/**
 * @brief Get the play status of the simulation
 * @return EINA_TRUE if playing
 */
Eina_Bool physic_simulation_play_get(void)
{
        return play_status;
}


/* Private ---------------- */

static Eina_Bool _main_tick(void *data)
{
        _physic_simulation_step(simulation_step);

        return EINA_TRUE;
}

static void _physic_simulation_step(double step)
{
        cpSpaceStep(space, step);

        //Get canvas size. We need that to calculate chipmunk coordinates.
        //Chipmunk coordinates starts at bottom/left instead of evas that starts at top/left.
        int bg_width = 0, bg_height = 0;

        Eina_List *l;
        PhysicObject *pobj;
        EINA_LIST_FOREACH(physic_objects, l, pobj)
        {
                int x, y;
                double rot;

                cpShape *shape = eina_list_data_get(pobj->shapes);
                if (!shape)
                {
                        WARN("No shape for physical object %p", pobj);
                        continue;
                }
                if (!pobj->obj)
                {
                        WARN("No evas object for physical object %p", pobj);
                        continue;
                }

                ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(pobj->obj)), NULL, NULL, &bg_width, &bg_height);

                //Bounding boxes
                if (pobj->show_bbox)
                {
                        if (!pobj->bouding_box)
                        {
                                Eina_List *ll;
                                cpShape *s;
                                EINA_LIST_FOREACH(pobj->shapes, ll, s)
                                {
                                        Evas_Object *o = evas_object_rectangle_add(evas_object_evas_get(pobj->obj));
                                        evas_object_color_set(o, 0, 0, 0, 110);
                                        evas_object_move(o, 100, 100);
                                        evas_object_resize(o, 200, 200);
                                        evas_object_show(o);

                                        pobj->bouding_box = eina_list_append(pobj->bouding_box, o);
                                }
                        }

                        Eina_List *ll, *ll2;
                        cpShape *s;
                        Evas_Object *o;
                        for (ll = pobj->shapes, s = eina_list_data_get(ll),
                             ll2 = pobj->bouding_box, o = eina_list_data_get(ll2);
                             ll && ll2;
                             ll = eina_list_next(ll), s = eina_list_data_get(ll),
                             ll2 = eina_list_next(ll2), o = eina_list_data_get(ll2))
                        {
                                evas_object_move(o, s->bb.l, bg_height - s->bb.t);
                                evas_object_resize(o, s->bb.r - s->bb.l, s->bb.t - s->bb.b);
                        }
                }

                //Don't move static bodies
                if (pobj->static_body) continue;

                int w, h;
                evas_object_geometry_get(pobj->obj, NULL, NULL, &w, &h);

                x = pobj->body->p.x - w / 2;
                y = (bg_height - pobj->body->p.y) - h / 2;
                rot = -(pobj->body->a * 180.0) / PI;

                DrawPoint new_pos = { x, y };
                DrawPoint center = { pobj->body->p.x, bg_height - pobj->body->p.y };

                if (pobj->move_callback)
                        pobj->move_callback(pobj, new_pos, center, rot);
        }
}

static int _collision_begin_cb(cpArbiter *arb, cpSpace *space, void *data)
{
        PhysicObject *pobj = (PhysicObject *)data;

        //Check if values are ok
        CP_ARBITER_GET_SHAPES(arb, a, b);

        if ((pobj->body == a->body && pobj->collision_pobj->body == b->body) ||
            (pobj->body == b->body && pobj->collision_pobj->body == a->body))
        {
                if (pobj->collision_callback)
                        pobj->collision_callback(pobj, pobj->collision_pobj, pobj->collision_data);
        }
        else
        {
                WARN("Collision detected but body does not match !");
        }

        return 1;
}



#include "E_Phys.h"

E_Phys_Particle *e_phys_particle_map_insert(E_Phys_Particle *head, E_Phys_Particle *p)
{
        int hd, vd;
        E_Phys_Particle *newhead;
        E_Phys_Particle *walk = NULL;

        newhead = head;

        /*
         * Decide if this is more appropriate as a vertical or horizontal
         * neighbor.
         */
        while (head != walk) {
                int coff;
                float *coord;
                E_Phys_Particle_Direction walkd, neighbord;

                walk = head;

                hd = p->cur.x - walk->cur.x;
                vd = p->cur.y - walk->cur.y;

		if (abs(vd) < abs(hd)) {
                        coord = &p->cur.y;
                        coff = coord - ((float *)p);

                        /* Place above head particle */
                        if (vd < 0) {
                                walkd = E_PHYS_PARTICLE_DIRECTION_ABOVE;
                                neighbord = E_PHYS_PARTICLE_DIRECTION_BELOW;
                        }
                        /* Place below head particle */
                        else {
                                walkd = E_PHYS_PARTICLE_DIRECTION_BELOW;
                                neighbord = E_PHYS_PARTICLE_DIRECTION_ABOVE;
                        }
                }
                else {
                        coord = &p->cur.x;
                        coff = coord - ((float *)p);

                        /* Place left of head particle */
                        if (hd < 0) {
                                walkd = E_PHYS_PARTICLE_DIRECTION_LEFT;
                                neighbord = E_PHYS_PARTICLE_DIRECTION_RIGHT;
                        }
                        /* Place right of head particle */
                        else {
                                walkd = E_PHYS_PARTICLE_DIRECTION_RIGHT;
                                neighbord = E_PHYS_PARTICLE_DIRECTION_LEFT;
                        }
                }

                /*
                 * Determine if we're at an insertion point or if we have to
                 * walk the map further.
                 */
                if (walk->neighbor[walkd]) {
                        float *ncoord;

                        ncoord = ((float *)walk->neighbor[walkd]) + coff;
                        if ((walkd < neighbord && (*coord < *ncoord)) ||
                            (walkd > neighbord && (*coord > *ncoord))) {
                                head = walk->neighbor[walkd];
                                head->steps++;
                                if (head->steps > newhead->steps)
                                        newhead = head;
                        }
                        else
                                walk->neighbor[walkd]->neighbor[neighbord] = p;
                }

                if (head == walk) {
                        p->neighbor[walkd] = walk->neighbor[walkd];
                        p->neighbor[neighbord] = walk;
                        walk->neighbor[walkd] = p;
                }
                else {
                        // printf("Walking direction %d\n", walkd);
                }
        }

        return newhead;
}

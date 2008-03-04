cdef extern from "Ecore_Data.h":

    ctypedef struct Ecore_List

    # Retrieve the current position in the list
    void *ecore_list_current(Ecore_List * list)
    void *ecore_list_first(Ecore_List * list)
    void *ecore_list_last(Ecore_List * list)
    int   ecore_list_index(Ecore_List * list)
    int   ecore_list_count(Ecore_List * list)

    # Traversing the list
    void *ecore_list_first_goto(Ecore_List * list)
    void *ecore_list_last_goto(Ecore_List * list)
    void *ecore_list_index_goto(Ecore_List * list, int index)
    void *ecore_list_goto(Ecore_List * list, void *_data)

    # Traversing the list and returning data
    void *ecore_list_next(Ecore_List * list)

    # Free the list and it's contents
    void ecore_list_destroy(Ecore_List *lst)

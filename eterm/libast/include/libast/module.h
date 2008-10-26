/*
 * Copyright (C) 1997-2004, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _LIBAST_MODULE_H_
#define _LIBAST_MODULE_H_

#define SPIF_MODULE(obj)                    ((spif_module_t) (obj))
#define SPIF_MODULE_CLASS(o)                ((spif_moduleclass_t) SPIF_OBJ_CLASS(o))
#define SPIF_OBJ_IS_MODULE(o)               (SPIF_OBJ_IS_TYPE(o, module))
#define SPIF_MODULE_ISNULL(s)               SPIF_OBJ_ISNULL(SPIF_OBJ(s))
#define SPIF_MODULECLASS_VAR(type)          spif_ ## type ## _moduleclass
#define SPIF_MODULE_CALL_METHOD(o, meth)    SPIF_MODULE_CLASS(o)->meth

#define SPIF_MODULE_NEW(type)               SPIF_MODULE((SPIF_CLASS(SPIF_CLASS_VAR(type)))->noo())
#define SPIF_MODULE_INIT(obj)               SPIF_OBJ_INIT(obj)
#define SPIF_MODULE_DONE(obj)               SPIF_OBJ_DONE(obj)
#define SPIF_MODULE_DEL(obj)                SPIF_OBJ_DEL(obj)
#define SPIF_MODULE_SHOW(obj, b, i)         SPIF_OBJ_SHOW(obj, b, i)
#define SPIF_MODULE_COMP(o1, o2)            SPIF_OBJ_COMP(o1, o2)
#define SPIF_MODULE_DUP(obj)                SPIF_OBJ_DUP(obj)
#define SPIF_MODULE_TYPE(obj)               SPIF_OBJ_TYPE(obj)

typedef spif_ptr_t (spif_getsym_func_t)(spif_charptr_t);

SPIF_DECL_OBJ(module) {
    SPIF_DECL_PARENT_TYPE(obj);
    SPIF_DECL_PROPERTY(str, name);
    SPIF_DECL_PROPERTY(str, path);
    SPIF_DECL_PROPERTY(ptr, module_handle);
    SPIF_DECL_PROPERTY(ptr, main_handle);
};

SPIF_DECL_OBJ(moduleclass) {
    SPIF_DECL_PARENT_TYPE(class);

    spif_func_t call;
    spif_func_t getsym;
    spif_func_t load;
    spif_func_t run;
    spif_func_t unload;
};

extern spif_class_t SPIF_CLASS_VAR(module);
extern spif_moduleclass_t SPIF_MODULECLASS_VAR(module);
extern spif_module_t spif_module_new(void);
extern spif_bool_t spif_module_del(spif_module_t);
extern spif_bool_t spif_module_init(spif_module_t);
extern spif_bool_t spif_module_done(spif_module_t);
extern spif_str_t spif_module_show(spif_module_t, spif_charptr_t, spif_str_t, size_t);
extern spif_cmp_t spif_module_comp(spif_module_t, spif_module_t);
extern spif_module_t spif_module_dup(spif_module_t);
extern spif_classname_t spif_module_type(spif_module_t);
extern spif_ptr_t spif_module_call(spif_module_t self, spif_charptr_t fname, spif_ptr_t data);
extern spif_ptr_t spif_module_getsym(spif_module_t self, spif_charptr_t sym);
extern spif_bool_t spif_module_load(spif_module_t self);
extern spif_bool_t spif_module_run(spif_module_t self);
extern spif_bool_t spif_module_unload(spif_module_t self);
SPIF_DECL_PROPERTY_FUNC(module, str, name);
SPIF_DECL_PROPERTY_FUNC(module, str, path);
SPIF_DECL_PROPERTY_FUNC(module, ptr, module_handle);
SPIF_DECL_PROPERTY_FUNC(module, ptr, main_handle);

#endif /* _LIBAST_MODULE_H_ */

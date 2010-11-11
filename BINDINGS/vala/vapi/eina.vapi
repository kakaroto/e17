/*
 * Copyright (C) 2009-2010 Michael 'Mickey' Lauer <mlauer@vanille-media.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */
[CCode (cprefix = "Eina_", lower_case_cprefix = "eina_", cheader_filename = "Eina.h")]
namespace Eina
{
    public bool init();
    public bool shutdown();

    [CCode (cname = "Eina_Compare_Cb", has_target = false)]
    public delegate int CompareCb(void* data1, void* data2);

    [CCode (cname = "Eina_Free_Cb", instance_pos = 0, has_target = false)]
    public delegate void FreeCb(void* data);

    public delegate bool Each(void* container, void* data);

    [CCode (cprefix = "EINA_SORT_")]
    public enum Sort
    {
        MIN,
        MAX
    }

    //=======================================================================
    [CCode (cname = "EINA_VERSION_MAJOR")]
    const uint major_version;
    [CCode (cname = "EINA_VERSION_MINOR")]
    const uint minor_version;

    [SimpleType]
    [CCode (cname = "Eina_Version")]
    class Version
    {
        int major;
        int minor;
        int micro;
        int revision;
    }
    Version version;

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_list_free", default_value = "NULL", copy_function = "eina_list_clone")]
    public class List<G>
    {
        public static bool init();
        public static bool shutdown();
        public void* data;
        public List<G> next;
        public List<G> prev;
        [ReturnsModifiedPointer ()]
        public void last();

        [ReturnsModifiedPointer ()]
        public void append(owned G data);
        [ReturnsModifiedPointer ()]
        public void append_relative(G data, G relative);
        [ReturnsModifiedPointer ()]
        public void append_relative_list(G data, List<G> relative);

        public uint count();

        public uint length {[CCode (cname = "eina_list_count")]get;}

        public unowned G data_find(owned G data);
        // ???
        public List<G> data_find_list(owned G data);

        public unowned G nth(uint n);
        public List<G> nth_list(int n);

        [CCode (cname = "eina_list_nth")]
        public unowned G get(uint n);

        public bool contains (G item)
        {
            return this.data_find( item ) != null;
        }

        [ReturnsModifiedPointer ()]
        public void prepend(owned G data);
        [ReturnsModifiedPointer ()]
        public void prepend_relative(G data, G relative);
        [ReturnsModifiedPointer ()]
        public void prepend_relative_list(G data, List<G> relative);

        [ReturnsModifiedPointer ()]
        public void remove(owned G data);
        [ReturnsModifiedPointer ()]
        public void remove_list(List<G> data);

        [ReturnsModifiedPointer ()]
        public void promote_list(List<G> move_list);
        [ReturnsModifiedPointer ()]
        public void demote_list(List<G> move_list);

        [ReturnsModifiedPointer ()]
        public void reverse();
        [ReturnsModifiedPointer ()]
        public void sort(uint size, CompareCb func);

        [ReturnsModifiedPointer ()]
        public void merge(List<G> right);
        [ReturnsModifiedPointer ()]
        public void sorted_merge(List<G> right, CompareCb func);

        [ReturnsModifiedPointer ()]
        public void search_sorted(CompareCb cb, G data);
        [ReturnsModifiedPointer ()]
        public void seach_sorted_near_list(CompareCb func, G data);
        [ReturnsModifiedPointer ()]
        public void search_unsorted(CompareCb cb, G data);

        public Eina.List<G> clone();
        public Eina.List<G> reverse_clone();

        public Eina.List<G> split_list(Eina.List<G> relative, out Eina.List<G> right);

        public Eina.Iterator<G> iterator_new();
        public Eina.Iterator<G> iterator_reversed_new();
        public Eina.Accessor<G> accessor_new();

    }

    //=======================================================================
    [Compact]
    public class InList<G>
    {
        public InList next;
        public InList prev;
        public InList last;
        public uint count();
        [ReturnsModifiedPointer ()]
        public void append(G data);
        [ReturnsModifiedPointer ()]
        public void remove(owned G data);
        [ReturnsModifiedPointer ()]
        public void remove_list(List<G> data);

        [ReturnsModifiedPointer ()]
        public void promote_list(List<G> move_list);
        [ReturnsModifiedPointer ()]
        public void demote_list(List<G> move_list);

        [ReturnsModifiedPointer ()]
        public void reverse();
        [ReturnsModifiedPointer ()]
        public void sort(uint size, CompareCb func);

        [ReturnsModifiedPointer ()]
        public void merge(List<G> right);
        [ReturnsModifiedPointer ()]
        public void sorted_merge(List<G> right, CompareCb func);

        [ReturnsModifiedPointer ()]
        public void search_sorted(CompareCb cb, G data);
        [ReturnsModifiedPointer ()]
        public void seach_sorted_near_list(CompareCb func, G data);
        [ReturnsModifiedPointer ()]
        public void search_unsorted(CompareCb cb, G data);

        public Eina.List<G> clone();
        public Eina.List<G> reverse_clone();

        public Eina.Iterator<G> iterator_new();
        public Eina.Accessor<G> accessor_new();

    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_iterator_free", simple_generics = true)]
    public class Iterator<G>
    {
        [CCode (cname = "Eina_Iterator_Next_Callback")]
        public delegate bool NextCallback(ref G data);

        [CCode (cname = "Eina_Iterator_Get_Container_Callback")]
        public delegate G GetContainerCallback();

        [CCode (cname = "Eina_Iterator_Free_Callback")]
        public delegate void FreeCallback();

        [CCode (cname = "Eina_Iterator_Lock_Callback")]
        public delegate bool LockCallback();

        public void foreach(Each callback);
        //[CCode (cname = "eina_iterator_container_get")]
        public unowned void* container_get();
        [CCode (cname="eina_iterator_next")]
        public bool get_next(out unowned G next);
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_accessor_free")]
    public class Accessor<G>
    {
        [CCode (cname = "Eina_Accessor_Get_At_Callback")]
        public delegate bool GetAtCallback(uint index, ref G data);

        [CCode (cname = "Eina_Accessor_Get_Container_Callback")]
        public delegate G GetContainerCallback();

        [CCode (cname = "Eina_Accessor_Free_Callback")]
        public delegate void FreeCallback();

        [CCode (cname = "Eina_Accessor_Lock_Callback")]
        public delegate bool LockCallback();

        public bool data_get(uint index, ref G data);
        public G container_get();
        public void over(Each cb, uint start, uint end);
        public bool lock();
        public bool unlock();
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_array_free")]
    public class Array<G>
    {
        public delegate bool Keep(G element);
        public static bool init();
        public static bool shutdown();
        public G* data;
        public uint total;
        public uint count;
        public uint step;

        public Array(uint step = 16);
        public void clean();
        public void flush();
        //public bool remove(Keep k);
        public bool foreach(Each e);

        public bool push(owned G data);
        public unowned G pop();
        public void data_set(G data);
        public G data_get(uint index);
        [CCode (cname = "eina_array_data_get")]
        public G get(uint index);
        [CCode (cname = "eina_array_data_set")]
        public void set(uint index, G data);
        public uint count_get();
        public uint length {[CCode (cname = "eina_array_count_get")]get;}
        public Eina.Iterator<G> iterator_new();
        public Eina.Accessor<G> accessor_new();
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_benchmark_free")]
    public class Benchmark
    {
        public static bool init();
        public static bool shutdown();
        [CCode (has_target = false)]
        public delegate void Specimens (int request);
        public Benchmark(string name, string run);
        public bool register(string name, Specimens bench_cb, int count_start, int count_end, int count_set);
        public unowned Eina.Array<string> run();
    }

    namespace Convert
    {
        [CCode (cname = "EINA_ERROR_CONVERT_P_NOT_FOUND")]
        public const Eina.Error P_NOT_FOUND;
        [CCode (cname = "EINA_ERROR_CONVERT_0X_NOT_FOUND")]
        public const Eina.Error 0X_NOT_FOUND;
        [CCode (cname = "EINA_ERROR_CONVERT_OUTRUN_STRING_LENGTH")]
        public const Eina.Error OUTRUN_STRING_LENGTH;

        public int itoa(int n, string s);
        public int xtoa(uint n, string s);
        public int dtoa(double d, string s);
        //vala doesn't support long long, use int64
        public bool atod(string src, int length, out int64 m, out int64 e);
        public int fptoa(F32p32 a, string des);
        public bool atofp(string src, int length, out F32p32 a);
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_counter_free")]
    public class Counter
    {
        [CCode (cname = "eina_counter_new")]
        public Counter(string name);
        public void start();
        public void stop();
        string dump;
    }

    //=======================================================================
    [CCode (cprefix = "Cpu_", lower_case_cprefix = "cpu_")]
    namespace Cpu
    {
        [CCode (cprefix = "EINA_CPU", has_type_id = false)]
        public enum Features
        {
            MMX,
            SSE,
            SSE2,
            SSE3,
            ALTIVEC,
            VIS,
            NEON,
        }
        public Features features_get();
        public int count();
    }

    //=======================================================================
    public struct Error : int
    {
        [CCode (cname = "eina_error_msg_register")]
        public Error(string msg);
        [CCode (cname = "eina_error_msg_static_register")]
        public Error.static(string msg);

        public static Error OUT_OF_MEMORY;

        public static Error get();
        public void set();
        [CCode (cname = "eina_error_msg_get" )]
        public unowned string? to_string();
        public static void print(ErrorLevel level, string file, string function, int line,string format, ...);
        public static void log_level_set(ErrorLevel level);
        public bool msg_modify(string msg);
    }

    //=======================================================================
    [CCode (cprefix = "EINA_ERROR_LEVEL_", has_type_id = false,cname = "Eina_Error_Level")]
    public enum ErrorLevel
    {
        ERR,
        WARN,
        INFO,
        DBG,
        LEVELS
    }

    //=======================================================================
    [SimpleType]
    public struct F16p16: int
    {
        public F16p16.int_from( int v );
        public int int_to();
        public F16p16.float_from( float v );
        public float float_to();
        public F16p16 add();
        public F16p16 sub();
        public F16p16 mul();
        public F16p16 sqrt();
        public uint fracc_get();
        public F32p32 to_f32p32();
        public F8p24 to_f8p24();
    }

    //=======================================================================
    [SimpleType]
    public struct F32p32: uint64
    {
        [CCode (cname = "eina_f32p32_int_from")]
        public F32p32.int(int32 i = 0);
        [CCode (cname = "eina_f32p32_double_from")]
        public F32p32.double(double d = 0.0);
        [CCode (cname = "eina_f32p32_int_to")]
        public int to_int();
        [CCode (cname = "eina_f32p32_double_to")]
        public double to_double();
        public F32p32 add(F32p32 b);
        public F32p32 sub(F32p32 b);
        public F32p32 mul(F32p32 b);
        public F32p32 scale(int b);
        public F32p32 div(F32p32 b);
        public F32p32 sqrt(F32p32 a);
        public int freacc_get();
        public F32p32 sin(F32p32 a);
        public F32p32 cos(F32p32 a);

        public F16p16 to_f16p16();
        public F8p24 to_f8p24();
    }

    //=======================================================================
    [SimpleType]
    public struct F8p24: int32
    {
        [CCode (cname = "eina_f8p24_int_from")]
        public F8p24.int(int i = 0);
        [CCode (cname = "eina_f8p24_float_from")]
        public F8p24.float(float f = 0);
        [CCode (cname = "eina_f8p24_int_to")]
        public int to_int();
        [CCode (cname = "eina_f8p24_float_to")]
        public float to_float();
        public F8p24 add(F8p24 b);
        public F8p24 sub(F8p24 b);
        public F8p24 mul(F8p24 b);
        public F8p24 scale(int b);
        public F8p24 div(F8p24 b);
        public F8p24 sqrt(F8p24 a);
        public int freacc_get();
        public F8p24 sin(F8p24 a);
        public F8p24 cos(F8p24 a);

        public F32p32 to_f32p32();
        public F16p16 to_f16p16();
    }

    //=======================================================================
    namespace File
    {
        [CCode (cname = "Eina_File_Dir_List_Cb")]
        public delegate void DirListCb(string name, string path);
        public static bool dir_list(string dir, bool recursive, DirListCb cb);
        public Iterator<string> ls(string dir);
        public Iterator<DirectInfo> stat_ls(string dir);
        public Iterator<DirectInfo> direct_ls(string dir);
        public Eina.Array<string> split(string path);
        [CCode (cprefix = "EINA_FILE_")]
        public enum Type
        {
            UNKNOWN,
            FIFO,
            CHR,
            DIR,
            BLK,
            REG,
            LNK,
            SOCK,
            WHT
        }
        [Compact]
        [CCode (cname = "Eina_File_Direct_Info")]
        class DirectInfo
        {
            string path;
            size_t name_start;
            size_t path_length;
            size_t name_length;
            Type type;
        }
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_hash_free", simple_generics = true)]
    public class Hash<K,V>
    {
        public static int superfast(void* key, int len);
        public static int dj2b(void* key, int len);

        [CCode (cname = "eina_hash_int32")]
        public static int int_32(uint* key, int len);

        [CCode (cname = "eina_hash_int64")]
        public static int int_64(ulong* key, int len);

        [CCode (cname = "Eina_Key_Length", has_target = false)]
        public delegate uint KeyLength(K data) ;

        [CCode (cname = "Eina_Key_Cmp", has_target = false)]
        public delegate int KeyCmp(K key1, int key1_length, K key2, int key2_length);

        [CCode (cname = "Eina_Key_Hash", has_target = false)]
        public delegate int KeyHash(K key, int key_length);

        [CCode (cname = "Eina_Hash_Foreach", has_target = false)]
        public delegate bool Foreach(Hash hash, K key, V data, void* fdata);

        public Hash(KeyLength key_length_cb, KeyCmp key_cmp_cb, KeyHash key_hash_cb, FreeCb free_cb, int buckets_pwer_size);
        public Hash.string_dj2b(FreeCb data_free_cb);

        [CCode (cname = "eina_hash_string_superfast_new", simple_generics = true)]
        public static Hash string_superfast(FreeCb? data_free_cb = null);

        [CCode (cname = "eina_hash_string_small_new")]
        public Hash.string_small(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_int32_new")]
        public Hash.int32(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_int64_new")]
        public Hash.int64(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_pointer_new")]
        public Hash.pointer(FreeCb data_free_cb);

        public Hash.stringshared(FreeCb data_free_cb);

        public bool add(K key, V data);
        [CCode (cname = "eina_hash_add")]
        public void set(K key, owned V data);
        public bool direct_add(K key, V data);
        public bool del(K key, V data);
        public V? find(K key);
        [CCode (cname = "eina_hash_find")]
        public unowned V? get(K key);
        public V? modify(K key, V data);
        public int population();
        public bool add_by_hash(K key, int key_length, int key_hash, V data);
        public bool del_by_key_hash(K key, int key_length, int key_hash);
        public bool del_by_key(K key);
        public bool del_by_data(V data);
        public bool del_by_hash(K key, int key_length, int key_hash, V data);
        public bool direct_add_by_hash(K key, int key_length, int key_hash, V data);
        public V? find_by_hash(K key, int key_length, int key_hash);
        public V? modify_by_hash(K key, int key_length, int key_hash, V data);
        public Iterator<K> iterator_key_new();
        public Iterator<V> iterator_data_new();
        public Iterator<HashTuple<K,V>> iterator_tuple_new();
        public void foreach(Foreach cb);
        public void free_buckets();
    }

    //=======================================================================
    [Compact]
    [CCode (simple_generics = true)]
    public class HashTuple<K,V>
    {
        public K key;
        public V data;
        public uint key_length;

    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_quadtree_free", simple_generics = true, lower_case_cprefix = "eina_quadtree_", cname = "Eina_QuadTree")]
    public class QuadTree<V>
    {
        [CCode (cprefix = "EINA_QUAD_", cname = "Eina_Quad_Direction")]
        public enum Direction
        {
            LEFT,
            RIGHT,
            BOTH
        }
        public delegate Direction Callback(V object, size_t middle);

        public QuadTree(size_t w, size_t h, Callback vertical, Callback horizontal);
        public void resize(size_t w, size_t h);
        public void cycle();
        public Item add(V object);
        public InList<Item> collide(int x, int y, int w, int h);

        [CCode (simple_generics = true)]
        public static V object(InList<V> list);
        [Compact]
        [CCode (cname = "Eina_QuadTree_Item", free_function = "eina_quadtree_del", lower_case_cprefix = "eina_quadtree_", simple_generics = true)]
        public class Item<V>
        {
            [CCode (cname = "eina_quadtree_add")]
            public Item(QuadTree tree, [CCode (type = "const void *")] V object);
            public bool del();
            public bool change();
            public bool hide();
            public bool show();
            public void increase();
        }
    }
    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_lalloc_free")]
    public class Lalloc
    {
        [CCode (has_target = false)]
        public delegate bool Alloc(void* user_data, int num);
        [CCode (has_target = false)]
        public delegate void Free (void* user_data);
        public Lalloc(void* data, Alloc alloc_cb, Free free_cb, int num_init);
        public bool elements_add(int num);
        public bool element_add();
    }

    //=======================================================================
    [Compact]
    public struct Magic : uint
    {
        [CCode (cname="eina_magic_string_get")]
        public string to_string();
        public string? string_get();
        public void string_set(string magic_name);
        [CCode (cname = "eina_magic_fail")]
        private static void fail_impl(void* data, Magic m, Magic req_m, string file, string func,int line );
        public inline static void fail(void* data, Magic m, Magic req_m)
        {
            fail_impl(data,m,req_m, GLib.Log.FILE, GLib.Log.METHOD, GLib.Log.LINE);
        }
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_mempool_del")]
    public class Mempool<G>
    {
        public static bool init();
        public static bool shutdown();
        public Backend backend;
        public G data;
        [CCode (cname = "EINA_ERROR_NOT_MEMPOOL_MODULE")]
        public Eina.Error NOT_MEMPOOL_MODULE;
        public Mempool(string module, string context, string options, ...);
        public G[] realloc(G element, uint size);
        public G[] malloc(uint size);
        public void free(G element);
        public void gc();
        public void statistics();
        public class Backend
        {

            [CCode (has_target = false)]
            public delegate void* BackendInit(string context,string options, void* var_args);
            [CCode (has_target = false)]
            public delegate void BackendFree(void* data, void* element);
            [CCode (has_target = false)]
            public delegate void* BackendAlloc(void* data, uint size);
            [CCode (has_target = false)]
            public delegate void* BackendRealloc(void* data, void* element, uint size);
            [CCode (has_target = false)]
            public delegate void BackendGc(void* data);
            [CCode (has_target = false)]
            public delegate void BackendStats(void* data);
            [CCode (has_target = false)]
            public delegate void BackendShutdown(void* data);
            public string name;
            public BackendInit init;
            public BackendFree free;
            public BackendAlloc alloc;
            public BackendRealloc realloc;
            public BackendGc garbage_collect;
            public BackendStats statistics;
            public BackendShutdown shutdown;
        }
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_module_free")]
    public class Module
    {
        public static bool init();
        public static bool shutdown();
        [CCode (has_target = false, cname = "Eina_Module_Cb")]
        public delegate bool Cb (Module m, void* data);
        [CCode (has_target = false, cname = "Eina_Module_Init")]
        public delegate bool Init();
        [CCode (has_target = false, cname = "Eina_Module_Shutdown")]
        public delegate bool Shutdown();
        [CCode (cname = "EINA_ERROR_WRONG_MODULE")]
        public static Eina.Error WRONG_MODULE;
        [CCode (cname = "EINA_ERROR_MODULE_INIT_FAILED")]
        public static Eina.Error MODULE_INIT_FAILED;
        public Module(string file);
        public bool load();
        public bool unload();
        public void* symbol_get(string symbol);
        public string file_get();
        public static string symbol_path_get(string symbol, string subdir);
        public static string environment_path_get(string env, string sub_dir);
        public static Eina.Array<Module> arch_list_get(Eina.Array<Module>? array, string path, string arch);
        public static Eina.Array<Module> list_get(Eina.Array array, string path, uint recursive, Cb cb, void* data);
        public static void list_load(Eina.Array<Module> list);
        public static void list_unload(Eina.Array<Module> list);
        public static void list_delete(Eina.Array<Module> list);
        public static Module find(Eina.Array<Module> array, string module);
    }

    //=======================================================================
    [CCode (cprefix = "EINA_RBTREE", cname = "Eina_Rbtree_Color")]
    public enum RbtreeColor
    {
        RED,
        BLACK
    }

    //=======================================================================
    [CCode (cprefix = "EINA_RBTREE", cname = "Eina_Rbtree_Direction")]
    public enum RbtreeDirection
    {
        LEFT,
        RIGHT
    }

    //=======================================================================
    [Compact]
    public class Rbtree
    {
        public Rbtree[] son;
        public RbtreeColor color;
        [CCode (cname = "Eina_Rbtree_Cmp_Node_Cb", has_target = false)]
        public delegate RbtreeDirection CmpNodeCb(Rbtree left, Rbtree right, void* data);
        [CCode (cname = "Eina_Rbtree_Cmp_Key_Cb", has_target = false)]
        public delegate int CmpKeyCb(Rbtree node, string key, int length, void* data);
        [CCode (cname = "Eina_Rbtree_Free_Cb", has_target = false)]
        public delegate void FreeCb(Rbtree node, void* data);
        [ReturnsModifiedPointer ()]
        public void inline_insert(Rbtree node, CmpNodeCb cb, void * data);
        [ReturnsModifiedPointer ()]
        public void inline_remove(Rbtree node, CmpNodeCb cmp, void* data);
        public void delete(FreeCb func, void* data);
        [ReturnsModifiedPointer ()]
        public void inline_lookup(void* key, int length, CmpKeyCb cmp, void* data);
        public Iterator<Rbtree> iterator_prefix();
        public Iterator<Rbtree> iterator_infix();
        public Iterator<Rbtree> iterator_postfix();
    }

    //=======================================================================
    [Compact]
    [CCode (cprefix = "Rectangle_")]
    public class Rectangle
    {
        public int x;
        public int y;
        public int w;
        public int h;
        [CCode (cname = "eina_spans_intersect")]
        public static int spans_intersect(int c1, int l1, int c2, int l2);
        public Rectangle(int x, int y, int h, int w);
        public bool is_empty();
        public void coords_from(int x, int y, int w, int h);
        public bool intersect(Rectangle r2);
        public bool xcoord_inside(int x);
        public bool ycoord_inside(int y);
        public bool coords_inside(int x, int y);
        public void union(Rectangle src);
        public bool intersection(Rectangle src);
        public void rescale_in(Rectangle in, Rectangle res );
        public void rescale_out(Rectangle in, Rectangle res );
        public Rectangle.Pool get();
        [Compact]
        [CCode (cprefix = "Pool_", free_function = "eina_rectangle_pool_delete")]
        public class Pool
        {
            [CCode (cname = "eina_rectangle_pool_add")]
            public Pool(int w, int h);
            public bool geometry_get(out int w, out int h);
            public void* data_get();
            public void data_set(void* data);
            public int count();
            public Rectangle request(int w, int h);
            //move to Eina.Rectangle ???
            public static void release(Rectangle rect);
        }
    }

    //=======================================================================
    namespace Safety
    {
        [CCode (cname = "EINA_SAFETY_ON_NULL_RETURN")]
        public static void on_null_return(bool expr);
        [CCode (cname = "EINA_SAFETY_ON_NULL_RETURN_VAL")]
        public static void on_null_return_val(bool expr, void* val);
        [CCode (cname = "EINA_SAFETY_ON_TRUE_RETURN")]
        public static void on_true_return(bool expr);
        [CCode (cname = "EINA_SAFETY_ON_TRUE_RETURN_VAL")]
        public static void on_true_return_val(bool expr, void* val);
        [CCode (cname = "EINA_SAFETY_ON_TRUE_RETURN")]
        public static void on_false_return(bool expr);
        [CCode (cname = "EINA_SAFETY_ON_TRUE_RETURN_VAL")]
        public static void on_false_return_val(bool expr, void* val);
        [CCode (cname = "EINA_ARG_NONNULL")]
        public static void arg_nonnull(void* idx, ...);
    }

    namespace Schedule
    {
        [CCode (cname = "eina_sched_prio_drop")]
        public static void drop_priority();
    }

    //=======================================================================
    [Compact]
    [CCode (lower_case_cprefix = "stringshare_", ref_function = "eina_stringshare_ref", unref_function = "eina_stringshare_del")]
    public class String : string
    {
        [ReturnsModifiedPointer ()]
        void add();
        [ReturnsModifiedPointer ()]
        void add_length( uint slen );
        [ReturnsModifiedPointer ()]
        void ref();
        void del();
        [CCode (cname = "eina_stringshare_strlen")]
        int length();
        [CCode (cname = "eina_stringshare_dump")]
        public static void dump();
    }

    //=======================================================================
    [CCode (free_function = "eina_tiler_free")]
    public class Tiler
    {
        [CCode (cname = "eina_tiler_new")]
        public Tiler(int w, int h);
        public void tile_size_set(int w, int h);
        public bool rect_add(Rectangle rect);
        public void rect_del(Rectangle rect);
        public void clear();
        public Iterator<Rectangle> iterator_new();
        public static Iterator<GridInfo> slicer_iterator_new(int x, int y, int w, int h, int tile_w, int tile_h);
        [Compact]
        [CCode (cname = "Eina_Tile_Grid_Info")]
        public class GridInfo
        {
            ulong col;
            ulong row;
            Rectangle rect;
            bool full;
        }
    }
}

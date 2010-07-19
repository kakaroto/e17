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
    public int init();
    public int shutdown();

    [CCode (cname = "Eina_Compare_Cb", has_target = false)]
    public delegate int CompareCb(void* data1, void* data2);

    [CCode (cname = "Eina_Free_Cb", instance_pos = 0, has_target = false)]
    public delegate void FreeCb(void* data);

    [CCode (instance_pos = 0, has_target = false)]
    public delegate bool Each(void* container, void* data, void* fdata );

    [CCode (cprefix = "EINA_SORT_")]
    public enum Sort
    {
        MIN,
        MAX
    }

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

        public Eina.Iterator iterator_new();
        public Eina.Accessor accessor_new();

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

        public Eina.Iterator iterator_new();
        public Eina.Accessor accessor_new();

    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_iterator_free")]
    public class Iterator<G>
    {
        [CCode (cname = "Eina_Iterator_Next_Callback")]
        public delegate bool NextCallback(ref G data);

        [CCode (cname = "Eina_Iterator_Get_Container_Callback")]
        public delegate G GetContainerCallback();

        [CCode (cname = "Eina_Iterator_Free_Callback")]
        public delegate void FreeCallback();

        public void foreach(Each callback, void* fdata);
        [CCode (cname = "eina_iterator_container_get")]
        public unowned G get();
        [CCode (cname="eina_iterator_next")]
        public bool get_next(ref G next);
        [CCode (cname="eflvala_eina_iterator_next")]
        public bool next()
        {
            G g = null;
            var result = this.get_next( ref g );
            return result;
        }
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

        public bool data_get(uint index, ref G data);
        public G container_get();
        public void over(Each cb, uint start, uint end);
    }

    //=======================================================================
    [Compact]
    [CCode (free_function = "eina_array_free")]
    public class Array<G>
    {
        public static bool init();
        public static bool shutdown();
        public G* data;
        public uint total;
        public uint count;
        public uint step;

        public Array(uint step = 0);
        public void clean();
        public void flush();

        public bool push(G data);
        public void pop();
        public void data_set(G data);
        public void data_get(uint index);
        public uint count_get();
        [CCode (cname="eina_array_iterator_new")]
        public Eina.Iterator<G> iterator();
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

        public static int itoa(int n, string s);
        public static int xtoa(uint n, string s);
        public static int dtoa(double d, string s);
        //vala doesn't support long long, use int64
        public static bool atod(string src, int length, out int64 m, out int64 e);
        //FIXME: add fptoa and atofp
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
        public static Features features_get();
    }

    //=======================================================================
    public struct Error : int
    {
        [CCode (cname = "eina_error_msg_register")]
        public  Error(string msg);
        public static Error OUT_OF_MEMORY;

        public static Error get();
        public void set();
        [CCode (cname = "eina_error_msg_get" )]
        public unowned string? to_string();
        public static void print(ErrorLevel level, string file, string function, int line,string format, ...);
        public static void log_level_set(ErrorLevel level);
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
    }

    //=======================================================================
    namespace File
    {
        [CCode (cname = "Eina_File_Dir_List_Cb", has_target=false)]
        public delegate void DirListCb(string name, string path, void* data);
        public static bool dir_list(string dir, bool recursive, DirListCb cb, void* data);
        public Eina.Array<string> split(string path);
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

        [CCode (cname = "eina_hash_string_superfast_new")]
        public static Hash? string_superfast(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_string_small_new")]
        public static Hash? string_small(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_int32_new")]
        public static Hash? int32(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_int64_new")]
        public static Hash? int64(FreeCb? data_free_cb);

        [CCode (cname = "eina_hash_pointer_new")]
        public static Hash? pointer(FreeCb data_free_cb);

        public bool add(K key, V data);
        [CCode (cname = "eina_hash_add")]
        public bool set(K key, owned V data);
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
        public V? find_by_hash(K key, int key_length, int key_hash);
        public V? modify_by_hash(K key, int key_length, int key_hash, V data);
        public Iterator iterator_key_new();
        public Iterator iterator_data_new();
        public Iterator iterator_tuple_new();
        public void foreach(Foreach cb, void* fdata);
    }

    //=======================================================================
    [Compact]
    public class HashTuple<K,V>
    {
        public K key;
        public V data;
        public uint key_length;

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
        [CCode (has_target = false)]
        public delegate bool Cb (Module m, void* data);
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
        public static string environment_get_path(string env, string sub_dir);
        public static Eina.Array<Module> list_get(Eina.Array array, string path, uint recursive, Cb cb, void* data);
        public static void list_load(Eina.Array<Module> list);
        public static void list_unload(Eina.Array<Module> list);
        public static void list_delete(Eina.Array<Module> list);
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
        public delegate void FreeCb(Rbtree node, void data);
        [ReturnsModifiedPointer ()]
        public void inline_insert(Rbtree node, CmpNodeCb cb, void * data);
        [ReturnsModifiedPointer ()]
        public void inline_remove(Rbtree node, CmpNodeCb cmp, void* data);
        public void delete(FreeCb func, void* data);
        [ReturnsModifiedPointer ()]
        public void inline_lookup(void* key, int length, CmpKeyCb cmp, void* data);
        public Iterator iterator_prefix();
        public Iterator iterator_infix();
        public Iterator iterator_postfix();
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
        public Iterator iterator_new();
    }
}

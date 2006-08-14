if $DL_AUTO_LOADED.nil? then
$DL_AUTO_LOADED = true

#require 'rubygems'
#require_gem 'ruby-breakpoint'
require 'dl/import'

# Base class for any DL-based object that has to encapsulate an (opaque pointer to an) external resource.
# Thanks to the consistent naming of constructors in the underlying C API
# it is possible to use the method_missing trick and avoid re-defining each wrapped method in ruby
# Setters and getters are also automatically handled (e.g. you can say K.foo=x instead of K.foo_set(x) and 
# x = K.foo instead of x = K.foo_get ).
# Also it is possible to wrap class hierarchies if the C interface implements them in a sane way.
#

# -------------------------------- GENERAL UTILITIES ------------------------------------

# Takes a string in CamelCasing and breaks it in its component words.
# "upper" defines if the words should be re-cased (nil to leave as-is)
# "separator" a separator to be added between the words (nil to return words array)
#
class String
    CAMEL_CASE = /[A-Z][a-z0-9]+/
    def uncamelize(upper = nil, separator = nil)
        words = []
        self.scan(CAMEL_CASE) { |word| 
            word = (upper) ? word.upcase : word.downcase if !upper.nil?
            words.push(word) 
        }
        return words.join(separator) if !separator.nil?
        return words
    end
end

class CClass
	# -------------------------------- PROPERTIES and CONFIGURATION ------------------------------------

	# DL::Ptr that holds the actual C pointer to this object (setup inside constructor)
	#
	attr_reader   :this

	# If not disabled on CClass, if normal processing for non-getter methods fails
	# a lookup for a getter is done restarting as if the getter was the original method looked for
	# Failure messages will still mention the non-getter, even if getters were tried and none found
	#
	def self.auto_getters=(value); @auto_getters = value;  end
	def self.auto_getters?; 
            if self == CClass then 
		@auto_getters = true if @auto_getters.nil?   # enforce default
		return @auto_getters == true
            else return CClass.auto_getters? end
	end

	# Sometimes the names of the "C classes" you need to wrap are illegal or inconvenient in ruby (e.g. "object", "module", "class" etc),
	# So you can add this delaration to the class definition to use a "call name" different from the actual ruby class name
	#
	def self.wraps_class(value); @wrapped_class = value.downcase; end
    def self.wrapped_class; return @wrapped_class; end

	# We add this property to DL::PtrData so that we can distinguish in initialize
	# between regular pointers (that should be passed on as parameters to _new, and
	# the special case of initialize being called from wrap (see below for both)
	# review: This is kind of an hack, but i can't think of a better way.
	#
	class DL::PtrData
        attr_accessor :just_needs_wrapping
	end
	
	# -------------------------------- PUBLIC INTERFACE -------------------------------------------------

	# Calls the _new method for the current "C class" and wrap the returned DL::PtrData,
	# unless the first parameter is a DL::PtrData with the special wrapping flag,
	# in which case the object will just wrap it.
	#
	def initialize(*args)
        if args.length > 0 && args[0].is_a?(DL::PtrData) && args[0].just_needs_wrapping then
            args[0].just_needs_wrapping = false
            @this = args[0]
        else
            @this = self.class.dynamic_dispatch_logic('new', args, self.class, nil)
            #@this = self.class::LIB_MODULE.send(self.class.callname('new'), *args)
        end
	end

	# Returns an object of this class that wraps the passed DL:DataPtr
	#
	def self.wrap(existing_ptr)
            return nil if !existing_ptr.is_a?(DL::PtrData)
            existing_ptr.just_needs_wrapping = true
            return self.new(existing_ptr)
	end
	
	# Overload the strong comparison operator.
	# Return true only if both CClass-based objects wrap the same pointer.
	#
	def ===(other)
            return (other.is_a?(CClass) && @this == other.this)
	end
	
	# Allows you to obtain a function pointer to any dynamic method in the class
	#  
	def self.function(id)
            name = dynamic_dispatch_logic(id.to_s, nil, self, nil, true)

            # ensure the function from DL is already loaded (dynamic_dispatch_logic might have found the stub)
            meth = self::LIB_MODULE[name]
            if meth.nil?
                proto = self::LIB_MODULE.prototype?(name)
                meth = self::LIB_MODULE.import(name, proto[0..0], proto[1..-1]) 
            end
            return meth
	end
        
	# -------------------------------- DYNAMIC DISPATCH -------------------------------------------

	# The following gets called whenever an unknown method is called on a CClass-derived object.
	# It will happen on both instance method calls and on class method ("static") calls.
	# This will try to find and call an appropriate method in the DL library wrapper and return its return value.
	# The following formats will be tried, in this order, when the previous one fails:
	#
	# 1. libbase_classname_method 
	# 2. repeat 1) but using 'classname' one step down the inheritance hierarchy until bottom of hierarchy is reached
	# 3. if autogetters are disabled, or if we're trying a getter already, or if this is a setter call: GIVE UP
	# 4. restart from 1) but trying with a getter (that is libbase_classname_method_get)
	#
	# Whenver we GIVE UP, then a NoMethodError exception is thrown.
	# Getters get a special handling, as outlined above, since they can't normally be distinguished from regular method
	# calls (setters can, since their names end with "=")
	# The difference between class methods and instance ones, is the fact that for instance methods the first parameter
	# to the DL wrapper is always the internal wrapped DL::Ptr. Class methods don't send this first parameter.
	# If lookup_only is true it will not call the function, just return the function name.
	#
	def self.dynamic_dispatch_logic(name, args, start_klass, thisp = nil, lookup_only = false, &block)
            orig_err = nil
            
            is_getter = /_get$/i.match(name)
            is_setter = /(=|_set)$/i.match(name)
            is_question = /(\?)$/i.match(name)
            is_new = /^new_/i.match(name)
            static = thisp.nil?
            
            klass = start_klass
            while true
                calln = callname(name, klass)                                   # Build the C function name for current class....
                function_exist = start_klass::LIB_MODULE.respond_to?(calln)
                if function_exist  
                    if lookup_only then CClass.dWL(">> [#{calln}] found for function pointer", :call_lk)
                    else CClass.dWR(">> [#{(static)?'S':'I'}] #{calln} ", :call) end
                                        
                    return calln if lookup_only 

                    args.insert(0,thisp) if !static                              # Prepend the "this pointer" to the list of params if not static call
                    markers = convert_args!(args)                               # Do some convenience conversions on args that are not automatically done by DL
                    
                    retval = start_klass::LIB_MODULE.send(calln, *args)
                    retval = (retval != 0) if is_question                        # Fix the return value to boolean for question methods
                    if block then                                                # Call the block with all arguments so ref arguments can be inspected.
                        ref_args = start_klass::LIB_MODULE._args_
                        revert_args!(ref_args, markers)
                        ref_args.delete_at(0) if (!static)
                        yield(retval, *ref_args)
                    end
                    
                    retval = start_klass.wrap(retval) if is_new 
                    return retval
                else
                    p = dyn_parent(klass)                                        # ... if the call fails, look up the parent class in the hierarchy ...
                    if !p.nil? then
                        CClass.dWL("[#{calln}] not found, trying parent #{p.inspect.to_s}", :call_lk)
                        klass = p                                                # ... and repeat everything as if the method was called on that class.
                        next
                    else
                        # if this is not a getter check, and we want auto_getters, then start over again looking for a getter
                        if !is_setter && auto_getters? then 
                            if !is_getter then  
                                # restart everything but making this method believe it was originally called with name_get
                                name += '_get'
                                is_getter = true
                                klass = start_klass
                                
                                CClass.dWL("[#{calln}] not found, but will start looking for a getter (#{name})", :call_lk)
                                next
                            end
                        end
        
                        m = "[#{name}] not found (last try [#{calln}])" # We exhausted all options. Give up !
                        CClass.dWL('!! ' + m, :call)
                        raise NameError, m
                    end
                end
            end
	end
	
	# Just call the dynamic dispatch with the current class and the wrapped pointer, to signal this is an instance method.
	#
	def method_missing(id, *args, &block)
        self.class.dynamic_dispatch_logic(id.to_s, args, self.class, @this, &block)
	end

	# Just call the dynamic dispatch with the current class and no wrapped pointer, to signal this is a class ("static") method.
	#
	def self.method_missing(id, *args, &block)
        dynamic_dispatch_logic(id.to_s, args, self, nil, &block)
	end

	# This does similar dynamic dispatch as the above, but relies on the fact that all wrapped constants are in an internal
	# class of the lib module named Constants.
	# The constants are looked up in this order and the first one found is returned: 
	# 1. LIBBASENAME_CLASSNAME_CONST
	# 2. LIBBASENAME_CONST
	# 3. CLASSNAME_CONST
	# Otherwise an exception is raised.
	# Also note that before all this is tried, the const name is tested to see if it matches  
	# the callback naming rule, and if it does, callback matching processing takes place instead.
	# todo: what to do if there's an unlikely name clash between a const name and a faux callback class name ? 
	#
	def self.const_missing(id)
        
        cb = lookup_callback(id) # this function should be mixed in on the toplevel module for this library.
        return cb if !cb.nil?

        consts = self.const_get(:LIB_MODULE)::Constants
        bname = self.const_get(:LIB_MODULE).const_get(:BASE_NAME).upcase
        selfname = realname(self)
        onbclass = (selfname.upcase != bname)

        begin
            n = bname
            n << '_' + self.name.upcase if onbclass    # if trying to get const on base lib class, don't repeat it twice
            n << '_' + id.to_s.upcase
            CClass.dWL("Trying missing const #{id.to_s.upcase} on class #{self.name} as #{n}", :const_lk)
            return consts.const_get(n)
        rescue NameError => err
            # if we're calling on base lib class there's no point in trying other forms, as they are all the same
            if onbclass then
                CClass.dWL("Const not found as #{n}, giving up.", :const_lk)
                throw err 
            end
            CClass.dWL("Const not found as #{n}", :const_lk)
        end
            
        begin
            n = "#{bname}_#{id.to_s.upcase}"
            CClass.dWL("Trying missing const #{id.to_s.upcase} on class #{self.name} as #{n}", :const_lk)
            return consts.const_get(n)
        rescue => err
            CClass.dWL("Const not found as #{n}", :const_lk)
        end
            
        begin
            n = "#{bname}_#{id.to_s.upcase}"
            CClass.dWL("Trying missing const #{id.to_s.upcase} on class #{self.name} as #{n}", :const_lk)
            return consts.const_get(n)
        rescue => err
            CClass.dWL("Const not found as #{n}, giving up.", :const_lk)
            throw err
        end
	end

        # Allows you to to call variadic functions (with va_list or ... in the signature)
        # function => function name, as you would call it normally as a method of CClass (not the full name).
        # ret_code => DL code for the return value type (see table at bottom of this file)
        # All other args will be converted normally and passed to the function. 
        # Their DL type code will be guessed as best as possible from the actual type.
        # Of course this will not work always, so you can pass a block too. The block will return an array
        # where each non-nil element will be the DL type code for the N-th parameter (it will be used instead
        # of the guessed one. You can also return a string (put "X" in the elements you don't want to override).
        # Returns the return vale and if you catch them, all the input params (in case some of them were modified
        # by the function call).
        # 
	# NOTE: At the moment this is still a work in progress - calling variadic functions will 
        # be completely transparent when the work is finished.
        #
        def va_call(function, ret_code, *args, &block) 
            name = self.class.dynamic_dispatch_logic(function.to_s, nil, self.class, @this, true)
            
            ret = self.class.va_call_exec(name, ret_code, @this, *args, &block)
            
            #ref_args.delete_at(0)
            return ret
        end
        
	def self.va_call(function, ret_code, *args, &block)
            name = dynamic_dispatch_logic(function.to_s, nil, self, nil, true)
            
            retval, *ref_args = va_call_exec(name, ret_code, *args, &block)
            return retval
        end

        def self.va_call_exec(name, ret_code, *args, &block)
            
            dWR(">> #{name} ", :call)
            markers = convert_args!(args)
            codes = get_type_codes(markers)
                        
            if !block.nil?
                changes = yield(codes) 
                changes.split('') if changes.is_a?(String)
                changes.each_with_index { |c,i| 
                    next if c.nil? || c == 'X'
                    codes[i] = c
                }
            end

            codes = ret_code + codes
            
            lib = self::LIB_MODULE.instance_variable_get(:@LIBS)[0]
            sym = lib[name, codes]
            retval, *ref_args = sym.call(*args)
            #revert_args!(ref_args, markers)

            return retval
	end

        #----------------------------------- INTERNAL UTILITIES -----------------------------------------------

	# Dynamically compose the "call name" (that is, the actual function name to call in C)
	# It if composed from the following parts:
	# - the basic library prefix
	# - the name of the class where the function was called with namespacing stripped (or its wraps_class alias)
	# - the actual name of the called function
	# - a _set postfix if it's a setter (that is, the function ends with "=" (which will be removed))
	#
	def self.callname(func, klass = nil)
            klass = self if klass.nil?    # if we were not passed a specific class, find the callname for this very same class.
            klassn = realname(klass)
        
            # automatically fix call name for setter or question methods.
            case func[-1].chr
            when '='; func = func[0...-1] + "_set"   
            when '?'; func = func[0...-1]
            end
            
            calln = self.const_get(:LIB_MODULE).const_get(:BASE_NAME)
            
            calln += '_' + klassn if klassn != calln    # if klass name is same of base name, we are calling a static method, so omit the base name
            calln += '_'
            calln + func
	end

        # Takes a class and returns its name without namespacing, or the name of the C class it wraps, if any.
        def self.realname(klass, downcase = true)
            # the klass name might not be the same of the "object" that we need to call if aliasing was done with "wraps_class"
            klassn = klass.instance_variable_get(:@wrapped_class)
            klassn = klass.name.split('::')[-1] if klassn.nil?  # otherwise take class name and strip namespacing
            klassn = klassn.downcase if downcase
            return klassn
        end
        

	# Find the first direct ancestor of klass that has CClass up in its ancestry
	# todo: verify that all this stuff holds in the face of modules, mixins and other ruby peculiarities
	#
	def self.dyn_parent(klass)
            klass.ancestors.each { |anc|
                next if anc == klass || anc == klass.const_get(:LIB_MODULE) || anc == CClass
                next if !anc.ancestors.include?(CClass)
                return anc
            }
            return nil
	end

	# Given an array of arguments it performs some conveninence automatic conversions on them (in place) and returns an
	# array of markers which can be used to revert the arguments to their original value (see revert_args!).
	# The conversions takes place as follows:
	# - Array : converted to pointers. If you think the conversion is not appropriate, fiddle with pack and pass the packed values.
	# - true, false : converted to 1 and 0
	# - DL::Symbol : converted to its pointer (part of the automated callback handling)
	# - CClass::Callback : converted to its pointer (part of the automated callback handling)
	# - any object that inherits from CClass : automatically extract the "this" pointer
	#
	def self.convert_args!(args)
            converted = []
            
            debug = CClass.debug?
            dbg = []
            
            args.each_with_index { |arg, i|
                if arg.is_a?(Array) 

                    converted[i] = Array
                    args[i] = arg.to_ptr # note: this is probably dangerous if the array elements aren't of the same type

                    dbg << "Array[#{arg.length}]" if debug
                    dbg[-1] += "(#{arg[0].class.name})" if debug && arg.length > 0
                   
                elsif arg.is_a?(TrueClass) || arg.is_a?(FalseClass)

                    converted[i] = TrueClass     # just to remember it was a boolean since we have no boolean class in ruby
                    args[i] = (arg == true) ? 1 : 0
                   
                    dbg << arg.to_s if debug
                elsif arg.is_a?(DL::Symbol) then
                    
                    converted[i] = DL::Symbol    # used to pass callbacks directly
                    args[i] = arg.to_ptr       
                    
                    dbg << arg.to_s if debug
                elsif arg.is_a?(CClass::Callback)
                    
                    converted[i] = CClass::Callback
                    args[i] = arg.func.to_ptr
                    
                    dbg << arg.to_s if debug
                elsif arg.is_a?(CClass) 
                    
                    dbg << arg.to_s if debug
                    converted[i] = arg.class     # when we revert we can wrap back to the right class
                    
                    args[i] = arg.this   
                else
                    if debug
                        # these are just for debug. they are not encoded, and in revert_args we don't revert them.
                        if arg.is_a?(DL::PtrData)
                            dbg << arg.inspect.split(' ')[0].gsub('#<DL::PtrData:0x','PTR:')
                        elsif arg.is_a?(String)
                            dbg << "\"#{arg}\""
                        elsif arg.nil?
                            dbg << "NULL"
                        else
                            dbg << arg.to_s if debug
                        end
                    end
                    converted[i] = arg.class
                end
            }
            
            CClass.dWL("(#{dbg.join(', ')})", :call) if debug
            
            return converted 
	end

	# Performs the reverse of convert_args! (in place), given the converted arg array and the markers array (which is left untouched)
	# Note: CClass::Callbacks and DL::Symbol function pointers are not automatically reverted to their symbols, they stay DL::DataPtr
	#
	def self.revert_args!(args, markers)
            markers.each_with_index { |type, i| 
                if args[i].nil?          then next
                elsif type == Array      then args[i] = args[i].to_a
                elsif type == TrueClass  then args[i] = (args[i] != 0)
                elsif type.ancestors.include?(CClass)
                    args[i] = type.wrap(args[i])
                elsif type == DL::Symbol then
                    # The case where the callback passed out is different than the one passed in is pretty esoteric 
                    # and probably better handled manually anyway. You get the DL::DataPtr still and you can work on that.
                    next  
                end
            }
	end

        def self.get_type_codes(markers)
            codes = ""
            markers.each { |type|
                # the when's are put into arrays since case uses === for compare, and type is of class Class and 
                # Class#=== won't compare right with another Class. 
                # Since Array#=== returns true if all elements are equal (using ==), it will work.
                if type == Fixnum then codes << 'I'
                elsif type == Bignum then codes << 'L'
                elsif type == Float then codes << 'D'
                elsif type == String then codes << 'S'
                elsif type == TrueClass || type == FalseClass then codes << 'I'
                elsif type == Array || type == DL::Symbol || type == DL::PtrData then codes << 'P'
                elsif type == NilClass then codes << 'P'
                else
                    #if type.ancestors.include?(CClass) || type.ancestors.include?(CClass::Callback) then codes << 'P'
                    #else codes << 'P'
                    codes << 'P'
                end
            }
            return codes
        end

        #-------------------------------------DEBUGGING-------------------------------------
        #
        
	# When enabled on CClass, will print debug logs for everything that gets tried during dynamic calls
	# The default debug_level is used (very verbose)
        #
	def self.debug=(value); @debug = (value == true); end
	def self.debug?; 
            if self == CClass then return @debug == true
            else return CClass.debug? end
        end
           
        # An array that lists all the kind of debug messages that are allowed to be written
        # :call      > final calls
        # :call_lk   > call lookups
        # :const_lk  > const lookups
        # :func_load > first time function load
        # :misc      > other misc messages
        # nil        > everything (default)
        #
        def self.debug_level=(value); @debug_level = value; end
	def self.debug_level 
            if self == CClass then return @debug_level
            else return CClass.debug_level end
	end    

        def self.dWL(msg, kind) ; CClass.dWR(msg + "\n", kind); end
        def self.dWR(msg, kind)
            $stdout.write(msg) if debug? && (debug_level.nil? || debug_level.include?(kind))
        end

	# ----------------------------------- CALLBACKS ------------------------------------
	#

	# This module shall be extend'ed by the root class of the hierarchy (the one
	# that has LIB_MODULE defined). This way all the methods will become *class methods*
	# of that class and the automatic callback lookup will work.
	#
	module CallbackMaker
            # This will return nil if the constant doesn't represent a callback.
            # Otherwise if it finds a corresponding prototype create a real class 
            # inheriting from Callback itself and returns it.
            # That class is also defined as a real class, so we don't have to look it
            # up again next time.
            #
            def lookup_callback(id)  # this will become a *class method* upon module's extend !
                r = id.to_s.uncamelize(nil, nil) 
                return nil if r[-1] != 'Callback'
                
                CClass.dWR("Callback: trying to match #{id.to_s} : ", :call_lk)

                cbname = compose_callback_name(r[0...-1])
                cbproto = self::LIB_MODULE.prototype?(cbname)

                if cbproto.nil?
                    CClass.dWL("Failed (tried #{cbname})", :call_lk) 
                    return nil
                end
                CClass.dWL("Found #{cbname}(#{cbproto}).", :call_lk)

                k = self.const_set(id, Class.new(Callback))
                k.const_set(:LIB_MODULE, self::LIB_MODULE)
                k.const_set(:PROTO, cbproto)
                k.const_set(:NAME, cbname)
                return k
            end 

            # If you want, you can redefine the following method, which will get called at
            # every callback lookup will use to compose a name in your library's convention 
            # from an array of capitalized words.
            # This will become a *class method* so beware how you redefine it in the class
            # that extends this module
            #
            def compose_callback_name(words); words.join('_'); end  
	end

	# This is the base class for all callbacks that use the automated callback system.
	# The bulk of the work is done by lookup_callback which sets up in the derived classes
	# all the constants that are used here
	#
	class Callback
        # Creates a callback from the supplied block using the callback prototype 
        # for self (a Callback-derived class)
        #
        def initialize(&block)
            @func = self.class::LIB_MODULE.bind_callback(self.class::NAME, &block)
        end
        attr_reader :func
	end
    
    # All the fake constant handling lives in the base class. But it's more natural to
    # use the module directly, since it's generally named like the library. So we just redirect
    # the requests for constants to the base class, if they fail here.
    #
    module Redirects
        def const_missing(id)
            begin 
                return super(id)
            rescue NameError
                return self::BASE_CLASS.const_get(id)
            end
        end
        
        # People will naturally want to include the module into the global namespace if they are
        # using just one wrapper library and don't have namespacing issues. But we don't want a lot
        # of this stuff to get mixed in into the global namespace. So we override the default "include"
        # handling to import only the CClass-derived classes inside this module.
        #
        def append_features(receiver)
            CClass.dWL("#{self.name} being included into #{receiver.name}", :misc)
            
            self.constants.each { |cname|
                c = self.const_get(cname)
                next if !c.ancestors.include?(CClass)

                # include the class only if it's not already included. ensures first come, first 
                # served behaviour in case of namespacing issues
                begin 
                    receiver.const_get(cname)
                rescue 
                    receiver.const_set(cname, c) 
                end
            }
        end
    end
end

# --------------------------------------- OTHER EXTERNAL UTILITIES -------------------------------------------------------

# The utility methods that DL uses for parsing C declarations are tucked away inside an internal module.
# Let's just make them accessible.
#
class DL::ImportUtils; #:nodoc: 
    extend DL::Importable::Internal;
end

# The callback pattern done using DL::Importable and the callback directive didn't look especially appealing since you had to 
# define you callback methods on the base library wrapper (the one extending DL::Importable).
# The following redefines it to allow for a different way of managing callbacks, where you just declare the callback prototype 
# in the base wrapper, then create callback objects on-the-fly from any arbitrary code block or ruby proc object when you need them.
# (togheter with the CClass::Callback above)
# 
module DL::Importable #:nodoc:
    RX_CUR_METHOD = /\:in \`(.*)\'/

    # Small container class with all data available about a wrapped C function
    #
    class CFunction
        attr_accessor :name, :plain_ret, :plain_args, :ret, :args
        def initialize(signature)
            @name, @plain_ret, @plain_args = parse_cproto(c_definition)    
            args = encode_argument_types(ret)[0]
        end
    end

    # Eats a C function prototype, parse the arg types and store it in an internal hash, keyed by the 
    # function name, ready for later reuse.
    #
    def prototype(c_definition)
        @cb_protos = Hash.new if @cb_protos.nil?
        func,ret,args = parse_cproto(c_definition)
        cbtypes = encode_argument_types(ret)[0]
        cbtypes += encode_argument_types(args)[0]
        @cb_protos[func.intern] = cbtypes
    end

    # Redefine "extern" so that it stores away the prototype along with the function name, 
    # so we can also use also function prototypes to define callbacks.
    # More importantly, this version doesn't load the original function from the dynamic 
    # library immediately. This would waste a lot of time on very large libraries.
    # Instead it just defines the dynamic method with a stub (see function_loader below)
    # which loads the actual method from the DL at the moment of actual call.
    #
    alias :extern_no_save :extern
    def extern(func_proto, is_varargs = false)
        @func_protos = Hash.new if @func_protos.nil?
        func,ret,args = parse_cproto(func_proto)
        cbtypes = encode_argument_types(ret)[0]
        cbtypes += encode_argument_types(args)[0]
        @func_protos[func.intern] = cbtypes
        self.class.send(:define_method, func.intern, function_loader)
    end

    # The proc returned from this method will figure out which actual    
    # method it was called as, then load if from the DL using DL:import,
    # then call it with its args.
    # Note that, since import redefines the method name it receives as first arg with
    # the actual DL function call, this stub will never get called twice for the
    # same method
    #
    def function_loader
        @func_loader = lambda { |*args|
            # what we do here is to get the first frame of the stack trace, and parse it to get the caller name.
            # it is fucking ugly but people in #ruby-lang confirm it's the only way to get the caller of the block.
            # let's just live with it. 
            funcname = RX_CUR_METHOD.match(caller(0)[0])[1]
        
            CClass.dWL("Loading function #{funcname}", :funcload)
            proto = @func_protos[funcname.intern]

            import(funcname, proto[0..0], proto[1..-1]) # this redefines the method with the actual DL function call instead of this stub 
            self.send(funcname, *args)
        } if @func_loader.nil?
        return @func_loader
    end
    
    #~ def varargs_caller(func_name, ret, fixed_args)
        #~ # notice this will have available all the local variables of the current method
        #~ lambda { |*args|
            #~ vargs = ret
            #~ vargs += fixed_args unless fixed_args.nil?
            #~ args_map = {
                #~ Fixnum => 'I',
                #~ Bignum => 'L',
                #~ DL::Ptr => 'P',
                #~ String => 'S',
                #~ NilClass => 'P'     # this can be anything, treating it as pointer seems the simplest idea
            #~ }
            #~ args.each { |arg| 
                #~ ty = args_map[arg.type]
                #~ ty = 'P' if ty.nil?
                #~ vargs += ty
            #~ }
            #~ s = lib[func_name, vargs]
            #~ rval,rargs = s.call(*args)
            
            #~ return rval
        #~ }
    #~ end

    # Given a function name, looks up a prototype for it and binds the given block to a callback object that is returned.
    #
    def bind_callback(func_name, &block)
        proto = prototype?(func_name)
        return nil if proto.nil?
        return DL::define_callback(proto, &block) 
    end

    # This can be useful to work around the limitation of N callbacks that DL has by design (N depends on a compile flag, default 10).
    # For a true solution however, install ruby-ffcall [http://raa.ruby-lang.org/project/ruby-ffcall/] and load module 'dl/ffcall-callback'.
    # This will replace DL::define_callback with a the ffcall version that doesn't have the limitation (todo: not tested yet)
    #
    def unbind_callback(dl_callback)
        DL::remove_callback(dl_callback) 
    end

    # Retrieve prototypes
    #
    def prototype?(func_name)
        func_name = func_name.intern if func_name.respond_to?(:intern)
        proto = @cb_protos[func_name]
        proto = @func_protos[func_name] if proto.nil?
        return proto
    end
    
    # Redefine to make it safer. Original definition would crash if 
    # symbol "name" not found.
    #
    def [](name)
        sy = @SYM[name.to_s]
        return nil if sy.nil?
        return sy[0] 
    end

    # Gets us a function from the libray just as we want it. Proto is not actually checked by the actual OS
    # loader, but matters to ruby/DL. In other words you can pass any proto you like, but then you must respect
    # it on the ruby side.
    #
    def bind_function(name, proto)
        # This assumes we have loaded one and only one library, but within the boundaries of 
        # dl_auto these are safe assumptions to make
        @LIBS[0][name, proto]
    end

end


#-------------------------------- DL Type Codes Table ----------------------------------------

#~ "unsigned int *"           "i"      "int"                      "I"
#~ "unsigned long *"          "l"      "long"                     "L"
#~ "char *"                   "c"      "unsigned char"            "C"
#~ "short *"                  "h"      "unsigned short"           "H"       "short"           "H"
#~ "int *"                    "i"      "unsigned int"             "I"
#~ "long *"                   "l"      "unsigned long"            "L"
#~ "float *"                  "f"      "float"                    "F"
#~ "double *"                 "d"      "double"                   "D"
#~ "unsigned char *"          "c"      "char"                     "C"

#~ "string (char*)"           "s"      "string (const char*)"     "S"
#~ "generic pointer"          "P"      "array (type[])"           "a"
#~ "void (only for returns)"  "0"

end
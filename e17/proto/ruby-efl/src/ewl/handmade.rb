    class EwlBase
        CONST_PREFIX = 'EWL_CALLBACK_'
        CB_PREFIX = 'Ewl_'
        
        class << self
            def compose_callback_name(words)
                CB_PREFIX + words.join('_')
            end
        
            # We redefine the Callback.lookup method so that if it first looks up
            # a callback constant, and only then if it fails looks up a regular 
            # callback. (This is because the use of such callbacks is much more
            # prevalent).
            #
            # It's your responsibility to pass only these callbacks to the
            # Widget#<< method (which internally calls Ewl::callback_append which 
            # don't accept other types of callbacks other than "standard" ones). 
            # 
            alias :lookup_callback_base :lookup_callback
        
            def lookup_callback(id)
                r = id.to_s.uncamelize(nil, nil)
                return nil if r[-1] != 'Callback' 	 
                
                constname = CONST_PREFIX + r[0...-1].map {|word| word.upcase }.join('_')
                $stdout.write("Callback: trying to match Ewl_Callback enum first (#{constname}) : ") if CClass.debug?
        
                begin
                    cbconst = LIB_MODULE::Constants.const_get(constname)
                rescue NameError => err; end
        
                # not a "standard" ewl callback. proceed with regular callback lookup
                if cbconst.nil?
                    $stdout.write("Failed.\n") if CClass.debug? 
                    return lookup_callback_base(id)
                end
                $stdout.write(constname + "\n") if CClass.debug?
        
                cbname = 'Ewl_Callback_Function'
                cbproto = LIB_MODULE.prototype?(cbname)
        
                k = const_set(id, Class.new(CClass::Callback))
                k.const_set(:LIB_MODULE, LIB_MODULE)
                k.const_set(:PROTO, cbproto)
                k.const_set(:NAME, cbname)
                k.const_set(:CBCONST, cbconst)
                return k
            end
        end
    end

    class Container
        def << (thing, &block)
            return super(thing, &block) if thing.is_a?(CClass::Callback)
            child_append(thing)
        end
    end

    class Widget
        def << (thing)
            return super(thing) unless thing.is_a?(CClass::Callback)
            EwlBase::callback_append(self, thing.class::CBCONST, thing.func, nil) 
        end
    end

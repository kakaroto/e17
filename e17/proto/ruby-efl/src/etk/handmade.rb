    Kernel.send(:remove_method, :type)    # this is deprecated anyway and masks etk_object_type_get

    # ------------------------------------------- SIGNALS ------------------------------------------------------------
    #
    # In ETK callbacks are handled differently. First, they're tied to Signals (events).
    # Signals are specific for each widget type, so that a click on a button is different from a click on a window.
    # So each Signal is identified with a name (kind of event) plus a widget type. 
    # Each Signal has a Marshaller, which tells the signature of the callback that will handle the Signal.
    # In other words, to know the callback signature we have to know the type of widget.
    #
    # We go about this in two ways. One, you add the widget class into the faux Signal-class name:
    # ButtonClickedSignal.new { |pthis, arg1, arg2| block } 
    # In this way you can already know which args will be passed to your block and name them with meaning.
    # Two, you just use the signal name, and the type will be resolved when you connect the Signal to the widget:
    # ClickedSignal.new { |*args| block }
    # Useful when you want to use the same code to handle different Signals on different widgets, but want to
    # do the same thing for each (e.g. tracing or something generic like that)
    #
    # What follows completely override the dl_auto Callback-handling mechanism and replace it with 
    # and ETK-specific one.
    
    # This class is similar to the dl_auto Callback class, but deals with all the Signal stuff.
    # 
    class SignalHandler
        # We need to create a map of all widget classes to their marshallers, which we do from getting all the
        # class constant definitions. 
        # All this stuff is only done once, the first time a marshaller is looked up, then is cached.
        #
        def self.proto_from_marshaller(marshaller)
            if @mar_proto_map.nil?
                @mar_proto_map = {}
                mars = LibEtk.methods.grep(/etk_marshaller_/)
                mars.each { |mar|
                    ret,params = mar.gsub('etk_marshaller_', '').split('__')
                    params = params.split('_')
                    params.map! { |par| p = map_marshaller_arg(par); (p=="0") ? "" : p }
                    # the 2 'P' params are "Etk_Object *object" and "void *data", which are always first and last argument
                    @mar_proto_map[LibEtk.symbol(mar)] = map_marshaller_arg(ret) + 'P' + params.join + 'P' 
                }
            end
            @mar_proto_map[marshaller]
        end

        # An annoying thing is that marshallers store the information about their callback signature in their name.
        # Once the name is broken down, we map its pieces it with this map. It is cached the first time it is accessed.
        #
        def self.map_marshaller_arg(par)
            @mar_type_map = {
                'VOID' => '0',
                'INT' => 'I',
                'FLAT' => 'F', 'DOUBLE' => 'D',
                'POINTER' => 'P'
            } if @mar_type_map.nil?
            v = @mar_type_map[par]
            return (v.nil?) ? '0' : v
        end
    end

    class FullSignalHandler < SignalHandler
        # If we are on a Signal that is type-specific, we can immediately bind it to the block.
        # 
        def initialize(&block)
            sig = Signal.wrap(Signal.lookup(self.class::SIGNAL, self.class::WIDGET_CLASS.type)) # note that kl.type calls ewl_object_type_get
            @marshaller = sig.marshaller
            proto = self.class.proto_from_marshaller(@marshaller)
            @callback = DL::define_callback(proto, &block)    
        end
        
        def callback(*args); return @callback; end
        def marshaller(*args); return @marshaller; end
    end

    class DelayedSignalHandler < SignalHandler
        # We can't do anything useful until we know the type, later on at signal_connect.
        # So we just store away the block
        #
        def initialize(&block)
            @block = block
        end

        # This should be called as soon as we know the type.
        # widget_type should be an object of type Etk::Type.
        # It will prepare the callback and store it. Every further request for this type will return from cache.
        #
        def prepare_for_type(widget_type)
            sig = Signal.wrap(Signal.lookup(self.class::SIGNAL, widget_type))
            if sig.nil?
                CClass.dWL("[ERR] No signal \"#{self.class::SIGNAL}\" exists for \"#{widget_type.name}\"", :misc)
                return [nil,nil]
            end
            marshaller = sig.marshaller
            proto = self.class.proto_from_marshaller(marshaller)
            cb = DL::define_callback(proto, @block)  

            @callback_type_map = {} if @callback_type_map.nil? 
            @callback_type_map[widget_type] = [cb, marshaller]
        end


        def callback(widget_type)
            if @callback_type_map.nil? then cb = prepare_for_type(widget_type) 
            elsif (cb = @callback_type_map[widget_type]) == nil then cb = prepare_for_type(widget_type) 
            end
            return cb[0]
        end
        def marshaller(widget_type)
            if @callback_type_map.nil? then cb = prepare_for_type(widget_type) 
            elsif (cb = @callback_type_map[widget_type]) == nil then cb = prepare_for_type(widget_type) 
            end
            return cb[1]
        end
    end

    class EtkBase
        CB_PREFIX = 'Etk_'
        CB_POSTFIX = '_Callback_Function'
        
        class << self
            def compose_callback_name(words)
                CB_PREFIX + words.join('_') + CB_POSTFIX
            end
        
            alias :lookup_callback_base :lookup_callback
        
            def lookup_callback(id)
                r = id.to_s.uncamelize(nil, nil)
                return nil if r.pop != 'Signal'

                # Let's figure out if we are giving the widget type name or not.
                # First assume the signal name is the last word, and reduce words from there.
                klname = nil
                signalname = []
                while r.length > 0
                    begin
                        kl = Etk.const_get(r.join('')) 
                        break
                    rescue; end
                    signalname.unshift(r.pop)
                end
                signalname = signalname.join('_').downcase

                dWL("Callback: created with signal name: #{signalname} and class: #{kl.name if !kl.nil?}", :misc)

                # Create a real class with most of this info built in, so we don't have to do this mess again.
                if kl.nil? then
                    k = const_set(id, Class.new(DelayedSignalHandler))
                else
                    k = const_set(id, Class.new(FullSignalHandler))
                    k.const_set(:WIDGET_CLASS, kl)
                end
                k.const_set(:SIGNAL, signalname)
                return k
            end
        end
    end
    
    class EObject
        def << (thing)
            return super(thing) unless thing.is_a?(Etk::SignalHandler)
            widget_type = Etk::Type.wrap(Etk::EObject.object_type(self))
            EtkBase.signal_connect(thing.class::SIGNAL, self, thing.callback(widget_type), nil);
        end
    end

    class Container
        def << (thing)
            return super(thing) unless thing.is_a?(Etk::Widget)
            self.add( thing )
        end
    end
    
    class Widget
        def tooltip=(text); ToolTips.tip_set(self, text); end
        def tooltip; ToolTips.tip_get(self); end
    end
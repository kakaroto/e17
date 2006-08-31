#This is Etk's "sister" library and needs Etk to work.

require File.dirname(File.expand_path(__FILE__)) + '/../etk/ruby-etk.rb'

class EnhanceBase
    def each_signal(widget_name, &block)
        f = self.signals_first(widget_name, [nil].to_ptr, [nil].to_ptr) { |ret, wn, a1, a2| 
            return if ret.nil?
            yield(a1.ptr.to_s, a2.ptr.to_s)
        }

        while true
            f = self.signals_next(f, [nil].to_ptr, [nil].to_ptr) { |ret, wn, a1, a2| 
                return if ret.nil?
                yield(a1.ptr.to_s, a2.ptr.to_s)
            }
        end
    end

   def each_widget(&block)
        e = self.widgets_start
        e = e.ref
        while true
            name = self.widgets_next(e) 

            return nil if name.nil?
            yield name
        end
    end

    def autoconnect(handlers_holder)
        each_widget { |widget_name| 

            widget = self.var_get(widget_name)
            
            each_signal(widget_name) { |signal_name, handler_name|

                # First let's check that there's actually a method that we can use as handler.
                begin hand_proc = handlers_holder.method(handler_name)
                rescue NameError
                    CClass.dWL("No handler defined for #{signal_name} => #{handler_name} on widget #{widget_name}", :misc)
                    next
                end

                # Create a type name that will be trigger the contruction of a SignalHandler in Etk. 
                # Then retrieve the type, thus forcing Etk to actually create it.
                
                # todo: this is inefficent,  but maybe better than reimplementing parts of SignalHandler
                signal_name = signal_name.split('_').map {|w| w.capitalize }.join + 'Signal' 
                sig = Etk::const_get(signal_name) 
                
                # convert the method into a block, instantiate the SignalHandler using that block, and finally
                # connect it to the widget
                hand_proc = hand_proc.to_proc
                sig = sig.new &hand_proc
                widget << sig if !sig.nil? and !widget.nil?
            }
        }
    end
    
    # This overrides the original var_get and automatically wraps the returned pointer to the right
    # Etk widget type (by querying it from the Etk runtime)
    # todo: this will be reworked when the autowrap feature is implemented globally in dl_auto itself
    #
    def var_get(name)
        w = self.class.var_get(self.this, name)
        return nil if w.nil?
        type = Etk::Type.wrap(Etk::EObject.object_type(w))
        type = type.name.gsub(/^Etk_/,'')
        
        begin kl = Etk::const_get(type)
        rescue NameError
            type.downcase!
            Etk::constants.each { |con|
                con = Etk::const_get(con)
                next if !con.ancestors.include?(CClass)
                next if type != con.wrapped_class
                kl = con
                break
            }
        end
        
        return w if kl.nil? 
        return kl.wrap(w)
    end
end

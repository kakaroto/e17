require 'rubygems'
require_gem 'ruby-breakpoint'
require_gem 'cast'
require 'open3'
 
# This is generally useful
#
class String
    def pad(to_len, with = ' ')
        pad = ''
        self.length.upto(to_len) do pad << with end
        return pad << self 
    end
end

class Autobindings

    # Outputs a little prettier headings
    #
    def title(text, glyph = '-', size = 65)
    '#' + ''.pad(size, glyph) + "\n" +
    '#  ' + text + "\n" +
    '#' + ''.pad(size, glyph) + "\n"
    end

    # Setup our defaults
    #
    def initialize()
        @gcc_cmd = '/usr/bin/gcc' 
        @gcc_params = '-E -P -w'
        @gcc_extra_params = ''
        @debug = false
        @debug_lines = false
        @graph = false
        @headers_path = ''
        @headers = []
        @config = {}
        @handmade = nil
    end

    # Various parameters. See documentation for exact meaning
    #
    attr_accessor :handmade, :gcc_cmd, :gcc_extra_params, :headers_path, 
                    :debug, :debug_lines, :graph
    attr_reader :config

    def config=(cfg)
        @config = cfg
        @config[:external_types] = [] if @config[:external_types].nil?
        @config[:type_aliases] = {} if @config[:type_aliases].nil?
        @config[:gcc_params] = '' if @config[:gcc_params].nil?
    end
    
    # Feed the header files to gcc for preprocessing, read the output and strip linemarkers
    #
    def run_preprocessor
        if @debug then
            puts title( "Preprocessing header files" )
            puts "Preprocessor: #{@gcc_cmd}"
            puts "Headers directory: #{@headers_path}"
            puts "Headers that will be processed: #{@config[:headers].join(', ')}"
            puts "Extra options from commandline: #{@gcc_extra_params}"
            puts "Extra options from config file: #{@config[:gcc_params]}" 
        end
        
        prevcur = Dir.pwd
        Dir.chdir(@headers_path)     # move to the headers dir, so we can pass just to names to the preprocessor instead of long paths
        
        params = [@gcc_params, @gcc_extra_params, @config[:gcc_params]].join(' ') 
        cmd = [@gcc_cmd, params, @config[:headers].join(' ')].join(' ')

        puts "Resulting command line is below:\n-- #{cmd}" if @debug

        @data = ''
        gcc_err = ''
        
        Open3.popen3(cmd) { |stdin, stdout, stderr| 
            stdout.read.each_line { |line|
                next if line[0].chr == '#'
                line = line.gsub(/__extension__/, '') # GCC extensions choke the C parser, this is the only way i have to remove them
                @data << line  
            }
            gcc_err = stderr.read
        }
        
        puts gcc_err if @debug
        
        Dir.chdir(prevcur)           # get back where we were, you never know...
        
        if @debug_lines then
            puts title("Preprocessor output:")
            i = 1
            part = ''
            @data.each_line { |l| puts "[#{i.to_s.pad(3,'0')}] " << l; i+=1;  }
        end
    end

    # parse all the data and extract only the information that is meaningful to our generator
    #
    def parse_data
        extra_types = @config[:external_types]
        extra_types = [] if extra_types.nil?
        if @debug then
            puts title( "Parsing C headers" )
            puts "#{extra_types.length} extra types will be passed to the C parser:\n#{extra_types.join(', ')}"
        end
        
        parser = C::Parser.new
        parser.type_names.merge(extra_types)
        #~ puts "--------------------------------"
        #~ puts @data
        #~ puts "--------------------------------"
        tree = parser.parse(@data)
                        
        @enums      = {}
        @types      = []
        @decls      = []
        @structs    = []
        @protos     = {}
        @synon      = {}
        @all_synon  = {}
        
        tree.entities.each { |e|
            if !(C::Declaration === e)
                puts "Skipping #{e.class.to_s}" if @debug
                next
            end

            if e.typedef?
                # check if this a typedef defining a pointer to function, which we will use for callbacks
                fp = FunctionPtr.parse(e)
                if !fp.nil? then
                    @protos[fp.name] = fp.to_s(false, false)
                else
                    name = e.type.to_s
                    name << e.declarators[0].indirect_type.to_s if C::Pointer === e.declarators[0].indirect_type
                    @all_synon[name] = e.declarators[0].name
                end 
            end
            
            # -- ENUMERATION
            if C::Enum === e.type then
                                
                if e.type.name.nil? # and !e.typedef? <<< todo: work more on this, see comment below too
                    puts "Skipping anonymous enum" if @debug
                    next
                end

                next if e.type.members.nil? # the enum was already defined, this is just the typedef. todo: refactor into the typedef case

                @types << 'enum ' + e.type.name
                lv = 0
                en = (@enums[e.type.name] = {})

                e.type.members.each { |mem|
                    
                    if mem.val.nil? then 
                        en[mem.name] = lv.to_i
                        lv += 1
                    else
                        # todo: Yes, this is very very brittle. There are a lot more arithmetics that 
                        # someone can do inside an enum that would screw up this. However this is the most used stuff.
                        #
                        if C::BitOr === mem.val
                            en[mem.name.to_s] = en[mem.val.expr1.to_s].to_i | en[mem.val.expr2.to_s].to_i
                        elsif C::BitAnd === mem.val 
                            en[mem.name.to_s] = en[mem.val.expr1.to_s].to_i & en[mem.val.expr2.to_s].to_i
                        elsif C::ShiftLeft === mem.val 
                            en[mem.name.to_s] = en[mem.val.expr1.to_s].to_i << en[mem.val.expr2.to_s].to_i
                        elsif C::ShiftRight === mem.val 
                            en[mem.name.to_s] = en[mem.val.expr1.to_s].to_i >> en[mem.val.expr2.to_s].to_i
                        elsif C::Negative === mem.val 
                            en[mem.name.to_s] = en[mem.val.expr.to_s].to_i * -1
                        else
                            en[mem.name.to_s] = mem.val.val.to_i
                        end
                    end
                }
        
            # -- STRUCTURE (todo: not yet implemented)
            elsif C::Struct === e.type then
                @types << 'struct ' + e.type.name
                @structs << [e.type.name, []]
                @synon[e.type.to_s] = @all_synon[e.type.to_s]
                # st = (@structs[e.type.name] = [])
                # e.type.members.each { |mem|
                #	#todo: nested structures, initializers, etc
                #	st << [mem.declarators[0].name, mem.declarators[0].type.to_s]
                #}
    
            # -- FUNCTION DECLARATION
            elsif e.declarators.length > 0 && C::Function === e.declarators[0].type
                functype = e.declarators[0].indirect_type
                decl = {}
                decl[:name] = e.declarators[0].name
                decl[:varargs] = e.declarators[0].type.var_args?
                decl[:full] = e.type.to_s + FunctionPtr.pointers(functype.type)
                decl[:full] << ' ' << decl[:name] << ' ('
                
                functype.params.each_with_index { |p,i|

                    if (C::CustomType === p.type && p.type.name == 'va_list') then
                        decl[:varargs] = true 
                        next # we leave this last va_args param out since it's a placeholder for the actual varargs params anyway
                    end
        
                    decl[:full] << ((i != 0) ? ', ' : '')
                                        
                    if C::Pointer === p.type && 
                       (C::Int === p.type.direct_type || C::Float === p.type.direct_type) then
                       decl[:full] << p.type.direct_type.to_s << ' ref'
                    else
                        fp = FunctionPtr.parse(p)
                        if fp.nil? then decl[:full] << p.type.to_s
                        else
                            # one of the function parameters is a pointer to an anonymous function
                            # we add it as void*, but then add also a prototype so it's possible to create
                            # a callback for it (use Ewl_Funcname_ParN as callback name)
                            #
                            decl[:full] << 'void*'
                            pname = decl[:name].split('_').map { |s| s.capitalize }.push('Arg'+i.to_s).join('_')
                            fp.name = pname
                            @protos[pname] = fp.to_s(false, false)
                        end
                    end
                } unless functype.params.nil?
                
                decl[:full] << ")"
                #breakpoint if functype.params.length > 0 unless functype.params.nil?
                @decls << decl
            else
                @synon[e.type.to_s] = @all_synon[e.type.to_s]
            end
        }
        
        puts to_s if @debug
        classgraph if @graph
    end

    class FunctionPtr
        attr_accessor :name, :ret, :params
        
        # Node shall be a C::Node
        # Note: if the function ptr returns a pointer to another function, we don't handle that case.
        #       Either typedef it (saner) or make this routine more powerful.
        #
        def self.parse(node)
            f = FunctionPtr.new
            if C::Declaration === node then # the function pointer is in a typedef
                if node.typedef? then
                dec = node.declarators[0]
                return nil unless !dec.nil? && C::Pointer === dec.indirect_type &&  
                                        C::Function === dec.indirect_type.type  
                f.name = dec.name
                
                ftype = dec.indirect_type.type
                f.ret = node.type.to_s  		# return type without any pointer qualifier
                f.ret << FunctionPtr.pointers(ftype.type)
                
                
                f.params = []
                ftype.params.each { |par| 
                    p = FunctionPtr.parse(par)
                    f.params << (( p.nil? ) ? par.type.to_s : p) 
                } unless ftype.params.nil?
                
                return f
                end 
            else    
                return nil unless node.type.is_a?(C::Type) &&  
                                C::Pointer === node.type && C::Function === node.type.type

                ftype = node.type.type
                f.params = []
                ftype.params.each { |par| 
                    p = FunctionPtr.parse(par)
                    f.params << (( p.nil? ) ? par.type.to_s : p) 
                }
                
                f.name = ftype.name if ftype.respond_to?(:name)
                f.ret = ' '
                while C::Pointer === ftype.type # type of function only stores pointer qualifiers in this case
                    f.ret << '*'
                    ftype = ftype.type
                end 
                f.ret.insert(0, ftype.type.to_s)
                return f
            end
            return nil
        end
            
        def to_s(anon=true, as_ptr=true)
            s = @ret + ' '
            s << '(*' if as_ptr
            s << @name unless anon
            s << ')' if as_ptr
            s << " (#{@params.map { |par| par.to_s }.join(', ')})"
            s
        end

        def self.pointers(ctype)
            p = ''
            while C::Pointer === ctype 
                p << '*'
                ctype = ctype.type
            end
            return p
        end
    end
    
    def ignored?(name)
        @config[:ignore].each { |ig|
        if (Regexp === ig && name.match(ig)) ||
           (String == ig && name == ig)
                puts "Ignoring symbol as requested: #{name}"
                return true 
        end
        }
        return false		
    end

    def create_dl_code
                                            
        declarations = ''
        @decls.each { |d| 
        next if ignored?(d[:name])
                    declarations << "\textern '#{d[:full]}'" 
        declarations << ", :varargs" if d[:varargs]
        declarations << "\n" 
            }

        aliases = @config[:external_types].collect { |typ| 
            talias = @config[:type_aliases][typ]
            talias = 'void' if talias.nil?
            "\ttypealias '#{typ}', '#{talias}'" 
        }

        internal_aliases = ""
        @synon.each { |name, typ| 
            internal_aliases << "\ttypealias '#{typ}', '#{name}'\n" 
        }

        prototypes = ''
        @protos.each { |name, decl| 
            prototypes << "\tprototype '#{decl}'\n" unless ignored?(name)

            talias = @config[:type_aliases][name]
            talias = 'void*' if talias.nil?
            aliases << "\ttypealias '#{name}', '#{talias}'"
        }

        aliases = aliases.join("\n")
                        
        @config[:type_aliases].each { |typ, ali| 
            next unless @config[:external_types].index(typ).nil?
            aliases << "\n\ttypealias '#{typ}', '#{ali}'"
        }
        
        enum_aliases = ''
        enumpool = "\tclass Constants\n"
        @enums.each { |enum, members|
            enumpool << "\t\t#----- #{enum.to_s}\n"
            enum_aliases << "\ttypealias '#{enum.to_s}', 'int'\n"
            members.each { |member, value| 
                    enumpool << "\t\t" << member.to_s.upcase << ' = ' << value.to_s << "\n"
            }
            enumpool << "\n"
        }
        enumpool << "\tend\n"

        struct_aliases = ''
        @structs.each { |struct| struct_aliases << "\ttypealias '#{struct}*', 'void*'\n" }

        base_class = nil

        classes = ""
        @config[:classes].each { |kl| 
            classes << "\nclass #{kl[:name]} < "
            # this is the "base class" of the module and get special treatment.
            # there should be only one of these
            if kl[:child_of].nil? then
            if !base_class.nil?
                    puts "[ERROR] There should be only one \"base class\" for each module. You have #{base_class} and #{kl[:name]} here, one should die."
                    return exit
            else
                    classes << "CClass"
                    classes << "\n\tLIB_MODULE = Lib#{@config[:module_base_name]}"
        classes << "\n\textend CClass::CallbackMaker"
                    base_class = kl[:name]
            end
            else
        classes << "#{kl[:child_of]}" 
            end
            classes << "\n"
            classes << "\twraps_class '#{kl[:wraps]}'\n" unless kl[:wraps].nil?
            classes << "end\n"
        }

        if base_class.nil?
            puts "[ERROR] There should be at least one \"base class\" for each module. It will be used to catch all calls that aren't handled by any object in the hierarchy."
            return exit
        end

        handmade = ""
        if @handmade 
                handmade << IO.read(@handmade) 
        end

        libname = @config[:library_name]
        libname = "lib#{@config[:module_base_name].downcase}" if libname.nil?
        
        basename = @config[:root_call_prefix]
        basename = @config[:module_base_name].downcase if basename.nil?

        dl = <<-END_DL_CODE
#-------------------------------------------------------
# This piece of wrapper code was automatically generated
#-------------------------------------------------------
# Basic Ruby/DL module, which contains all the declarations of the wrapped functions, type aliases, callbacks
#
require File.dirname(File.expand_path(__FILE__)) + '/../dl_auto.rb'

module Lib#{@config[:module_base_name]}
	extend DL::Importable
	dlload '#{libname}.so'
	BASE_NAME = '#{basename}'

    # Type aliases (from external types)
    #
#{aliases}
	
    # Type aliases (from enumerations)
    #
#{enum_aliases}

    # Type aliases (from structures)
    #
#{struct_aliases}

    # Type aliases (internal from the headers)
    #
#{internal_aliases}

    # Function declarations
    #
#{declarations}

    # Callback prototypes
    #
#{prototypes}

    # Constants (from enumerations)
    #
#{enumpool}

end

module #{@config[:module_base_name]}

#------------------------------------------------------
# Class definitions.
#
#{classes}
    
    BASE_CLASS = #{base_class}
    extend CClass::Redirects

# Hand made customizations
#
#{handmade}

end


END_DL_CODE
  
    return dl
    end
    
    def to_s
        o = ''
        o += title "#{@types.length} Types:" + @types.join(" | ") + "\n"
                        
        o += title "#{@all_synon.length} Type aliases:"
        @all_synon.each { |syn,syn_to|
            o += "#{syn} => #{syn_to}\n"
        }
        o += "\n"

        o += title "#{@decls.length} Functions:"
        o += @decls.collect { |d| d[:full] }.join("\n")
        o += "\n"
        
        o += title "#{@protos.length} Prototypes:"
        o += @protos.values.join("\n")
        o += "\n"
        
        o += title "#{@enums.length} Enumerations:"
        @enums.each { |name,mem|
            o += name + "{\n"
            mem.each { |k,v| 
                    o += "\t\t#{k} => #{v}\n"
            }
            o += "}\n"
        }
        o += "\n"
        
        o += title "#{@structs.length} Structures:"
        @structs.each { |name, str|
            o += name + "\n"
            #~ o += name + '{ '
            #~ str.each { |mem| 
                    #~ o += "\t\t#{mem[0]} => #{mem[1]}\n"
            #~ }
            #~ o += "}\n"
        }
        o += "\n"
        
        return o
    end

    def classgraph(k = nil, lev = 0)
        if k.nil?
            k = @config[:classes][0]
            puts title('Class tree') 
        end 
        
        puts "#{''.pad(lev-1,' ')} #{k[:name]} " + ((!k[:wraps].nil?) ? "(#{k[:wraps]})" : '')
        
        @config[:classes].each { |sub|
            classgraph(sub, lev+1) if (sub[:child_of] == k[:name])
        }
    end
    
    def run(config_file, output_file)
        begin
            puts "Reading and parsing bindings configuration file"
            cfg = IO::read(config_file);
            cfg = eval(cfg);
            @config = cfg
            puts "Preprocessing the headers"
            run_preprocessor
            puts "Parsing the C code"
            parse_data
            puts "Generating Ruby bindings"
            dlcode = create_dl_code
            puts "Writing output file"
            open(output_file,'w') { |out| out.write(dlcode) }
            puts "Done !"
        rescue => e
            puts e.to_s
            puts e.backtrace
            return false
        end
        return true
    end
end

#------------------------------------------------------------------------
# You can run this file from the command line to generate your bindings,
# after you have written a config file
#------------------------------------------------------------------------

if __FILE__ == $0

def show_usage
    puts <<ENDUSAGE
USAGE: autobindings.rb [OPTIONS] config_file output_file
OPTIONS:
--preprocessor : want to use something different than GCC ?
--preprocessor-params : you can feed it extra parameters.
--headers-path : don't bog down your config file (and gcc) with long paths
--handmade : path to a file that will be inserted verbatim after the generated code
DEBUG OPTIONS:
--debug : traces what we gathered from the C parser
--lines : display all the preprocessed code with line numbers.
--graph : prints a graph of the class hierarchy
ENDUSAGE
end

	require 'getoptlong'
	
	h2dl = Autobindings.new
	
	begin 
	opts = GetoptLong.new(
		[ '--preprocessor', GetoptLong::REQUIRED_ARGUMENT ],
		[ '--preprocessor-params', GetoptLong::REQUIRED_ARGUMENT ],
		[ '--headers-path', GetoptLong::REQUIRED_ARGUMENT ],
		[ '--handmade', GetoptLong::REQUIRED_ARGUMENT ],
		[ '--debug', GetoptLong::NO_ARGUMENT ],
		[ '--lines', GetoptLong::NO_ARGUMENT ],
		[ '--graph', GetoptLong::NO_ARGUMENT ]
		
	)
	
	output = nil
	opts.each { |opt, arg|
		case opt
		when '--preprocessor'; h2dl.gcc_cmd = arg.to_s
		when '--preprocessor-params'; h2dl.gcc_extra_params = arg.to_s
		when '--headers-path'; h2dl.headers_path = arg.to_s
		when '--handmade'; h2dl.handmade = arg.to_s
		when '--debug'; h2dl.debug = true
		when '--lines'; h2dl.debug_lines = true
		when '--graph'; h2dl.graph = true
		end
	}
	rescue => err
        show_usage
        exit -1
	end

	if ARGV.length < 2
        show_usage
        exit -1
	end

    exit -1 if !h2dl.run(ARGV[0], ARGV[1])
    
end
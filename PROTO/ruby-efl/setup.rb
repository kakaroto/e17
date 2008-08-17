require 'getoptlong'
require 'rbconfig'

#require 'rubygems'
#require_gem 'ruby-breakpoint'

class RubyEFLSetup
    BINDINGS = {
        :etk     => { :cvs => '../../libs/etk/src/lib',         :sys => 'src/lib' }, 
        :enhance => { :cvs => '../../libs/enhance/src/lib',     :sys => 'src/lib' }, 
        :ewl     => { :cvs => '../../libs/ewl/src/lib', :sys => 'src/lib' }, 
    }

    USAGE = <<-USAGE
-------------- Ruby-EFL build and install script

Usage: ruby setup.rb [OPTIONS] COMMAND [BINDINGS] 
- COMMAND
  * build   : just build the bindings
  * install : build and install into ruby libraries dir
  
- BINDINGS
  comma separated list of bindings you want. (default is everything => "#{BINDINGS.map{|k,v| k.to_s}.join(',')}")
  
- OPTIONS
  * --headers : cvs => parse headers from same CVS tree (default)
                sys => parse headers from installed library headers [NOT SUPPORTED YET!] 
  * --verbose : shows the verbose output of bindings compilation
  * --prefix  : path where you want to install (default #{Config::CONFIG['sitelibdir']})
  * --rootcmd : su   => use 'su root -c' to run commands that require root privileges
                sudo => use sudo to run commands that require root privileges (default)
USAGE

    def initialize
        @here = File.dirname(File.expand_path(__FILE__))
        @headers = 'cvs'
        @verbose = false
        @inst = Config::CONFIG['sitelibdir']
        @root = 'sudo'
        
       	begin 
            opts = GetoptLong.new(
                [ '--headers', GetoptLong::REQUIRED_ARGUMENT ],
                [ '--verbose', GetoptLong::NO_ARGUMENT ],
                [ '--prefix', GetoptLong::REQUIRED_ARGUMENT ],
                [ '--rootcmd', GetoptLong::REQUIRED_ARGUMENT ]
            )
            
            output = nil
            opts.each { |opt, arg|
                case opt
                when '--headers'; @headers = arg if arg == 'sys'
                when '--verbose'; @verbose = true
                when '--prefix' ; @inst = File.expand_path(arg)
                when '--rootcmd'; @root = 'su root -c' if arg == 'su'
                end
            }
        rescue => err
            puts USAGE
            exit 1
        end

        if ARGV.size < 1 then puts USAGE; exit 1; end
        act = ARGV[0]

        if ARGV.length < 2 then @targets = []
        else @targets = ARGV[1].split(',').map { |lib| lib.strip.downcase.intern } end
        
        @targets = BINDINGS.map{|k,v| k} if @targets.nil? || @targets.length == 0 

        result = false
        case act 
        when "build"; result = build
        when "install"
            if @inst.nil? || @inst.length == 0 then
                puts "[ERR] No system installation directory found and no --prefix option found"
            else result = install end
        else; puts USAGE
        end
        
        exit((result)?0:-1)
    end
    
    def build
        check_prerequisites
        
        @targets.each { |lib| return false if !build_binding(lib) }
        return true
    end
    
    def build_binding(name)
        if !BINDINGS.include?(name) 
            puts "[WARN] Binding #{name.to_s} is not supported. Ignoring." 
            return false
        end

        puts "\n[>>>] Building bindings for #{name.to_s}"

        d = "#{@here}/src/#{name.to_s}"

        builder = Autobindings.new
        if @verbose then
            builder.debug = true
            builder.debug_lines = true
            builder.graph = true
        end

        if @headers == 'cvs' then builder.headers_path = File.expand_path("#{@here}/#{BINDINGS[name][:cvs]}")
        else 
            p = `#{name.to_s}-config --build-dir`
            breakpoint
            builder.headers_path = "#{p.chomp}/#{BINDINGS[name][:sys]}"
        end

        builder.handmade = d + '/handmade.rb'
        
        return builder.run("#{d}/config.rb", "#{d}/ruby-#{name.to_s}.rb")
    end

    def install
        check_prerequisites
        
        @targets.each { |lib| 
            return false if !build_binding(lib) || !install_binding(lib)
        }
        return true
    end

    def install_binding(name)
        puts "\n[>>>] Installing bindings for #{name.to_s} into #{@inst}\n"
        
        base = "#{@inst}/ruby-efl"
        begin
            fs_cmd('mkdir -p', "#{base}/#{name.to_s}" )
            fs_cmd('cp', "#{@here}/src/dl_auto.rb", base + '/')
            fs_cmd('cp', "#{@here}/src/#{name.to_s}/ruby-#{name.to_s}.rb", "#{base}/#{name.to_s}/")
        rescue => e
            puts e.to_s
            puts "[ERR] File system operation failed during installation"
            return false
        end
        return true
    end

    def fs_cmd(cmd, *args)
        c = "#{@root} #{cmd} #{args[0]}"
        c << ' ' << args[1] if cmd == 'cp'
        puts c
        `#{c}`
    end

    def check_prerequisites
        # find out if we have all prerequisites

        begin
            require 'rubygems'
        rescue LoadError
            puts "[ERROR] You need to install rubygems ( http://www.rubygems.org )"
            exit(-1)
        end

        begin
            require 'cast'
        rescue LoadError
            puts "[ERROR] You need to install the CAST C parser ver 0.1.0 ( gem install cast OR http://cast.rubyforge.org )"
            exit(-1)
        end

        begin
            require 'cast'
        rescue LoadError
            puts "[ERROR] You need to install the CAST C parser ver 0.1.0 ( gem install cast OR http://cast.rubyforge.org )"
            exit(-1)
        end

        begin
            require 'dl/ffcall-callback'
        rescue LoadError
            puts "[WARN] To be able to use more than 10 callbacks (ruby/dl limitation) install: \n" + 
                 "- ffcall 1.10 ( http://www.haible.de/bruno/packages-ffcall-README.html )\n" +
                 "- ruby-ffcall 0.0.2 ( http://raa.ruby-lang.org/project/ruby-ffcall/ )"
        end
        
        # we will need this later. makes sense to fail now if it's not there.
        require @here + '/src/autobindings.rb'
    end
end

RubyEFLSetup.new
# $Id$

class Slider
	def initialize(edje, xmms, part, config)
		@edje = edje
		@xmms = xmms
		@part = part
		@config = config
		@val = nil

		self.value = @xmms.configval_get(config).wait.value

		@edje.on_signal("drag", @part) do
			v = on_drag(@edje.part(@part).get_drag_value)
			@xmms.configval_set(@config, v).wait unless v.nil?
		end
	end

	def value=(v)
	end

protected
	def on_drag(d)
	end
end

class VolumeSlider < Slider
	attr_reader :part, :config

	def initialize(edje, xmms, part, config)
		super
	end

	def value=(v)
		v = (1.0 - v.split("/")[0].to_f / 100.0).abs

		if !@val || @val != v
			@val = v
			@edje.part(@part).set_drag_value(0.1, v)
		end
	end

	def on_drag(d)
		ret = nil
		v = (1.0 - d[1]).abs

		if @val != v
			@val = v
			ret = "%i/%i" % [].fill(v * 100, 0..1)
		end

		ret
	end
end

class EqSlider < Slider
	attr_reader :part, :config

	def initialize(edje, xmms, part, config)
		super
	end

	def value=(v)
		v = (1.0 - v.to_f).abs

		if !@val || @val != v
			@val = v
			@edje.part(@part).set_drag_value(0.1, v)
		end
	end

	def on_drag(d)
		ret = nil
		v = (1.0 - d[1]).abs

		if @val != v
			@val = v
			ret = v.to_s
		end

		ret
	end
end

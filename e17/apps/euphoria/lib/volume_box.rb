# $Id$

class VolumeBox
	def initialize(edje, xmms, part)
		@edje = edje
		@xmms = xmms
		@part = part

		self.value = @xmms.playback_volume_get.wait.value

		@edje.on_signal("VOL_INCR") do
			unless @vol[:left] == 100
				tmp = @vol[:left] + 1
				@xmms.playback_volume_set("left", tmp).wait
			end

			unless @vol[:right] == 100
				tmp = @vol[:right] + 1
				@xmms.playback_volume_set("right", tmp).wait
			end
		end

		@edje.on_signal("VOL_DECR") do
			unless @vol[:left] == 0
				tmp = @vol[:left] - 1
				@xmms.playback_volume_set("left", tmp).wait
			end

			unless @vol[:right] == 0
				tmp = @vol[:right] - 1
				@xmms.playback_volume_set("right", tmp).wait
			end
		end
	end

	def value=(v)
		unless v.nil?
			@vol = v
			@edje.part("vol_display_text").text = v[:left].to_s
		end
	end
end

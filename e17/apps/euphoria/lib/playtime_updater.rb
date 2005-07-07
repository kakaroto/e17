# $Id$

class PlaytimeUpdater
	INTERVAL = 0.25

	def initialize(xmms, &block)
		@xmms = xmms
		@block = block
		@timer = nil
		@current_pos = nil

		on_timer
	end

	def enabled=(v)
        if !v && !@timer.nil?
			@timer.delete
			@timer = nil
		elsif v
			@current_pos = nil
			on_timer

            @timer = Ecore::Timer.new(INTERVAL) do
				on_timer
				true
			end
        end
	end

	private
	def on_timer
		pltime = @xmms.playback_playtime.wait.value
		cur_pos = pltime / 1000

		if @current_pos != cur_pos
			@current_pos = cur_pos
			@block.call(@current_pos)
		end
	end
end

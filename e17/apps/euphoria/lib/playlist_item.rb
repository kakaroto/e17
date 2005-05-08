# $Id$

class PlaylistItem
	attr_reader :id, :edje

	def initialize(id, xmms, playlist)
		@id = id
		@position = playlist.size

		@xmms = xmms
		@playlist = playlist

		@edje = nil
		@height = 0

		@props = {}

		@xmms.medialib_get_info(id).notifier do |res|
			self.properties = res.value
		end
	end

	def method_missing(method)
		@props[method]
	end

	def duration
		@props[:duration].to_i / 1000
	end

	def show(eet, container)
		@edje = Edje::Edje.new(container.evas)
		@edje.load(eet, "playlist_item")
		w, @height = @edje.get_size_min

		set_parts_text

		w, h = @edje.get_size_min
		@edje.resize(w, h)

		container.append_element(@edje)

		# if a playlist item gets hilighted, un-hilight all other items
		@edje.on_signal("playlist_item.selected") do
			@playlist.each do |item|
				if item != self
					item.edje.emit_signal("playlist_item.unselected", "")
				end
			end
		end

		@edje.on_signal("playlist_item.play") do
			@xmms.playlist_set_next(@position).wait
			@xmms.playback_tickle.wait
			@xmms.playback_start.wait
		end

=begin
		@edje.on_signal("PLAYLIST_SCROLL_UP", "") do
			@container.scroll(@height * 3)
		end

		@edje.on_signal("PLAYLIST_SCROLL_DOWN", "") do
			@container.scroll(@height * -3)
		end
=end
	end

	def hilight
		@edje.emit_signal("playlist_item.selected", "") unless @edje.nil?
	end

	def visible?
		# since we don't actually hide the edje, but just delete it,
		# we don't need to check @edje.visible? here
		!@edje.nil?
	end

	def hide
		@playlist.container.remove_element(@edje)
		@edje = nil
	end

	def properties=(hash)
		@props = hash || {}
		@props.default = "unknown"

		set_parts_text unless @edje.nil?
	end

	private
	def set_parts_text
		@edje.part("duration").text =
			"%i:%02i" % [duration / 60, duration % 60]
		@edje.part("title").text = "%i %s - %s" % [@position + 1, artist, title]
	end
end

# $Id$

require "euphoria/playlist_item"

class Playlist < Array
	attr_reader :container

	def initialize(xmms, on_cur_item_changed)
		super()

		@xmms = xmms
		@on_cur_item_changed = on_cur_item_changed
		@ee = nil
		@eet = nil
		@container = nil

		@xmms.playlist_list.notifier { |res| push(*res.value) }

		begin
			@current_pos = @xmms.playlist_current_pos.wait.value
		rescue Xmms::Result::ValueError
			@current_pos = nil
		end

		@xmms.broadcast_playlist_current_pos.notifier do |res|
			@current_pos = res.value

			tmp = find { |i| i.hilighted }
			tmp.hilighted = false unless tmp.nil?

			current_item.hilighted = true
		end

		@xmms.broadcast_playlist_changed.notifier do |res|
			case res.value[:type]
				when Xmms::Result::PLAYLIST_CHANGED_ADD
					self << res.value[:id]
				when Xmms::Result::PLAYLIST_CHANGED_REMOVE
					# the daemon sends the position of the song, not the
					# unique mlib id
					delete(self[res.value[:position]])
				when Xmms::Result::PLAYLIST_CHANGED_CLEAR
					clear
			end
		end

		@xmms.broadcast_medialib_entry_changed.notifier do |res|
			@xmms.medialib_get_info(res.value).notifier do |res2|
				props = res2.value

				find_all { |i| i.id == props[:id] }.each do |item|
					item.properties = props

					if current_item && current_item.id == item.id
						@on_cur_item_changed.call(item)
					end
				end
			end
		end
	end

	def current_item
		@current_pos && self[@current_pos]
	end

	def show(eet, is_separate)
		return unless @ee.nil?

		@eet = eet

		if is_separate
			setup_ee
		else
			@ee = Euphoria.instance.ee
			@edje = Euphoria.instance.edje
		end

		@container = Esmart::Container.new(@edje.evas)
		@container.name = "playlist"
		@container.direction = Esmart::Container::VERTICAL
		@container.spacing = 0
		@container.fill_policy = Esmart::Container::FILL_X
		@container.show

		@edje.part("playlist.container").swallow(@container)

		if is_separate
			@edje.on_signal("close") do
				each { |i| i.hide }

				@edje = nil
				@container = nil
				@dragger = nil
				@ee.delete
				@ee = nil
			end
		end

		@edje.on_signal("drag", "playlist.scrollbar.handle") do
			@container.scroll_percent =
				@edje.part("playlist.scrollbar.handle").
				get_drag_value.last
		end

		if is_separate
			@edje.on_signal("file_dialog.open") do
				Euphoria.instance.show_file_dialog
			end
		end

		@edje.on_signal("playlist_item.remove") do
			find_all { |i| i.selected }.each do |i|
				@xmms.playlist_remove(i.position)
			end
		end

		each { |i| i.show(eet) }

		current_item && (current_item.hilighted = true)
	end

	def <<(id)
		push(id)
	end

	def push(*args)
		args.each do |id|
			item = PlaylistItem.new(id, @xmms, self)
			super(item)

			item.show(@eet) unless @container.nil?
			(item.selected = true) if size == 1
		end

		self
	end

	def delete(item)
		unless item.is_a?(PlaylistItem)
			item = find { |i| i.id == item }
		end

		item.hide if item.visible?
		super

		# fix up positions of the remaining items
		self[item.position..-1].each do |item|
			item.position -= 1
		end
	end

	def clear
		each { |i| i.hide if i.visible? }
		super
	end

	private
	def setup_ee
		@ee = Ecore::Evas::SoftwareX11.new
		@ee.title = "Euphoria Playlist"
		@ee.borderless = true

		@ee.on_pre_render { Edje::thaw }
		@ee.on_post_render { Edje::freeze }

		@ee.on_resize do
			x, y, w, h = @ee.geometry

			@edje.resize(w, h)
			@dragger.resize(w, h)
		end

		@dragger = Esmart::Draggies.new(@ee)
		@dragger.name = "dragger"
		@dragger.button = 1
		@dragger.show

		@edje = Edje::Edje.new(@ee.evas)
		@edje.load(@eet, "playlist")
		@edje.name = "edje"
		@edje.show

		w, h = @edje.get_size_max
		@ee.set_size_max(w, h)

		w, h = @edje.get_size_min
		@ee.set_size_min(w, h)

		@ee.resize(w, h)
		@ee.show

		@ee.shaped = !@edje.data("shaped").nil?
	end
end

# $Id$

require "euphoria/playlist_item"

class Playlist < Array
	def initialize(xmms)
		super()

		@xmms = xmms
		@ee = nil
		@eet = nil
		@container = nil

		begin
			@current_pos = @xmms.playlist_current_pos.wait.value
		rescue XmmsClient::ValueError
			@current_pos = nil
		end

		@xmms.broadcast_playlist_current_pos.notifier do |res|
			@current_pos = res.value
			current_item.hilight
		end
	end

	def current_item
		@current_pos && self[@current_pos]
	end

	def show(eet)
		return unless @ee.nil?

		@eet = eet
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
		@edje.load(eet, "playlist")
		@edje.name = "edje"
		@edje.show

		w, h = @edje.get_size_max
		@ee.set_size_max(w, h)

		w, h = @edje.get_size_min
		@ee.set_size_min(w, h)

		@ee.resize(w, h)
		@ee.show

		@ee.shaped = !@edje.data("shaped").nil?

		@container = Esmart::Container.new(@ee.evas)
		@container.name = "playlist"
		@container.direction = Esmart::Container::VERTICAL
		@container.spacing = 0
		@container.fill_policy = Esmart::Container::FILL_X
		@container.show

		@edje.part("playlist.container").swallow(@container)

		@edje.on_signal("close") do
			each { |i| i.hide }

			@edje = nil
			@container = nil
			@dragger = nil
			@ee.delete
			@ee = nil
		end

		@edje.on_signal("drag", "playlist.scrollbar.handle") do
			@container.scroll_percent =
				@edje.part("playlist.scrollbar.handle").
				get_drag_value.last
		end

		each { |i| i.show(eet, @container) }

		current_item && current_item.hilight
	end

	def <<(id)
		push(id)
	end

	def push(*args)
		args.each do |id|
			item = PlaylistItem.new(id, @xmms, self)
			super(item)

			item.show(@eet, @container) unless @container.nil?
		end

		self
	end

	def delete(item)
		unless item.is_a?(PlaylistItem)
			item = find { |i| i.id == item }
		end

		item.hide if i.visible?
		super(item)
	end

	def clear
		each { |i| i.hide if i.visible? }
		super
	end
end

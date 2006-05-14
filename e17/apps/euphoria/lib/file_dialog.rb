require "esmart_file_dialog"

class FileDialog < Ecore::Evas::SoftwareX11
	def FileDialog.open(eet)
		fd = FileDialog.new(eet) { |file| yield file }
		fd.show
	end

	def initialize(eet)
		super()

		setup_ee

		@fd = Esmart::FileDialog.new(evas, eet) do |op|
			case op
			when :ok
				@fd.selections.each do |sel|
					yield File.join(@fd.current_directory, sel)
				end
			when :cancel
				close
			end
		end

		@fd.show

		w, h = @fd.edje.get_size_max
		set_size_max(w, h)

		w, h = @fd.edje.get_size_min
		set_size_min(w, h)

		resize(w, h)
	end

	def close
		@bg = nil
		@dragger = nil
		delete
	end

	private
	def setup_ee
		self.title = "Euphoria File Dialog"
		self.borderless = true

		on_pre_render { Edje.thaw }
		on_post_render { Edje.freeze }

		on_resize do
			x, y, w, h = geometry

			@dragger.resize(w, h)
			@bg.resize(w, h)
			@fd.resize(w, h)
		end

		@bg = Evas::Rectangle.new(evas)
		@bg.set_color(89, 94, 97, 255)
		@bg.show

		@dragger = Esmart::Draggies.new(self)
		@dragger.button = 1
		@dragger.show
	end
end

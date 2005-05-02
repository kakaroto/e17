require "esmart_file_dialog"

class FileDialog
	def FileDialog.open(eet)
		fd = FileDialog.new(eet) { |file| yield file }
		fd.show
	end

	def initialize(eet)
		setup_ee

		@fd = Esmart::FileDialog.new(@ee.evas, eet) do |op|
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
		@ee.set_size_max(w, h)

		w, h = @fd.edje.get_size_min
		@ee.set_size_min(w, h)

		@ee.resize(w, h)
	end

	def show
		@ee.show
	end

	def close
		@bg = nil
		@dragger = nil
		@ee.delete
		@ee = nil
	end

	private
	def setup_ee
		@ee = Ecore::Evas::SoftwareX11.new
		@ee.title = "Euphoria File Dialog"
		@ee.borderless = true

		@ee.on_pre_render { Edje.thaw }
		@ee.on_post_render { Edje.freeze }

		@ee.on_resize do
			x, y, w, h = @ee.geometry

			@dragger.resize(w, h)
			@bg.resize(w, h)
			@fd.resize(w, h)
		end

		@bg = Evas::Rectangle.new(@ee.evas)
		@bg.set_color(89, 94, 97, 255)
		@bg.show

		@dragger = Esmart::Draggies.new(@ee)
		@dragger.button = 1
		@dragger.show
	end
end

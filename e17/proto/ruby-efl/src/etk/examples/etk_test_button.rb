class ButtonExample < Example
    def initialize
        super()
        self.title = "Etk Button Test"
        self.border_width = 5
        EtkBase.signal_connect("delete_event", self, Window.function(:hide_on_delete), nil)
    end
    
    def create_widgets
        return if !@box.nil? 
        
        box = VBox.new(false, 3)
        self << box

        button_normal = Button.new_with_label("Normal button")
        button_normal.tooltip = "This is a normal button"
        box.pack_start(button_normal, false, false, 0)

        image = Image.new_from_file(PACKAGE_DATA_DIR + "/images/e_icon.png")
        button_normal = Button.new_with_label("Button with an image")
        button_normal.tooltip = "This is a normal button with an image"
        button_normal.image = image
        box.pack_start(button_normal, false, false, 0)

        button_normal = Button.new
        button_normal.tooltip = "This is a normal button with nothing"
        box.pack_start(button_normal, false, false, 0)

        button_check = CheckButton.new_with_label("Check button")
        button_check.tooltip = "This is a check button"
        box.pack_start(button_check, false, false, 0)

        button_check = CheckButton.new
        button_check.tooltip = "This is another check button"
        box.pack_start(button_check, false, false, 0)

        button_radio = RadioButton.new_with_label("Radio button", nil)
        button_radio.tooltip = "This is a radio button"
        box.pack_start(button_radio, false, false, 0)

        button_radio = RadioButton.new_from_widget(button_radio)
        button_radio.tooltip = "This is another radio button"
        box.pack_start(button_radio, false, false, 0)

        button_toggle = ToggleButton.new_with_label("Toggle button")
        button_toggle.tooltip = "This is a toggle button"
        box.pack_start(button_toggle, false, false, 0)

        button_toggle = ToggleButton.new
        button_toggle.tooltip = "This is another toggle button"
        box.pack_start(button_toggle, false, false, 0)
        
        @box = box
    end

    def run
        create_widgets
        self.show_all
    end
end
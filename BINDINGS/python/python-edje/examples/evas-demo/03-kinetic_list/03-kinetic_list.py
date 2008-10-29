#!/usr/bin/env python

import evas
import edje
import ecore
import ecore.evas
import sys
import os
import time

WIDTH = 800
HEIGHT = 480
TITLE = "Kinetic List Test"
WM_NAME = "KineticListTest"
WM_CLASS = "main"

class ResizableImage(evas.ClippedSmartObject):
    def __init__(self, ecanvas):
        evas.ClippedSmartObject.__init__(self, ecanvas)
        self.image_object = evas.Image(ecanvas)
        self.member_add(self.image_object)

    def file_set(self, filename):
        self.image_object.file_set(filename)
        self.image_object.show()

    def resize(self, w, h):
        self.image_object.size = (w, h)
        self.image_object.fill_set(0, 0, w, h)


class KineticList(evas.ClippedSmartObject):
    (
        SCROLL_PAGE_FORWARD,
        SCROLL_PAGE_BACKWARD,
        SCROLL_STEP_FORWARD,
        SCROLL_STEP_BACKWARD,
        SCROLL_PIXELS_DOWN,
        SCROLL_PIXELS_UP
    ) = range(6)


    def __init__(self, ecanvas, file, item_width=-1, item_height=-1):
        '''
        if item_width or item_height is left out the width (resp. height)
        of the List element is used.
        '''
        evas.ClippedSmartObject.__init__(self, ecanvas)
        self.elements = []
        self.objects = []
        self.image_objects = []
        self.w = 32
        self.h = 32

        self.realized = False

        self.top_pos = 0
        self.last_top_pos = 0
        self.last_start_row = -1

        self.canvas = ecanvas
        self.edje_file = file

        self.row_width = item_width
        self.row_height = item_height

        self.__manage_objects()

        self.mouse_down = False
        self.last_y_pos = 0
        self.start_pos = 0
        self.mouse_moved = False
        self.continue_scrolling = False
        self.is_scrolling = False
        self.do_freeze = False

    def freeze(self):
        self.do_freeze = True

    def thaw(self):
        self.do_freeze = False
        if self.realized:
            self.__update_variables_after_new_elements()
            self.__update_screen()

    def scroll(self, scroll_type, amount=1):
        self.continue_scrolling = False

        if scroll_type == self.SCROLL_PAGE_FORWARD:
            self.top_pos += amount * self.row_height * self.max_visible_rows
        elif scroll_type == self.SCROLL_PAGE_BACKWARD:
            self.top_pos -= amount * self.row_height * self.max_visible_rows
        elif scroll_type == self.SCROLL_STEP_FORWARD:
            self.top_pos += amount * self.row_height
        elif scroll_type == self.SCROLL_STEP_BACKWARD:
            self.top_pos -= amount * self.row_height
        elif scroll_type == self.SCROLL_PIXELS_DOWN:
            self.top_pos += amount
        elif scroll_type == self.SCROLL_PIXELS_UP:
            self.top_pos -= amount
        else:
            return

        self.__update_screen()

    def __on_mouse_clicked(self, edje_obj, emission, source, data=None):
        for obj in self.objects:
            if obj != edje_obj:
                obj.signal_emit("fadeout", "")

        edje_obj.signal_emit("open", "")

    def __on_mouse_move(self, edje_obj, emission, source, data=None):
        if self.mouse_down:
            x_pos, y_pos = self.canvas.pointer_canvas_xy
            diff = int(self.last_y_pos - y_pos)

            if diff == 0:
                return

            self.mouse_moved = True

            # Reset the data if the direction of the mouse move is changed
            if self.last_diff != -1 and (diff < 0) != (self.last_diff < 0):
                self.last_y_pos = y_pos
                self.start_pos = y_pos
                self.start_time = time.time()

            self.last_diff = diff
            self.top_pos += diff

            self.last_y_pos = y_pos
            self.__update_screen()
            self.last_update_time = time.time()

    def __on_mouse_down(self, edje_obj, emission, source, data=None):
        if not self.is_scrolling:
            self.mouse_moved = False

        self.continue_scrolling = False
        self.mouse_down = True

        x_pos, y_pos = self.canvas.pointer_canvas_xy

        self.last_diff = -1
        self.last_y_pos = y_pos
        self.start_pos = y_pos
        self.start_time = time.time()
        self.last_update_time = time.time()

    def __on_mouse_up(self, edje_obj, emission, source, data=None):
        if self.mouse_down:
            self.mouse_down = False

            x_pos, end_pos = self.canvas.pointer_canvas_xy

            if not self.mouse_moved and not self.is_scrolling:
                self.__on_mouse_clicked(edje_obj, emission, source)
                return

            self.mouse_moved = False
            self.is_scrolling = False

            # do not scroll automatically if the finger was paused
            if time.time() - self.last_update_time > 0.1:
                return

            end_time = time.time()

            pos_diff =  end_pos - self.start_pos
            time_diff = end_time - self.start_time

            self.pixel_per_sec = pos_diff / time_diff
            self.continue_scrolling = True
            self.__do_scroll()

    def __do_scroll(self):
        self.is_scrolling = True

        if self.continue_scrolling == False:
            return

        diff = int(self.pixel_per_sec / 10)

        if abs(self.pixel_per_sec) - diff <= self.row_height:
            offset = self.top_pos % self.row_height

            if offset >= self.row_height / 2:
                self.sign = 1
                offset = self.row_height - offset
            else:
                self.sign = -1

            self.pixels_left = offset
            self.__do_magnetic_scroll()

            return

        if diff != 0:
            self.top_pos -= diff
            self.pixel_per_sec -= self.pixel_per_sec / 10
            self.__update_screen()

        ecore.timer_add(0.02, self.__do_scroll)

    def __do_magnetic_scroll(self):
        if self.pixels_left <= 0 or abs(self.pixel_per_sec) < 1:
            self.mouse_moved = False
            self.is_scrolling = False
            return

        self.pixel_per_sec -= (self.pixel_per_sec / 10)

        pixels_to_substract = int(abs(self.pixel_per_sec / 10))
        if abs(pixels_to_substract) < 1:
            pixels_to_substract = 1

        if self.pixels_left - pixels_to_substract > 0:
            self.pixels_left -= pixels_to_substract
            self.top_pos += self.sign * pixels_to_substract
        else:
            self.top_pos += self.sign * self.pixels_left
            self.pixels_left = 0

        self.__update_screen()
        ecore.timer_add(0.1, self.__do_magnetic_scroll)

    def row_add(self, label, image):
        self.elements.append((label, image))

        if not self.do_freeze:
            self.__update_variables_after_new_elements()
            self.__update_screen()

    def __manage_objects(self):
        remain = (self.h % self.row_height) > 1
        needed_objects = ((self.h / self.row_height) + 1 + remain) * (self.w / self.row_width)
        current_objects = len(self.objects)

        if current_objects < needed_objects:
            for i in range(current_objects, needed_objects):
                obj = edje.Edje(self.canvas);
                obj.file_set(self.edje_file, "list_item");

                obj.signal_callback_add("mouse,move", "*",
                                        self.__on_mouse_move)
                obj.signal_callback_add("mouse,down,1", "*",
                                        self.__on_mouse_down)
                obj.signal_callback_add("mouse,up,1", "*",
                                        self.__on_mouse_up)

                obj.size = (self.row_width, self.row_height)
                obj.clip = self
                self.objects.append(obj)

                image_obj = ResizableImage(self.canvas)
                image_obj.size = (100, 75)
                obj.part_swallow("thumbnail", image_obj);
                self.image_objects.append(image_obj)

        elif needed_objects < current_objects:
            for i in range(needed_objects, current_objects):
                pass # Make this work, it throws exception that makes
                     # things stop working properly
                #del self.objects[i]

    def __update_variables_after_resize(self):
        self.max_visible_rows = (self.h / self.row_height) + 1
        self.max_horiz_elements = (self.w / self.row_width)
        self.max_visible_elements = self.max_visible_rows * \
                                    self.max_horiz_elements

        # Invalidate variable in order to repaint all rows
        # Some might not have been painted before (Didn't
        # fit on the screen
        self.last_start_row = -1

        self.__update_variables_after_new_elements()

    def __update_variables_after_new_elements(self):
        if not self.realized:
            return

        self.min_pos = 0
        remainer1 = (len(self.elements) % self.max_horiz_elements) > 0
        remainer2 = (self.h % self.row_height) > 0
        self.row_amount = (len(self.elements) / self.max_horiz_elements) + \
                          remainer1 + remainer2
        self.max_pos = self.row_height * \
                       (self.row_amount - self.max_visible_rows + 1)

    def __update_screen(self):
        remainer = (self.h % self.row_height) > 0
        row_offset = (self.top_pos / self.row_height)
        pixel_offset = - (self.top_pos % self.row_height)
        start_row = row_offset
        end_row = self.max_visible_rows + row_offset + remainer

        SCROLL_DOWN = self.top_pos > self.last_top_pos
        SCROLL_UP = self.top_pos < self.last_top_pos

        # Let's not move over the last element
        if SCROLL_DOWN and self.last_top_pos >= self.max_pos:
            self.top_pos = self.max_pos
            self.last_top_pos = self.top_pos
            self.continue_scrolling = False
            return

        # Let's not move over the first element
        if SCROLL_UP and self.last_top_pos <= self.min_pos:
            self.top_pos = self.min_pos
            self.last_top_pos = self.top_pos
            self.continue_scrolling = False
            return

        # Overflow scrolling down
        if SCROLL_DOWN and end_row > self.row_amount:
            offset = end_row - self.row_amount
            end_row -= offset
            start_row -= offset
            row_offset -= offset - 1
            self.top_pos = self.max_pos
            pixel_offset = 0

        # Overflow scrolling up
        if SCROLL_UP and start_row < 0:
            self.top_pos = self.min_pos
            end_row -= start_row
            start_row = 0
            row_offset = 0
            pixel_offset = 0

        self.last_top_pos = self.top_pos

        if start_row != self.last_start_row:
            for i in range(0, len(self.objects)):
                self.objects[i].hide()

        for i in range(start_row, end_row):
            row_iter = i - start_row

            for k in range(self.max_horiz_elements):
                obj_iter = row_iter * self.max_horiz_elements + k
                data_iter = i * self.max_horiz_elements + k

                try:
                    label, image = self.elements[data_iter]
                except Exception, e:
                    break;

                offset = (self.w %
                          (self.row_width * self.max_horiz_elements)) / 2
                x = self.row_width * k + self.top_left[0] + offset
                y = self.top_left[1] + self.row_height * (i - row_offset) - \
                    5 + pixel_offset

                self.objects[obj_iter].move(x, y)

                if start_row != self.last_start_row:
                    self.image_objects[obj_iter].file_set(image)
                    self.objects[obj_iter].part_text_set("label", label)
                    self.objects[obj_iter].show()

        self.last_start_row = start_row

    def resize(self, w, h):
        if self.row_width == -1 or self.row_width == self.w:
            self.row_width = w

        if self.row_height == -1 or self.row_height == self.h:
            self.row_height = h

        self.w = w
        self.h = h

        self.__manage_objects()

        for obj in self.objects:
            obj.size = (self.row_width, self.row_height)

        self.realized = True
        self.__update_variables_after_resize()
        self.__update_screen()


class TestView(object):
    def on_key_down(self, obj, event):
        if event.keyname in ("F6", "f"):
            self.evas_obj.fullscreen = not self.evas_obj.fullscreen
        elif event.keyname == "Escape":
            ecore.main_loop_quit()
        elif event.keyname == "a":
            self.list_obj.scroll(KineticList.SCROLL_PIXELS_UP, 1)
        elif event.keyname == "z":
            self.list_obj.scroll(KineticList.SCROLL_PIXELS_DOWN, 1)
        elif event.keyname == "Up":
            self.list_obj.scroll(KineticList.SCROLL_STEP_BACKWARD)
        elif event.keyname == "Down":
            self.list_obj.scroll(KineticList.SCROLL_STEP_FORWARD)

    def __init__(self, evas_canvas, items):
        self.evas_obj = evas_canvas.evas_obj
        f = os.path.splitext(sys.argv[0])[0] + ".edj"
        try:
            self.main_group = edje.Edje(self.evas_obj.evas, file=f,
                                        group="main")
        except edje.EdjeLoadError, e:
            raise SystemExit("error loading %s: %s" % (f, e))

        self.main_group.size = self.evas_obj.evas.size
        self.evas_obj.data["main"] = self.main_group

        self.main_group.show()

        self.main_group.on_key_down_add(self.on_key_down)

        self.main_group.focus = True

        self.list_obj = KineticList(self.evas_obj.evas, file=f, item_height=85)
        self.list_obj.freeze()
        for i in items:
            self.list_obj.row_add(i[0], i[1])
        self.list_obj.thaw()

        self.main_group.part_swallow("list", self.list_obj);


class EvasCanvas(object):
    def __init__(self, fullscreen, engine, size):
        if options.engine == "x11":
            f = ecore.evas.SoftwareX11
        elif options.engine == "x11-16":
            if ecore.evas.engine_type_supported_get("software_x11_16"):
                f = ecore.evas.SoftwareX11_16
            else:
                print "warning: x11-16 is not supported, fallback to x11"
                f = ecore.evas.SoftwareX11

        self.evas_obj = f(w=size[0], h=size[1])
        self.evas_obj.callback_delete_request = self.on_delete_request
        self.evas_obj.callback_resize = self.on_resize

        self.evas_obj.title = TITLE
        self.evas_obj.name_class = (WM_NAME, WM_CLASS)
        self.evas_obj.fullscreen = fullscreen
        self.evas_obj.size = size
        self.evas_obj.show()

    def on_resize(self, evas_obj):
        x, y, w, h = evas_obj.evas.viewport
        size = (w, h)
        for key in evas_obj.data.keys():
            evas_obj.data[key].size = size

    def on_delete_request(self, evas_obj):
        ecore.main_loop_quit()


if __name__ == "__main__":
    from optparse import OptionParser

    def parse_geometry(option, opt, value, parser):
        try:
            w, h = value.split("x")
            w = int(w)
            h = int(h)
        except Exception, e:
            raise optparse.OptionValueError("Invalid format for %s" % option)
        parser.values.geometry = (w, h)

    usage = "usage: %prog [options]"
    op = OptionParser(usage=usage)
    op.add_option("-e", "--engine", type="choice",
                  choices=("x11", "x11-16"), default="x11-16",
                  help=("which display engine to use (x11, x11-16), "
                        "default=%default"))
    op.add_option("-n", "--no-fullscreen", action="store_true",
                  help="do not launch in fullscreen")
    op.add_option("-g", "--geometry", type="string", metavar="WxH",
                  action="callback", callback=parse_geometry,
                  default=(800, 480),
                  help="use given window geometry")
    op.add_option("-f", "--fps", type="int", default=50,
                  help="frames per second to use, default=%default")

    # Handle options and create output window
    options, args = op.parse_args()
    edje.frametime_set(1.0 / options.fps)
    canvas = EvasCanvas(fullscreen=not options.no_fullscreen,
                        engine=options.engine,
                        size=options.geometry)
    items = []
    d = os.path.dirname(sys.argv[0])
    for i in xrange(1000):
        c = (i % 8) + 1
        items.append(("Item %d" % i, os.path.join(d, "thumb_%d.jpg" % c)))

    view = TestView(canvas, items)
    ecore.main_loop_begin()

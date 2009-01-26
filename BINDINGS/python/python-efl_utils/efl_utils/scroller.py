# Copyright (C) 2007-2008 Atur Duque de Souza
#
# This file is part of python-efl_utils.
#
# python-efl_utils is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# python-efl_utils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this python-efl_utils.  If not, see <http://www.gnu.org/licenses/>.

import ecore
import evas

class TextScroller(evas.ClippedSmartObject):
    """Animation to scroll text.

    This class makes easier to scroll text inside some place.

    @note: you should never use TextScrollerAnimation. Use classes that derivates
       from this (the ones that actually can move text inside a clipper)
    """
    def __init__(self, canvas, n_scroll=1, period=15, **kargs):
        """Constructor.

        @param canvas: evas canvas.
        @keyword space: space between two texts
        @keyword n_scroll: number of times to scroll the text
        @keyword period: time (in seconds) to wait until start time again
        @keyword font: font settings just as evas.Text implements
        @keyword text: text to scroll
        @keyword color: color of text to scroll
        """
        if self.__class__ is TextScroller:
            raise TypeError("Should not instantiate 'TextScroller' directly")

        evas.ClippedSmartObject.__init__(self, canvas)
        self.clipper.geometry_set(*self.geometry_get())
        self.space = self.clipper.size[0] / 2

        self.animator = None
        self.repeat_timer = None
        self.period = period
        self.n_scroll = n_scroll

        # text options
        if "text" not in kargs:
            raise ValueError("You must supply a text to scroll")
        self.text1 = self.Text(**kargs)
        self.text2 = self.Text(**kargs)
        self.text1.show()
        self.text2.show()

    def resize(self, w, h):
        if self.size == (w, h):
            return
        self.clipper.resize(w, h)
        self.space = w / 2

    def start(self, *args):
        if not self.animator:
            if self.repeat_timer:
                self.repeat_timer.delete()
                self.repeat_timer = None
            self._setup_start_pos()
            self.text1_times = 0
            self.text2_times = 0
            self.move_text1 = True
            self.move_text2 = False
            self.animator = ecore.animator_add(self._scroll_text)

    def stop(self):
        if self.animator:
            self.animator.delete()
            self.animator = None

    def text_get(self):
        return self.text1.text

    def text_set(self, text):
        self.stop()
        self.text1.text_set(text)
        self.text2.text_set(text)
        self._setup_start_pos()

    text = property(text_get, text_set)

    def _setup_start_pos(self):
        raise NotImplementedError("You must implement a method to put text "
                                  "on an initial position")

    def _move_text(self, text):
        raise NotImplementedError("You must implement a method to move text "
                                  "in one direction")

    def _check_move(self, text1, text2):
        raise NotImplementedError("You must implement a method to check if we "
                                  "can start moving the second text")

    def _check_limit(self, text):
        raise NotImplementedError("You must implement a method to check if the "
                                  "text reached the end of the clipper")

    def _check_stop(self, text):
        raise NotImplementedError("You must implement a method to check if the "
                                  "text reached the stop position")

    def _scroll_text(self):
        # move text1
        if self.move_text1:
            self._move_text(self.text1)

            # start moving text2
            if self.move_text1 and not self.move_text2:
                move = self._check_move(self.text1, self.text2)
                if move:
                    if self.text2_times < self.n_scroll or self.n_scroll == 0:
                        self.move_text2 = True
                        self.text2_times += 1

            # reset text1
            if self._check_limit(self.text1):
                self.move_text1 = False

            # stop animation
            if self.text1_times == self.n_scroll and self._check_stop(self.text1):
                self.animator.delete()
                self.animator = None
                if self.period > 0:
                    self.repeat_timer = ecore.timer_add(self.period,
                                                        self.start,
                                                        None, None, None)
        # move text2
        if self.move_text2:
            self._move_text(self.text2)

            # stop animation if n_scroll = 0
            if self.n_scroll == 0 and self._check_stop(self.text2):
                self.animator.delete()
                self.animator = None
                if self.period > 0:
                    self.repeat_timer = ecore.timer_add(self.period,
                                                        self.start,
                                                        None, None, None)

            # start moving text1
            if self.move_text2 and not self.move_text1:
                if self._check_move(self.text2, self.text1):
                    self.move_text1 = True
                    self.text1_times += 1

            # reset text 2
            if self._check_limit(self.text2):
                self.move_text2 = False

        return True

class TextScrollerLeft(TextScroller):
    def _setup_start_pos(self):
        self.text1.move(*self.clipper.top_left)
        self.text2.move(*self.clipper.top_right)

    def _move_text(self, text):
        xtl, ytl = text.top_left
        step = xtl - 1
        text.move(step, ytl)

    def _check_move(self, text1, text2):
        xtr, ytr = text1.top_right
        if (self.clipper.top_right[0] - xtr) > self.space:
            text2.move(*self.clipper.top_right)
            return True
        return False

    def _check_limit(self, text):
        xtr, ytr = text.top_right
        if xtr == self.clipper.top_left[0]:
            return True
        return False

    def _check_stop(self, text):
        xtl, ytl = text.top_left
        if xtl == self.clipper.top_left[0]:
            return True
        return False


class TextScrollerRight(TextScroller):
    def _setup_start_pos(self):
        self.text1.top_right_set(*self.clipper.top_right)
        self.text2.top_right_set(*self.clipper.top_left)

    def _move_text(self, text):
        xtl, ytl = text.top_left
        step = xtl + 1
        text.move(step, ytl)

    def _check_move(self, text1, text2):
        xtl, ytl = text1.top_left
        if (xtl - self.clipper.top_left[0]) > self.space:
            text2.top_right_set(*self.clipper.top_left)
            return True
        return False

    def _check_limit(self, text):
        xtl, ytl = text.top_left
        if xtl == self.clipper.top_right[0]:
            return True
        return False

    def _check_stop(self, text):
        xtr, ytr = text.top_right
        if xtr == self.clipper.top_right[0]:
            return True
        return False


class TextScrollerUp(TextScroller):
    def _setup_start_pos(self):
        self.text1.move(*self.clipper.top_left)
        self.text2.move(*self.clipper.bottom_left)

    def _move_text(self, text):
        xtl, ytl = text.top_left
        step = ytl - 1
        text.move(xtl, step)

    def _check_move(self, text1, text2):
        xbl, ybl = text1.bottom_left
        if (ybl - self.clipper.top_left[1]) < self.space:
            text2.move(*self.clipper.bottom_left)
            return True
        return False

    def _check_limit(self, text):
        xbl, ybl = text.bottom_left
        if ybl == self.clipper.top_left[1]:
            return True
        return False

    def _check_stop(self, text):
        xtl, ytl = text.top_left
        if ytl == self.clipper.top_left[1]:
            return True
        return False


class TextScrollerDown(TextScroller):
    def _setup_start_pos(self):
        self.text1.bottom_left_set(*self.clipper.bottom_left)
        self.text2.bottom_left_set(*self.clipper.top_left)

    def _move_text(self, text):
        xtl, ytl = text.top_left
        step = ytl + 1
        text.move(xtl, step)

    def _check_move(self, text1, text2):
        xtl, ytl = text1.top_left
        if (self.clipper.bottom_left[1] - ytl) < self.space:
            text2.bottom_left_set(*self.clipper.top_left)
            return True
        return False

    def _check_limit(self, text):
        xtl, ytl = text.top_left
        if ytl == self.clipper.bottom_left[1]:
            return True
        return False

    def _check_stop(self, text):
        xbl, ybl = text.bottom_left
        if ybl == self.clipper.bottom_left[1]:
            return True
        return False

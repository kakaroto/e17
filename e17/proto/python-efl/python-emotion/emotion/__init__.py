#!/usr/bin/env python

import c_emotion

EMOTION_EVENT_MENU1 = 0
EMOTION_EVENT_MENU2 = 1
EMOTION_EVENT_MENU3 = 2
EMOTION_EVENT_MENU4 = 3
EMOTION_EVENT_MENU5 = 4
EMOTION_EVENT_MENU6 = 5
EMOTION_EVENT_MENU7 = 6
EMOTION_EVENT_UP = 7
EMOTION_EVENT_DOWN = 8
EMOTION_EVENT_LEFT = 9
EMOTION_EVENT_RIGHT = 10
EMOTION_EVENT_SELECT = 11
EMOTION_EVENT_NEXT = 12
EMOTION_EVENT_PREV = 13
EMOTION_EVENT_ANGLE_NEXT = 14
EMOTION_EVENT_ANGLE_PREV = 15
EMOTION_EVENT_FORCE = 16
EMOTION_EVENT_0 = 17
EMOTION_EVENT_1 = 18
EMOTION_EVENT_2 = 19
EMOTION_EVENT_3 = 20
EMOTION_EVENT_4 = 21
EMOTION_EVENT_5 = 22
EMOTION_EVENT_6 = 23
EMOTION_EVENT_7 = 24
EMOTION_EVENT_8 = 25
EMOTION_EVENT_9 = 26
EMOTION_EVENT_10 = 27

EmotionModuleInitError = c_emotion.EmotionModuleInitError

class Emotion(c_emotion.Emotion):
    def __new__(type, canvas, module_filename="emotion_decoder_xine.so",
                module_params=None, size=None, pos=None, geometry=None,
                color=None, name=None):
        obj = c_emotion.Emotion.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(module_filename=module_filename,
                               module_params=module_params,
                               size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj

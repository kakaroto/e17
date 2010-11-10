#ifndef ENJOY_MPRIS_H
#define ENJOY_MPRIS_H

#include <E_DBus.h>
#include "private.h"

typedef enum {
  CAN_GO_NEXT = 1 << 0,
  CAN_GO_PREV = 1 << 1,
  CAN_PAUSE = 1 << 2,
  CAN_PLAY = 1 << 3,
  CAN_SEEK = 1 << 4,
  CAN_PROVIDE_METADATA = 1 << 5,
  CAN_HAS_TRACKLIST = 1 << 6
} MPRIS_Capabilities;

void mpris_init(void);
void mpris_shutdown(void);

void mpris_signal_player_caps_change(int caps);
void mpris_signal_player_status_change(int playback, int shuffle, int repeat, int endless);
void mpris_signal_player_track_change(Song *song);
void mpris_signal_tracklist_tracklist_change(int size);
void mpris_signal_emit(const char *root, const char *signal_name, int arg_type, void *arg_value);

#endif /* ENJOY_MPRIS_H */

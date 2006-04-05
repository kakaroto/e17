
#ifndef E_MOD_MIXER_H_INCLUDED
#define E_MOD_MIXER_H_INCLUDED

Mixer *e_volume_mixer_open(Volume *volume, Mixer_Name *name);
int e_volume_mixer_close(Volume *volume, Mixer *mixer);

int e_volume_mixer_ref(Volume *volume, Mixer *mixer);
int e_volume_mixer_unref(Volume *volume, Mixer *mixer);
int e_volume_mixer_unref_close(Volume *volume, Mixer *mixer);

#endif // E_MOD_MIXER_H_INCLUDED

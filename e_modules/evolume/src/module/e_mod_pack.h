
#ifndef E_MOD_PACK_H_INCLUDED
#define E_MOD_PACK_H_INCLUDED


int e_volume_pack_pack_nth(Volume_Face* face, Mixer_Slider* slider,
		int nth);
int e_volume_pack_unpack(Volume_Face* face, Mixer_Slider* slider);
int e_volume_pack_pack_weight(Volume_Face* face, Mixer_Slider* slider);

int e_volume_pack_swap(Volume_Face* face, Mixer_Slider* src,
		Mixer_Slider* dest);

#endif // E_MOD_PACK_H_INCLUDED

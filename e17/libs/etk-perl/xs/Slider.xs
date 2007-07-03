#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"


MODULE = Etk::VSlider	PACKAGE = Etk::VSlider	PREFIX = etk_vslider_

Etk_VSlider *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	CODE:
	RETVAL = ETK_VSLIDER(etk_vslider_new(lower, upper, value, step_increment, page_increment));
	OUTPUT:
	RETVAL

MODULE = Etk::HSlider	PACKAGE = Etk::HSlider	PREFIX = etk_hslider_

Etk_HSlider *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	CODE:
	RETVAL = ETK_HSLIDER(etk_hslider_new(lower, upper, value, step_increment, page_increment));
	OUTPUT:
	RETVAL

MODULE = Etk::Slider	PACKAGE = Etk::Slider	PREFIX = etk_slider_

void
etk_slider_label_set(slider, label)
	Etk_Slider * slider
	const char * label
	ALIAS:
	LabelSet=1

const char *
etk_slider_label_get(slider)
	Etk_Slider * slider
	ALIAS:
	LabelGet=1

void
etk_slider_inverted_set(slider, inverted)
	Etk_Slider * slider
	Etk_Bool inverted
	ALIAS:
	InvertedSet=1

Etk_Bool
etk_slider_inverted_get(slider)
	Etk_Slider * slider
	ALIAS:
	InvertedGet=1

void
etk_slider_update_policy_set(slider, policy)
	Etk_Slider * slider
	Etk_Slider_Update_Policy policy
	ALIAS:
	UpdatePolicySet=1

Etk_Slider_Update_Policy
etk_slider_update_policy_get(slider)
	Etk_Slider * slider
	ALIAS:
	UpdatePolicyGet=1



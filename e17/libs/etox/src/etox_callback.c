#include "Etox_private.h"
#include "Etox.h"

static void __handle_callbacks(void *_data, Evas _e, Evas_Object _o,
			       int _b, int _x, int _y);


Etox_Callback
etox_callback_new(void)
{
  Etox_Callback callback;

  callback = malloc(sizeof(struct _Etox_Callback));
  callback->bits = NULL;
  return callback;
}

void
etox_callback_free(Etox_Callback callback)
{
  if (!callback)
    return;

  if (callback->bits)
    ewd_list_destroy(callback->bits);
  FREE(callback);
}

void
etox_callback_add(Etox_Callback callback, Etox_Callback_Type type,
		  void (*func) (void *_data, Etox_Callback _cb, Etox _e,
				int _b, int _x, int _y), void *data)
{
  Etox_Callback_Bit cb_bit;

  if (!callback || !type)
    return;

  if (!callback->bits)
    {
      callback->bits = ewd_list_new();
      ewd_list_set_free_cb(callback->bits,
			   EWD_FREE_CB(_etox_callback_bit_free));
    }
  cb_bit = _etox_callback_bit_new();
  cb_bit->type = type;
  cb_bit->func = func;
  cb_bit->data = data;
  ewd_list_append(callback->bits, cb_bit);
}

void
etox_callback_del(Etox_Callback callback, Etox_Callback_Type type)
{
  Ewd_List *l;
  Etox_Callback_Bit cb_bit;

  if (!callback || !type)
    return;

  l = ewd_list_new();
  ewd_list_set_free_cb(l, EWD_FREE_CB(_etox_callback_bit_free));

  /* add all bits with type 'type' to list 'l' */
  ewd_list_goto_first(callback->bits);
  while ((cb_bit = (Etox_Callback_Bit) ewd_list_next(callback->bits)))
    if (cb_bit->type == type)
      ewd_list_append(l, cb_bit);

  /* remove bits that 'l' contains from 'callback->bits' */
  ewd_list_goto_first(l);
  while ((cb_bit = (Etox_Callback_Bit) ewd_list_next(l)))
    {
      ewd_list_goto(callback->bits, cb_bit);
      ewd_list_remove(callback->bits);
    }

  /* free the bits that 'l' contains and 'l' itself */
  ewd_list_destroy(l);
}

Etox_Callback_Bit
_etox_callback_bit_new(void)
{
  Etox_Callback_Bit cb_bit;

  cb_bit = (Etox_Callback_Bit) malloc(sizeof(struct _Etox_Callback_Bit));
  cb_bit->type = ETOX_CALLBACK_TYPE_NULL;
  cb_bit->data = NULL;
  cb_bit->func = NULL;

  return cb_bit;
}

void
_etox_callback_bit_free(Etox_Callback_Bit cb_bit)
{
  if (!cb_bit)
    return;
  FREE(cb_bit);
}


static void
__handle_callbacks(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		   int _y)
{
  Etox_Data data;
  Etox_Callback_Bit cb_bit = NULL;
  Etox etox = NULL;
  Etox_Callback cb = NULL;

  data = (Etox_Data) _data;
  cb_bit = (Etox_Callback_Bit) _etox_data_get(data, 0);
  etox = (Etox) _etox_data_get(data, 1);
  cb = (Etox_Callback) _etox_data_get(data, 2);

  cb_bit->func(cb_bit->data, cb, etox, _b, _x, _y);
}

void
_etox_callback_create(Etox e, Etox_Callback callback, Evas_Object ev_obj)
{
  Etox_Callback_Bit cb_bit;
  Etox_Data data = NULL;

  if (!e || !e->bits || ewd_list_is_empty(e->bits))
    return;
  if (!callback || !callback->bits || ewd_list_is_empty(callback->bits))
    return;

  /* create 'junk-list' if it doesn't exist already */
  if (!e->callback_data)
    {
      e->callback_data = ewd_list_new();
      ewd_list_set_free_cb(e->callback_data, EWD_FREE_CB(_etox_data_free));
    }

  ewd_list_goto_first(callback->bits);
  while ((cb_bit = (Etox_Callback_Bit) ewd_list_next(callback->bits)))
    {
      /* create Etox_Data */
      data = _etox_data_new(2);
      _etox_data_set(data, 0, cb_bit);
      _etox_data_set(data, 1, e);
      _etox_data_set(data, 2, callback);

      /* add Etox_Data to 'junk-list' */
      ewd_list_append(e->callback_data, data);

      switch (cb_bit->type)
	{
	case ETOX_CALLBACK_TYPE_MOUSE_IN:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_MOUSE_IN,
			    __handle_callbacks, data);
	  break;
	case ETOX_CALLBACK_TYPE_MOUSE_OUT:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_MOUSE_OUT,
			    __handle_callbacks, data);
	  break;
	case ETOX_CALLBACK_TYPE_MOUSE_DOWN:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_MOUSE_DOWN,
			    __handle_callbacks, data);
	  break;
	case ETOX_CALLBACK_TYPE_MOUSE_UP:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_MOUSE_UP,
			    __handle_callbacks, data);
	  break;
	case ETOX_CALLBACK_TYPE_MOUSE_MOVE:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_MOUSE_MOVE,
			    __handle_callbacks, data);
	  break;
	case ETOX_CALLBACK_TYPE_FREE:
	  evas_callback_add(e->evas, ev_obj, CALLBACK_FREE,
			    __handle_callbacks, data);
	  break;
	default:
	  D_PRINT("Unknown callback (type = %d)\n", cb_bit->type);
	  break;
	}
    }
}

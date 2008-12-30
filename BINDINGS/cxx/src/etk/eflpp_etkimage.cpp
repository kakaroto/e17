#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkimage.h"

namespace efl {

EtkImage::EtkImage( Etk_Object *o )
{
    _o = o;
    _managed = false;
}

void EtkImage::setFromFile( const string &filename, const string &key )
{
    etk_image_set_from_file( ETK_IMAGE( _o ), filename.c_str (), key.c_str () );
}

void EtkImage::getFile( string &outFilename, string &outKey, bool eetLoaded )
{
    char *filename = NULL;
    char *key = NULL;
  
    etk_image_file_get( ETK_IMAGE( _o ), &filename, &key);
    outFilename = filename;
    if (key)
    {
        outKey = key;
        eetLoaded = true;
    }
    else
    {
        eetLoaded = false;
    }
}

void EtkImage::setFromEdje (const string &filename, const string &group)
{
    etk_image_set_from_edje (ETK_IMAGE( _o ), filename.c_str (), group.c_str ());
}

void EtkImage::getEdje (string &outFilename, string &outGroup)
{
    char *filename = NULL;
    char *group = NULL;
  
    etk_image_edje_get( ETK_IMAGE( _o ), &filename, &group);
    outFilename = filename;
    outGroup = group;
}

void EtkImage::setFromStock (Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
{
    etk_image_set_from_stock (ETK_IMAGE( _o ), stock_id, stock_size);
}

Etk_Stock_Id EtkImage::getStockId ()
{
    Etk_Stock_Id stock_id;
    etk_image_stock_get (ETK_IMAGE (_o), &stock_id, NULL);
    return stock_id;
}

Etk_Stock_Size EtkImage::getStockSize ()
{
    Etk_Stock_Size stock_size;
    etk_image_stock_get (ETK_IMAGE (_o), NULL, &stock_size);
    return stock_size;
}

void EtkImage::setFromEvasObject (const EvasObject &eo)
{
    etk_image_set_from_evas_object (ETK_IMAGE (_o), eo.obj ());
}

CountedPtr <EvasObject> EtkImage::getEvasObject ()
{
    Evas_Object *eo = etk_image_evas_object_get (ETK_IMAGE (_o));
    return CountedPtr <EvasObject> (EvasObject::wrap (eo));
}

void EtkImage::setFromData (int width, int height, void *data, bool copy)
{
    etk_image_set_from_data (ETK_IMAGE (_o), width, height, data, copy);
}

void *EtkImage::getData (bool for_writing)
{
    return etk_image_data_get (ETK_IMAGE (_o), for_writing);
}

Etk_Image_Source EtkImage::getSource ()
{
    return etk_image_source_get (ETK_IMAGE (_o));
}

void EtkImage::update ()
{
    etk_image_update (ETK_IMAGE (_o));
}

void EtkImage::rectUpdate (int x, int y, int w, int h)
{
    etk_image_update_rect (ETK_IMAGE (_o), x, y, w, h);
}

int EtkImage::getWidth ()
{
    int width;
    etk_image_size_get (ETK_IMAGE (_o), &width, NULL);
    return width;
}

int EtkImage::getHeight ()
{
    int height;
    etk_image_size_get (ETK_IMAGE (_o), NULL, &height);
    return height;
}

void EtkImage::setAspect (bool keep_aspect)
{
    etk_image_keep_aspect_set (ETK_IMAGE (_o), keep_aspect);
}

bool EtkImage::getKeepAspect ()
{
    return etk_image_keep_aspect_get (ETK_IMAGE (_o));
}

void EtkImage::setRatio (double aspect_ratio)
{
    etk_image_aspect_ratio_set (ETK_IMAGE (_o), aspect_ratio);
}

double EtkImage::getRatio ()
{
    return etk_image_aspect_ratio_get (ETK_IMAGE (_o));
}

EtkImage *EtkImage::wrap( Etk_Object* o )
{
    return new EtkImage( o );
}

} // end namespace efl

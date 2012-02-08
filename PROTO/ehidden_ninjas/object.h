
#include <string>

#include "typedef.h"

namespace eHiddenNinja
{
   class Object
     {

      public:
        Object(std:: string& name, std:: unsigned int& id) :name(name), id(id) {
        virtual ~Object();

      private:
        std:: string name;
        std:: unsigned int id;
     };


   class Block : public Object
   {
    public:
       Block() :Object(string(""), ID_BLOCK);

    private:
       vector2<VAR_TYPE> pos;
   }

}

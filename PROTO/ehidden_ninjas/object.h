
#include <string>

#include "defines.h"

namespace eHiddenNinja
{
   class Object
     {
      private:
#if DEBUG_MODE
        static unsigned int TOTAL_CNT;
#endif
        std:: string name;
        unsigned int id;

      public:
         Object(std:: string& name, std:: unsigned int& id) : name(name),
                                                              id(id)
           {
#if DEBUG_MODE
              ++TOTAL_CNT;
#endif
           }
         virtual ~Object()
           {
#if DEBUG_MODE
              --TOTAL_CNT;
#endif
           }

         std:: string& GetString()
           {
              return &this->name;
           }

         unsigned int GetId()
           {
              return this->id;
           }

         virtual Initialize() {}
         virtual Release() {}
         virtual SetImgObj(Evas_Object*) = 0;
         virtual const Evas_Object *GetImgObj() = 0;

         static void PrintDbgInfo()
           {
#if DEBUG_MODE

              PRINT_DBG("OBJECT COUNT: %d\n", Object ::TOTAL_CNT);
#endif
           }
     };

   class Block: public Object
     {
      public:
         Block() :Object(string(""), ID_BLOCK)
           {
#if DEBUG_MODE
              PRING_DBG("Created a Block");
#endif
           }

         ~Block()
           {
#if DEBUG_MODE
              PRINT_DBG("Destroyed a Block");
#endif
           }

      private:
        vector2<ELEMENT_TYPE> pos;
     }

}

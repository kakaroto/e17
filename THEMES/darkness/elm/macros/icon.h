#define ICON(NAME, FILE) \
   ICON_TOOLBAR(NAME, FILE) \
   ICON_MENU(NAME, FILE)

#define ICON_TOOLBAR(NAME, FILE) \
   group { \
      name: "elm/icon/"#NAME"/default"; \
      alias: "elm/icon/toolbar/"#NAME"/default"; \
      images.image: FILE COMP; \
      min: 32 32; \
      parts { \
         part { \
         name: "base"; \
            description { \
               state: "default" 0.0; \
               aspect: 1.0 1.0; \
               aspect_preference: BOTH; \
               image.normal: FILE; \
            } \
         } \
      } \
   }


#define ICON_MENU(NAME, FILE) \
   group { \
      name: "elm/icon/menu/"#NAME"/default"; \
      images.image: FILE COMP; \
      min: 24 24; \
      max: 24 24; \
      parts { \
         part { \
            name: "base"; \
            description { \
               state: "default" 0.0; \
               aspect: 1.0 1.0; \
               aspect_preference: BOTH; \
               image.normal: FILE; \
            } \
         } \
      } \
   }

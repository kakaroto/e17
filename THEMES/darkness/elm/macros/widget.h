#define PART_EVENTS(NAME, extra) \
         part { name: NAME"_events"; \
            type: RECT; \
            ignore_flags: ON_HOLD; \
            description { \
               state: "default" 0.0; \
               rel1.to: NAME; \
               rel2.to: NAME; \
               color: 0 0 0 0; \
            } \
            extra \
         }

#define ENABLE_PROGRAM_PART \
         name: "enable"; \
         signal: "elm,state,enabled"; \
         source: "elm"; \
         action: STATE_SET "default" 0.0;

#define DISABLE_PROGRAM_PART \
         name: "disable"; \
         signal: "elm,state,disabled"; \
         source: "elm"; \
         action: STATE_SET "disabled" 0.0;


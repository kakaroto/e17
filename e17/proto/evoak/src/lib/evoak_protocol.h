#ifndef EVOAK_PROTOCOL_H
#define EVOAK_PROTOCOL_H

#define                      EVOAK_PROTOCOL_VERSION 1

typedef struct _Evoak_Protocol Evoak_Protocol;

struct _Evoak_Protocol
{
   int                  (*dec) (void *d, int s, void *pp);
   void                *(*enc) (void *p, int *s);
};



/* FIXME:
 * 
 * api to implement...
 * proto_flush <- take all state changes and send result to server
 *
 * protocol to implement...
 * 
 * * = fully done
 * . = partly done
 * 
 * requests:
 * 
 * * CONNECT       <- connect client info
 * * SYNC          <- empty request, will get SYNC back
 * * CLIENT_FREEZE <- freeze all object changes to client objects until thwed - then perform all at once
 * * CLIENT_THAW   <- thaw client and perform all frozen requests at once
 * * OBJ_ADD       <- create a new object (rect, image, text, poly, grad, edje)
 * * OBJ_DEL       <- delete an object
 * * OBJ_MOVE      <- move an object to x, y
 * * OBJ_RESIZE    <- resize an object to w x h
 * * OBJ_SHOW      <- show object
 * * OBJ_HIDE      <- hide object
 * * OBJ_CLIP      <- set clipper of object
 * * OBJ_COLOR_SET <- set color of object
 * * OBJ_LAYER_SET <- set layer of object
 * * OBJ_STACK     <- raise, lower, raise_above, raise_below object
 * * OBJ_EVPROP    <- change event pass/repeat properties
 * * OBJ_EVMASK    <- set event mask
 * * OBJ_FOCUS_SET <- set focus for an object (old focus will go away)
 * * OBJ_IMAGE_FILE_SET
 * * OBJ_IMAGE_FILL_SET
 * * OBJ_IMAGE_BORDER_SET
 * * OBJ_IMAGE_SCALE_SET
 * * OBJ_IMAGE_SIZE_SET
 * * OBJ_IMAGE_ALPHA_SET
 * * OBJ_LINE_SET
 * * OBJ_GRAD_ANGLE_SET
 * * OBJ_GRAD_GRAD_SET
 * * OBJ_POLY_POINTS_SET
 * * OBJ_TEXT_SOURCE_SET
 * * OBJ_TEXT_FONT_SET
 * * OBJ_TEXT_TEXT_SET
 * 
 * OBJ_EDJE_FILE_SET
 * OBJ_EDJE_SIGNAL_EMIT
 * OBJ_EDJE_SWALLOW
 * OBJ_EDJE_MESSAGE_SEND
 * 
 * NB: do font paths client-side and specify file path fully
 * 
 * GRAB_KEY
 * UNGRAB_KEY
 * 
 * events:
 * 
 * * CANVAS_INFO         <- canvas changed size and/or rotation
 * * SYNC_REPLY          <- reply to SYNC request
 * OBJ_VISIBLE         <- new visible state 1/0 (only if in event MASK)
 * OBJ_MOVED           <- new x, y (only if in event MASK)
 * OBJ_RESIZED         <- new size (only if in event MASK)
 * OBJ_RESTACKED       <- raised, lowered, layer changed, stacked above/below (only if in event MASK)
 * OBJ_CLIPPED         <- new clipper or 0 if none (only if in event MASK)
 * OBJ_COLORED         <- r, g, b, a new (only if in event MASK)
 * OBJ_EDJE_SIGNAL     <- signal emitted by edje object (only if in event MASK)
 * 
 * user input device events...
 * 
 * * MOUSE_IN_OUT
 * * MOUSE_MOVE
 * * MOUSE_BUTTON
 * * WHEEL
 * * KEY
 * 
 * requests from a manager client...
 * 
 * CLIENT_INFO   <- enable/disable client info
 * CLIENT_KILL   <- kill off a specific client
 * MANAGER_...   <- all the manager requests sent onto target client (below)
 * 
 * MANAGER_REQUEST_HIDE    <- manager has requested the client hide itself
 * MANAGER_REQUEST_SHOW    <- manager has requested the client show itself
 * MANAGER_REQUEST_LOCATE  <- manager requests the client only live within the defined rectangle
 * MANAGER_REQUEST_STACK   <- manager requests the client be on top/bottom ona  particular layer etc.
 * MANAGER_REQUEST_SYNC    <- sync req from manager to client (must respond)
 * MANAGER_REQUEST_FOCUS   <- sync req from manager to client (must respond)
 * MANAGER_REQUEST_UNFOCUS <- sync req from manager to client (must respond)
 * 
 * CLIENT_ADD     <- add of a client
 * CLIENT_DEL    <- del of a client
 * CLIENT_DEAD   <- server thinks a client is "dead" but still connected
 * CLIENT_ALIVE  <- dead client woke up from dead state
 * 
 * events a client is never aware of...
 * 
 * LOWLEVEL_PING <- sent by server to clients periodically to see if they are still alive or not - client must reply with this and the same ID it recieved
 * 
 * requests for client that wants to handle input methods...
 * 
 * IM_REGISTER <- client (un)registers that it wants to handle imput methods
 * IM_SEND     <- send a resultant IM keystroke or buffer
 * 
 */
#define                      EVOAK_PR_NONE 0
#define                      EVOAK_PR_CONNECT 1
typedef struct {
   int                       proto_min;
   int                       proto_max;
   char                     *client_name;
   char                     *client_class;
   char                     *auth_key;
}                            Evoak_PR_Connect;
#define                      EVOAK_PR_CANVAS_INFO 2
typedef struct {
   int                       w;
   int                       h;
   int                       rot;
}                            Evoak_PR_Canvas_Info;
#define                      EVOAK_PR_SYNC 3
#define                      EVOAK_PR_SYNC_REPLY 4
#define                      EVOAK_PR_OBJECT_ADD 5
typedef struct {
   unsigned char             type;
}                            Evoak_PR_Object_Add;
#define EVOAK_OBJECT_TYPE_NONE      0
#define EVOAK_OBJECT_TYPE_RECTANGLE 1
#define EVOAK_OBJECT_TYPE_IMAGE     2
#define EVOAK_OBJECT_TYPE_TEXT      3
#define EVOAK_OBJECT_TYPE_LINE      4
#define EVOAK_OBJECT_TYPE_POLYGON   5
#define EVOAK_OBJECT_TYPE_GRADIENT  6
#define EVOAK_OBJECT_TYPE_EDJE      7
#define EVOAK_OBJECT_TYPE_LAST      8
#define                      EVOAK_PR_OBJECT_DEL 6
#define                      EVOAK_PR_OBJECT_MOVE 7
typedef struct {
   int                       x, y;
}                            Evoak_PR_Object_Move;
#define                      EVOAK_PR_OBJECT_RESIZE 8
typedef struct {
   int                       w, h;
}                            Evoak_PR_Object_Resize;
#define                      EVOAK_PR_OBJECT_SHOW 9
#define                      EVOAK_PR_OBJECT_HIDE 10
#define                      EVOAK_PR_OBJECT_CLIP_SET 11
typedef struct {
   int                       clipper_id;
}                            Evoak_PR_Object_Clip_Set;
#define                      EVOAK_PR_OBJECT_COLOR_SET 12
typedef struct {
   unsigned char             r, g, b, a;
}                            Evoak_PR_Object_Color_Set;
#define                      EVOAK_PR_OBJECT_LAYER_SET 13
typedef struct {
   int                       layer;
}                            Evoak_PR_Object_Layer_Set;
#define                      EVOAK_PR_OBJECT_STACK 14
typedef struct {
   int                       relative_id;
   unsigned char             relative;
}                            Evoak_PR_Object_Stack;
#define EVOAK_RELATIVE_ABOVE 0
#define EVOAK_RELATIVE_BELOW 1
#define                      EVOAK_PR_OBJECT_EVENT_PROP_SET 15
typedef struct {
   unsigned char             property;
   unsigned char             value;
}                            Evoak_PR_Object_Event_Prop_Set;
#define EVOAK_PROPERTY_PASS   0
#define EVOAK_PROPERTY_REPEAT 1
#define                      EVOAK_PR_OBJECT_EVENT_MASK_SET 16
typedef struct {
   int                       mask1;
   int                       mask2; /* future */
}                            Evoak_PR_Object_Event_Mask_Set;
#define EVOAK_EVENT_MASK1_MOUSE_MOVE          (1 << 0)
#define EVOAK_EVENT_MASK1_MOUSE_IN            (1 << 1)
#define EVOAK_EVENT_MASK1_MOUSE_OUT           (1 << 2)
#define EVOAK_EVENT_MASK1_MOUSE_DOWN          (1 << 3)
#define EVOAK_EVENT_MASK1_MOUSE_UP            (1 << 4)
#define EVOAK_EVENT_MASK1_MOUSE_WHEEL         (1 << 5)
#define EVOAK_EVENT_MASK1_KEY_DOWN            (1 << 6)
#define EVOAK_EVENT_MASK1_KEY_UP              (1 << 7)
#define EVOAK_EVENT_MASK1_OBJ_VISIBLE_CHANGE  (1 << 8)
#define EVOAK_EVENT_MASK1_OBJ_MOVED           (1 << 9)
#define EVOAK_EVENT_MASK1_OBJ_RESIZED         (1 << 10)
#define EVOAK_EVENT_MASK1_OBJ_COLORED         (1 << 11)
#define EVOAK_EVENT_MASK1_OBJ_EDJE_SIGNAL     (1 << 12)
#define EVOAK_EVENT_MASK1_OBJ_FREED           (1 << 13)
#define EVOAK_EVENT_MASK1_OBJ_FOCUS_IN        (1 << 14)
#define EVOAK_EVENT_MASK1_OBJ_FOCUS_OUT       (1 << 15)
#define EVOAK_EVENT_MASK1_OBJ_RESTACKED       (1 << 16)
#define                      EVOAK_PR_OBJECT_FOCUS_SET 17
typedef struct {
   unsigned char             onoff;
}                            Evoak_PR_Object_Focus_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILE_SET 18
typedef struct {
   char                     *file;
   char                     *key;
}                            Evoak_PR_Object_Image_File_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL_SET 19
typedef struct {
   int                       x, y, w, h;
}                            Evoak_PR_Object_Image_Fill_Set;
#define                      EVOAK_PR_CLIENT_FREEZE 20
#define                      EVOAK_PR_CLIENT_THAW 21
#define                      EVOAK_PR_OBJECT_IMAGE_BORDER_SET 22
typedef struct {
   short                     l, r, t, b;
}                            Evoak_PR_Object_Image_Border_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET 23
typedef struct {
   unsigned char             onoff;
}                            Evoak_PR_Object_Image_Smooth_Scale_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_SIZE_SET 24
typedef struct {
   short                     w, h;
}                            Evoak_PR_Object_Image_Size_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_ALPHA_SET 25
typedef struct {
   unsigned char             onoff;
}                            Evoak_PR_Object_Image_Alpha_Set;
#define                      EVOAK_PR_OBJECT_EVENT_MOUSE_MOVE 26
typedef struct {
   int                       x, y, px, py;
   int                       bmask;
   int                       modmask, lockmask;
}                            Evoak_PR_Object_Event_Mouse_Move;
#define EVOAK_MODIFIER_MASK_SHIFT (1 << 0)
#define EVOAK_MODIFIER_MASK_CTRL  (1 << 1)
#define EVOAK_MODIFIER_MASK_ALT   (1 << 2)
#define EVOAK_MODIFIER_MASK_SUPER (1 << 3)
#define EVOAK_MODIFIER_MASK_HYPER (1 << 4)
#define EVOAK_LOCK_MASK_CAPS      (1 << 0)
#define EVOAK_LOCK_MASK_SCROLL    (1 << 1)
#define EVOAK_LOCK_MASK_NUM       (1 << 2)
#define EVOAK_LOCK_MASK_SHIFT     (1 << 3)
#define                      EVOAK_PR_OBJECT_EVENT_MOUSE_DOWNUP 27
typedef struct {
   int                       x, y;
   int                       b;
   int                       modmask, lockmask;
   unsigned char             downup;
}                            Evoak_PR_Object_Event_Mouse_Downup;
#define                      EVOAK_PR_OBJECT_EVENT_MOUSE_INOUT 28
typedef struct {
   int                       x, y;
   int                       bmask;
   int                       modmask, lockmask;
   unsigned char             inout;
}                            Evoak_PR_Object_Event_Mouse_Inout;
#define                      EVOAK_PR_OBJECT_EVENT_MOUSE_WHEEL 29
typedef struct {
   int                       dir;
   int                       z;
   int                       x, y;
   int                       modmask, lockmask;
}                            Evoak_PR_Object_Event_Mouse_Wheel;
#define                      EVOAK_PR_OBJECT_EVENT_KEY_DOWNUP 30
typedef struct {
   int                       modmask, lockmask;
   unsigned char             downup;
   char                     *keyname;
   char                     *string;
}                            Evoak_PR_Object_Event_Key_Downup;
#define                      EVOAK_PR_OBJECT_LINE_SET 31
typedef struct {
   int                       x1, y1, x2, y2;
}                            Evoak_PR_Object_Line_Set;
#define                      EVOAK_PR_OBJECT_GRAD_ANGLE_SET 32
typedef struct {
   int                       ang100;
}                            Evoak_PR_Object_Grad_Angle_Set;
#define                      EVOAK_PR_OBJECT_GRAD_COLOR_ADD 33
typedef struct {
   int                       dist;
   unsigned char             r, g, b, a;
}                            Evoak_PR_Object_Grad_Color_Add;
#define                      EVOAK_PR_OBJECT_GRAD_COLOR_CLEAR 34
#define                      EVOAK_PR_OBJECT_POLY_POINT_ADD 35
typedef struct {
   int                       x, y;
}                            Evoak_PR_Object_Poly_Point_Add;
#define                      EVOAK_PR_OBJECT_POLY_POINT_CLEAR 36
#define                      EVOAK_PR_OBJECT_TEXT_SOURCE_SET 37
typedef struct {
   char                     *source;
}                            Evoak_PR_Object_Text_Source_Set;
#define                      EVOAK_PR_OBJECT_TEXT_FONT_SET 38
typedef struct {
   int                       size;
   char                     *font;
}                            Evoak_PR_Object_Text_Font_Set;
#define                      EVOAK_PR_OBJECT_TEXT_TEXT_SET 39
typedef struct {
   char                     *text;
}                            Evoak_PR_Object_Text_Text_Set;

/* protocol optimization requests making protocol smaller */
#define                      EVOAK_PR_OBJECT_MOVE8 40
typedef struct {
   char                      x, y;
}                            Evoak_PR_Object_Move8;
#define                      EVOAK_PR_OBJECT_MOVE16 41
typedef struct {
   short                      x, y;
}                            Evoak_PR_Object_Move16;
#define                      EVOAK_PR_OBJECT_MOVE_REL8 42
typedef struct {
   char                      x, y;
}                            Evoak_PR_Object_Move_Rel8;
#define                      EVOAK_PR_OBJECT_MOVE_REL16 43
typedef struct {
   short                      x, y;
}                            Evoak_PR_Object_Move_Rel16;
#define                      EVOAK_PR_OBJECT_RESIZE8 44
typedef struct {
   unsigned char             w, h;
}                            Evoak_PR_Object_Resize8;
#define                      EVOAK_PR_OBJECT_RESIZE16 45
typedef struct {
   short                     w, h;
}                            Evoak_PR_Object_Resize16;
#define                      EVOAK_PR_OBJECT_RESIZE_REL8 46
typedef struct {
   char                      w, h;
}                            Evoak_PR_Object_Resize_Rel8;
#define                      EVOAK_PR_OBJECT_RESIZE_REL16 47
typedef struct {
   short                     w, h;
}                            Evoak_PR_Object_Resize_Rel16;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL8_SET 48
typedef struct {
   char                      x, y, w, h;
}                            Evoak_PR_Object_Image_Fill8_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL16_SET 49
typedef struct {
   short                     x, y, w, h;
}                            Evoak_PR_Object_Image_Fill16_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET 50
typedef struct {
   unsigned char             w, h;
}                            Evoak_PR_Object_Image_Fill_Size8_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET 51
typedef struct {
   short                     w, h;
}                            Evoak_PR_Object_Image_Fill_Size16_Set;
#define                      EVOAK_PR_OBJECT_IMAGE_FILL_ALL_SET 52

/***/
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_MOVE 53
typedef struct {
   int                       x, y;
}                            Evoak_PR_Object_Event_Object_Move;
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_RESIZE 54
typedef struct {
   int                       w, h;
}                            Evoak_PR_Object_Event_Object_Resize;
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_RESTACK 55
typedef struct {
   int                       relative_id;
   char                      abovebelow;
}                            Evoak_PR_Object_Event_Object_Restack;
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_LAYER_SET 56
typedef struct {
   int                       l;
}                            Evoak_PR_Object_Event_Object_Layer_Set;
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_SHOW 57
#define                      EVOAK_PR_OBJECT_EVENT_OBJECT_HIDE 58
#define                      EVOAK_PR_OBJECT_EDJE_FILE_SET 59
typedef struct {
   char                     *file;
   char                     *group;
}                            Evoak_PR_Object_Edje_File_Set;
/***/
#define                      EVOAK_PR_OBJECT_EDJE_SWALLOW 60
typedef struct {
   int                       swallow_id;
   char                     *part;
}                            Evoak_PR_Object_Edje_Swallow;
#define                      EVOAK_PR_OBJECT_EDJE_UNSWALLOW 61
typedef struct {
   int                       swallow_id;
}                            Evoak_PR_Object_Edje_Unswallow;
#define                      EVOAK_PR_OBJECT_EDJE_TEXT_SET 62
typedef struct {
   char                     *part;
   char                     *text;
}                            Evoak_PR_Object_Edje_Text_Set;
#define                      EVOAK_PR_OBJECT_EDJE_TEXT_CHANGED 63
typedef struct {
   char                     *part;
   char                     *text;
}                            Evoak_PR_Object_Edje_Text_Changed;
#define                      EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN 64
typedef struct {
   int                       callback_id;
   char                     *emission;
   char                     *source;
}                            Evoak_PR_Object_Edje_Signal_Listen;
#define                      EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN 65
typedef struct {
   int                       callback_id;
   char                     *emission;
   char                     *source;
}                            Evoak_PR_Object_Edje_Signal_Unlisten;
#define                      EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT 66
typedef struct {
   char                     *emission;
   char                     *source;
}                            Evoak_PR_Object_Edje_Signal_Emit;
#define                      EVOAK_PR_OBJECT_EDJE_SIGNAL 67
typedef struct {
   int                       callback_id;
   char                     *emission;
   char                     *source;
}                            Evoak_PR_Object_Edje_Signal;
/***/

#define                      EVOAK_PR_LAST 68

extern Evoak_Protocol _evoak_proto[];

void _evoak_protocol_init(void);
void _evoak_protocol_shutdown(void);
    
#endif

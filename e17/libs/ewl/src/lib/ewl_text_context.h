/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEXT_CONTEXT_H
#define EWL_TEXT_CONTEXT_H

/**
 * @addtogroup Ewl_Text_Context Ewl_Text_Context: A text formatting context
 * @brief Defines a class for text formatting information
 *
 * @{
 */

/**
 * A context node for formatting information
 */
typedef struct Ewl_Text_Context Ewl_Text_Context;

/**
 * @brief Stores context information for the different formatting nodes
 */
struct Ewl_Text_Context
{
                                 
        const char *font;        /**< Font name */
        const char *font_source; /**< The font source */
        unsigned int styles;     /**< Styles set in this node */
        unsigned int align;      /**< Text alignment */
        Ewl_Text_Wrap wrap;      /**< Text wrap setting */
        char size;               /**< Font size */
        Ewl_Color_Set color;     /**< Font colour */

        struct
        {
                Ewl_Color_Set bg;               /**< Background colour */
                Ewl_Color_Set glow;             /**< Glow colour */
                Ewl_Color_Set outline;          /**< Outline colour */
                Ewl_Color_Set shadow;           /**< Shadow colour */
                Ewl_Color_Set strikethrough;    /**< Strikethrough colour */
                Ewl_Color_Set underline;        /**< Underline colour */
                Ewl_Color_Set double_underline; /**< Double underline colour */
        } style_colors;          /**< Colour information */

        unsigned int ref_count;  /**< Number of references to this context */
        const char *format;      /**< This context's format string */
};

int               ewl_text_context_init(void);
void              ewl_text_context_shutdown(void);

Ewl_Text_Context *ewl_text_context_new(void);

Ewl_Text_Context *ewl_text_context_find(Ewl_Text_Context *tx,
                        unsigned int context_mask, Ewl_Text_Context *tx_change);

void              ewl_text_context_acquire(Ewl_Text_Context *tx);
int               ewl_text_context_release(Ewl_Text_Context *tx);

void              ewl_text_context_print(Ewl_Text_Context *tx, const char *indent);
void              ewl_text_context_format_string_create(Ewl_Text_Context *ctx);

/**
 * @}
 */

#endif


/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEXT_FMT_H
#define EWL_TEXT_FMT_H

#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_text_context.h"

/**
 * @addtogroup Ewl_Text_Fmt Ewl_Text_Fmt: Contains the text formatting
 * information
 * @brief Defines a class for text formatting information
 *
 * @{
 */

/**
 * Ewl_Text_Fmt_Node
 */
typedef struct Ewl_Text_Fmt_Node Ewl_Text_Fmt_Node;

/**
 * @brief Text formatting node
 */
struct Ewl_Text_Fmt_Node
{
        Ewl_Text_Context *tx;                /**< Nodes formatting context */
        unsigned int char_len;                /**< Node character length */
        unsigned int byte_len;                /**< Node byte length */
};

/**
 * Ewl_Text_Fmt
 */
typedef struct Ewl_Text_Fmt Ewl_Text_Fmt;

/**
 * @brief Text formatting information
 */
struct Ewl_Text_Fmt
{
        Ecore_DList *nodes;                /**< List of formatting nodes */
        void *text;                        /**< Parent Ewl_Text pointer */

        struct
        {
                unsigned int char_idx;        /**< Character index of the current node pointer */
                unsigned int byte_idx;        /**< Byte index of the current node pointer */
        } current_node;                        /**< Current node information */

        struct
        {
                unsigned int char_len;        /**< Text character length */
                unsigned int byte_len;        /**< Text byte length */
        } length;                        /**< Length information for this formatting list */
};

Ewl_Text_Fmt        *ewl_text_fmt_new(Ewl_Text *t);
void                 ewl_text_fmt_destroy(Ewl_Text_Fmt *fmt);
void                 ewl_text_fmt_clear(Ewl_Text_Fmt *fmt);

void                 ewl_text_fmt_node_prepend(Ewl_Text_Fmt *fmt,
                                        Ewl_Text_Context *tx,
                                        unsigned int char_len,
                                        unsigned int byte_len);
void                 ewl_text_fmt_node_append(Ewl_Text_Fmt *fmt,
                                        Ewl_Text_Context *tx,
                                        unsigned int char_len,
                                        unsigned int byte_len);
void                 ewl_text_fmt_node_insert(Ewl_Text_Fmt *fmt,
                                        unsigned int char_idx,
                                        Ewl_Text_Context *tx,
                                        unsigned int char_len,
                                        unsigned int byte_len);
void                 ewl_text_fmt_node_delete(Ewl_Text_Fmt *fmt,
                                        unsigned int idx,
                                        unsigned int char_len);
unsigned int         ewl_text_fmt_node_count_get(Ewl_Text_Fmt *fmt);

Ewl_Text_Fmt_Node   *ewl_text_fmt_get(Ewl_Text_Fmt *fmt, unsigned int char_idx);
Ewl_Text_Fmt_Node   *ewl_text_fmt_get_last(Ewl_Text_Fmt *fmt);
Ewl_Text_Fmt_Node   *ewl_text_fmt_get_first(Ewl_Text_Fmt *fmt);
Ewl_Text_Fmt_Node   *ewl_text_fmt_get_current(Ewl_Text_Fmt *fmt);

void                 ewl_text_fmt_goto(Ewl_Text_Fmt *fmt, unsigned int idx);

void                 ewl_text_fmt_apply(Ewl_Text_Fmt *fmt, unsigned int context_mask,
                                        Ewl_Text_Context *change,
                                        unsigned int char_idx,
                                        unsigned int char_len);

void                 ewl_text_fmt_char_to_byte(Ewl_Text_Fmt *fmt,
                                unsigned int char_idx, unsigned int char_len,
                                unsigned int *byte_idx, unsigned int *byte_len);
void                 ewl_text_fmt_byte_to_char(Ewl_Text_Fmt *fmt,
                                unsigned int byte_idx, unsigned int byte_len,
                                unsigned int *char_idx, unsigned int *char_len);

void                 ewl_text_fmt_walk(Ewl_Text_Fmt *fmt,
                                void (*cb)(Ewl_Text_Fmt_Node *, Ewl_Text *,
                                                unsigned int byte_idx));

void                 ewl_text_fmt_dump(Ewl_Text_Fmt *fmt);


Ewl_Text_Fmt_Node   *ewl_text_fmt_node_new(Ewl_Text_Context *tx,
                                        unsigned int char_len,
                                               unsigned int byte_len);
/**
 * @}
 */

#endif


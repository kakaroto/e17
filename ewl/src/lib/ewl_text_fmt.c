/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_text_fmt.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

static void ewl_text_fmt_node_free(void *node);

/**
 * @param t: The parent Ewl_Text widget
 * @return Returns a new Ewl_Text_Fmt structure on success or NULL on
 * failure
 * @brief Creates and initializes a new Ewl_Text_Fmt structure
 */
Ewl_Text_Fmt *
ewl_text_fmt_new(Ewl_Text *t)
{
        Ewl_Text_Fmt *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);

        fmt = NEW(Ewl_Text_Fmt, 1);
        if (!fmt) DRETURN_PTR(NULL, DLEVEL_STABLE);

        fmt->nodes = ecore_dlist_new();
        if (!fmt->nodes)
        {
                ewl_text_fmt_destroy(fmt);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }
        ecore_dlist_free_cb_set(fmt->nodes, ewl_text_fmt_node_free);

        fmt->text = t;

        DRETURN_PTR(fmt, DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to destroy
 * @return Returns no value
 * @brief Cleans up the memory used by the Ewl_Text_Fmt structure
 */
void
ewl_text_fmt_destroy(Ewl_Text_Fmt *fmt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        if (fmt->nodes) ecore_dlist_destroy(fmt->nodes);
        FREE(fmt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @return Returns no value
 * @brief Clears the formatting information
 */
void
ewl_text_fmt_clear(Ewl_Text_Fmt *fmt)
{
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        while ((node = ecore_dlist_first_remove(fmt->nodes)))
        {
                if (node->tx) ewl_text_context_release(node->tx);
                FREE(node);
        }

        fmt->current_node.char_idx = 0;
        fmt->current_node.byte_idx = 0;

        fmt->length.char_len = 0;
        fmt->length.byte_len = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param tx: The context to prepend
 * @param char_len: The character length being prepended
 * @param byte_len: The byte length being prepended
 * @return Returns no value
 * @brief Prepends the @a tx for @a char_len bytes to @a fmt
 */
void
ewl_text_fmt_node_prepend(Ewl_Text_Fmt *fmt, Ewl_Text_Context *tx,
                        unsigned int char_len, unsigned int byte_len)
{
        Ewl_Text_Fmt_Node *node = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        fmt->length.char_len += char_len;
        fmt->length.byte_len += byte_len;

        if (tx)
        {
                node = ewl_text_fmt_node_new(tx, char_len, byte_len);
                ecore_dlist_prepend(fmt->nodes, node);
        }
        else
        {
                node = ewl_text_fmt_get_first(fmt);
                node->char_len += char_len;
                node->byte_len += byte_len;
        }

        /* we prepended so we need to add the nodes lengths to the current
         * format position length so we're still in the right spot */
        fmt->current_node.char_idx += char_len;
        fmt->current_node.byte_idx += byte_len;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param tx: The context to append
 * @param char_len: The character length to append
 * @param byte_len: The byte length to append
 * @return Returns no value
 * @brief Appends @a tx for @a char_len bytes to @a fmt.
 */
void
ewl_text_fmt_node_append(Ewl_Text_Fmt *fmt, Ewl_Text_Context *tx,
                        unsigned int char_len, unsigned int byte_len)
{
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        node = ewl_text_fmt_get_last(fmt);
        if (tx && (!node || (node->char_len > 0)))
        {
                node = ewl_text_fmt_node_new(tx, char_len, byte_len);
                ecore_dlist_append(fmt->nodes, node);
        }
        else
        {
                /* if we've got a new context to use and our current
                 * formatting node has no text in it replace its context
                 * with the new one */
                if (tx && (node->char_len == 0))
                {
                        ewl_text_context_release(node->tx);
                        node->tx = tx;
                        ewl_text_context_acquire(node->tx);
                }

                node->char_len += char_len;
                node->byte_len += byte_len;
        }

        fmt->length.char_len += char_len;
        fmt->length.byte_len += byte_len;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param idx: The index to insert into
 * @param tx: The context to append
 * @param char_len: The character length to append
 * @param byte_len: The byte length to append
 * @return Returns no value
 * @brief Inserts @a tx at @a idx for @a char_len bytes to @a fmt.
 */
void
ewl_text_fmt_node_insert(Ewl_Text_Fmt *fmt, unsigned int idx,
                        Ewl_Text_Context *tx, unsigned int char_len,
                        unsigned int byte_len)
{
        Ewl_Text_Fmt_Node *node = NULL, *cur_node, *left = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        fmt->length.char_len += char_len;
        fmt->length.byte_len += byte_len;

        if (!tx)
        {
                node = ewl_text_fmt_get_current(fmt);
                node->char_len += char_len;
                node->byte_len += byte_len;

                DRETURN(DLEVEL_STABLE);
        }

        node = ewl_text_fmt_node_new(tx, char_len, byte_len);
        cur_node = ecore_dlist_current(fmt->nodes);

        /* insert at the current location */
        if (fmt->current_node.char_idx == idx)
                ecore_dlist_insert(fmt->nodes, node);

        /* insert at the end of the current node */
        else if ((fmt->current_node.char_idx + cur_node->char_len) == idx)
        {
                ecore_dlist_next(fmt->nodes);
                if (ecore_dlist_current(fmt->nodes))
                {
                        ecore_dlist_insert(fmt->nodes, node);

                        /* update our index as we're currently on the next node */
                        fmt->current_node.char_idx += cur_node->char_len;
                        fmt->current_node.byte_idx += cur_node->byte_len;
                }
                else
                {
                        ecore_dlist_append(fmt->nodes, node);
                        ecore_dlist_last_goto(fmt->nodes);

                        /* we haven't incremented the sizes yet so we're at
                         * the end of the last set of nodes */
                        fmt->current_node.char_idx = fmt->length.char_len;
                        fmt->current_node.byte_idx = fmt->length.byte_len;
                }
        }
        else
        {
                /* insert into the middle of the current node */
                left = ewl_text_fmt_node_new(node->tx,
                                idx - fmt->current_node.char_idx, 0);
                ewl_text_fmt_char_to_byte(fmt, idx,
                                        idx - fmt->current_node.char_idx,
                                        NULL, &(left->byte_len));

                cur_node->char_len -= left->char_len;
                cur_node->byte_len -= left->byte_len;

                /* insert inserts to the right, so move past the current node so we
                 * can insert the two new ones. If we have a node to the left we
                 * insert the left node then the new node so the new one is in the
                 * middle. If there is no node to the left we insert the new node
                 * then the left node. */
                ecore_dlist_next(fmt->nodes);
                if (ecore_dlist_current(fmt->nodes))
                {
                        ecore_dlist_insert(fmt->nodes, left);
                        ecore_dlist_insert(fmt->nodes, node);
                }
                else
                {
                        ecore_dlist_append(fmt->nodes, node);
                        ecore_dlist_last_goto(fmt->nodes);
                        ecore_dlist_append(fmt->nodes, left);
                }
                fmt->current_node.char_idx += cur_node->char_len;
                fmt->current_node.byte_idx += cur_node->byte_len;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param idx: The index to delete from
 * @param char_len: The character length to delete
 * @return Returns no value
 * @brief Removes any formatting for @a char_len bytes starting at @a idx
 */
void
ewl_text_fmt_node_delete(Ewl_Text_Fmt *fmt, unsigned int idx,
                                        unsigned int char_len)
{
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        if (idx < fmt->current_node.char_idx)
        {
                DWARNING("The current position (%u) is higher than the to "
                                "be to remove position (%u)\n",
                                fmt->current_node.char_idx, idx);
        }
        /* adjust the char length, we'll adjust the byte length later */
        fmt->length.char_len -= char_len;

        node = ecore_dlist_current(fmt->nodes);
        while (char_len > 0)
        {
                unsigned int available;

                /* how much space is available in this node to be removed */
                available = node->char_len - (idx - fmt->current_node.char_idx);
                if (available > char_len) available = char_len;

                node->char_len -= available;
                if (node->char_len > 0)
                {
                        unsigned int byte_len = 0;

                        /* Note: we already removed the related text part
                         * so it is save to use the new shorter text here */
                        ewl_text_fmt_char_to_byte(fmt,
                                        fmt->current_node.char_idx,
                                        node->char_len,
                                        NULL, &byte_len);

                        /* deduct the byte count */
                        fmt->length.byte_len -= node->byte_len - byte_len;
                        node->byte_len = byte_len;

                        fmt->current_node.char_idx += node->char_len;
                        fmt->current_node.byte_idx += node->byte_len;
                        ecore_dlist_next(fmt->nodes);
                        node = ecore_dlist_current(fmt->nodes);
                }
                else
                {
                        /* we still need to adjust the global byte count */
                        fmt->length.byte_len -= node->byte_len;

                        ecore_dlist_remove(fmt->nodes);
                        ewl_text_fmt_node_free(node);
                        node = ecore_dlist_current(fmt->nodes);
                }
                char_len -= available;
        }

        /* make sure we've still got a current node. if not reset to the
         * first node in the list */
        node = ecore_dlist_current(fmt->nodes);
        if (!node)
        {
                ecore_dlist_first_goto(fmt->nodes);
                fmt->current_node.char_idx = 0;
                fmt->current_node.byte_idx = 0;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @return Returns the number of formatting nodes
 * @brief Retrives the number of nodes in the formatting information
 */
unsigned int
ewl_text_fmt_node_count_get(Ewl_Text_Fmt *fmt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fmt, 0);

        DRETURN_INT(ecore_dlist_count(fmt->nodes), DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @return Returns the last node in the formatting information
 * @brief Retrives the last formatting node
 */
Ewl_Text_Fmt_Node *
ewl_text_fmt_get_last(Ewl_Text_Fmt *fmt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fmt, NULL);

        DRETURN_PTR(ecore_dlist_last_goto(fmt->nodes),DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @return Returns the first node in the formatting information
 * @brief Retrives the first formatting node
 */
Ewl_Text_Fmt_Node *
ewl_text_fmt_get_first(Ewl_Text_Fmt *fmt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fmt, NULL);

        DRETURN_PTR(ecore_dlist_first_goto(fmt->nodes), DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @return Returns the current node in the formatting information
 * @brief Retrives the current formatting node
 */
Ewl_Text_Fmt_Node *
ewl_text_fmt_get_current(Ewl_Text_Fmt *fmt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fmt, NULL);

        DRETURN_PTR(ecore_dlist_current(fmt->nodes), DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param idx: The index to get the node for
 * @return Returns the node containing the character index @a idx
 * @brief Returns the character index @a idx node from @a fmt
 */
Ewl_Text_Fmt_Node *
ewl_text_fmt_get(Ewl_Text_Fmt *fmt, unsigned int idx)
{
        Ewl_Text_Fmt_Node *node, *current_node;
        unsigned int cur_byte, cur_char;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(fmt, NULL);

        cur_char = fmt->current_node.char_idx;
        cur_byte = fmt->current_node.byte_idx;
        current_node = ecore_dlist_current(fmt->nodes);

        ewl_text_fmt_goto(fmt, idx);
        node = ecore_dlist_current(fmt->nodes);

        ecore_dlist_goto(fmt->nodes, current_node);
        fmt->current_node.char_idx = cur_char;
        fmt->current_node.byte_idx = cur_byte;

        DRETURN_PTR(node, DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_fmt to work with
 * @param idx: The index to go to
 * @return Returns no value
 * @brief Positions the last at character index @a idx
 */
void
ewl_text_fmt_goto(Ewl_Text_Fmt *fmt, unsigned int idx)
{
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        if (fmt->length.char_len != EWL_TEXT(fmt->text)->length.chars)
        {
                DWARNING("The character length of the fmt (%u) is not"
                                " equal to the length of the text (%u)\n",
                                fmt->length.char_len,
                                EWL_TEXT(fmt->text)->length.chars);
        }

        if (idx >= fmt->length.char_len)
        {
                node = ecore_dlist_last_goto(fmt->nodes);

                fmt->current_node.char_idx =
                        fmt->length.char_len - node->char_len;
                fmt->current_node.byte_idx =
                        fmt->length.byte_len - node->byte_len;

                DRETURN(DLEVEL_STABLE);
        }

        node = ecore_dlist_first_goto(fmt->nodes);
        fmt->current_node.char_idx = 0;
        fmt->current_node.byte_idx = 0;

        while ((fmt->current_node.char_idx + node->char_len) <= idx)
        {
                fmt->current_node.char_idx += node->char_len;
                fmt->current_node.byte_idx += node->byte_len;

                ecore_dlist_next(fmt->nodes);
                node = ecore_dlist_current(fmt->nodes);
        }

#if 0
        /* set to start of text */
        if (idx == 0)
        {
                fmt->current_node.char_idx = 0;
                fmt->current_node.byte_idx = 0;
                ecore_dlist_first_goto(fmt->nodes);
        }

        /* set to end of text */
        else if (idx >= fmt->length.char_len)
        {
                node = ecore_dlist_last_goto(fmt->nodes);

                fmt->current_node.char_idx =
                        fmt->length.char_len - node->char_len;
                fmt->current_node.byte_idx =
                        fmt->length.byte_len - node->byte_len;
        }

        /* set to after our current position
          * walk forward until we cover the given position */
        else if (fmt->current_node.char_idx < idx)
        {
                node = ecore_dlist_current(fmt->nodes);
                while ((node->char_len + fmt->current_node.char_idx) < idx)
                {
                        fmt->current_node.char_idx += node->char_len;
                        fmt->current_node.byte_idx += node->byte_len;

                        ecore_dlist_next(fmt->nodes);
                        if (!(node = ecore_dlist_current(fmt->nodes)))
                        {
                                DWARNING("Incorrectly walked off list.\n");
                                node = ecore_dlist_last_goto(fmt->nodes);
                                fmt->current_node.char_idx =
                                        fmt->length.char_len - node->char_len;
                                fmt->current_node.byte_idx =
                                        fmt->length.byte_len - node->byte_len;
                                       break;
                        }
                }
        }

        /* set before our position
         * walk back until we're less then the given position */
        else
        {
                while (fmt->current_node.char_idx > idx)
                {
                        ecore_dlist_previous(fmt->nodes);
                        if (!(node = ecore_dlist_current(fmt->nodes)))
                        {
                                DWARNING("Incorrectly walked off list.\n");
                                node = ecore_dlist_first_goto(fmt->nodes);
                                fmt->current_node.char_idx = 0;
                                fmt->current_node.byte_idx = 0;
                                       break;
                        }

                        fmt->current_node.char_idx -= node->char_len;
                        fmt->current_node.byte_idx -= node->byte_len;
                }
        }

        if (!(node = ecore_dlist_current(fmt->nodes)))
        {
                DWARNING("Unable to place cursor at %d", idx);

                ecore_dlist_first_goto(fmt->nodes);
                fmt->current_node.char_idx = 0;
                fmt->current_node.byte_idx = 0;
        }
#endif

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to dump the formatting from
 * @return Returns no value
 * @brief Prints out the formatting information
 */
void
ewl_text_fmt_dump(Ewl_Text_Fmt *fmt)
{
        Ewl_Text_Fmt_Node *node, *cur_node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        cur_node = ecore_dlist_current(fmt->nodes);
        ecore_dlist_first_goto(fmt->nodes);

        while ((node = ecore_dlist_next(fmt->nodes)))
        {
                printf("%d chars, %d bytes\n", node->char_len, node->byte_len);
                ewl_text_context_print(node->tx, "    ");
                printf("\n\n");
        }

        ecore_dlist_goto(fmt->nodes, cur_node);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param char_idx: The character index to convert
 * @param char_len: The cahracter length to convert
 * @param byte_idx: Where to store the byte index
 * @param byte_len: Where to store the byte length
 * @return Returns no value
 * @brief Calculates the byte index for the given @a char_idx and stores
 * into @a byte_idx. Also calculates the @a byte_len for @a char_len
 */
/* This _HAS_ to leave the list with the same current pointer is when it
 * started */
void
ewl_text_fmt_char_to_byte(Ewl_Text_Fmt *fmt,
                        unsigned int char_idx, unsigned int char_len,
                        unsigned int *byte_idx, unsigned int *byte_len)
{
        Ewl_Text *t;
        Ewl_Text_Fmt_Node *current_node, *node = NULL;
        unsigned int cur_char_idx = 0, bidx = 0;
        void *(*move)(Ecore_DList *list);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        t = fmt->text;
        current_node = ecore_dlist_current(fmt->nodes);
        cur_char_idx = fmt->current_node.char_idx;
        bidx = fmt->current_node.byte_idx;

        /* first step is to find the actual node that this text represents */
        if (fmt->current_node.char_idx < char_idx)
        {
                /* walk forward until we cover the given position */
                node = ecore_dlist_current(fmt->nodes);
                while ((node->char_len + cur_char_idx) < char_idx)
                {
                        cur_char_idx += node->char_len;
                        bidx += node->byte_len;

                        ecore_dlist_next(fmt->nodes);
                        node = ecore_dlist_current(fmt->nodes);
                        if (!node) break;
                }
                move = ecore_dlist_previous;

                if (!node)
                {
                        node = ecore_dlist_last_goto(fmt->nodes);
                        fmt->current_node.char_idx = fmt->length.char_len - node->char_len;
                        fmt->current_node.byte_idx = fmt->length.byte_len - node->byte_len;
                }
        }
        else
        {
                /* walk back until we're less then the given position */
                while (cur_char_idx > char_idx)
                {
                        ecore_dlist_previous(fmt->nodes);
                        node = ecore_dlist_current(fmt->nodes);
                        if (!node) break;

                        cur_char_idx -= node->char_len;
                        bidx -= node->byte_len;
                }
                move = ecore_dlist_next;

                if (!node)
                {
                        ecore_dlist_first_goto(fmt->nodes);
                        fmt->current_node.char_idx = 0;
                        fmt->current_node.byte_idx = 0;
                }
        }

        /* we still need to count within this node */
        while (cur_char_idx < char_idx)
        {
                unsigned int bytes;

                ewl_text_text_next_char(t->text + bidx, &bytes);
                bidx += bytes;
                cur_char_idx ++;
        }

        if (byte_len)
        {
                if (char_len == 0) *byte_len = 0;
                else
                {
                        char *txt;

                        txt = t->text + bidx;
                        cur_char_idx = 0;
                        while (cur_char_idx < char_len)
                        {
                                unsigned int bytes;

                                txt = ewl_text_text_next_char(txt, &bytes);
                                *byte_len += bytes;
                                cur_char_idx ++;
                        }
                }
        }

        if (byte_idx) *byte_idx = bidx;

        node = ecore_dlist_current(fmt->nodes);
        while (node != current_node)
        {
                move(fmt->nodes);
                node = ecore_dlist_current(fmt->nodes);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param byte_idx: Where to store the byte index
 * @param byte_len: Where to store the byte length
 * @param char_idx: The character index to convert
 * @param char_len: The cahracter length to convert
 * @return Returns no value
 * @brief Calculates the char index for the given @a byte_idx and stores
 * into @a char_idx. Also calculates the @a char_len for @a byte_len
 */
void
ewl_text_fmt_byte_to_char(Ewl_Text_Fmt *fmt,
                        unsigned int byte_idx, unsigned int byte_len,
                        unsigned int *char_idx, unsigned int *char_len)
{
        Ewl_Text *t;
        Ewl_Text_Fmt_Node *current_node, *node = NULL;
        unsigned int byte_count = 0, cidx = 0;
        void *(*move)(Ecore_DList *list);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);

        t = fmt->text;
        current_node = ecore_dlist_current(fmt->nodes);
        if (fmt->current_node.byte_idx < byte_idx)
        {
                byte_count = fmt->current_node.byte_idx;
                cidx = fmt->current_node.char_idx;

                /* walk forward until we cover the given position */
                node = ecore_dlist_current(fmt->nodes);
                while ((node->byte_len + byte_count) < byte_idx)
                {
                        byte_count += node->byte_len;
                        cidx += node->char_len;

                        ecore_dlist_next(fmt->nodes);
                        node = ecore_dlist_current(fmt->nodes);
                        if (!node) break;
                }

                if (!node)
                {
                        node = ecore_dlist_last_goto(fmt->nodes);
                        fmt->current_node.char_idx = fmt->length.char_len - node->char_len;
                        fmt->current_node.byte_idx = fmt->length.byte_len - node->byte_len;
                }
                move = ecore_dlist_previous;
        }
        else
        {
                /* walk back until we're less then the given position */
                node = ecore_dlist_current(fmt->nodes);
                while (byte_count > byte_idx)
                {
                        ecore_dlist_previous(fmt->nodes);
                        node = ecore_dlist_current(fmt->nodes);
                        if (!node) break;

                        byte_count -= node->byte_len;
                        cidx -= node->char_len;
                }

                if (!node)
                {
                        ecore_dlist_first_goto(fmt->nodes);
                        fmt->current_node.char_idx = 0;
                        fmt->current_node.byte_idx = 0;
                }
                move = ecore_dlist_next;
        }

        /* we still need to count within this node */
        while (byte_count < byte_idx)
        {
                unsigned int bytes;

                ewl_text_text_next_char(t->text + byte_count, &bytes);
                byte_count += bytes;
                cidx ++;
        }

        if (char_len)
        {
                if (byte_len == 0) *char_len = 0;
                else
                {
                        char *txt;

                        txt = t->text + byte_idx;
                        byte_count = 0;
                        while (byte_count < byte_len)
                        {
                                unsigned int bytes;

                                txt = ewl_text_text_next_char(txt, &bytes);
                                byte_count += bytes;
                                (*char_len) ++;
                        }
                }
        }

        if (char_idx) *char_idx = cidx;

        node = ecore_dlist_current(fmt->nodes);
        while (node != current_node)
        {
                move(fmt->nodes);
                node = ecore_dlist_current(fmt->nodes);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param cb: The function to call for each formatting node
 * @return Returns no value
 * @brief Walks the formatting information and calls @a cb for each node
 */
void
ewl_text_fmt_walk(Ewl_Text_Fmt *fmt,
        void (*cb)(Ewl_Text_Fmt_Node *, Ewl_Text *, unsigned int byte_idx))
{
        Ewl_Text_Fmt_Node *current_node, *node;
        unsigned int byte_idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(fmt);
        DCHECK_PARAM_PTR(cb);

        current_node = ecore_dlist_current(fmt->nodes);
        ecore_dlist_first_goto(fmt->nodes);
        while ((node = ecore_dlist_next(fmt->nodes)))
        {
                cb(node, fmt->text, byte_idx);
                byte_idx += node->byte_len;
        }
        ecore_dlist_goto(fmt->nodes, current_node);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fmt: The Ewl_Text_Fmt to work with
 * @param context_mask: The set of changes in this context
 * @param change: The context containing the changes
 * @param char_idx: The character index to apply too
 * @param char_len: The character length to apply too
 * @return Returns no value
 * @brief Applies the @a change set to @a fmt starting at @a char_idx for @a
 * char_len characters
 */
void
ewl_text_fmt_apply(Ewl_Text_Fmt *fmt, unsigned int context_mask,
                        Ewl_Text_Context *change, unsigned int char_idx,
                        unsigned int char_len)
{
        unsigned int cursor_idx;
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(change);

        if ((char_len == 0) || (context_mask == EWL_TEXT_CONTEXT_MASK_NONE))
                DRETURN(DLEVEL_STABLE);

        cursor_idx = char_idx;

        /* make sure we're where we think we are */
        ewl_text_fmt_goto(fmt, cursor_idx);

        node = ecore_dlist_current(fmt->nodes);
        while (char_len > 0)
        {
                /* we've walked off the end of the list */
                if (!node) break;

                /* covers entire node */
                if ((char_idx == fmt->current_node.char_idx)
                                && (node->char_len <= char_len))
                {
                        Ewl_Text_Context *new_tx;

                        new_tx = ewl_text_context_find(node->tx,
                                                context_mask, change);
                        ewl_text_context_release(node->tx);
                        node->tx = new_tx;

                        char_idx += node->char_len;
                        char_len -= node->char_len;
                        ecore_dlist_next(fmt->nodes);

                        fmt->current_node.char_idx += node->char_len;
                        fmt->current_node.byte_idx += node->byte_len;
                }

                /* start is the same, node is longer then needed */
                else if (char_idx == fmt->current_node.char_idx)
                {
                        unsigned int blen = 0;
                        Ewl_Text_Fmt_Node *new;

                        ewl_text_fmt_char_to_byte(fmt,
                                        char_idx, char_len, NULL, &blen);

                        new = ewl_text_fmt_node_new(
                                ewl_text_context_find(node->tx,
                                                context_mask, change),
                                                char_len, blen);

                        node->char_len -= new->char_len;
                        node->byte_len -= new->byte_len;

                        ecore_dlist_insert(fmt->nodes, new);
                        ecore_dlist_next(fmt->nodes);

                        fmt->current_node.char_idx += new->char_len;
                        fmt->current_node.byte_idx += new->byte_len;

                        char_idx += new->char_len;
                        char_len -= new->char_len;
                }

                /* starts are different, need to set context on end of node */
                else
                {
                        Ewl_Text_Fmt_Node *new;
                        unsigned int blen = 0, right_skip;

                        if (char_idx < fmt->current_node.char_idx)
                        {
                                DWARNING("Cursor index less than formatting node index");
                                right_skip = 0;
                        }
                        else
                                right_skip = char_idx - fmt->current_node.char_idx;
                        ewl_text_fmt_char_to_byte(fmt, fmt->current_node.char_idx,
                                                        right_skip, NULL, &blen);

                        new = ewl_text_fmt_node_new(node->tx, right_skip, blen);

                        ecore_dlist_insert(fmt->nodes, new);
                        ecore_dlist_next(fmt->nodes);
                        if (!ecore_dlist_current(fmt->nodes))
                                ecore_dlist_last_goto(fmt->nodes);

                        fmt->current_node.char_idx += new->char_len;
                        fmt->current_node.byte_idx += new->byte_len;

                        /* update node to be the new formatting piece */
                        node->tx = ewl_text_context_find(node->tx,
                                                context_mask, change);
                        node->char_len -= new->char_len;
                        node->byte_len -= new->byte_len;

                        /* the rest of the node is covered */
                        if (node->char_len <= char_len)
                        {
                                char_len -= node->char_len;
                                char_idx += node->char_len;

                                ecore_dlist_next(fmt->nodes);
                                fmt->current_node.char_idx += node->char_len;
                                fmt->current_node.byte_idx += node->byte_len;
                        }

                        /* we need to split the node */
                        else
                        {
                                Ewl_Text_Context *tx;
                                unsigned int blen = 0;

                                tx = new->tx;
                                ewl_text_fmt_char_to_byte(fmt,
                                                char_idx, char_len,
                                                NULL, &blen);

                                /* we create a new node which will be our
                                 * formatting and reset node to be original
                                 * formatting */
                                new = ewl_text_fmt_node_new(node->tx,
                                                char_len, blen);

                                ecore_dlist_insert(fmt->nodes, new);
                                ecore_dlist_next(fmt->nodes);
                                if (!ecore_dlist_current(fmt->nodes))
                                        ecore_dlist_last_goto(fmt->nodes);

                                fmt->current_node.char_idx += new->char_len;
                                fmt->current_node.byte_idx += new->byte_len;

                                node->tx = tx;
                                node->char_len -= new->char_len;
                                node->byte_len -= new->byte_len;

                                char_len -= new->char_len;
                                char_idx += new->char_len;
                        }
                }
                node = ecore_dlist_current(fmt->nodes);
        }

        /* Reset to the original cursor index */
        if (!node) ecore_dlist_last_goto(fmt->nodes);
        ewl_text_fmt_goto(fmt, cursor_idx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Text_Fmt_Node stuff
 */

/**
 * @param tx: The context to set
 * @param char_len: The length of this node
 * @param byte_len: The byte length of this node
 * @return Returns a new Ewl_Text_Fmt_Node no success or NULL on failure
 * @brief Creates a new Ewl_Text_Fmt_Node structure
 */
Ewl_Text_Fmt_Node *
ewl_text_fmt_node_new(Ewl_Text_Context *tx, unsigned int char_len,
                                        unsigned int byte_len)
{
        Ewl_Text_Fmt_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);

        node = NEW(Ewl_Text_Fmt_Node, 1);
        if (!node) DRETURN_PTR(NULL, DLEVEL_STABLE);

        node->tx = tx;
        ewl_text_context_acquire(node->tx);
        node->char_len = char_len;
        node->byte_len = byte_len;

        DRETURN_PTR(node, DLEVEL_STABLE);
}

static void
ewl_text_fmt_node_free(void *node)
{
        Ewl_Text_Fmt_Node *n;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(node);

        n = node;
        if (n->tx) ewl_text_context_release(n->tx);
        n->tx = NULL;

        FREE(node);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


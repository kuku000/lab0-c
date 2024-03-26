/* Linux-like doubly-linked list implementation */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* "typeof" is a GNU extension.
 * Reference: https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
 */
#if defined(__GNUC__) || defined(__clang__)
#define __LIST_HAVE_TYPEOF_H 1
#endif

#ifndef container_of
#ifdef __LIST_HAVE_TYPEOF_H
#define container_of(ptr, type, member)                            \
    __extension__({                                                \
        const __typeof__(((type *) 0)->member) *__pmember = (ptr); \
        (type *) ((char *) __pmember - offsetof(type, member));    \
    })
#else
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) -offsetof(type, member)))
#endif
#endif

#define LIST_HEAD_H(head) struct list_head head = {&(head), &(head)}

static inline void INIT_LIST_HEAD_H(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}

static inline void list_add_h(struct list_head *node, struct list_head *head)
{
    struct list_head *next = head->next;

    next->prev = node;
    node->next = next;
    node->prev = head;
    head->next = node;
}

static inline void list_add_tail_h(struct list_head *node,
                                   struct list_head *head)
{
    struct list_head *prev = head->prev;

    prev->next = node;
    node->next = head;
    node->prev = prev;
    head->prev = node;
}

static inline void list_del_h(struct list_head *node)
{
    struct list_head *next = node->next;
    struct list_head *prev = node->prev;

    next->prev = prev;
    prev->next = next;

#ifdef LIST_POISONING
    node->prev = (struct list_head *) (0x00100100);
    node->next = (struct list_head *) (0x00200200);
#endif
}

static inline void list_del_init_h(struct list_head *node)
{
    list_del(node);
    INIT_LIST_HEAD(node);
}

static inline int list_empty_h(const struct list_head *head)
{
    return (head->next == head);
}

static inline int list_is_singular_h(const struct list_head *head)
{
    return (!list_empty_h(head) && head->prev == head->next);
}

static inline void list_splice_h(struct list_head *list, struct list_head *head)
{
    struct list_head *head_first = head->next;
    struct list_head *list_first = list->next;
    struct list_head *list_last = list->prev;

    if (list_empty_h(list))
        return;

    head->next = list_first;
    list_first->prev = head;

    list_last->next = head_first;
    head_first->prev = list_last;
}

static inline void list_splice_tail_h(struct list_head *list,
                                      struct list_head *head)
{
    struct list_head *head_last = head->prev;
    struct list_head *list_first = list->next;
    struct list_head *list_last = list->prev;

    if (list_empty_h(list))
        return;

    head->prev = list_last;
    list_last->next = head;

    list_first->prev = head_last;
    head_last->next = list_first;
}

static inline void list_splice_init_h(struct list_head *list,
                                      struct list_head *head)
{
    list_splice_h(list, head);
    INIT_LIST_HEAD_h(list);
}

static inline void list_splice_tail_init_h(struct list_head *list,
                                           struct list_head *head)
{
    list_splice_tail_h(list, head);
    INIT_LIST_HEAD_H(list);
}

static inline void list_cut_position_h(struct list_head *head_to,
                                       struct list_head *head_from,
                                       struct list_head *node)
{
    struct list_head *head_from_first = head_from->next;

    if (list_empty_h(head_from))
        return;

    if (head_from == node) {
        INIT_LIST_HEAD_H(head_to);
        return;
    }

    head_from->next = node->next;
    head_from->next->prev = head_from;

    head_to->prev = node;
    node->next = head_to;
    head_to->next = head_from_first;
    head_to->next->prev = head_to;
}

static inline void list_move_h(struct list_head *node, struct list_head *head)
{
    list_del_h(node);
    list_add_h(node, head);
}

static inline void list_move_tail_h(struct list_head *node,
                                    struct list_head *head)
{
    list_del_h(node);
    list_add_tail_h(node, head);
}

#define list_entry_h(node, type, member) container_of(node, type, member)


#define list_first_entry_h(head, type, member) \
    list_entry_h((head)->next, type, member)

#define list_last_entry_h(head, type, member) \
    list_entry_h((head)->prev, type, member)

#define list_for_each_h(node, head) \
    for (node = (head)->next; node != (head); node = node->next)

#ifdef __LIST_HAVE_TYPEOF_H
#define list_for_each_entry_h(entry, head, member)                     \
    for (entry = list_entry((head)->next, __typeof__(*entry), member); \
         &entry->member != (head);                                     \
         entry = list_entry(entry->member.next, __typeof__(*entry), member))
#endif

#define list_for_each_safe_h(node, safe, head)                   \
    for (node = (head)->next, safe = node->next; node != (head); \
         node = safe, safe = node->next)

#define list_for_each_entry_safe_h(entry, safe, head, member)              \
    for (entry = list_entry((head)->next, __typeof__(*entry), member),     \
        safe = list_entry(entry->member.next, __typeof__(*entry), member); \
         &entry->member != (head); entry = safe,                           \
        safe = list_entry(safe->member.next, __typeof__(*entry), member))

#undef __LIST_HAVE_TYPEOF_H

#ifdef __cplusplus
}
#endif

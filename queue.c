#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        free(head);
        return NULL;
    } else {
        head->prev = head;
        head->next = head;
        return head;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *entry, *safe;
    if (!head || list_empty(head)) {
        free(head);
        return;
    }
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *insert = malloc(sizeof(element_t));
    if (!insert) {
        free(insert);
        return false;
    }

    insert->value = (char *) malloc((strlen(s) + 1) * sizeof(char));
    if (!insert->value) {
        free(insert->value);
        free(insert);
        return false;
    }
    strncpy(insert->value, s, strlen(s));
    *(insert->value + strlen(s)) = '\0';
    list_add(&insert->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *insert = malloc(sizeof(element_t));
    if (!insert) {
        return false;
    }
    insert->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!insert->value) {
        free(insert);
        return false;
    }
    strncpy(insert->value, s, strlen(s));
    *(insert->value + strlen(s)) = '\0';
    list_add_tail(&insert->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_element = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, remove_element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return remove_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *remove_element = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, remove_element->value, bufsize - 1);
        *(sp + bufsize - 1) = '\0';
    }
    return remove_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    struct list_head *cur;
    int count = 0;
    list_for_each (cur, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *left = head->prev;
    struct list_head *right = head->next;
    while (true) {
        if (left == right) {
            list_del(right);
            q_release_element(list_entry(right, element_t, list));
            break;
        } else if (right->prev == left) {
            list_del(left);
            q_release_element(list_entry(left, element_t, list));
            break;
        }
        right = right->next;
        left = left->prev;
    }
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *cur = head->next;
    struct list_head *pre_cur;
    int count = 0;
    for (; cur != head->prev; cur = cur->next) {
        if (count > 0) {
            list_del(cur->prev);
            q_release_element(list_entry(cur->prev, element_t, list));
        }
        count = 0;
        for (pre_cur = cur->next; pre_cur != head; pre_cur = pre_cur->next) {
            if (strcmp(list_entry(cur, element_t, list)->value,
                       list_entry(pre_cur, element_t, list)->value) == 0) {
                list_del(pre_cur);
                q_release_element(list_entry(pre_cur, element_t, list));
                count++;
            }
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *cur;
    for (cur = head->next; cur != head && cur != head->prev; cur = cur->next) {
        struct list_head *next = cur->next;
        list_del(cur);
        list_add(cur, next);
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *left = head->prev;
    struct list_head *right = head->next;

    while (left != right && right->prev != left) {
        struct list_head *temp = right;
        list_del(right);
        list_add(right, left);
        list_del(left);
        list_add(left, temp);
        left = left->prev;
        right = right->next;
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    /*if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *safe, *cur;
    list_for_each_safe(cur, safe, head){



    }*/

    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *mergeTwoList(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    if (strcmp(list_entry(l1, element_t, list)->value,
               list_entry(l2, element_t, list)->value) <= 0) {
        head = l1;
        l1 = l1->next;
    } else {
        struct list_head *temp = l1;
        l1 = l2;
        l2 = temp;
        head = l1;
        l1 = l1->next;
    }
    list_del_init(head);
    while (l1->next != head && l2->next != head) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) <= 0) {
            l1 = l1->next;
            list_move_tail(l1->prev, head);

        } else {
            l2 = l2->next;
            list_move_tail(l2->prev, head);
        }
    }
    if (l1->next != head) {
        while (l1->next != head) {
            l1 = l1->next;
            list_move_tail(l1->prev, head);
        }
    } else {
        while (l2->next != head) {
            l2 = l2->next;
            list_move_tail(l2->prev, head);
        }
    }
    return head;
}

/* Sort elements of queue in ascending/descending order */
struct list_head *mergeTwoList(struct list_head *list1,
                               struct list_head *list2);
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *list[100000];
    struct list_head *cur, *safe;
    int count = 0;
    int size = q_size(head);
    list_for_each_safe (cur, safe, head) {
        list[count] = cur;
        list_del_init(list[count]);
        count++;
    }
    for (int interval = 1; interval < size; interval *= 2) {
        for (int i = 0; i + interval < size; i += interval * 2) {
            list[i] = mergeTwoList(list[i], list[i + interval]);
        }
    }
    INIT_LIST_HEAD(head);
    list_add_tail(head, list[0]);
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
/*int q_ascend(struct list_head *head)
{
    if(!head || list_empty(head))
        return 0;
    if(list_is_singular(head))
        return 1;
    struct list_head *cur = head->next;
    while(cur->next != head){
        struct list_head *com = cur->next, *temp;
        while(com != head){
            if(strcmp(list_entry(cur, element_t, list)->value, list_entry(com,
element_t, list)->value ) > 0){ if (com->prev != head) com->prev->next =
com->next; if (com->next != head) com->next->prev = com->prev; temp = com->next;
                list_del_init(com);
                q_release_element(list_entry(com, element_t, list));
                com = temp;
            }
            else{
                com = com->next;
            }

        }
        cur = cur->next;
    }
    return q_size(head);
}*/

int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *slow = head->next;
    element_t *slow_node = list_entry(slow, element_t, list);
    while (slow->next != head) {
        element_t *fast_node = list_entry(slow->next, element_t, list);
        if (strcmp(slow_node->value, fast_node->value) > 0) {
            list_del(&fast_node->list);
            q_release_element(fast_node);
        } else {
            slow = slow->next;
        }
    }
    return q_size(head);

    return q_size(head);
}



/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */

int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *slow = head->prev;
    element_t *slow_node = list_entry(slow, element_t, list);
    while (slow->prev != head) {
        element_t *fast_node = list_entry(slow->prev, element_t, list);
        if (strcmp(slow_node->value, fast_node->value) < 0) {
            list_del(&fast_node->list);
            q_release_element(fast_node);
        } else {
            slow = slow->prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

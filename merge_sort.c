#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define my_assert(test, message) \
    do {                         \
        if (!(test))             \
            return message;      \
    } while (0)
#define my_run_test(test)       \
    do {                        \
        char *message = test(); \
        tests_run++;            \
        if (message)            \
            return message;     \
    } while (0)

#define N 10

typedef struct list_item {
    int value;
    struct list_item *next;
} list_item_t;

typedef struct {
    struct list_item *head;
} list_t;

static list_item_t items[N];
static list_t l;

void print_list(list_item_t *head)
{
    list_item_t *cur = head;
    while (cur) {
        printf("%d\n", cur->value);
        cur = cur->next;
    }
}

static list_item_t *merge_two_list(list_item_t *l1, list_item_t *l2)
{
    list_item_t *head = NULL;
    list_item_t **ptr = &head;

    while (l1 && l2) {
        if (l1->value <= l2->value) {
            *ptr = l1;
            l1 = l1->next;
        } else {
            *ptr = l2;
            l2 = l2->next;
        }
        ptr = &(*ptr)->next;
    }

    if (l1)
        *ptr = l1;
    if (l2)
        *ptr = l2;

    return head;
}

static void split_list(list_item_t *head, list_item_t **l1, list_item_t **l2)
{
    list_item_t *fast = head, *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    *l1 = head;
    *l2 = slow->next;
    slow->next = NULL;
}

static void merge_sort(list_item_t **head)
{
    if (!(*head) || !(*head)->next)
        return;

    list_item_t *l1 = NULL, *l2 = NULL;

    split_list(*head, &l1, &l2);

    merge_sort(&l1);
    merge_sort(&l2);

    *head = merge_two_list(l1, l2);
}

static int list_size(list_t *l)
{
    list_item_t *p = l->head;
    int len = 0;

    while (p) {
        p = p->next;
        len++;
    }

    return len;
}

static list_t *list_reset(void)
{
    srand(time(NULL));
    for (size_t i = 0; i < N; i++) {
        items[i].value = rand() % 1000;
        items[i].next = NULL;
    }
    l.head = NULL;
    return &l;
}

static inline void list_insert_before(list_t *l,
                                      list_item_t *before,
                                      list_item_t *item)
{
    list_item_t **p;
    for (p = &l->head; *p != before; p = &(*p)->next)
        ;
    *p = item;
    (*p)->next = before;
}

static char *test_list(void)
{
    /* Test inserting at the end */
    list_reset();
    my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
    for (size_t i = 0; i < N; i++)
        list_insert_before(&l, NULL, &items[i]);
    my_assert(list_size(&l) == N, "Final list size should be N");
    print_list(l.head);

    /* Sort by merge sort */
    printf("----MergeSort----\n");
    merge_sort(&l.head);
    print_list(l.head);

    return NULL;
}

int tests_run = 0;

static char *test_suite(void)
{
    my_run_test(test_list);
    return NULL;
}


int main(void)
{
    printf("---=[ List tests\n");
    char *result = test_suite();
    if (result)
        printf("ERROR: %s\n", result);
    else
        printf("ALL TESTS PASSED\n");
    printf("Tests run: %d\n", tests_run);
    return !!result;
}

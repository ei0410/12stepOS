#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    Term,
    Insert,
    Append,
    Delete,
    Remove,
    Clear,
    Print,
} Menu;

typedef struct __node {
    char          name[20];
    char          tel[16];
    struct __node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} List;

Node *AllocNode(void)
{
    return ((Node *)calloc(1, sizeof(Node)));
}

void InitList(List *list)
{
    list->head = list->tail = AllocNode();
}

void InsertNode(List *list, const char *name, const char *tel)
{
    Node *ptr = list->head; //pointer to head node before insert

    list->head = AllocNode();
    strcpy(list->head->name, name);
    strcpy(list->head->tel , tel );
    list->head->next = ptr;
}

void AppendNode(List *list, const char *name, const char *tel)
{
    Node *ptr = list->tail; //pointer to tail node before insert

    list->tail = AllocNode();
    strcpy(ptr->name, name);
    strcpy(ptr->tel , tel );
    ptr->next = list->tail;
}

void DeleteNode(List *list)
{
    if (list->head != list->tail) {
        Node *ptr = list->head->next;
        free(list->head);
        list->head = ptr;
    }
}

void RemoveNode(List *list)
{
    if (list->head != list->tail) {
        if (list->head->next == list->tail) {
            DeleteNode(list);
        } else {
            Node *curr, *prev;

            curr = list->head;
            while (curr->next != list->tail) {
                prev = curr;
                curr = curr->next;
            }
            prev->next = list->tail;
            free(curr);
        }
    }
}

void ClearList(List *list)
{
    Node *ptr = list->head;
    while (ptr != list->tail) {
        Node *ptr2 = ptr->next;
        free(ptr);
        ptr = ptr2; 
    }
    list->head = list->tail;
}

void PrintList(List *list)
{
    Node *ptr;

    ptr = list->head;
    while (ptr != list->tail) {
        printf("%s <<%s>> \n", ptr->name, ptr->tel);
        ptr = ptr->next;
    }
}

Node Read(char *message)
{
    Node temp;

    printf("%s\n", message);

    printf("name:");  scanf("%s", temp.name);
    printf("tel :");  scanf("%s", temp.tel);

    return (temp);
}

Menu SelectMenu(void)
{
    int ch;

    do {
        puts("(1) insert node to head  (2) insert node to tail");
        puts("(3) delete node of head  (4) delete node of tail");
        puts("(5) delete all the nodes (6) print  node of all the nodes");
        puts("(0) end ");
        printf("number ");
        scanf("%d", &ch);
        if (ch < 0 || ch > 6) {
            printf("Nothing\n");
        } else {
            ;
        }
    } while (ch < Term || ch > Print);
    return ((Menu)ch);
}

int main(void)
{
    Menu menu;
    List list;

    InitList(&list);

    do {
        Node x;
        switch (menu = SelectMenu()) {
            case Insert:
                x = Read("insert to head");
                InsertNode(&list, x.name, x.tel);
                break;
            case Append:
                x = Read("insert to tail");
                AppendNode(&list, x.name, x.tel);
                break;
            case Delete:
                DeleteNode(&list);
                break;
            case Remove:
                RemoveNode(&list);
                break;
            case Clear:
                ClearList(&list);
                break;
            case Print:
                PrintList(&list);
                break;
            default:
                break;
        }
    } while (menu != Term);

    ClearList(&list);

    return 0;
}

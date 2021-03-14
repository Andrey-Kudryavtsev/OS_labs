#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SUCCESS 0
#define FAILURE 1
#define ERROR -1

typedef struct Node
{
    char *line;
    struct Node *next;
} t_Node;

typedef struct List
{
    int size;
    t_Node *head;
    t_Node *tail;
} t_List;

t_Node * initNode(char *line)
{
    t_Node *node = (t_Node*) malloc(sizeof(t_Node));
    if (node == NULL) return NULL;
    node->line = (char*) malloc((strlen(line) + 1) * sizeof(char));
    if (node->line == NULL) return NULL;
    strcpy(node->line, line);
    node->next = NULL;
    return node;
}

void deleteNode(t_Node *node)
{
    if (node == NULL) return;
    free(node->line);
    free(node);
}

t_List * initList()
{
    t_List *lst = (t_List*) malloc(sizeof(t_List));
    if (lst == NULL) return NULL;
    lst->size = 0;
    lst->head = NULL;
    lst->tail = NULL;
    return lst;
}

void deleteList(t_List *lst)
{
    if (lst == NULL) return;
    t_Node *node = lst->head;
    while (node != NULL)
    {
        lst->head = lst->head->next;
        deleteNode(node);
        node = lst->head;
    }
    free(lst);
}

int pushback(t_List *lst, char *line)
{
    if (lst == NULL || line == NULL) return ERROR;
    t_Node *node = initNode(line);
    if (node == NULL) return ERROR;
    if (lst->size == 0)
    {
        lst->head = node;
        lst->tail = node;
    } else
    {
        lst->tail->next = node;
        lst->tail = node;
    }
    (lst->size)++;
    return SUCCESS;
}

int main()
{
    t_List *lst = initList();
    if (lst == NULL)
    {
        perror("lab4.out incurred an error");
        exit(FAILURE);
    }
    char buf[LINE_MAX];
    while (fgets(buf, LINE_MAX, stdin) != NULL)
    {
        int pushbackResult = pushback(lst, buf);
        if (pushbackResult == ERROR)
        {
            perror("lab4.out incurred an error");
            deleteList(lst);
            exit(FAILURE);
        }
        if (buf[0] == '.') break;
    }
    int ferrorResult = ferror(stdin);
    if (ferrorResult != SUCCESS)
    {
        perror("lab4.out incurred an error");
        deleteList(lst);
        exit(FAILURE);
    }
    printf("list size: %d\n", lst->size);
    for (t_Node *node = lst->head; node != NULL; node = node->next)
    {
        printf("%s", node->line);
    }
    deleteList(lst);
    exit(SUCCESS);
}

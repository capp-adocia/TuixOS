/* kernel/queue.c */

#include <stddef.h>
#include <Tuix/queue.h>

void enqueue(struct queue* q, struct list_head* node)
{
    if(!q || !node)
    {
        serial_printf("enqueue操作失败,指针不能为NULL\n");
        return;
    }
    node->next = NULL;

    // 队列为空
    if (q->tail == NULL)
        q->head = node;
    else
        q->tail->next = node;

    q->tail = node;
}

struct list_head* dequeue(struct queue* q)
{
    if(!q || !q->head)
    {
        serial_printf("dequeue操作失败,指针不能为NULL\n");
        return NULL;
    }
    
    struct list_head* head = q->head;
    q->head = q->head->next;
    head->next = NULL; // 隔离出队的元素

    // 如果取出最后一个元素
    if (q->head == NULL)
        q->tail = NULL;

    return head;
}

bool queue_empty(struct queue* q)
{
    return q == NULL || q->head == NULL;
}

struct list_head* queue_peek(struct queue* q)
{
    if (queue_empty(q)) return NULL;
    return q->head;
}

void queue_init(struct queue* q)
{
    if (q) { q->head = NULL;q->tail = NULL; }
}

bool check_queue_init(struct queue* q)
{
    if(q)
    {
        if(q->head == NULL && q->tail == NULL)
            return true;
        else
           return false;
    }
    serial_printf("queue_init操作失败,指针不能为NULL\n");
    return false;
}


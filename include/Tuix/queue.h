/* include/Tuix/queue.h */

#ifndef I_T_QUEUE_H
#define I_T_QUEUE_H

#include <stddef.h>
#include <Tuix/serial.h>

struct list_head
{
    struct list_head* next;
    struct list_head* prev;    
};

struct queue
{
    struct list_head* head;
    struct list_head* tail;
};

/**
 * 入队
 * @param q 队列指针
 * @param node 结点
 */
void enqueue(struct queue* q, struct list_head* node);

/**
 * 出队
 * @param q 队列指针
 * @return list结构体指针
 */
struct list_head* dequeue(struct queue* q);

/**
 * 检查队列是否为空
 * @param q 队列指针
 * @return true为空，false为非空
 */
bool queue_empty(struct queue* q);

/**
 * 获取队列头部但不出队
 * @param q 队列指针
 * @return list结构体指针
 */
struct list_head* queue_peek(struct queue* q);

/**
 * 初始化队列
 * @param q 队列指针
 */
void queue_init(struct queue* q);

/**
 * 检查队列是否初始化
 * @param q 队列指针
 */
bool check_queue_init(struct queue* q);

#endif

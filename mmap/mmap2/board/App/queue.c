#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue *queue_init()
{
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}

int queue_is_empty(Queue *queue)
{
  return (queue->front == NULL) ? 1 : 0;
}

int queue_is_full(Queue *queue)
{
  return queue->rear == NULL;
}

int queue_enqueue(Queue *queue, unsigned char *data)
{
  Node *node = (Node *)malloc(sizeof(Node));
  node->data = data;
  node->next = NULL;

  if (queue_is_empty(queue))
  {
    queue->front = node;
    queue->rear = node;
  }
  else
  {
    queue->rear->next = node;
    queue->rear = node;
  }

  return 0;
}

char *queue_dequeue(Queue *queue)
{
  if (queue_is_empty(queue))
    return NULL;

  Node *node = queue->front;
  unsigned char *data = node->data;

  queue->front = node->next;
  if (queue->front == NULL)
    queue->rear = NULL;

  free(node);

  return data;
}

void queue_free(Queue *queue)
{
  while (!queue_is_empty(queue))
  {
    unsigned char *data = queue_dequeue(queue);
    if (data != NULL)
    {
      free(data); // 메시지 메모리 해제
    }
  }

  free(queue); // 큐 큐체 메모리 해제
}
typedef struct node
{
    unsigned char * data;
    struct node *next;
} Node;

typedef struct queue
{
    Node *front;
    Node *rear;
} Queue;

Queue *queue_init();
int queue_is_empty(Queue *queue);
int queue_is_full(Queue *queue);
int queue_enqueue(Queue *queue, unsigned char *data);
char *queue_dequeue(Queue *queue);
void queue_free(Queue *queue);
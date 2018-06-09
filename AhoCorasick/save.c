#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// TODO: Change list to a vector
struct list
{
    struct list *next;
    struct list *last;
    int value;
};

struct trie
{
    struct trie *next[26];
    struct trie *fail;
    struct list *values;
    int pos;
};

struct trieList
{
    struct trie *cur;
    struct trieList *next;
};

struct queue
{
    struct trieList *next;
    struct trieList **last;
};

void push(struct queue *q, struct trie *val)
{
    struct trieList *node = (struct trieList *)malloc(sizeof(struct trieList));
    node->next = NULL;
    node->cur = val;
    *(q->last) = node;
    q->last = &(node->next);
}

struct trie *pop(struct queue *q)
{
    struct trieList *node = q->next;
    q->next = node->next;
    return node->cur;
}

void addValue(struct trie *fsm, int value)
{
    struct list *l = (struct list *)malloc(sizeof(struct list));
    l->value = value;
    l->next = fsm->values;
    if (l->next != NULL)
    {
        l->last = l->next->last;
    }
    else
    {
        l->last = l;
    }
    fsm->values = l;
}

void addValues(struct trie *first)
{
    struct list **node = &(first->values);

    if (*node != NULL)
    {
        node = &((*node)->last->next);
    }
    *node = first->fail->values;
}

void addWord(struct trie *fsm, char *word, int index)
{
    int i = 0;
    char c = word[i];
    size_t size = sizeof(struct trie);
    struct trie *cur = fsm;
    while (c != '\0')
    {
        struct trie *next = cur->next[c - 'a'];
        if (next == NULL)
        {
            next = (struct trie *)malloc(size);
            for (int j = 0; j < 26; j++)
            {
                next->next[j] = NULL;
            }
            next->fail = NULL;
            next->values = NULL;
            cur->next[c - 'a'] = next;
            next->pos = c - 'a';
        }
        cur = next;
        c = word[++i];
    }
    addValue(cur, index);
}

void computeFail(struct trie *fsm)
{
    struct queue q;
    q.last = &(q.next);
    for (int i = 0; i < 26; i++)
    {
        struct trie *cur = fsm->next[i];
        if (cur != NULL)
        {
            push(&q, cur);
            cur->fail = fsm;
        }
    }
    while (q.next != NULL)
    {
        struct trie *cur = pop(&q);

        for (int i = 0; i < 26; i++)
        {
            struct trie *fail = cur->fail;
            struct trie *child = cur->next[i];
            if (child != NULL)
            {
                push(&q, child);
                int pos = child->pos;
                while (fail->next[pos] == NULL && fail->fail != NULL)
                {
                    fail = fail->fail;
                }
                if (fail->next[pos] == NULL)
                {
                    child->fail = fail;
                }
                else
                {
                    child->fail = fail->next[pos];
                    addValues(child);
                }
            }
        }
    }
}

struct trie *buildTrie(char **genes, int length)
{
    struct trie *fsm = (struct trie *)malloc(sizeof(struct trie));
    for (int j = 0; j < 26; j++)
    {
        fsm->next[j] = NULL;
    }
    fsm->fail = NULL;
    fsm->values = NULL;
    for (int i = 0; i < length; i++)
    {
        addWord(fsm, genes[i], i);
    }
    computeFail(fsm);
    return fsm;
}

unsigned long long getHealth(struct list *values, int *health, int first, int last)
{
    unsigned long long sum = 0;
    struct list *cur = values;
    while (cur != NULL)
    {
        int value = cur->value;
        if (first <= cur->value && cur->value <= last)
        {
            sum += health[value];
        }
        cur = cur->next;
    }
    return sum;
}

unsigned long long int search(struct trie *fsm, char *d, int first, int last, int *health)
{
    unsigned long long int sum = 0;
    int i = 0;
    struct trie *cur = fsm;
    while (d[i] != '\0')
    {
        int index = d[i] - 'a';
        while (cur->fail != NULL && cur->next[index] == NULL)
        {
            cur = cur->fail;
        }
        struct trie *found = cur->next[index];
        if (found != NULL)
        {
            cur = found;
            sum += getHealth(found->values, health, first, last);
        }
        i++;
    }

    return sum;
}

char *readline();
char **split_string(char *);

FILE *in;

int main()
{

    FILE *fptr = fopen(getenv("OUTPUT_PATH"), "w");
    in = fopen("input", "r");

    char *n_endptr;
    char *n_str = readline();
    int n = strtol(n_str, &n_endptr, 10);

    if (n_endptr == n_str || *n_endptr != '\0')
    {
        exit(EXIT_FAILURE);
    }

    char **genes_temp = split_string(readline());

    char **genes = malloc(n * sizeof(char *));

    for (int i = 0; i < n; i++)
    {
        char *genes_item = *(genes_temp + i);

        *(genes + i) = genes_item;
    }

    struct trie *fsm = buildTrie(genes, n);
    char **health_temp = split_string(readline());

    int *health = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        char *health_item_endptr;
        char *health_item_str = *(health_temp + i);
        int health_item = strtol(health_item_str, &health_item_endptr, 10);

        if (health_item_endptr == health_item_str || *health_item_endptr != '\0')
        {
            exit(EXIT_FAILURE);
        }

        *(health + i) = health_item;
    }

    char *s_endptr;
    char *s_str = readline();
    int s = strtol(s_str, &s_endptr, 10);

    if (s_endptr == s_str || *s_endptr != '\0')
    {
        exit(EXIT_FAILURE);
    }

    unsigned long long int min = LLONG_MAX;
    unsigned long long int max = 0;
    for (int s_itr = 0; s_itr < s; s_itr++)
    {
        char **firstLastd = split_string(readline());

        char *first_endptr;
        char *first_str = firstLastd[0];
        int first = strtol(first_str, &first_endptr, 10);

        if (first_endptr == first_str || *first_endptr != '\0')
        {
            exit(EXIT_FAILURE);
        }

        char *last_endptr;
        char *last_str = firstLastd[1];
        int last = strtol(last_str, &last_endptr, 10);

        if (last_endptr == last_str || *last_endptr != '\0')
        {
            exit(EXIT_FAILURE);
        }

        char *d = firstLastd[2];

        // have d here, *health, genes, n, first, last
        unsigned long long int sum = search(fsm, d, first, last, health);
        if (sum > max)
            max = sum;
        if (sum < min)
            min = sum;
    }
    printf("%llu %llu\n", min, max);
    //fprintf(fptr, "%llu %llu\n", min, max);

    fclose(fptr);
    return 0;
}

char *readline()
{
    size_t alloc_length = 1024;
    size_t data_length = 0;
    char *data = malloc(alloc_length);

    while (true)
    {
        char *cursor = data + data_length;
        char *line = fgets(cursor, alloc_length - data_length, in);

        if (!line)
        {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n')
        {
            break;
        }

        size_t new_length = alloc_length << 1;
        data = realloc(data, new_length);

        if (!data)
        {
            break;
        }

        alloc_length = new_length;
    }

    if (data[data_length - 1] == '\n')
    {
        data[data_length - 1] = '\0';
    }

    data = realloc(data, data_length);

    return data;
}

char **split_string(char *str)
{
    char **splits = NULL;
    char *token = strtok(str, " ");

    int spaces = 0;

    while (token)
    {
        splits = realloc(splits, sizeof(char *) * ++spaces);
        if (!splits)
        {
            return splits;
        }

        splits[spaces - 1] = token;

        token = strtok(NULL, " ");
    }

    return splits;
}

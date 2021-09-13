#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char open;
    char close;
} p_tuple;

typedef struct {
    p_tuple *data;
    size_t length;
    size_t count;
} p_stack;
const size_t P_STACK_DEFAULT_LENGTH = 16;

const p_tuple P_TUPLES[] =
{
    (p_tuple){ .open = '(', .close = ')' },
    (p_tuple){ .open = '[', .close = ']' },
    (p_tuple){ .open = '{', .close = '}' },
};
const int P_TUPLES_LENGTH = sizeof(P_TUPLES) / sizeof(P_TUPLES[0]);

p_stack create_stack() 
{
    return (p_stack) 
    {
        .data = NULL,
        .length = 0,
        .count = 0,
    };
}

int push_stack(p_stack *s, p_tuple item)
{
    if (s->count + sizeof(p_tuple) > s->length)
    {
        size_t new_length = s->length == 0 
            ? P_STACK_DEFAULT_LENGTH 
            : s->length * 2;
        p_tuple *new_data = calloc(new_length, sizeof(p_tuple));
        memcpy(new_data, s->data, s->length * sizeof(p_tuple));
        free(s->data);
        s->data = new_data;
        s->length = new_length;
    }

    s->data[s->count++] = item;
    return 0;
}

size_t get_stack_count(p_stack *s)
{
    return s->count;
}

int peek_stack(p_stack *s, p_tuple *tuple)
{
    if (tuple == NULL) return 1;
    if (s->count == 0) return 2;

    *tuple = s->data[s->count - 1];
    return 0;
}

int pop_stack(p_stack *s, p_tuple *tuple)
{
    if (tuple == NULL) return 1;
    if (s->count == 0) return 2;

    tuple = &s->data[s->count--];
    return 0;
}

void free_stack(p_stack *s) 
{
    free(s->data);
    s->data = NULL;
    s->count = 0;
    s->length = 0;
}

void print_help()
{
    printf("You must specify which file to read from as an argument to the program.\n");
}

int get_p_index(char c)
{
    for (int i = 0; i < P_TUPLES_LENGTH; i++)
    {
        if (c == P_TUPLES[i].open || c == P_TUPLES[i].close)
        {
            return i;
        }
    }
    return -1;
}

int run_on_file(FILE *file)
{
    char buf[255];
    int ret = 0;

    if (fgets(buf, sizeof(buf) / sizeof(buf[0]), file) == NULL)
    {
        perror("Unable to read from file");
        return 1;
    }

    p_stack s = create_stack();
    p_tuple b;
    do
    {
        int l = strlen(buf);
        for (int i = 0; i < l; i++)
        {
            int pindex = get_p_index(buf[i]);
            if (pindex == -1) continue;
            p_tuple et = P_TUPLES[pindex];

            if (buf[i] == et.open)
            {
                if (push_stack(&s, P_TUPLES[pindex]))
                {
                    printf("Unable to push stack\n");
                    return 1;
                }
            }
            else if (buf[i] == et.close)
            {
                p_tuple t;
                if (get_stack_count(&s) == 0)
                {
                    printf("ERROR: Unexpected '%c'\n", buf[i]);
                    return 1;
                }
                if (peek_stack(&s, &t))
                {
                    printf("Unable to peek stack\n");
                    return 1;
                }
                if (buf[i] != t.close)
                {
                    printf("ERROR: Expected '%c', got '%c'\n", t.close, buf[i]);
                    return 1;
                }
                if (pop_stack(&s, &t))
                {
                    printf("Unable to pop stack\n");
                    return 1;
                }
            }
        }
    } while (fgets(buf, sizeof(buf) / sizeof(buf[0]), file) != NULL);

    if (get_stack_count(&s) != 0)
    {
        p_tuple t;
        peek_stack(&s, &t);
        printf("ERROR: Unclosed '%c'\n", t.open);
        return 1;
    }

    printf("OK!\n");
    free_stack(&s);
    return ret;
}

int handle_file(char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Unable to open file");
        return 1;
    }
    return run_on_file(file);
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        print_help();
        return 1;
    }
    handle_file(argv[1]);
}
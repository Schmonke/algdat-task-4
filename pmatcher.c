#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/** 
 * p_tuple represents a tuple containing an open and close token for a 
 * parenthesis/square bracket.
 */
typedef struct {
    char open;
    char close;
} p_tuple;

/**
 * p_stack is a stack for p_tuples.
 */
typedef struct {
    p_tuple *data;
    size_t length;
    size_t count;
} p_stack;
const size_t P_STACK_DEFAULT_LENGTH = 16;

/**
 * All of the p_tuples to match. 
 */
const p_tuple P_TUPLES[] =
{
    (p_tuple){ .open = '(', .close = ')' },
    (p_tuple){ .open = '[', .close = ']' },
    (p_tuple){ .open = '{', .close = '}' },
};
const int P_TUPLES_LENGTH = sizeof(P_TUPLES) / sizeof(P_TUPLES[0]);

/** 
 * Creates a new zero-sized stack.
 */
p_stack create_stack() 
{
    return (p_stack) 
    {
        .data = NULL,
        .length = 0,
        .count = 0,
    };
}

/**
 * Pushes a p_tuple onto the stack.
 */
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

/**
 * Gets the count of items on the stack.
 */
size_t get_stack_count(p_stack *s)
{
    return s->count;
}

/**
 * Gets a copy of stack top element and sets it to specified
 * tuple.
 */
int peek_stack(p_stack *s, p_tuple *tuple)
{
    if (tuple == NULL) return 1;
    if (s->count == 0) return 2;

    *tuple = s->data[s->count - 1];
    return 0;
}

/**
 * Pops the last element from the stack and writes it to tuple.
 */
int pop_stack(p_stack *s, p_tuple *tuple)
{
    if (tuple == NULL) return 1;
    if (s->count == 0) return 2;

    tuple = &s->data[s->count--];
    return 0;
}

/**
 * Frees the stack from memory.
 */
void free_stack(p_stack *s) 
{
    free(s->data);
    s->data = NULL;
    s->count = 0;
    s->length = 0;
}

/**
 * Gets the p_tuple index in P_TUPLES from the topmost element in the stack.
 */
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

/**
 * Runs pmatcher on a specific file.
 */
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

        // Loop over all characters in the buffer.
        for (int i = 0; i < l; i++)
        {
            // Checks whether the character is a p_tuple.
            int pindex = get_p_index(buf[i]);
            if (pindex == -1) continue;
            p_tuple et = P_TUPLES[pindex];

            if (buf[i] == et.open)
            {
                // Push open p onto stack
                if (push_stack(&s, P_TUPLES[pindex]))
                {
                    printf("Unable to push stack\n");
                    return 1;
                }
            }
            else if (buf[i] == et.close)
            {
                // Try to peek stack.
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

                // Pop last element off the stack if it is matching, else, show error.
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

    // A non-empty stack means where are unclosed ps.
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

/**
 * Opens and reads reads file from specified filepath before running program on file.
 */
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

void print_help()
{
    printf(
        "You must specify which file to read from as an argument to the program.\n"
        "Usage: pmatcher <file_name>\n"
        "E.g. ./pmatcher ~/main.c\n"
    );
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
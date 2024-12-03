#include <stdio.h>
#include <stdlib.h>

/*
double linked list for base dictionary
every node has
next pointerr
previous pointer
idex Number
element sring
*/

// dictionary linked list token
struct token
{
    // links
    struct token *next;
    // data
    unsigned index;
    char *string;
    int ref;
};

int str_len(char *s);                       // returns string length
int str_cmp(char *str1, char *str2, int j); // compare srings
char *str_copy(char *source, int j);        // copy by length

void printDict(struct token *head); // print dictionary linked list

int get_bit_count(unsigned index);
void write_compressed_binary(const char *filename, struct token *head);

int main(int argc, char const *argv[])
{
    // Define  File streams
    FILE *inFile;

    // Create head of dictionary
    struct token *head = (struct token *)malloc(sizeof(struct token));
    head->index = 0;
    head->string = "_";

    // open file for reed
    inFile = fopen(argv[1], "r");

    char buffer[128] = {}; // space for sring operation
    char ch;               // reeded char

    while (1)
    {
        struct token *cursor = head; // reset dictionary cursor
        int lastI = 0;               // holds last equal token address
        int j = 0;                   // holds buffer string length

        //
        ch = fgetc(inFile);
        buffer[j++] = ch;

        if (ch == EOF) // if input file ends
        {
            break;
        }

        while (1) // compare buffer with dictionary tokens by index iteration
        {
            if (str_cmp(cursor->string, buffer, j)) // if token exist in dictionary ; scale up the token
            {
                lastI = cursor->index; // hold the last reference point
                ch = fgetc(inFile);    // read nev character from file
                buffer[j++] = ch;      // add new character to buffer
                continue;
            }

            if (cursor->next == NULL) //  if dictionary ends ; add new token
            {
                cursor->next = (struct token *)malloc(sizeof(struct token)); // create
                int inx = cursor->index;                                     // hold index
                cursor = cursor->next;                                       // go forward

                cursor->index = 1 + inx;              // add index
                cursor->ref = lastI;                  // add referronce  point
                cursor->string = str_copy(buffer, j); // add sring

                // ı shud be wrrie o file in  here
                printf("%d%c", cursor->ref, cursor->string[j - 1]);

                break; // whenever unique string occurs restart
            }

            cursor = cursor->next; // continue to search
        }
    }

    fclose(inFile);
    printf("\n");    // close input file
    printDict(head); // print the token dictionary
    write_compressed_binary("compressed.lz78", head);
    return 0;
}

int get_bit_count(unsigned index)
{
    int size = 0;
    while (index)
    {
        index = index / 2;
        size++;
    }
    return size;
}

void write_compressed_binary(const char *filename, struct token *head)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer = 0; // 8-bit temporary buffer
    int bit_count = 0;        // Tracks how many bits have been written to the buffer

    struct token *current = head->next;

    while (current != NULL)
    {
        // 1. Get the number of bits required for the reference
        int ref_bits = get_bit_count(current->ref);
        if (ref_bits == -1)
        {
            fprintf(stderr, "Invalid reference index: %u\n", current->ref);
            exit(EXIT_FAILURE);
        }

        // 2. Write the reference to the buffer
        for (int i = ref_bits - 1; i >= 0; i--)
        {
            buffer = (buffer << 1) | ((current->ref >> i) & 1); // Add the corresponding bit
            bit_count++;

            // Write the buffer to the file if it's full
            if (bit_count == 8)
            {
                fwrite(&buffer, sizeof(unsigned char), 1, file);
                buffer = 0;
                bit_count = 0;
            }
        }

        // 3. Write the character to the buffer
        unsigned char new_char = current->string[str_len(current->string) - 1];
        for (int i = 7; i >= 0; i--)
        {
            buffer = (buffer << 1) | ((new_char >> i) & 1); // Add the character bit
            bit_count++;

            if (bit_count == 8)
            {
                fwrite(&buffer, sizeof(unsigned char), 1, file);
                buffer = 0;
                bit_count = 0;
            }
        }

        current = current->next;
    }

    // Write remaining bits in the buffer if it is partially filled
    if (bit_count > 0)
    {
        buffer <<= (8 - bit_count); // Fill remaining bits with zeros
        fwrite(&buffer, sizeof(unsigned char), 1, file);
    }

    fclose(file);
    printf("Compressed data has been written to the binary file '%s'.\n", filename);
}

char *str_copy(char *source, int j)
{
    char *dest = (char *)malloc((j + 1) * sizeof(char));
    for (int k = 0; k < j; k++)
    {
        dest[k] = source[k];
    }
    dest[j] = '\0';
    return dest;
}

int str_cmp(char *str1, char *str2, int j)
{
    // if its the same pointer can return immediately
    // also covers the case where both are null pointers
    if (str1 == str2)
        return 1;
    // if str1 and str2 has different sizes //useless
    if (str_len(str1) != j)
    {
        return 0;
    }

    // see if for length of str1 > 1, all letters in str2 match
    // we also go up through the nul char to make sure the size
    // of both strings is the same

    for (int k = 0; k < j; k++)
        if (str1[k] != str2[k])
        {
            // printf("notS ");
            return 0;
        }
    return 1;
}

int str_len(char *s)
{
    int j = 0, sum = 0;
    char c = s[0];

    while (c != '\0')
    {
        sum++;
        c = s[++j];
    }
    return sum;
}

void printDict(struct token *head)
{
    struct token *current = head;
    while (current != NULL)
    {
        printf("%d\t%d*%s --> \n", current->index, current->ref, current->string);
        current = current->next;
    }
    printf("NULL\n"); // Listenin sonunu belirtmek için
}
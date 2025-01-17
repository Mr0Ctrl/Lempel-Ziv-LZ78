#include <stdio.h>
#include <stdlib.h>

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

#pragma region function protypes
// string functions
int str_len(char *s);                       // returns string length
int str_cmp(char *str1, char *str2, int j); // compare srings
char *str_copy(char *source, int j);        // copy by length
char *str_dup(char *s);
char *str_cat(char *dest, const char *src);

int get_bit_count(unsigned index);

// compression functions
char *decompress_string(char *compressed);
struct token *compress_file_to_dict(const char *filename);
void write_compressed_binary(const char *filename, struct token *head);
void readBinaryFile(const char *filename);

void printDict(struct token *head); // print dictionary linked list
#pragma endregion

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Compress file to dictionary
    struct token *head = compress_file_to_dict(argv[1]);

    printf("\n"); // Print a newline
    // printDict(head); // Print the token dictionary

    // Write compressed data to binary file
    write_compressed_binary("compressed.lz78", head);

    readBinaryFile("compressed.lz78");

    char *compressed = "0a1a0b1b2a";
    printf("%s\n",compressed);
    char *decompressed = decompress_string(compressed);
    printf("Decompressed string: %s\n", decompressed);

    free(decompressed); 
    return 0;
}

char *decompress_string(char *compressed) {
    int len = str_len(compressed); 
    char **dict = malloc(len * sizeof(char *)); 
    if (!dict) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++) {
        dict[i] = NULL; 
    }

    char *result = malloc(len * 10 * sizeof(char)); 
    if (!result) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    result[0] = '\0'; 

    int index = 0;
    for (int i = 0; i < len; ) {
        int ref = compressed[i++] - '0'; 

        char character = compressed[i++];

        char *entry;
        if (ref == 0) {
            entry = malloc(2 * sizeof(char));
            entry[0] = character;
            entry[1] = '\0';
        } else {
            int prev_len = str_len(dict[ref - 1]);
            entry = malloc((prev_len + 2) * sizeof(char));
            entry = str_copy(dict[ref - 1], prev_len );
            entry[prev_len] = character;
            entry[prev_len + 1] = '\0';
        }

        // 4. Sözlüğe ekle ve sonucu güncelle
        dict[index++] = entry;
        str_cat(result, entry); // Sonucu genişlet
    }

    // Bellek temizliği
    for (int i = 0; i < index; i++) {
        free(dict[i]);
    }
    free(dict);

    return result; // Orijinal string'i döndür
}


struct token *compress_file_to_dict(const char *filename)
{
    FILE *inFile = fopen(filename, "r");
    if (!inFile)
    {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    // Create head of dictionary
    struct token *head = (struct token *)malloc(sizeof(struct token));
    if (!head)
    {
        perror("Failed to allocate memory for head");
        exit(EXIT_FAILURE);
    }
    head->index = 0;
    head->string = "_";
    head->next = NULL;

    char buffer[128] = {}; // Space for string operations
    char ch;               // Read character

    while (1)
    {
        struct token *cursor = head; // Reset dictionary cursor
        int lastI = 0;               // Holds last equal token address
        int j = 0;                   // Holds buffer string length

        ch = fgetc(inFile);
        buffer[j++] = ch;

        if (ch == EOF) // If input file ends
        {
            break;
        }

        while (1) // Compare buffer with dictionary tokens by index iteration
        {
            if (str_cmp(cursor->string, buffer, j)) // If token exists in dictionary
            {
                lastI = cursor->index; // Hold the last reference point
                ch = fgetc(inFile);    // Read new character from file
                buffer[j++] = ch;      // Add new character to buffer
                continue;
            }

            if (cursor->next == NULL) // If dictionary ends, add new token
            {
                cursor->next = (struct token *)malloc(sizeof(struct token)); // Create
                if (!cursor->next)
                {
                    perror("Failed to allocate memory for token");
                    exit(EXIT_FAILURE);
                }

                int inx = cursor->index; // Hold index
                cursor = cursor->next;   // Go forward

                cursor->index = 1 + inx;              // Add index
                cursor->ref = lastI;                  // Add reference point
                cursor->string = str_copy(buffer, j); // Add string
                cursor->next = NULL;                  // Initialize next pointer

                printf("%d%c|", cursor->ref, cursor->string[j - 1]);
                break; // Whenever unique string occurs restart
            }

            cursor = cursor->next; // Continue to search
        }
    }

    fclose(inFile); // Close input file
    return head;    // Return dictionary head
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

/*
A text compressed into this format is not suitable for encoding because\n
the index and characters cannot be distinguished from each other since\n
the length of the index values ​​are different from each other.\n
\n
This is my opinion.\n
I try but I didn't succeed\n
*/
void readBinaryFile(const char *filename)
{
    printf("\nA text compressed into this format is not suitable for encoding because\nthe index and characters cannot be distinguished from each other since\nthe length of the index values ​​are different from each other.\n\nThis is my opinion.\nI tried decode but I didn't succeed\n");

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Failed to open input file");
        return;
    }

    unsigned char byte;
    int step = 0;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1)
    {

        if (step == 0)
        {
            printf("%c", byte);//fist element always came without index  
            step++;
            continue;
        }
        // for (int i = 7; i >= 0; i--) {
        //     printf("%d", (byte >> i) & 1);
        // }

        printf(" ");
    }
    printf("\n");
    fclose(file);
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

#pragma region sring functions
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

char *str_dup(char *s)
{
    // String'in uzunluğunu bul
    size_t len = str_len(s);

    // Yeni bir bellek bloğu tahsis et (uzunluk + 1, çünkü null-terminator gereklidir)
    char *copy = (char *)malloc(len + 1);

    // Eğer bellek tahsisi başarısız olduysa
    if (!copy)
    {
        perror("Failed to allocate memory for string copy");
        exit(EXIT_FAILURE);
    }

    // Orijinal string'i yeni belleğe kopyala
    s = str_copy(copy, len);

    return copy; // Yeni kopyayı döndür
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

char *str_cat(char *dest, const char *src) {
    char *original_dest = dest;

    // dest'in sonuna git
    while (*dest != '\0') {
        dest++;
    }

    // src'yi dest'e ekle
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }

    // Sonuna null karakter ekle
    *dest = '\0';

    return original_dest; // Birleştirilmiş string'i döndür
}
#pragma endregion
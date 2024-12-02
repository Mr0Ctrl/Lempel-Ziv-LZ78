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
struct token
{
    //links
    struct token* next;
    struct token* previous;
    //data
    unsigned index;
    char* string; 
    int ref;
};

int str_len(char * s);
int str_cmp(char* str1, char* str2,int j);
char* str_copy(char* source,int j);
void printDict(struct token *head);

int main(int argc, char const *argv[])
{
    /*
    steps:
    two file needed instant j will write directly(firrst codect system afterthat binary system)
    read - search if exist scale up ? write with last index

    string compare function needed
    */
    FILE *inFile;
    FILE *outFile;

    struct token* head = (struct token*)malloc(sizeof(struct token));
    head->index = 0;
    head->string="_";
    
    printf("%s",head->string);

    printf("%s\n",argv[1]);
    inFile = fopen(argv[1], "r");

    char buffer[128] =  {};
    char ch;
    //int  i = 0;//holds  index

    while (1){// 
        struct token* cursor = head;
        int lastI = 0;
        int j = 0;//holds  buffer lengh

        ch = fgetc(inFile);
        buffer[j++] = ch;
        while(1){
            //compare buffer with dictionary by index iteration
            printf("%d\t",j);
            printf("%s - %s \n",cursor->string,str_copy(buffer,j));
            
            if (str_cmp(cursor->string,buffer,j))
            {   
                printf("same\n");
                lastI = cursor->index;
                ch = fgetc(inFile);
                buffer[j++] = ch;
                //if  exist go  new node
                continue;;
            }

            //if not add  dictionary
            if(cursor->next==NULL){
                printf("new token\n");
                cursor->next = (struct token*)malloc(sizeof(struct token));//create
                int inx = cursor->index;//hold index
                cursor  =  cursor->next;//go forward
                cursor->index=1+inx;//add index
                cursor->ref = lastI;
                cursor->string = str_copy(buffer,j);
                break;
            }

            cursor = cursor->next;
            //if exist continue
            
        }
        if(ch == EOF){
            break;
        }
    }

    fclose(inFile);
    printDict(head);
    //fclose(outFile);
    return 0;
}

char* str_copy(char* source,int j){
    char* dest = (char*)malloc((j + 1) * sizeof(char));
    for (int k = 0; k < j; k++) {
        dest[k] = source[k];
    }
    dest[j] = '\0';
    return dest;
}

int str_cmp(char* str1, char* str2,int j){
    // if its the same pointer can return immediately
    // also covers the case where both are null pointers
    if (str1 == str2) 
        return 1;
    //if str1 and str2 has different sizes //useless
    // size_t len = str_len(str1);
    // if (str_len(str1) == str_len(str1))
    //     return 0;

    // see if for length of str1 > 1, all letters in str2 match
    // we also go up through the nul char to make sure the size
    // of both strings is the same

    for (int k=0; k<=j; k++)        
        if (str1[k] != str2[k]) 
            return 0;
    return 1;

}

int str_len(char * s) {
      int j = 0, sum = 0;
      char c = s[0];

      while(c != '\0') {
            sum++;
            c = s[++j];
      }
      return sum;
}

void printDict(struct token *head) {
    struct token *current = head;
    while (current != NULL) {
        printf("%d*%s --> ",current->ref, current->string);
        current = current->next;
    }
    printf("NULL\n");  // Listenin sonunu belirtmek için
}

/*

        //if dictionary empty directly add
        if (head->string == NULL){
            head->string = str_copy(&ch ,j+1);
            printf("%s\n",head->string);
            continue;
        }

*/
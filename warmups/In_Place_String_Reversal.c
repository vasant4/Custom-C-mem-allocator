#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char reverse_str(char *s){
    if (s == NULL){
        return 0;
    }
    char *start = s;
    char *end = s + strlen(s)-1;
    while (start < end){
        *start = *start ^ *end;
        *end = *start ^ *end;
        *start = *start ^ *end;
        start ++;
        end --; 
    }
    return 0;
}

int main (){
    char arr[] = "hello";   /* case 1 */
    char *ptr  = strdup("hello");   /* case 2 */

    printf("Here is the string arr before swap: %s\n", arr);
    reverse_str(arr);
    printf("Here is the string after swap of arr: %s\n", arr);
    
    reverse_str(ptr);
    printf("Here is the string after swap of ptr %s\n", ptr);
    free(ptr);
}
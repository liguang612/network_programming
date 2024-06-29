#include <stdio.h>
#include <string.h>
#include <malloc.h>

char* s = NULL;

void Append(char** s, const char* x)
{
    int oldLen;
    if (*s == NULL)
    {
        oldLen = 0;
        *s = (char*)calloc(1, sizeof(char));
    } else {
        oldLen = strlen(*s);
    }
    int newLen = oldLen + strlen(x) + 1;
    *s = (char*)realloc(*s, newLen);
    memset(*s + oldLen, 0, newLen - oldLen);

    sprintf(*s + strlen(*s), "%s", x);
}

int main()
{    
    Append(&s, "Xin ");
    Append(&s, "chao ");
    Append(&s, "cac ");
    Append(&s, "ban!");
    printf("%s", s);
    free(s);
}
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int main(){
    char * a;
    int s = strtoul("0x987", &a, 16);
    printf("%d", s + 0x10);
    return 0;
}
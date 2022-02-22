#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

int main(){
    int seed = time(0); //time(0)返回的是系统的时间（从1970.1.1午夜算起），单位：秒
    srand(seed);//播种
    printf("%d\n", rand() % 3);//输出0～2
    return 0;
}
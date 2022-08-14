// #include <stdio.h>
// #include <time.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <assert.h>
// #include <string.h>
// #include <stdlib.h>



// // void asd(int a){
// //     int b = 0;
// //     printf("%d\n", b);
// //     b += 2;
// // }
// int main(){
//     printf("hello world.\n");
//     return 0;
// }


void fun() {
  extern unsigned char _end;  // _end是什么?
  unsigned char *p = &_end;
  *p = 0;
  while(*p != 0xff);
  *p = 0x33;
  *p = 0x34;
  *p = 0x86;
}
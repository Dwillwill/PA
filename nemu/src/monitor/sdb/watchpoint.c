#include "sdb.h"

#define NR_WP 5

typedef struct watchpoint {
  int NO;
  char *exp;
  uint32_t *value_of_exp;
  struct watchpoint *next;
  
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
// void pint(){
//   printf("HEAD:\n");
//   WP* p = head;
//   while(1){
//     printf("%d: %s\n", p->NO, p->exp);
//     p = p->next;
//     if( p == NULL ){
//       break;
//     }
//   }
//   printf("FREE_:\n");
//   p = free_;
//   int i = 0;
//   while(1){
//     if( p == NULL ){
//       break;
//     }
//     i++;
//     p = p->next;
    
//   }
//   printf("%d\n", i);
// }

void new_wp(char *exp){
  WP *p = free_, *q = head;
  bool success = true;
  u_int32_t value = expr(exp, success);

  free_ = free_->next;

  p->exp = (char *) malloc(128);
  p->value_of_exp = (uint32_t *) malloc(sizeof(uint32_t));
  memcpy(p->value_of_exp, &value, 4);
  strcpy(p->exp, exp); 

  p->next = NULL;
  if(q == NULL) head = p;
  else {
    while(q->next != NULL){
      q = q->next;
    }
    q->next = p;
  }
  // printf("The watch point's number is %d\n", p->NO);
  // printf("The watch point's exp is %s\n", p->exp);
  // pint();
  return ;
}

void free_wp(int index){
  WP *p = head, *pre, *tmp;
  int flag = 0; 
  if(head->next == NULL && head->NO != index){
    printf("No such watchpoint!\n");
    return ;
  }
  if(head->NO == index){
    flag = 1;
    tmp = head; head = head->next;
    if(free_ == NULL) {
      free(tmp->exp);
      free(tmp->value_of_exp);
      free_ = tmp;
    }
    else {
      tmp->next = free_; 
      free_ = tmp;
    }
    // pint();
    return ;
  }
  do{
    pre = p; p = p->next;
    if(p->NO == index){
      flag = 1;
      pre->next = p->next;
      free(p->exp);
      free(p->value_of_exp);
      if(free_ == NULL) {
        p->next = NULL;
        free_ = p;
      }
      else {
        p->next = free_;
        free_ = p;
      }
    }
  }while(p->next != NULL);
  // pint();
  if(flag == 0){
    printf("No such watchpoint!\n");
    return ;
  }
  return;
}

int is_wp_change(){
  WP *p = head;
  uint32_t temp_value;
  bool success = true;
  int flag = 0;
  while(p != NULL){
    temp_value = expr(p->exp, success);
    if(temp_value != *(p->value_of_exp)){
      flag = 1;
      printf("NO %d: $(%s) = %u -------->  %u\n", p->NO, p->exp, *(p->value_of_exp), temp_value);
      memcpy(p->value_of_exp, &temp_value, 4);
    }
    p = p->next;
  }
  return flag;
}

void print_all_wp(){
  WP *p = head;
  while(p != NULL){
    printf("NO %d: $(%s) = %u\n", p->NO, p->exp, *(p->value_of_exp));
    p = p->next;
  }
}
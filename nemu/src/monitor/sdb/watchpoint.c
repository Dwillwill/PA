#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  char *exp;
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
void pint(){
  WP* p = head;
  while(1){
    printf("%d: %s\n", p->NO, p->exp);
    p = p->next;
    if( p == NULL ){
      break;
    }
  }
}

void new_wp(char *exp){
  WP *p = free_, *q = head;
  free_ = free_->next;
  
  p->exp = exp; p->next = NULL;
  if(q == NULL) head = p;
  else {
    while(q->next != NULL){
      q = q->next;
    }
    q->next = p;
  }
  printf("The watch point's number is %d\n", p->NO);
  printf("The watch point's exp is %s\n", p->exp);
  pint();
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
    if(free_ == NULL) free_ = tmp;
    else {
      tmp->next = free_; 
      free_ = tmp;
    }
    return ;
  }
  do{
    pre = p; p = p->next;
    if(p->NO == index){
      flag = 1;
      pre->next = p->next;
      if(free_ == NULL) free_ = p;
      else {
        p->next = free_; 
        free_ = p;
      }
    }
  }while(p->next != NULL);
  if(flag == 0){
    printf("No such watchpoint!\n");
    return ;
  }
  return;
}


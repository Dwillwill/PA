#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <stdbool.h> 
#include <memory/host.h>
#include <memory/paddr.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_UNEQ, TK_AND, TK_NUM, TK_DEREF, TK_REG, TK_HEX

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[a-f0-9]+", TK_HEX}, // hex numbers
  {"[0-9]+", TK_NUM},   // numbers
  {"\\$[a-z0-9]+", TK_REG},   // reg name
  {"\\+", '+'},         // expr sign
  {"\\-", '-'},         
  {"\\*", '*'},         
  {"\\/", '/'},         
  {"\\(", '('},         
  {"\\)", ')'},         
  {"==", TK_EQ},        // equal
  {"!=", TK_UNEQ},      // unequal
  {"&&", TK_AND},       // logical
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  int is_legitimate[32];
  int num_of_parentheses = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
        switch (rules[i].token_type) {
          case '+': tokens[nr_token++].type = '+'; break; 
          case '-': tokens[nr_token++].type = '-'; break;
          case '*': tokens[nr_token++].type = '*'; break;
          case '/': tokens[nr_token++].type = '/'; break;
          case '(': 
            tokens[nr_token++].type = '(';
            is_legitimate[num_of_parentheses++] = 1;
            break;
          case ')': 
            tokens[nr_token++].type = ')'; 
            is_legitimate[num_of_parentheses++] = -1;
            break;
          case TK_NUM: 
            if(substr_len > 32){
              panic("The length of number can't over 32!\n");
            }
            tokens[nr_token].type = TK_NUM; 
            memcpy(tokens[nr_token++].str, substr_start, substr_len); 
            break;
          case TK_HEX: 
            tokens[nr_token].type = TK_HEX;
            memcpy(tokens[nr_token++].str, substr_start, substr_len);
            break; 
          case TK_REG: 
            tokens[nr_token].type = TK_REG;
            memcpy(tokens[nr_token++].str, substr_start + 1, substr_len);
            break; 
          case TK_EQ: tokens[nr_token++].type = TK_EQ; break; 
          case TK_UNEQ: tokens[nr_token++].type = TK_UNEQ; break;
          case TK_AND: tokens[nr_token++].type = TK_AND; break;  
          case TK_NOTYPE: break;
          default: panic("This token type is not defined\n");
        }
        break;
      }
    }
    //is_legitimate?
    int ans = 0;
    for(int i = 0; i < num_of_parentheses; i++){
      ans += is_legitimate[i];
      if(ans < 0){
        panic("Bad expression because of the parentheses!\n");
      }
    }
    // printf("The expression is legitimate.\n");

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}


bool check_parentheses(int p, int q){
  int parentheses[32];
  int num_of_p = 0;
  int flag = 0;
  if(tokens[p].type != '('  || tokens[q].type != ')'){
    return false;
  }
  for(int i = p + 1; i <= q - 1; i++){
    if(tokens[i].type == '('){
      parentheses[num_of_p++] = 1;
    }
    else if(tokens[i].type == ')'){
      parentheses[num_of_p++] = -1;
    }
  }
  for(int i = 0; i < num_of_p; i++){
    flag += parentheses[i];
    if(flag < 0) return false;
  }
  if(flag > 0) return false;
  return true;
}



#define AND 5
#define UNEQ 6
#define EQ 6
#define PLUS 7
#define MINUS 7
#define MULT 8
#define DIV 8
#define LEFT_BRACKET 9
#define RIGHT_BRACKET -9

int find_main_op(int p, int q){
  int op_and_parentheses[32];
  int index[32];
  int num = 0;
  for(int i = p ; i <= q ; i++){
    if(tokens[i].type == '+'){
      op_and_parentheses[num] = PLUS;
      index[num++] = i;
    }
    else if(tokens[i].type == '-'){
      op_and_parentheses[num] = MINUS;
      index[num++] = i;
    }
    else if(tokens[i].type == '*'){
      op_and_parentheses[num] = MULT;
      index[num++] = i;
    }
    else if(tokens[i].type == '/'){
      op_and_parentheses[num] = DIV;
      index[num++] = i;
    }
    else if(tokens[i].type == '('){
      op_and_parentheses[num] = LEFT_BRACKET;
      index[num++] = i;
    }
    else if(tokens[i].type == ')'){
      op_and_parentheses[num] = RIGHT_BRACKET;
      index[num++] = i;
    }
    else if(tokens[i].type == TK_AND){
      op_and_parentheses[num] = AND;
      index[num++] = i;
    }
    else if(tokens[i].type == TK_UNEQ){
      op_and_parentheses[num] = UNEQ;
      index[num++] = i;
    }
    else if(tokens[i].type == TK_EQ){
      op_and_parentheses[num] = EQ;
      index[num++] = i;
    }
  }
  int op_point = 0;
  int op_flag = 0;
  int dis_parentheses;
  for(int i = 0; i < num; i++){
    if(op_and_parentheses[i] == LEFT_BRACKET){
      dis_parentheses = LEFT_BRACKET;
      do{
        i = i + 1;
        if(op_and_parentheses[i] == RIGHT_BRACKET){
          dis_parentheses += RIGHT_BRACKET;
        }
        else if(op_and_parentheses[i] == LEFT_BRACKET){
          dis_parentheses += LEFT_BRACKET;
        }
      }while(dis_parentheses != 0);
      if(op_flag == 0){
        op_point = i + 1;
      }
    }
    else if(op_and_parentheses[op_point] <= op_and_parentheses[i]){
      op_flag = 1;
      continue;
    }
    else if(op_and_parentheses[op_point] > op_and_parentheses[i]){
      op_flag = 1;
      op_point = i;
    }
  }
  if(op_point + 1 <= num - 1 && op_and_parentheses[op_point] == 2 && op_and_parentheses[op_point - 1] == RIGHT_BRACKET && op_and_parentheses[op_point + 1] != LEFT_BRACKET && op_and_parentheses[op_point + 1] != RIGHT_BRACKET){
    op_point = op_point + 1;
  }
  return index[op_point];
}

word_t eval(int p ,int q){
  if(p > q){
    panic("Bad expression\n");
  }
  else if(p == q){
    word_t num;
    int flag = 0;
    if(tokens[p].type == TK_HEX){
      char *tmp;
      num = strtoul(tokens[p].str, &tmp, 16);
      flag = 1;
    }
    else if(tokens[p].type == TK_NUM){
      char *tmp;
      num = strtoul(tokens[p].str, &tmp, 10);
      flag = 1;
    }
    else if(tokens[p].type == TK_REG){
      bool success = false;
      char *reg_name =  tokens[p].str;
      // printf("%s\n", reg_name);
      num = isa_reg_str2val(reg_name, success);
      flag = 1;
    }
    if(flag == 0){
      panic("The value is not defined!\n");
    }
    return num;
  }
  else if(check_parentheses(p, q)){
    return eval(p + 1, q - 1);
  }
  else if(tokens[p].type == TK_DEREF){
    word_t pointer = eval(p + 1, q);
    uint32_t *start_point = (uint32_t *)guest_to_host(pointer);
    return *start_point;
  }

  else{
    int op = find_main_op(p, q);
    word_t val1 = eval(p, op - 1);
    word_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': 
        // printf("%u\n", val1 + val2); 
        return val1 + val2;
      case '-': 
        // printf("%u\n", val1 - val2); 
        return val1 - val2;
      case '*': 
        // printf("%u\n", val1 * val2); 
        return val1 * val2;
      case '/': 
        // printf("%u\n", val1 / val2); 
        return val1 / val2;
      case TK_AND: 
        // printf("%u\n", val1 && val2); 
        return val1 && val2;
      case TK_EQ: 
        // printf("%u\n", val1 == val2); 
        return val1 == val2;
      case TK_UNEQ: 
        // printf("%u\n", val1 != val2); 
        return val1 != val2;
      default: assert(0);
    }
  }
}

word_t expr(char *e, bool success) {
  if (!make_token(e)) {
    success = false;
    panic("Failed to make tokens!\n");
  } 
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '+' || \
    tokens[i - 1].type == '-' || tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || \
    tokens[i - 1].type == '(' || tokens[i - 1].type == TK_AND || \
    tokens[i - 1].type == TK_EQ || tokens[i - 1].type == TK_UNEQ) ) {
      tokens[i].type = TK_DEREF;
    }
  }

  for(int i = 0 ; i < nr_token; i++){
    printf("type:%d   str:%s\n", tokens[i].type, tokens[i].str);
  }
  return eval(0, nr_token - 1);
}

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/host.h>
#include <memory/paddr.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  char *line_read = NULL;

  // if (line_read) {
  //   printf("%s\n", line_read);
  //   free(line_read);
  //   line_read = NULL;s
  // }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_p(char *args) {
  bool success = false;
  word_t answer = expr(args, success);
  printf("The value of expression is :  %u\n", answer);
  return answer;
}

static int cmd_si(char *args){
  if(args == NULL){
    printf("Debug information:\n");
    cpu_exec(1);
    return 0;
  }
  char *ptr;
  uint64_t n = strtoul(args, &ptr, 10);
  printf("Debug information:\n");
  cpu_exec(n); 
  return 0;
}

static int cmd_info(char *args){
  if(strcmp(args, "r") == 0){
    isa_reg_display();
  }
  // else if(strcmp(args, "w")){
  //   ...
  // }
  return 0;
}

static int cmd_x(char *args) {
  if(args == NULL){
    printf("Please write the essential args!");
    return 0;
  }
  char *ptr;
  uint32_t offset = strtoul(args, &ptr, 10);
  uint32_t start_address = cmd_p(ptr);
  // printf("%u\n", offset);
  // printf("%#x\n", start_address);
  uint32_t *start_point = (uint32_t *)guest_to_host(start_address);
  printf("Scan the memory with the start of 0x%08x:\n", start_address);
  for(int i = 0; i < offset; i++){
    printf("0x%08x  0x%08x\n", start_address, *start_point);
    start_address += 4;
    start_point++;
  }
  return 0;
}

static int cmd_w(char *args) {
  // int a;
  // sscanf(args, "%d", &a);
  // if(a == 1){
  //   new_wp("234567654");
  // }
  // else if(a == 2){
  //   new_wp("asdfds");
  // }
  new_wp(args);
  return 0;
}

static int cmd_d(char *args) {
  int index;
  sscanf(args, "%d", &index);
  free_wp(index);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "si", "Let the execution of the program stop after running N instructions", cmd_si },
  { "info", "Print the status of the execution of the program", cmd_info },
  { "x", "Scan the memory", cmd_x },
  { "p", "Get the value of Expression", cmd_p },
  { "w", "Watch point", cmd_w },
  { "d", "Delete the watch point", cmd_d },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

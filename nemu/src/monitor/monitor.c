#include <isa.h>
#include <memory/paddr.h>
#include <elf.h>


void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ASNI_FMT("ON", ASNI_FG_GREEN), ASNI_FMT("OFF", ASNI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ASNI_FMT(str(__GUEST_ISA__), ASNI_FG_YELLOW ASNI_BG_RED));
  printf("For help, type \"help\"\n");
  // Log("Exercise: Please remove me in the source code and compile NEMU again.");
  // assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

//Analyse the ELF
static char *elf = NULL;
char *strtab;
Elf32_Sym *elf_symtab;
int count; //number of symtab's members

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static void analyse_elf(){
  //Load the elf into nemu
  if(elf == NULL){
    Log("No elf is given. No elf need to be analysed.");
    return ;
  }
  FILE *fp = fopen(elf, "rb");
  Assert(fp, "Can not open '%s'", elf);
  fseek(fp, 0, SEEK_END); //https://www.runoob.com/cprogramming/c-function-fseek.html
  long size = ftell(fp);
  Log("The elf is %s, size = %ld", elf, size);

  //Analyse the elf
  fseek(fp, 0, SEEK_SET); 
  Elf32_Ehdr elf_head;
  if(!fread(&elf_head, sizeof(Elf32_Ehdr), 1, fp))  panic("Read failed!\n");
  if(elf_head.e_ident[0] != 0x7f || elf_head.e_ident[1] != 'E' || elf_head.e_ident[2] != 'L' || elf_head.e_ident[3] != 'F'){
    panic("Not a ELF file!\n");
  }
  Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * elf_head.e_shnum);
  fseek(fp, elf_head.e_shoff, SEEK_SET);
  if(!fread(shdr, sizeof(Elf32_Shdr) * elf_head.e_shnum, 1, fp))  panic("Read failed!\n");
  rewind(fp);
  fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);
  char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
  char *temp = shstrtab;

  if(!fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp))  panic("Read failed!\n");
  for(int i = 0; i < elf_head.e_shnum; i++){
    temp = shstrtab;
    temp = temp + shdr[i].sh_name;
    // printf("%s\n", temp);

    // strtab
    if(strcmp(temp, ".strtab") == 0){
      strtab = (char*)malloc(sizeof(char) * shdr[i].sh_size);
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      if(!fread(strtab, shdr[i].sh_size, 1, fp))  panic("Read failed!\n");
      // printf("%s\n", 147 + strtab);
    }

    // symtab
    if(strcmp(temp, ".symtab") == 0){
      count = shdr[i].sh_size / sizeof(Elf32_Sym);
      // printf("%d\n", count);
      elf_symtab = (Elf32_Sym*)malloc(sizeof(Elf32_Sym) * count);
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      if(!fread(elf_symtab, sizeof(Elf32_Sym) * count, 1, fp))  panic("Read failed!\n");
      // for(int j = 0; j < count; j++){
      //   if(elf_symtab[j].st_info == 18){  //fun type why 18?
      //     printf("name:%s\n", strtab + elf_symtab[j].st_name);
      //     printf("addr:%x\n", elf_symtab[j].st_value);
      //     printf("size:%d\n", elf_symtab[j].st_size);
      //   }
      // }
    }
  }
  free(shdr);
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"elf"      , required_argument, NULL, 'e'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'e': elf = optarg; break;
      case 1: img_file = optarg; return optind - 1;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-e,--elf=FILE           analyse the elf\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Load the elf to be analysed. */
  analyse_elf();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif

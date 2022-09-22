#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static void audio_play(void *userdata, uint8_t *stream, int len) {
    // printf("read len:%d, count:%d\n", len, audio_base[reg_count]);
    // if (audio_base[reg_count] < len) len = audio_base[reg_count];
    // if(audio_base[reg_count] == 0){
    //   printf("count zero\n");
    //   return;
    // }
    SDL_memset(stream, 0, len);
    len = (len < audio_base[reg_sbuf_size]) ? len : audio_base[reg_sbuf_size];
    memcpy(stream, sbuf, len);
    sbuf += len;
    audio_base[reg_count] -= len;
    // if (len > nread) {
    //   printf("len > nread\n");
    //   memset(stream + nread, 0, len - nread);
    // }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if(audio_base[reg_init] == 1){
    SDL_AudioSpec s = {};
    s.freq = audio_base[reg_freq];
    s.format = AUDIO_S16SYS;
    s.channels = audio_base[reg_channels];
    s.samples = audio_base[reg_samples];
    s.callback = audio_play;
    s.userdata = NULL;
    int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (ret == 0) {
      SDL_OpenAudio(&s, NULL);
      SDL_PauseAudio(0);
    }
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif
  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);

  //init
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
  memset(sbuf, 0, audio_base[reg_sbuf_size]);
  audio_base[reg_init] = 0;
  audio_base[reg_count] = 0;
}

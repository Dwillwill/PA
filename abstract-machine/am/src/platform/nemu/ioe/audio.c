#include <am.h>
#include <nemu.h>
#include <klib.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)
#define AUDIO_SB_ADDR        (0xa1200000)

static uint32_t *freq = (uint32_t *)AUDIO_FREQ_ADDR;
static uint32_t *channels = (uint32_t *)AUDIO_CHANNELS_ADDR;
static uint32_t *samples = (uint32_t *)AUDIO_SAMPLES_ADDR;
volatile uint32_t *count = (uint32_t *)AUDIO_COUNT_ADDR;
static uint32_t *total = (uint32_t *)AUDIO_SBUF_SIZE_ADDR;
static uint32_t *init_audio = (uint32_t *)AUDIO_INIT_ADDR;
static uint8_t *buf = (uint8_t *)AUDIO_SB_ADDR;

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = false;
  cfg->bufsize = *total;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  *freq = ctrl->freq;
  *channels = ctrl->channels;
  *samples = ctrl->samples;
  *init_audio = 1;
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = *count;
}


void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int len = ctl->buf.end - ctl->buf.start;
  memcpy(buf, ctl->buf.start, len);
  buf += len;
  if(buf ==  (uint8_t *)AUDIO_SB_ADDR + *total){
    *count = *total;
    while(*count > 0);
    buf = (uint8_t *)AUDIO_SB_ADDR;
    memset(buf, 0, *total);
  }
  if(len != 4096){
    *count = (uint8_t *)AUDIO_SB_ADDR - buf;
    while(1);
  }
}

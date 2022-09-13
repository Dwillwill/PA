#include <am.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

static uint32_t *freq = (uint32_t *)AUDIO_FREQ_ADDR;
static uint32_t *channels = (uint32_t *)AUDIO_CHANNELS_ADDR;
static uint32_t *samples = (uint32_t *)AUDIO_SAMPLES_ADDR;
static volatile uint32_t *count = (uint32_t *)AUDIO_COUNT_ADDR;
static uint32_t *total = (uint32_t *)AUDIO_SBUF_SIZE_ADDR;


void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = *total;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {

  *freq = ctrl->freq;
  *channels = ctrl->channels;
  *samples = ctrl->samples;
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = *count;
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int rest_of_buf = *total - *count;
  int len = ctl->buf.end - ctl->buf.start;
  if(rest_of_buf >= len){
    uint8_t *buf = (uint32_t *)CONFIG_SB_ADDR;
    buf += *count;
    memcpy(buf, ctl->buf, len);
    *count += len; 
  }else{
    while(1){
      if(*count <= *total - len){
        memcpy((uint32_t *)CONFIG_SB_ADDR, ctl->buf, len);
        *count += len; 
        break;
      }
    }
  }
}

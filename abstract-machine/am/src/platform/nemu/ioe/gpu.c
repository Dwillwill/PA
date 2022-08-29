#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint16_t * screen_height_width = (uint16_t *)VGACTL_ADDR;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = screen_height_width[1], .height = screen_height_width[0],
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint16_t * screen_height_width = (uint16_t *)VGACTL_ADDR;
  int start = ctl->y * screen_height_width[1] + ctl->x;
  int index = 0;
  for(int i = 0; i < ctl->w * ctl->h; i++){
    index = start + (i / ctl->w) * screen_height_width[1] + (i % ctl->w);
    fb[index] = *(uint32_t *)ctl->pixels;
    ctl->pixels = ctl->pixels + 4;
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

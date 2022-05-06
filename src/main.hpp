#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "libraries/lvgl/lvgl.h"
#include "libraries/lv_drivers/display/fbdev.h"
#include "libraries/lv_drivers/indev/evdev.h"

#include "services/display_service/display_init.hpp"

#include "config/config.h"

uint32_t custom_tick_get(void){
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}   

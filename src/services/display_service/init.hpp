#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "lvgl.h"
#include "../libraries/lv_drivers/display/fbdev.h"
#include "../libraries/lv_drivers/indev/evdev.h"

#include "../socket_service/init.hpp"
#include "../assets/config.h"

namespace display{
    class disp{
        public:
            int lvgl_init();
            void splash();
            void display_init();
    };
};


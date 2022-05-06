#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "lvgl.h"
#include "../libraries/lv_drivers/display/fbdev.h"
#include "../libraries/lv_drivers/indev/evdev.h"

#include "../config/config.h"
#include "dashboard.hpp"

namespace display{
    int lvgl_init(){

        lv_init();

        fbdev_init();

        evdev_init();

        /*A small buffer for LittlevGL to draw the screen's content*/
        static lv_color_t buf[DISP_BUF_SIZE];

        /*Initialize a descriptor for the buffer*/
        static lv_disp_draw_buf_t disp_buf;
        lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

        /*Initialize and register a display driver*/
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.draw_buf   = &disp_buf;
        disp_drv.flush_cb   = fbdev_flush;
        disp_drv.hor_res    = HOR_RES;
        disp_drv.ver_res    = VER_RES;
        lv_disp_drv_register(&disp_drv);

        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = evdev_read;
        lv_indev_drv_register(&indev_drv);

        style_init();

        return (0);
    }

    void splash(){        
        lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 30);
        lv_obj_set_size(spinner, 60, 60);
        lv_obj_add_style(spinner, &arc_style, LV_PART_MAIN);
        lv_obj_add_style(spinner, &spinner_style, LV_PART_INDICATOR);
        lv_obj_set_align(spinner, LV_ALIGN_CENTER);
        lv_obj_set_pos(spinner, 0, HOR_RES/4);
        
        for(int i=0;i<500;i++) {
            lv_task_handler();
            usleep(5000);
        }

        lv_obj_del(spinner);
    }

    void display_init(){
        lvgl_init();
     
        splash();
        
        main_menu::menu_app();
    }
}

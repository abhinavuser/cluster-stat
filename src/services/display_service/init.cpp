#include "init.hpp"

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

        return (0);
    }

    void splash(){
        lv_style_t spinner_style;
        lv_style_init(&spinner_style);
        lv_style_set_arc_color(&spinner_style, lv_color_hex(0x494D66));

        lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1666, 30);
        lv_obj_set_size(spinner, 60, 60);
        lv_obj_add_style(spinner, &spinner_style, 0);
        lv_obj_set_align(spinner, LV_ALIGN_CENTER);
        lv_obj_set_pos(spinner, 0, HOR_RES/4);

        
        for(int i=0;i<1200;i++) {
            lv_task_handler();
            usleep(5000);
        }

        lv_obj_del(spinner);
    }

    void disp::display_init(){
        lvgl_init();

        lv_style_t background_style;
        lv_style_init(&background_style);
        lv_style_set_bg_color(&background_style, lv_color_hex(BG_COLOR));
        lv_obj_add_style(lv_scr_act(), &background_style, 0);

        lv_style_t chart_style;
        lv_style_init(&chart_style);
        lv_style_set_bg_color(&chart_style, lv_color_hex(FG_COLOR));
        lv_style_set_line_color(&chart_style, lv_color_hex(FG_COLOR));
        lv_style_set_border_color(&chart_style, lv_color_hex(FG_COLOR));
        

        splash();

        // start app from here:

        lv_obj_t* label = lv_label_create(lv_scr_act());
        lv_label_set_recolor(label, true);
        lv_label_set_text_fmt(label, "#ffffff 0.0#\n");
        lv_obj_center(label);

        lv_obj_t * chart;
        chart = lv_chart_create(lv_scr_act());
        lv_obj_set_size(chart, 1000, 200);
        lv_chart_set_point_count(chart, 100);
        lv_obj_center(chart);
        lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
        lv_obj_add_style(chart, &chart_style, 0);

        /*Add two data series*/
        lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

        mutex m;

        /*Handle LitlevGL tasks (tickless mode)*/
        while(1) {
            m.lock();
            //float a = socket_service::cpu_buf[0];
            lv_label_set_text_fmt(label, "#ffffff %0.2f#\n", socket_service::cpu_buf[0]);
            lv_obj_center(label);

            lv_chart_set_next_value(chart, ser1, (int)socket_service::mem_percent_buf[0]);

            m.unlock();

            lv_task_handler();
            usleep(5000*100);
        }
    }
}
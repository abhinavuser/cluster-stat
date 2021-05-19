#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
//#include "lv_examples/lv_examples.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "lv_drivers/indev/evdev.h"

#include <stdio.h>
#include <stdlib.h>

#define DISP_BUF_SIZE (80 * LV_HOR_RES_MAX)
#define BUFSIZE 128

LV_FONT_DECLARE(main_font);

LV_IMG_DECLARE(bg_image);

int shell_parse_output(char* cmd) {
    char buf[BUFSIZE];
    int output;
    FILE* fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

    while (fgets(buf, BUFSIZE, fp) != NULL) {
        sscanf(buf, "%d", &output);
    }

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        return -1;
    }

    return output;
}

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*Touch device init*/
    evdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer   = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_t* disp;
    disp = lv_disp_drv_register(&disp_drv);

    /*Initialize and register a touch driver*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv);

    /*background image*/
    int x_pos = 0;
    int x_v = 1;

    lv_obj_t * img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img, &bg_image);
    lv_img_set_zoom(img, 532);
    lv_obj_set_style_local_bg_opa(img, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    /* cpu info */

    int cpu_percent;

    lv_obj_t * carc = lv_arc_create(lv_scr_act(), NULL);
    lv_arc_set_end_angle(carc, 200);
    lv_obj_set_size(carc, 800, 800);
    lv_arc_set_range(carc, 0, 100);
    lv_obj_align(carc, NULL, LV_ALIGN_CENTER, 0, -400);

    lv_obj_t * carc_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(carc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(carc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(carc_label, "CPU");
    lv_obj_align(carc_label, carc, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * carc_percent = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_color(carc_percent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(carc_percent, "--%%");
    lv_obj_align(carc_percent, carc, LV_ALIGN_CENTER, 10, 40);



    /* ram info */
    int ram_percent, ram_value;

    lv_obj_t * rarc = lv_arc_create(lv_scr_act(), NULL);
    lv_arc_set_end_angle(rarc, 200);
    lv_obj_set_size(rarc, 400, 400);
    lv_arc_set_range(rarc, 0, 100);
    lv_obj_align(rarc, NULL, LV_ALIGN_CENTER, 200, 200);

    lv_obj_t * rarc_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(rarc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(rarc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(rarc_label, "RAM");
    lv_obj_align(rarc_label, rarc, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * rarc_usage = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_color(rarc_usage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(rarc_usage, "---- MB");
    lv_obj_align(rarc_usage, rarc, LV_ALIGN_CENTER, 0, 80);


    /* temp info */
    int temp_value;

    lv_obj_t * tarc = lv_arc_create(lv_scr_act(), NULL);
    lv_arc_set_end_angle(tarc, 200);
    lv_obj_set_size(tarc, 400, 400);
    lv_arc_set_range(tarc, 0, 100);
    lv_obj_align(tarc, NULL, LV_ALIGN_CENTER, -200, 200);

    lv_obj_t * tarc_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_font(tarc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(tarc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(tarc_label, "TEMP");
    lv_obj_align(tarc_label, tarc, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * tarc_level = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_text_color(tarc_level, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(tarc_level, "-- C");
    lv_obj_align(tarc_level, tarc, LV_ALIGN_CENTER, 0, 80);


    lv_obj_t * cpu_chart;
    cpu_chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(cpu_chart, 800, 200);
    lv_chart_set_point_count(cpu_chart, 40);
    lv_obj_align(cpu_chart, NULL, LV_ALIGN_CENTER, 0, 500);
    lv_chart_set_type(cpu_chart, LV_CHART_TYPE_LINE);  

    lv_chart_series_t * cpu_series = lv_chart_add_series(cpu_chart, LV_COLOR_SILVER);

    static lv_style_t cpu_style;
    lv_style_init(&cpu_style);
    lv_style_set_border_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    lv_style_set_line_width(&cpu_style, LV_STATE_DEFAULT, 40);

    static lv_style_t ram_style;
    lv_style_init(&ram_style);
    lv_style_set_border_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

    static lv_style_t temp_style;
    lv_style_init(&temp_style);
    lv_style_set_border_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);

    static lv_style_t cpu_bgstyle;
    lv_style_init(&cpu_bgstyle);
    lv_style_set_bg_opa(&cpu_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&cpu_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&cpu_bgstyle, LV_STATE_DEFAULT, 40);

    static lv_style_t ram_bgstyle;
    lv_style_init(&ram_bgstyle);
    lv_style_set_bg_opa(&ram_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&ram_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    static lv_style_t temp_bgstyle;
    lv_style_init(&temp_bgstyle);
    lv_style_set_bg_opa(&temp_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&temp_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_obj_add_style(carc, LV_ARC_PART_INDIC, &cpu_style);
    lv_obj_add_style(rarc, LV_ARC_PART_INDIC, &ram_style);
    lv_obj_add_style(tarc, LV_ARC_PART_INDIC, &temp_style);

    lv_obj_add_style(carc, LV_OBJ_PART_MAIN, &cpu_bgstyle);
    lv_obj_add_style(rarc, LV_OBJ_PART_MAIN, &ram_bgstyle);
    lv_obj_add_style(tarc, LV_OBJ_PART_MAIN, &temp_bgstyle);

    lv_obj_set_style_local_bg_opa(cpu_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(cpu_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {

        x_pos += x_v;

        if(x_pos < -20){
            x_v*=-1;
            }
        else if(x_pos > 20){
            x_v*=-1;
            }

        lv_obj_align(img, NULL, LV_ALIGN_CENTER, x_pos, 0);

        cpu_percent = shell_parse_output("top -b -n 1| grep 'Cpu(s)' | awk '{print $2}' | awk -F. '{print $1}'");
        ram_percent = shell_parse_output("free | grep Mem | awk '{print $3/$2}'");
        ram_value = shell_parse_output("free | grep Mem | awk '{print $3/1000.0}'");
        temp_value = shell_parse_output("cat /sys/class/thermal/thermal_zone0/temp");

        lv_arc_set_value(carc, cpu_percent);
        lv_arc_set_value(rarc, ram_percent);
        lv_arc_set_value(tarc, temp_value/1000);

        lv_chart_set_next(cpu_chart, cpu_series, cpu_percent);

        if(cpu_percent >= 50 && cpu_percent < 70){
            lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
            lv_obj_add_style(carc, LV_ARC_PART_INDIC, &cpu_style);
        }
        else if(cpu_percent >= 70){
            lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_RED);
            lv_obj_add_style(carc, LV_ARC_PART_INDIC, &cpu_style);
        }
        else{
            lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
            lv_obj_add_style(carc, LV_ARC_PART_INDIC, &cpu_style);
        }

        if(ram_percent >= 50 && ram_percent < 70){
            lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
            lv_obj_add_style(rarc, LV_ARC_PART_INDIC, &ram_style);
        }
        else if(ram_percent >= 70){
            lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_RED);
            lv_obj_add_style(rarc, LV_ARC_PART_INDIC, &ram_style);
        }
        else{
            lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
            lv_obj_add_style(rarc, LV_ARC_PART_INDIC, &ram_style);
        }

        if(temp_value/1000 >= 50 && temp_value/1000 < 70){
            lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
            lv_obj_add_style(tarc, LV_ARC_PART_INDIC, &temp_style);
        }
        else if(temp_value/1000 >= 70){
            lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_RED);
            lv_obj_add_style(tarc, LV_ARC_PART_INDIC, &temp_style);
        }
        else{
            lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
            lv_obj_add_style(tarc, LV_ARC_PART_INDIC, &temp_style);
        }

        lv_label_set_text_fmt(carc_percent, "%d %%", cpu_percent);
        lv_label_set_text_fmt(rarc_usage, "%d MB", ram_value);
        lv_label_set_text_fmt(tarc_level, "%d C", temp_value/1000);

        lv_task_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
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

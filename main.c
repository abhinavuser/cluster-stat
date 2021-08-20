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
#include "main.h"

#define DISP_BUF_SIZE (80 * LV_HOR_RES_MAX)

LV_FONT_DECLARE(main_font);
LV_FONT_DECLARE(secondary_font);
LV_FONT_DECLARE(tertiary_font);

LV_IMG_DECLARE(bg_image);
LV_IMG_DECLARE(corner);

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


    /* misc variables */
    char line[BUFSIZE];
    char ip_address[16]; 
    FILE* fp;
    int nodes = 0;
    int i = 0;
    int flag = 1;

    /*Initialize the config file*/
    if((fp = fopen("node.conf", "r")) == NULL){
        printf("ERROR! could not read configuration file\n");
        flag = 0;
    }

    else{
        printf("reading configuration file...\n");
        while(fgets(line, BUFSIZE, fp) != NULL){
            
            if(line[0] == '#' || line[0] == '\n' )continue;
            
            else{
                sscanf(line, "%s\n", ip_address);
                nodes++;
            }
        }
        printf("number of nodes: %d\n", nodes);
        fseek(fp, 0, SEEK_SET);
    }

    int mem_alloc = (16 * nodes) - 1; 

    char ip_addresses[nodes][16];
    char hosts[mem_alloc];

    if(flag){
        while(fgets(line, BUFSIZE, fp) != NULL){
            
            if(line[0] == '#' || line[0] == '\n' ) continue;
            
            else{
                sscanf(line, "%s\n", ip_address);
                strcpy(ip_addresses[i], ip_address);
                strcat(hosts, ip_address);

                if(i != nodes - 1)strcat(hosts, ",");
                
                i++;
            }
        }
        fclose(fp);
    }

    /*Create the final command*/
    char cmd[mem_alloc];
    char tmp_cmd[mem_alloc + 128];

    if(flag)sprintf(cmd, "mpiexec -n %d --host %s ", nodes, hosts);

    /*******************
        UI elements
    *******************/

    /*background image*/
    int x_pos = 0;
    int x_v = 0;

    bg_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(bg_img, &bg_image);
    //lv_img_set_zoom(img, 532);
    lv_obj_set_style_local_bg_opa(bg_img, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_align(bg_img, NULL, LV_ALIGN_CENTER, 0, 0);

    tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_opa(tabview, LV_TABVIEW_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_color(tabview, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(tabview, LV_TABVIEW_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_obj_set_style_local_bg_opa(tabview, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT, LV_OPA_20);

    about_tab = lv_tabview_add_tab(tabview, "About");
    main_tab = lv_tabview_add_tab(tabview, "Home");
    menu_tab = lv_tabview_add_tab(tabview, "Menu"); 
    lv_tabview_set_tab_act(tabview, 1, LV_ANIM_OFF);

    /* rounded corners */
    corner_1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(corner_1, &corner);
    lv_obj_set_style_local_bg_opa(corner_1, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_align(corner_1, NULL, LV_ALIGN_IN_TOP_RIGHT, 80, -80);

    corner_2 = lv_img_create(lv_scr_act(), corner_1);
    lv_obj_align(corner_2, NULL, LV_ALIGN_IN_BOTTOM_LEFT, -80, 80);

    corner_3 = lv_img_create(lv_scr_act(), corner_1);
    lv_obj_align(corner_3, NULL, LV_ALIGN_IN_TOP_LEFT, -80, -80);

    /* bg element blur element */
    bg_element = lv_obj_create(main_tab, NULL);
    lv_obj_set_style_local_bg_color(bg_element, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_opa(bg_element, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
    lv_obj_set_style_local_border_opa(bg_element, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_radius(bg_element, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 100);
    lv_obj_set_size(bg_element, 800, 800);
    lv_obj_align(bg_element, main_tab, LV_ALIGN_CENTER, 0, 0);

    /* top element blur element */
    top_element = lv_obj_create(main_tab, bg_element);
    lv_obj_set_size(bg_element, 1080, 80);
    lv_obj_set_style_local_radius(bg_element, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_align(bg_element, main_tab, LV_ALIGN_IN_TOP_MID, 0, -30);

    /* time_clock element */
    int time_clock_flag = 0;
    char min[3];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    time_clock_shadow = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_color(time_clock_shadow, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_text_font(time_clock_shadow, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &secondary_font);
    lv_label_set_text(time_clock_shadow, "--:--");
    lv_obj_align(time_clock_shadow, main_tab, LV_ALIGN_IN_TOP_MID, -2, 18);

    time_clock = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_color(time_clock, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_obj_set_style_local_text_font(time_clock, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &secondary_font);
    lv_label_set_text(time_clock, "--:--");
    lv_obj_align(time_clock, main_tab, LV_ALIGN_IN_TOP_MID, 0, 20);

    /********* 
    about menu 
    *********/

    cluster_stats = lv_label_create(about_tab, NULL);
    lv_obj_set_style_local_text_font(cluster_stats, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &tertiary_font);
    lv_obj_set_style_local_text_color(cluster_stats, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_obj_align(cluster_stats, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    cluster_stats_shadow = lv_label_create(about_tab, cluster_stats);
    lv_obj_set_style_local_text_color(cluster_stats, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_align(cluster_stats, NULL, LV_ALIGN_IN_TOP_LEFT, 18, 18);

    /*************
    data elements
    *************/

    /* cpu usage chart */
    cpu_chart = lv_chart_create(main_tab, NULL);
    lv_obj_set_size(cpu_chart, 300, 300);
    lv_chart_set_point_count(cpu_chart, 60);
    lv_obj_align(cpu_chart, main_tab, LV_ALIGN_CENTER, 0, -200);
    lv_chart_set_type(cpu_chart, LV_CHART_TYPE_LINE);  

    lv_chart_series_t * cpu_series = lv_chart_add_series(cpu_chart, LV_COLOR_GREEN);
    
    /* ram usage chart */
    ram_chart = lv_chart_create(main_tab, NULL);
    lv_obj_set_size(ram_chart, 300, 300);
    lv_chart_set_point_count(ram_chart, 60);
    lv_obj_align(ram_chart, main_tab, LV_ALIGN_CENTER, 200, 200);
    lv_chart_set_type(ram_chart, LV_CHART_TYPE_LINE);  

    lv_chart_series_t * ram_series = lv_chart_add_series(ram_chart, LV_COLOR_GREEN);

    /* temp usage chart */
    temp_chart = lv_chart_create(main_tab, NULL);
    lv_obj_set_size(temp_chart, 300, 300);
    lv_chart_set_point_count(temp_chart, 60);
    lv_obj_align(temp_chart, main_tab, LV_ALIGN_CENTER, -200, 200);
    lv_chart_set_type(temp_chart, LV_CHART_TYPE_LINE);  

    lv_chart_series_t * temp_series = lv_chart_add_series(temp_chart, LV_COLOR_GREEN);

    /* cpu info */
    cpu_arc = lv_arc_create(main_tab, NULL);
    lv_arc_set_end_angle(cpu_arc, 200);
    lv_obj_set_size(cpu_arc, 400, 400);
    lv_arc_set_range(cpu_arc, 0, 100);
    lv_obj_align(cpu_arc, main_tab, LV_ALIGN_CENTER, 0, -200);

    cpu_arc_label = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_font(cpu_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(cpu_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(cpu_arc_label, "CPU");
    lv_obj_align(cpu_arc_label, cpu_arc, LV_ALIGN_CENTER, 0, 0);

    cpu_arc_label_shadow = lv_label_create(main_tab, cpu_arc_label);
    lv_obj_set_style_local_text_color(cpu_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_align(cpu_arc_label_shadow, cpu_arc, LV_ALIGN_CENTER, 4, 4);

    cpu_arc_percent = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_color(cpu_arc_percent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_obj_set_style_local_text_font(cpu_arc_percent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &secondary_font);
    lv_label_set_text(cpu_arc_percent, "--%%");
    lv_obj_align(cpu_arc_percent, cpu_arc, LV_ALIGN_CENTER, 10, 80);


    /* ram info */
    ram_arc = lv_arc_create(main_tab, NULL);
    lv_arc_set_end_angle(ram_arc, 200);
    lv_obj_set_size(ram_arc, 400, 400);
    lv_arc_set_range(ram_arc, 0, 100);
    lv_obj_align(ram_arc, main_tab, LV_ALIGN_CENTER, 200, 200);

    ram_arc_label = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_font(ram_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(ram_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(ram_arc_label, "RAM");
    lv_obj_align(ram_arc_label, ram_arc, LV_ALIGN_CENTER, 0, 0);

    ram_arc_label_shadow = lv_label_create(main_tab, ram_arc_label);
    lv_obj_set_style_local_text_color(ram_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_align(ram_arc_label_shadow, ram_arc, LV_ALIGN_CENTER, 4, 4);

    ram_arc_usage = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_color(ram_arc_usage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);    
    lv_obj_set_style_local_text_font(ram_arc_usage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &secondary_font);
    lv_label_set_text(ram_arc_usage, "---- MB");
    lv_obj_align(ram_arc_usage, ram_arc, LV_ALIGN_CENTER, 0, 80);


    /* temp info */
    temp_arc = lv_arc_create(main_tab, NULL);
    lv_arc_set_end_angle(temp_arc, 200);
    lv_obj_set_size(temp_arc, 400, 400);
    lv_arc_set_range(temp_arc, 0, 100);
    lv_obj_align(temp_arc, main_tab, LV_ALIGN_CENTER, -200, 200);

    temp_arc_label = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_font(temp_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &main_font);
    lv_obj_set_style_local_text_color(temp_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_label_set_text(temp_arc_label, "TEMP");
    lv_obj_align(temp_arc_label, temp_arc, LV_ALIGN_CENTER, 0, 0);

    temp_arc_label_shadow = lv_label_create(main_tab, temp_arc_label);
    lv_obj_set_style_local_text_color(temp_arc_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_align(temp_arc_label_shadow, temp_arc, LV_ALIGN_CENTER, 4, 4);

    temp_arc_level = lv_label_create(main_tab, NULL);
    lv_obj_set_style_local_text_color(temp_arc_level, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_obj_set_style_local_text_font(temp_arc_level, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &secondary_font);
    lv_label_set_text(temp_arc_level, "-- C");
    lv_obj_align(temp_arc_level, temp_arc, LV_ALIGN_CENTER, 0, 80);

    /* styling */    

    lv_style_init(&cpu_style);
    lv_style_set_border_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_line_width(&cpu_style, LV_STATE_DEFAULT, 30);

    lv_style_init(&ram_style);
    lv_style_set_border_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_line_width(&ram_style, LV_STATE_DEFAULT, 30);

    lv_style_init(&temp_style);
    lv_style_set_border_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_line_width(&temp_style, LV_STATE_DEFAULT, 30);

    lv_style_init(&cpu_bgstyle);
    lv_style_set_bg_opa(&cpu_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&cpu_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&cpu_bgstyle, LV_STATE_DEFAULT, 30);

    lv_style_init(&ram_bgstyle);
    lv_style_set_bg_opa(&ram_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&ram_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&ram_bgstyle, LV_STATE_DEFAULT, 30);

    lv_style_init(&temp_bgstyle);
    lv_style_set_bg_opa(&temp_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_border_opa(&temp_bgstyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_line_width(&temp_bgstyle, LV_STATE_DEFAULT, 30);

    lv_obj_add_style(cpu_arc, LV_ARC_PART_INDIC, &cpu_style);
    lv_obj_add_style(ram_arc, LV_ARC_PART_INDIC, &ram_style);
    lv_obj_add_style(temp_arc, LV_ARC_PART_INDIC, &temp_style);

    lv_obj_add_style(cpu_arc, LV_OBJ_PART_MAIN, &cpu_bgstyle);
    lv_obj_add_style(ram_arc, LV_OBJ_PART_MAIN, &ram_bgstyle);
    lv_obj_add_style(temp_arc, LV_OBJ_PART_MAIN, &temp_bgstyle);

    lv_obj_set_style_local_line_opa(cpu_arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_20);
    lv_obj_set_style_local_line_opa(ram_arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_20);
    lv_obj_set_style_local_line_opa(temp_arc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_20);

    
    lv_obj_set_style_local_bg_opa(cpu_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(cpu_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_line_opa(cpu_chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, LV_OPA_30);
    
    lv_obj_set_style_local_bg_opa(ram_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(ram_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_line_opa(ram_chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, LV_OPA_30);

    lv_obj_set_style_local_bg_opa(temp_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(temp_chart, LV_CHART_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_line_opa(temp_chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, LV_OPA_30);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {

        x_pos += x_v;

        if(x_pos < -200){
            x_v*=-1;
            }
        else if(x_pos > 200){
            x_v*=-1;
            }

        lv_obj_align(bg_img, NULL, LV_ALIGN_CENTER, x_pos, 0); 
        
        if(flag){
            strcpy(tmp_cmd, cmd);
            strcat(tmp_cmd, "top -b -n 1| grep 'Cpu(s)' | awk '{print $2}' | awk -F. '{print $1}'");
            cpu_percent = multi_shell_parse_output(tmp_cmd, 0);

            strcpy(tmp_cmd, cmd);
            strcat(tmp_cmd, "free | grep Mem | awk '{print 100 * ($3/$2)}'");
            ram_percent = multi_shell_parse_output(tmp_cmd, 0);

            strcpy(tmp_cmd, cmd);
            strcat(tmp_cmd, "free | grep Mem | awk '{print $3/1000.0}'");
            ram_value = multi_shell_parse_output(tmp_cmd, 1);

            strcpy(tmp_cmd, cmd);
            strcat(tmp_cmd, "cat /sys/class/thermal/thermal_zone0/temp");
            temp_value = multi_shell_parse_output(tmp_cmd, 0);
        }

        else{
            cpu_percent = shell_parse_output("top -b -n 1| grep 'Cpu(s)' | awk '{print $2}' | awk -F. '{print $1}'");
            ram_percent = shell_parse_output("free | grep Mem | awk '{print 100 * ($3/$2)}'");
            ram_value = shell_parse_output("free | grep Mem | awk '{print $3/1000.0}'");
            temp_value = shell_parse_output("cat /sys/class/thermal/thermal_zone0/temp");
        }

        ram_max = ram_value * (100/ram_percent);

        lv_arc_set_value(cpu_arc, cpu_percent);
        lv_arc_set_value(ram_arc, ram_percent);
        lv_arc_set_value(temp_arc, temp_value/1000);

        lv_chart_set_next(cpu_chart, cpu_series, cpu_percent);
        lv_chart_set_next(ram_chart, ram_series, ram_percent);
        lv_chart_set_next(temp_chart, temp_series, temp_value/1000);

        main_refresh();

        lv_label_set_text_fmt(cpu_arc_percent, "%d %%", cpu_percent);
        lv_label_set_text_fmt(ram_arc_usage, "  %d /\n %d MB", ram_value, ram_max);
        lv_label_set_text_fmt(temp_arc_level, "%d C", temp_value/1000);

        lv_obj_align(ram_arc_usage, ram_arc, LV_ALIGN_CENTER, 0, 80);
        lv_obj_align(cpu_arc_percent, cpu_arc, LV_ALIGN_CENTER, 10, 80);
        lv_obj_align(temp_arc_level, temp_arc, LV_ALIGN_CENTER, 0, 80);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        if(tm.tm_min < 10){
            sprintf(min, "0%d", tm.tm_min);
        }
        else{
            sprintf(min, "%d", tm.tm_min);
        }

        if (time_clock_flag == 0){
            time_clock_flag++;
            lv_label_set_text_fmt(time_clock_shadow, "%d:%c%c", tm.tm_hour, min[0], min[1]);
            lv_label_set_text_fmt(time_clock, "%d:%c%c", tm.tm_hour, min[0], min[1]);
        }
        else{
            time_clock_flag = 0;
            lv_label_set_text_fmt(time_clock_shadow, "%d %c%c", tm.tm_hour, min[0], min[1]);
            lv_label_set_text_fmt(time_clock, "%d %c%c", tm.tm_hour, min[0], min[1]);
        }

        about_refresh(flag, nodes);

        lv_task_handler();
        usleep(100000);
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
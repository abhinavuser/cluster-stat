#include <stdio.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"

#define BUFSIZE 128

/* shell functions */
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

/* meta input:
0: mean value
1: sum value
*/

int multi_shell_parse_output(char* cmd, int meta) {
    char buf[BUFSIZE];
    int output;
    FILE* fp;

    int total = 0;
    int i = 0;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

    while (fgets(buf, BUFSIZE, fp) != NULL) {
        sscanf(buf, "%d\n", &output);
        total += output;
        i++;
    }

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        return -1;
    }

    if(meta == 0){
        output = total/i;
    }

    if(meta == 1){
        output = total;
    }

    return output;
}


/* lvgl objects */
lv_obj_t * bg_img;

lv_obj_t * tabview;

lv_obj_t * about_tab;
lv_obj_t * main_tab;
lv_obj_t * menu_tab;

lv_obj_t * corner_1;
lv_obj_t * corner_2;
lv_obj_t * corner_3;

lv_obj_t * bg_element;
lv_obj_t * top_element;

lv_obj_t * time_clock_shadow;
lv_obj_t * time_clock;

lv_obj_t * cpu_arc;
lv_obj_t * cpu_arc_label;
lv_obj_t * cpu_arc_label_shadow;
lv_obj_t * cpu_arc_percent;

lv_obj_t * ram_arc;
lv_obj_t * ram_arc_label;
lv_obj_t * ram_arc_label_shadow;
lv_obj_t * ram_arc_usage;

lv_obj_t * temp_arc;
lv_obj_t * temp_arc_label;
lv_obj_t * temp_arc_label_shadow;
lv_obj_t * temp_arc_level;

lv_obj_t * cpu_chart;
lv_obj_t * ram_chart;
lv_obj_t * temp_chart;

lv_obj_t * cluster_stats;
lv_obj_t * cluster_stats_shadow;

static lv_style_t cpu_style;
static lv_style_t ram_style;
static lv_style_t temp_style;

static lv_style_t cpu_bgstyle;
static lv_style_t ram_bgstyle;
static lv_style_t temp_bgstyle;

/* misc variables */
int cpu_percent;

int ram_percent, ram_value;
int ram_max;

int temp_value;

int main_refresh(){
    if(cpu_percent >= 50 && cpu_percent < 70){
            lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
            lv_obj_add_style(cpu_arc, LV_ARC_PART_INDIC, &cpu_style);
    }
    else if(cpu_percent >= 70){
        lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_add_style(cpu_arc, LV_ARC_PART_INDIC, &cpu_style);
    }
    else{
        lv_style_set_line_color(&cpu_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_obj_add_style(cpu_arc, LV_ARC_PART_INDIC, &cpu_style);
    }

    if(ram_percent >= 50 && ram_percent < 70){
        lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_add_style(ram_arc, LV_ARC_PART_INDIC, &ram_style);
    }
    else if(ram_percent >= 70){
        lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_add_style(ram_arc, LV_ARC_PART_INDIC, &ram_style);
    }
    else{
        lv_style_set_line_color(&ram_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_obj_add_style(ram_arc, LV_ARC_PART_INDIC, &ram_style);
    }

    if(temp_value/1000 >= 50 && temp_value/1000 < 70){
        lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
        lv_obj_add_style(temp_arc, LV_ARC_PART_INDIC, &temp_style);
    }
    else if(temp_value/1000 >= 70){
        lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_obj_add_style(temp_arc, LV_ARC_PART_INDIC, &temp_style);
    }
    else{
        lv_style_set_line_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_obj_add_style(temp_arc, LV_ARC_PART_INDIC, &temp_style);
    }

    return(0);
}

int about_refresh(int flag, int nodes){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char min[3];
    char sec[3];
    
    if(tm.tm_min < 10){
        sprintf(min, "0%d", tm.tm_min);
    }
    else{
        sprintf(min, "%d", tm.tm_min);
    }

    if(tm.tm_sec < 10){
        sprintf(sec, "0%d", tm.tm_sec);
    }
    else{
        sprintf(sec, "%d", tm.tm_sec);
    }

    lv_label_set_text_fmt(cluster_stats_shadow, 
    "Surveillant system\n\n"
    "System time: %d:%c%c:%c%c \n\n"
    "Cluster mode enabled: %d \n"
    "Number of nodes: %d \n\n"
    "CPU usage: %d %% \n\n"
    "RAM used (in megabytes): %d MB \n"
    "Total RAM available to system: %d MB \n\n"
    "System temperature: %d C \n",
    tm.tm_hour, min[0], min[1], sec[0], sec[1],
    flag,
    nodes,
    cpu_percent,
    ram_value,
    ram_max,
    temp_value/1000);

    lv_label_set_text_fmt(cluster_stats, lv_label_get_text(cluster_stats_shadow));
}

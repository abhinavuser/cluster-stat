#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "lvgl.h"

#include "../socket_service/socket_init.hpp"
#include "styling.hpp"

namespace display{
    namespace main_menu{

        static void event_handler(lv_event_t * e)
        {
            lv_event_code_t code = lv_event_get_code(e);
            if(code == LV_EVENT_CLICKED) {
                exit(0);
            }
        }

        struct cluster_menu{
            lv_obj_t* cpu_arc;
            lv_obj_t* mem_arc;
            lv_obj_t* cpu_label;
            lv_obj_t* mem_label;
            lv_obj_t* cpu_chart;
            lv_obj_t* mem_chart;
            lv_chart_series_t * cpu_series;
            lv_chart_series_t * mem_series;
            lv_obj_t* temp_label;
            lv_obj_t* btn;
            lv_obj_t* btn_label;
        };

        cluster_menu cluster_init(lv_obj_t* tab){
            cluster_menu menu_obj;

            int box_size = round(HOR_RES/3);
            int box_padding = round(box_size/3);

            /*cpu arc*/
            menu_obj.cpu_arc = lv_arc_create(tab);
            lv_obj_set_size(menu_obj.cpu_arc, box_size, box_size);
            lv_arc_set_rotation(menu_obj.cpu_arc, 135);
            lv_arc_set_bg_angles(menu_obj.cpu_arc, 0, 270);
            lv_obj_remove_style(menu_obj.cpu_arc, NULL, LV_PART_KNOB);
            lv_obj_add_style(menu_obj.cpu_arc, &cpu_arc_style, LV_PART_INDICATOR);
            lv_obj_add_style(menu_obj.cpu_arc, &arc_style, LV_PART_MAIN);
            lv_arc_set_value(menu_obj.cpu_arc, 0);
            lv_obj_align(menu_obj.cpu_arc, LV_ALIGN_CENTER, -(box_size/2 + box_padding), 0);

            menu_obj.mem_arc = lv_arc_create(tab);
            lv_obj_set_size(menu_obj.mem_arc, box_size, box_size);
            lv_arc_set_rotation(menu_obj.mem_arc, 135);
            lv_arc_set_bg_angles(menu_obj.mem_arc, 0, 270);
            lv_obj_remove_style(menu_obj.mem_arc, NULL, LV_PART_KNOB);
            lv_obj_add_style(menu_obj.mem_arc, &mem_arc_style, LV_PART_INDICATOR);
            lv_obj_add_style(menu_obj.mem_arc, &arc_style, LV_PART_MAIN);
            lv_arc_set_value(menu_obj.mem_arc, 0);
            lv_obj_align(menu_obj.mem_arc, LV_ALIGN_CENTER, box_size/2 + box_padding, 0);

            menu_obj.cpu_label = lv_label_create(menu_obj.cpu_arc);
            lv_label_set_recolor(menu_obj.cpu_label, true);
            lv_obj_set_style_text_font(menu_obj.cpu_label, &lv_font_montserrat_20, 0);
            lv_label_set_text_fmt(menu_obj.cpu_label, "#aaaaaa CPU: 0.00 %%#");
            lv_obj_align(menu_obj.cpu_label, LV_ALIGN_CENTER, 0, 0);

            menu_obj.mem_label = lv_label_create(menu_obj.mem_arc);
            lv_label_set_recolor(menu_obj.mem_label, true);
            lv_obj_set_style_text_font(menu_obj.mem_label, &lv_font_montserrat_20, 0);
            lv_label_set_text_fmt(menu_obj.mem_label, "#aaaaaa RAM: 0 MB /# #aaaaaa 0.00 %%#");
            lv_obj_align(menu_obj.mem_label, LV_ALIGN_CENTER, 0, 0);

            menu_obj.temp_label = lv_label_create(tab);
            lv_label_set_recolor(menu_obj.temp_label, true);
            lv_obj_set_style_text_font(menu_obj.temp_label, &lv_font_montserrat_20, 0);
            lv_label_set_text_fmt(menu_obj.temp_label, "#aaaaaa 0.00 °C#");
            lv_obj_align(menu_obj.temp_label, LV_ALIGN_CENTER, 0, 0);

            menu_obj.cpu_chart = lv_chart_create(menu_obj.cpu_arc);
            lv_obj_set_size(menu_obj.cpu_chart, round(box_size/1.75), round(box_size/3));
            lv_chart_set_point_count(menu_obj.cpu_chart, 100);
            lv_obj_align(menu_obj.cpu_chart, LV_ALIGN_BOTTOM_MID, 0, 0);        
            lv_chart_set_type(menu_obj.cpu_chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
            lv_obj_add_event_cb(menu_obj.cpu_chart, grad_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
            lv_obj_add_style(menu_obj.cpu_chart, &chart_style, 0);
            lv_obj_add_style(menu_obj.cpu_chart, &chart_line_style, LV_PART_INDICATOR);
            lv_obj_set_style_bg_opa(menu_obj.cpu_chart, LV_OPA_20, 0);
            
            menu_obj.cpu_series = lv_chart_add_series(menu_obj.cpu_chart, lv_color_hex(CPU_COLOR), LV_CHART_AXIS_PRIMARY_Y);

            menu_obj.mem_chart = lv_chart_create(menu_obj.mem_arc);
            lv_obj_set_size(menu_obj.mem_chart, round(box_size/1.75), round(box_size/3));
            lv_chart_set_point_count(menu_obj.mem_chart, 100);
            lv_obj_align(menu_obj.mem_chart, LV_ALIGN_BOTTOM_MID, 0, 0);        
            lv_chart_set_type(menu_obj.mem_chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
            lv_obj_add_event_cb(menu_obj.mem_chart, grad_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
            lv_obj_add_style(menu_obj.mem_chart, &chart_style, 0);
            lv_obj_add_style(menu_obj.mem_chart, &chart_line_style, LV_PART_INDICATOR);
            lv_obj_set_style_bg_opa(menu_obj.mem_chart, LV_OPA_20, 0);
            
            menu_obj.mem_series = lv_chart_add_series(menu_obj.mem_chart, lv_color_hex(MEM_COLOR), LV_CHART_AXIS_PRIMARY_Y);

            menu_obj.btn = lv_btn_create(tab);
            lv_obj_add_event_cb(menu_obj.btn, event_handler, LV_EVENT_ALL, NULL);
            lv_obj_set_style_bg_color(menu_obj.btn, lv_color_hex(BTN_COLOR), 0);
            lv_obj_set_size(menu_obj.btn, 80, 60);
            lv_obj_align(menu_obj.btn, LV_ALIGN_CENTER, 0, 60);

            menu_obj.btn_label = lv_label_create(menu_obj.btn);
            lv_obj_set_style_text_font(menu_obj.btn_label, &lv_font_montserrat_28, 0);
            lv_obj_set_style_text_color(menu_obj.btn_label, lv_color_hex(BTN_FONT_COLOR), 0);
            lv_label_set_text_fmt(menu_obj.btn_label, "%s", LV_SYMBOL_LOOP);

            return menu_obj;
        }


        struct node_graph{
            lv_obj_t* chart;
            lv_obj_t* label;
            lv_obj_t* status_tag;
            lv_chart_series_t* series_1;
            lv_chart_series_t* series_2;
        };

        std::vector<node_graph> node_init(lv_obj_t * tab){
            std::vector<node_graph> menu_obj;

            int x_padding = 20;
            int y_padding = 20;
            int y_pos = y_padding;

            int graphs_per_screen = 6;

            int node_hor = HOR_RES - 3*(x_padding);
            int node_ver = round(VER_RES/graphs_per_screen) - 1.5 * y_padding;

            for(int i = 0; i<socket_service::json_buf.size();i++){
                node_graph node_obj;

                node_obj.chart = lv_chart_create(tab);
                lv_obj_set_size(node_obj.chart, node_hor, node_ver);
                lv_chart_set_point_count(node_obj.chart, 100);
                lv_obj_set_pos(node_obj.chart, x_padding/2, y_pos);        /*incremental Y coord position*/
                lv_chart_set_type(node_obj.chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
                lv_obj_add_event_cb(node_obj.chart, grad_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
                lv_obj_add_style(node_obj.chart, &chart_style, 0);
                lv_obj_add_style(node_obj.chart, &chart_line_style, LV_PART_INDICATOR);

                node_obj.label = lv_label_create(node_obj.chart);
                
                lv_label_set_recolor(node_obj.label, true);
                lv_obj_align(node_obj.label, LV_ALIGN_TOP_LEFT, 40, 10);
                lv_obj_set_style_text_font(node_obj.label, &lv_font_montserrat_20, 0);

                node_obj.status_tag = lv_obj_create(node_obj.chart);
                lv_obj_set_size(node_obj.status_tag, 20, 20);
                lv_obj_align(node_obj.status_tag, LV_ALIGN_TOP_LEFT, 10, 10);
                lv_obj_set_style_border_opa(node_obj.status_tag, LV_OPA_0, 0);
                lv_obj_set_style_bg_color(node_obj.status_tag, lv_palette_main(LV_PALETTE_GREEN), 0);

                node_obj.series_1 = lv_chart_add_series(node_obj.chart, lv_color_hex(CPU_COLOR), LV_CHART_AXIS_PRIMARY_Y);
                node_obj.series_2 = lv_chart_add_series(node_obj.chart, lv_color_hex(MEM_COLOR), LV_CHART_AXIS_PRIMARY_Y);

                menu_obj.push_back(node_obj);
                y_pos+=node_ver + y_padding;
            }

            return menu_obj;
        }
        
        void menu_app(){        
            lv_obj_t *tabview;
            tabview = lv_tabview_create(lv_scr_act(), LV_DIR_BOTTOM, 80);
            lv_obj_set_style_bg_color(tabview, lv_color_hex(BG_COLOR), 0);

            lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
            lv_obj_set_style_bg_color(tab_btns, lv_color_hex(BTN_COLOR), 0);
            lv_obj_set_style_text_color(tab_btns, lv_color_hex(BTN_FONT_COLOR), 0);
            lv_obj_set_style_bg_color(tab_btns, lv_color_hex(MENU_ACCENT_COLOR), LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_set_style_text_color(tab_btns, lv_color_hex(MENU_ACCENT_COLOR), LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_set_style_border_color(tab_btns, lv_color_hex(MENU_ACCENT_COLOR), LV_PART_ITEMS | LV_STATE_CHECKED);
            lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);

            lv_obj_t* tab_1 = lv_tabview_add_tab(tabview, "cluster view");
            lv_obj_t* tab_2 = lv_tabview_add_tab(tabview, "node view");

            cluster_menu cluster_menu_obj = cluster_init(tab_1);
            std::vector<node_graph> node_menu_obj = node_init(tab_2);
            
            mutex m;
    
            while(1) {
                m.lock();

                int n = socket_service::json_buf.size();

                /* cluster menu updating */
                float cpu_avg = 0;
                int mem_avg = 0;
                float mem_percent_avg = 0;
                float temp_avg = 0;

                for(int i = 0; i<socket_service::json_buf.size();i++){
                    cpu_avg += socket_service::cpu_buf[i]; 
                    mem_avg += socket_service::mem_buf[i]/1000;
                    mem_percent_avg += socket_service::mem_percent_buf[i];
                    temp_avg += socket_service::temp_buf[i]/1000;
                }

                cpu_avg = cpu_avg/n;
                mem_percent_avg = mem_percent_avg/n;
                temp_avg = temp_avg/n;

                lv_chart_set_next_value(cluster_menu_obj.cpu_chart, cluster_menu_obj.cpu_series, cpu_avg);
                lv_chart_set_next_value(cluster_menu_obj.mem_chart, cluster_menu_obj.mem_series, mem_percent_avg);

                lv_arc_set_value(cluster_menu_obj.cpu_arc, cpu_avg);
                lv_label_set_text_fmt(cluster_menu_obj.cpu_label, "#%s CPU: %.2f %%#", primary_font_color.c_str(), cpu_avg);

                lv_arc_set_value(cluster_menu_obj.mem_arc, mem_percent_avg);
                lv_label_set_text_fmt(cluster_menu_obj.mem_label, "#%s RAM: %d MB /# #%s %.2f %%#", 
                                                                                        primary_font_color.c_str(),
                                                                                        mem_avg, 
                                                                                        primary_font_color.c_str(),
                                                                                        mem_percent_avg);

                lv_label_set_text_fmt(cluster_menu_obj.temp_label, "#%s %.2f °C#", primary_font_color.c_str(), temp_avg);

                /* node menu updating */
                for(int i = 0; i<n ;i++){

                    node_graph node_obj = node_menu_obj[i];

                    lv_chart_set_next_value(node_obj.chart, node_obj.series_1, (int)socket_service::cpu_buf[i]);
                    lv_chart_set_next_value(node_obj.chart, node_obj.series_2, (int)socket_service::mem_percent_buf[i]);

                    lv_label_set_text_fmt(node_obj.label, "#%s %s@##%s %s#\n\n"
                                                            "#%s CPU: %.2f %%#\n\n"
                                                            "#%s MEM: %d MB#\n\n"
                                                            "#%s MEM PERCENT: %.2f %%#\n\n"
                                                            "#%s TEMP: %.2f °C#",
                                                                primary_font_color.c_str(), 
                                                                socket_service::host_id[i].c_str(),
                                                                primary_font_color.c_str(),
                                                                socket_service::ip_addresses[i].c_str(),
                                                                secondary_font_color.c_str(),
                                                                socket_service::cpu_buf[i],
                                                                secondary_font_color.c_str(), 
                                                                socket_service::mem_buf[i]/1000,
                                                                secondary_font_color.c_str(),
                                                                socket_service::mem_percent_buf[i],
                                                                secondary_font_color.c_str(),
                                                                socket_service::temp_buf[i]/1000
                                                                );

                    if(socket_service::brownout_buf[i]){
                        lv_obj_set_style_bg_color(node_obj.status_tag, lv_palette_main(LV_PALETTE_RED), 0);
                    }
                    else{
                        lv_obj_set_style_bg_color(node_obj.status_tag, lv_palette_main(LV_PALETTE_GREEN), 0);
                    }
                }

                m.unlock();
                lv_task_handler();
                usleep(5000*50);
            }
        }
    }
}

/*
rainbow: "  #80198c IP: %s#\n\n  #ba0072 CPU: %.2f %%#\n\n  #db2950 MEM: %d MB#\n\n  #e55d2c MEM PERCENT: %.2f %%#\n\n  #db8d00 TEMP: %.2f °C#\n"
*/

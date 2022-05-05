#include "lvgl.h"
#include "json.hpp"

using nlohmann::json;

namespace display{

    /* gradient implementation adopted from LVGLv8 examples page*/ 
    static void grad_draw_event_cb(lv_event_t * e){
        lv_obj_t * obj = lv_event_get_target(e);

        /*Add the faded area before the lines are drawn*/
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
        if(dsc->part == LV_PART_ITEMS) {
            if(!dsc->p1 || !dsc->p2) return;

            /*Add a line mask that keeps the area below the line*/
            lv_draw_mask_line_param_t line_mask_param;
            lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
            int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

            /*Add a fade effect: transparent bottom covering top*/
            lv_coord_t h = lv_obj_get_height(obj);
            lv_draw_mask_fade_param_t fade_mask_param;
            lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,obj->coords.y2);
            int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

            /*Draw a rectangle that will be affected by the mask*/
            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_opa = LV_OPA_20;
            draw_rect_dsc.bg_color = dsc->line_dsc->color;

            lv_area_t a;
            a.x1 = dsc->p1->x;
            a.x2 = dsc->p2->x - 1;
            a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
            a.y2 = obj->coords.y2;
            lv_draw_rect(&a, dsc->clip_area, &draw_rect_dsc);

            /*Remove the masks*/
            lv_draw_mask_free_param(&line_mask_param);
            lv_draw_mask_free_param(&fade_mask_param);
            lv_draw_mask_remove_id(line_mask_id);
            lv_draw_mask_remove_id(fade_mask_id);
        }
    }


    /* style sheets*/
    lv_style_t background_style;

    lv_style_t chart_style;
    lv_style_t chart_line_style;

    lv_style_t spinner_style_indicator;

    lv_style_t cpu_arc_style;
    lv_style_t mem_arc_style;

    lv_style_t tab_style;

    /* theme colors */
    int BG_COLOR;
    int ARC_BG_COLOR;
    int CHART_BG_COLOR;
    int CHART_LINE_COLOR;
    int CPU_COLOR;
    int MEM_COLOR;
    int PRIMARY_FONT_COLOR;
    int SECONDARY_FONT_COLOR;
    int MENU_ACCENT_COLOR;
    int BTN_COLOR;
    int BTN_FONT_COLOR;

    std::string bg_color;
    std::string arc_bg_color;
    std::string chart_bg_color;
    std::string chart_line_color;
    std::string cpu_color;
    std::string mem_color;
    std::string primary_font_color;
    std::string secondary_font_color;
    std::string menu_accent_color;
    std::string btn_color;
    std::string btn_font_color;

    void style_init(){
        // parse theming json file
        std::ifstream config_handler("theme.json");
        json theme_config;

        if(config_handler.is_open()){
            config_handler >> theme_config;

        }
        else{
            cout << "Error! could not open config file\n";
        }

        
        /* getting custom color values from parsed theme config*/
        bg_color = theme_config["BG_COLOR"];
        arc_bg_color = theme_config["ARC_BG_COLOR"];
        chart_bg_color = theme_config["CHART_BG_COLOR"];
        chart_line_color = theme_config["CHART_LINE_COLOR"];
        cpu_color = theme_config["CPU_COLOR"];
        mem_color = theme_config["MEM_COLOR"];
        primary_font_color = theme_config["PRIMARY_FONT_COLOR"];
        secondary_font_color = theme_config["SECONDARY_FONT_COLOR"];
        menu_accent_color = theme_config["MENU_ACCENT_COLOR"];
        btn_color = theme_config["BTN_COLOR"];
        btn_font_color = theme_config["BTN_FONT_COLOR"]; 
        
        BG_COLOR = std::stoi(bg_color, nullptr, 16);
        ARC_BG_COLOR = std::stoi(arc_bg_color, nullptr, 16);
        CHART_BG_COLOR = std::stoi(chart_bg_color, nullptr, 16);
        CHART_LINE_COLOR = std::stoi(chart_line_color, nullptr, 16);
        CPU_COLOR = std::stoi(cpu_color, nullptr, 16);
        MEM_COLOR = std::stoi(mem_color, nullptr, 16);
        PRIMARY_FONT_COLOR = std::stoi(primary_font_color, nullptr, 16);
        SECONDARY_FONT_COLOR = std::stoi(secondary_font_color, nullptr, 16);
        MENU_ACCENT_COLOR = std::stoi(menu_accent_color, nullptr, 16);
        BTN_COLOR = std::stoi(btn_color, nullptr, 16);
        BTN_FONT_COLOR = std::stoi(btn_font_color, nullptr, 16);

        /*LVGL style sheets*/
        lv_style_init(&background_style);
        lv_style_set_bg_color(&background_style, lv_color_hex(BG_COLOR));
        lv_obj_add_style(lv_scr_act(), &background_style, 0); 

        lv_style_init(&chart_style);
        lv_style_set_bg_color(&chart_style, lv_color_hex(CHART_BG_COLOR));
        lv_style_set_line_color(&chart_style, lv_color_hex(CHART_LINE_COLOR));
        lv_style_set_border_color(&chart_style, lv_color_hex(CHART_LINE_COLOR));

        lv_style_init(&chart_line_style);
        lv_style_set_size(&chart_line_style, 0);

        lv_style_init(&spinner_style_indicator);
        lv_style_set_arc_color(&spinner_style_indicator, lv_color_hex(ARC_BG_COLOR));

        lv_style_init(&cpu_arc_style);
        lv_style_set_arc_color(&cpu_arc_style, lv_color_hex(CPU_COLOR));

        lv_style_init(&mem_arc_style);
        lv_style_set_arc_color(&mem_arc_style, lv_color_hex(MEM_COLOR));

        lv_style_init(&tab_style);
        //lv_style_set_(&tab_style, );
    }
}

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

    /* theme colors */
    int FG_COLOR;
    int BG_COLOR;
    int PRIMARY_LINE_COLOR;
    int SECONDARY_LINE_COLOR;


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
        std::string fg_color = theme_config["FG_COLOR"];
        std::string bg_color = theme_config["BG_COLOR"];
        std::string primary_line_color = theme_config["PRIMARY_LINE_COLOR"];
        std::string secondary_line_color = theme_config["SECONDARY_LINE_COLOR"];

        FG_COLOR = std::stoi(fg_color, nullptr, 16);
        BG_COLOR = std::stoi(bg_color, nullptr, 16);
        PRIMARY_LINE_COLOR = std::stoi(primary_line_color, nullptr, 16);
        SECONDARY_LINE_COLOR = std::stoi(secondary_line_color, nullptr, 16);

        /*LVGL style sheets*/
        lv_style_init(&background_style);
        lv_style_set_bg_color(&background_style, lv_color_hex(BG_COLOR));
        lv_obj_add_style(lv_scr_act(), &background_style, 0); 

        lv_style_init(&chart_style);
        lv_style_set_bg_color(&chart_style, lv_color_hex(FG_COLOR));
        lv_style_set_line_color(&chart_style, lv_color_hex(FG_COLOR));
        lv_style_set_border_color(&chart_style, lv_color_hex(FG_COLOR));

        lv_style_init(&chart_line_style);
        lv_style_set_size(&chart_line_style, 0);

        lv_style_init(&spinner_style_indicator);
        lv_style_set_arc_color(&spinner_style_indicator, lv_color_hex(SECONDARY_LINE_COLOR));

        lv_style_init(&cpu_arc_style);
        lv_style_set_arc_color(&cpu_arc_style, lv_palette_main(LV_PALETTE_GREEN));

        lv_style_init(&mem_arc_style);
        lv_style_set_arc_color(&mem_arc_style, lv_palette_main(LV_PALETTE_BLUE));
    }
}
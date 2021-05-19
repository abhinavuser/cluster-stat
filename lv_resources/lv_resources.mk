LV_DRIVERS_DIR_NAME ?= lv_resources

CSRCS += main_font.c
CSRCS += bg_image.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_resources
VPATH += :$(LVGL_DIR)/lv_resources

CFLAGS += "-I$(LVGL_DIR)/lv_resources"

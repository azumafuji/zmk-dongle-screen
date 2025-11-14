/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

#include "lock_status.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct lock_status_state {
    zmk_hid_indicators_t indicators;
};

static struct lock_status_state get_state(const zmk_event_t *eh) {
    return (struct lock_status_state){.indicators = zmk_hid_indicators_get_current_profile()};
}

static void set_lock_status(struct zmk_widget_lock_status *widget, struct lock_status_state state) {
    char text[4] = "";
    if (state.indicators & ZMK_HID_INDICATOR_CAPS_LOCK) {
        strcat(text, "C");
    }
    if (state.indicators & ZMK_HID_INDICATOR_NUM_LOCK) {
        strcat(text, "N");
    }
    if (state.indicators & ZMK_HID_INDICATOR_SCROLL_LOCK) {
        strcat(text, "S");
    }
    lv_label_set_text(widget->lock_label, text);
}

static void lock_status_update_cb(struct lock_status_state state) {
    struct zmk_widget_lock_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_lock_status(widget, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_lock_status, struct lock_status_state, lock_status_update_cb,
                            get_state)
ZMK_SUBSCRIPTION(widget_lock_status, zmk_hid_indicators_changed);

int zmk_widget_lock_status_init(struct zmk_widget_lock_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 240, 20);

    widget->lock_label = lv_label_create(widget->obj);
    lv_obj_align(widget->lock_label, LV_ALIGN_TOP_LEFT, 0, 0);

    sys_slist_append(&widgets, &widget->node);

    widget_lock_status_init();
    return 0;
}

lv_obj_t *zmk_widget_lock_status_obj(struct zmk_widget_lock_status *widget) {
    return widget->obj;
}

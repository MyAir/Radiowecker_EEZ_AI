#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <lvgl.h>

class OtaManager {
public:
    OtaManager();
    void begin();

private:
    // UI Elements
    lv_obj_t *ota_screen;
    lv_obj_t *ota_label;
    lv_obj_t *ota_progress_bar;

    void create_ota_screen();
};

#endif // OTAMANAGER_H

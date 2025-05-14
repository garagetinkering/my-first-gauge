/* Basic Driver setup for the ESP32-S3 2.1 inch LCD Driver board  */
/* Author: Andy Valentine - Garage Tinkering                      */
/*                                                                */

#include <Arduino.h>
#include "CANBus_Driver.h"
#include "LVGL_Driver.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Preferences.h>

// Library dependancy
//.#include "UltimateGauge.h"

// Library dependancy
#include "fonts/code_sb_100.h"
#include "fonts/code_r_30.h"

lv_color_t PALETTE_BLACK        = LV_COLOR_MAKE(0, 0, 0);
lv_color_t PALETTE_OFFWHITE     = LV_COLOR_MAKE(220, 220, 220);
lv_color_t PALETTE_GREY         = LV_COLOR_MAKE(160, 160, 160);
lv_color_t PALETTE_DARK_GREY    = LV_COLOR_MAKE(60, 60, 60);
lv_color_t PALETTE_AMBER        = LV_COLOR_MAKE(250, 140, 0);
lv_color_t PALETTE_RED          = LV_COLOR_MAKE(220, 0, 0);
lv_color_t PALETTE_GREEN        = LV_COLOR_MAKE(0, 220, 0);

// Incoming ESPNow data
typedef struct struct_data {
  uint8_t flag;
  uint8_t speed_mph;
  uint16_t rpm;
  uint8_t coolant_temp;
  bool indicating_left;
  bool indicating_right;
} struct_data;

// Struct Objects
struct_data ESPNowData;

// Generic control variables
bool initial_load         = false; // has the first data been received
volatile bool data_ready  = false; // new espnow data (resets)

const int RPM_LOWER       = 3600;
const int RPM_IDEAL_MIN   = 4000;
const int RPM_IDEAL_MAX   = 4800;
const int RPM_REDLINE     = 6600;

// Screens
lv_obj_t *overlay_scr;  // special case - always on top, good for alerts
lv_obj_t *main_scr;

// Global components
lv_obj_t *speed_value;  // main text number
lv_obj_t *speed_unit;  // speed unit label
lv_obj_t *rpm_lv1_l;
lv_obj_t *rpm_lv2_l;
lv_obj_t *rpm_lv3_l;
lv_obj_t *rpm_lv1_r;
lv_obj_t *rpm_lv2_r;
lv_obj_t *rpm_lv3_r;
lv_obj_t *indicator_left;
lv_obj_t *indicator_right;

// Font styles
static lv_style_t style_icons;
static lv_style_t style_speed;
static lv_style_t style_label;


void WiFi_init(void) {
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void Drivers_Init(void) {
  I2C_Init();
  LCD_Init();
  WiFi_init();
  CANBus_Init();
  Lvgl_Init();
}

// ESPNow received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  int8_t new_channel = 0;

  // Write to the correct structure based on ESPNow flag
  // Flags in UltimateGauge dependancy
  switch (incomingData[0]) {
    case (0): // incoming from CANBUS Sender with flag ID 0
      memcpy(&ESPNowData, incomingData, sizeof(ESPNowData));  // copy incoming data to ESPNowData
      data_ready = true;
      break;
  }
}

void Style_Init(void) {
  // set global styles
  lv_style_init(&style_icons);
  lv_style_set_text_font(&style_icons, &lv_font_montserrat_40);
  lv_style_set_text_color(&style_icons, PALETTE_GREEN);

  lv_style_init(&style_speed);
  lv_style_set_text_font(&style_speed, &code_sb_100);
  lv_style_set_text_color(&style_speed, PALETTE_OFFWHITE);

  lv_style_init(&style_label);
  lv_style_set_text_font(&style_label, &code_r_30);
  lv_style_set_text_color(&style_label, PALETTE_GREY);
  lv_style_set_text_letter_space(&style_label, 1);
}

void Update_Speed(void) {
  int incoming_speed = ESPNowData.speed_mph;

  char speed_label[4];
  sprintf(speed_label, "%d", incoming_speed);

  lv_label_set_text(speed_value, speed_label);
}

void Update_RPM(void) {
  lv_color_t BULLET_1_COLOR = PALETTE_DARK_GREY;
  lv_color_t BULLET_2_COLOR = PALETTE_DARK_GREY;
  lv_color_t BULLET_3_COLOR = PALETTE_DARK_GREY;

  if (ESPNowData.rpm > RPM_REDLINE) {
    BULLET_1_COLOR = PALETTE_RED;
    BULLET_2_COLOR = PALETTE_RED;
    BULLET_3_COLOR = PALETTE_RED;
  } else {
    if (ESPNowData.rpm > RPM_LOWER) {
      BULLET_3_COLOR = PALETTE_AMBER;
    }
    if (ESPNowData.rpm > RPM_IDEAL_MIN) {
      BULLET_2_COLOR = PALETTE_GREEN;
    }
    if (ESPNowData.rpm > RPM_IDEAL_MAX) {
      BULLET_1_COLOR = PALETTE_RED;
    }
  }

  lv_obj_set_style_bg_color(rpm_lv1_l, BULLET_1_COLOR, 0);
  lv_obj_set_style_bg_color(rpm_lv1_r, BULLET_1_COLOR, 0);
  lv_obj_set_style_bg_color(rpm_lv2_l, BULLET_2_COLOR, 0);
  lv_obj_set_style_bg_color(rpm_lv2_r, BULLET_2_COLOR, 0);
  lv_obj_set_style_bg_color(rpm_lv3_l, BULLET_3_COLOR, 0);
  lv_obj_set_style_bg_color(rpm_lv3_r, BULLET_3_COLOR, 0);
}

void Update_Indicators(void) {
  if (ESPNowData.indicating_left) {
    lv_obj_set_style_text_opa(indicator_left, LV_OPA_100, 0);
  } else {
    lv_obj_set_style_text_opa(indicator_left, LV_OPA_0, 0);
  }

  if (ESPNowData.indicating_right) {
    lv_obj_set_style_text_opa(indicator_right, LV_OPA_100, 0);
  } else {
    lv_obj_set_style_text_opa(indicator_right, LV_OPA_0, 0);
  }
}

// update with incoming values from ESPNow
void Update_Values(void) {
  Update_Speed();
  Update_RPM();
  Update_Indicators();
}

lv_obj_t* Create_RPM_Bullet(lv_obj_t* parent, int size, int x_ofs, int y_ofs) {
  lv_obj_t* obj = lv_obj_create(parent);
  lv_obj_set_size(obj, size, size);
  lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, y_ofs);
  lv_obj_set_style_bg_color(obj, PALETTE_DARK_GREY, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
  return obj;
}

// create the elements on the main scr
void Main_Scr_UI(void) {
  speed_value = lv_label_create(main_scr);
  lv_label_set_text(speed_value, "0");
  lv_obj_align(speed_value, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(speed_value, &style_speed, LV_PART_MAIN);

  speed_unit = lv_label_create(main_scr);
  lv_label_set_text(speed_unit, "mph");
  lv_obj_align(speed_unit, LV_ALIGN_CENTER, 0, 60);
  lv_obj_add_style(speed_unit, &style_label, LV_PART_MAIN);

  indicator_left = lv_label_create(main_scr);
  lv_label_set_text(indicator_left, LV_SYMBOL_LEFT);
  lv_obj_align(indicator_left, LV_ALIGN_CENTER, -180, 0);
  lv_obj_add_style(indicator_left, &style_icons, LV_PART_MAIN);
  
  indicator_right = lv_label_create(main_scr);
  lv_label_set_text(indicator_right, LV_SYMBOL_RIGHT);
  lv_obj_align(indicator_right, LV_ALIGN_CENTER, 180, 0);
  lv_obj_add_style(indicator_right, &style_icons, LV_PART_MAIN);

  rpm_lv1_l = Create_RPM_Bullet(main_scr, 38, -24, -178);
  rpm_lv1_r = Create_RPM_Bullet(main_scr, 38,  24, -178);
  rpm_lv2_l = Create_RPM_Bullet(main_scr, 34, -68, -168);
  rpm_lv2_r = Create_RPM_Bullet(main_scr, 34,  68, -168);
  rpm_lv3_l = Create_RPM_Bullet(main_scr, 30, -106, -150);
  rpm_lv3_r = Create_RPM_Bullet(main_scr, 30,  106, -150);
}

void Screen_Init(void) {
  overlay_scr = lv_layer_top();

  main_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(main_scr, PALETTE_BLACK, 0);

  lv_screen_load(main_scr);
}

void Values_Init(void) {
   ESPNowData.indicating_left = true;
   ESPNowData.indicating_right = true;
}

void Make_Initial_UI(void) {
  Screen_Init();
  Main_Scr_UI();
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("begin");

  Drivers_Init();
  Style_Init();
  Values_Init();

  Make_Initial_UI();
  Set_Backlight(100);

  Update_Indicators();
}

void loop(void) {
  lv_timer_handler();

  // CAN_FRAME can_message;

  // if (CAN0.read(can_message)) {
  //   digitalWrite(ACC_LED_PIN, HIGH);
  //   Serial.print("CAN MSG: 0x");
  //   Serial.print(can_message.id, HEX);
  //   Serial.print(" [");
  //   Serial.print(can_message.length, DEC);
  //   Serial.print("] <");
  //   for (int i = 0; i < can_message.length; i++) {
  //     if (i != 0) Serial.print(":");
  //     Serial.print(can_message.data.byte[i], HEX);
  //   }
  //   Serial.println(">");
  // }

  // data acquired from ESPNow
  if (data_ready) {
    Update_Values();
    data_ready = false;
  }

  vTaskDelay(pdMS_TO_TICKS(5));
}
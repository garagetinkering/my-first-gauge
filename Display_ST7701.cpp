#include "Display_ST7701.h"  
#include "driver/gpio.h"
      
spi_device_handle_t SPI_handle = NULL;     
esp_lcd_panel_handle_t panel_handle = NULL;            
void ST7701_WriteCommand(uint8_t cmd) {
  spi_transaction_t spi_tran = {
    .cmd = 0,
    .addr = cmd,
    .length = 0,
    .rxlength = 0,
  };
  spi_device_transmit(SPI_handle, &spi_tran);
}

void ST7701_WriteData(uint8_t data) {
  spi_transaction_t spi_tran = {
    .cmd = 1,
    .addr = data,
    .length = 0,
    .rxlength = 0,
  };
  spi_device_transmit(SPI_handle, &spi_tran);
}

void ST7701_CS_EN(){
  gpio_set_level(LCD_CS_PIN, 0);

  vTaskDelay(pdMS_TO_TICKS(10));
}
void ST7701_CS_Dis(){
  gpio_set_level(LCD_CS_PIN, 1);

  vTaskDelay(pdMS_TO_TICKS(10));
}
void ST7701_Reset(){
  gpio_set_level(LCD_RST_PIN, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
  gpio_set_level(LCD_RST_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(50));
}
void ST7701_Init() {
  spi_bus_config_t buscfg = {
    .mosi_io_num = LCD_MOSI_PIN,
    .miso_io_num = -1,
    .sclk_io_num = LCD_CLK_PIN,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 64,
  };

  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
  spi_device_interface_config_t devcfg = {
    .command_bits = 1,
    .address_bits = 8,
    .mode = SPI_MODE0,
    .clock_speed_hz = 40000000,
    .spics_io_num = -1,                      
    .queue_size = 1,
  };
  spi_bus_add_device(SPI2_HOST, &devcfg, &SPI_handle);            

  gpio_reset_pin(LCD_CS_PIN);
  gpio_set_direction(LCD_CS_PIN, GPIO_MODE_OUTPUT);

  ST7701_CS_EN();
  ST7701_WriteCommand(0xFF);
  ST7701_WriteData(0x77);
  ST7701_WriteData(0x01);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x10);

  ST7701_WriteCommand(0xC0);
  ST7701_WriteData(0x3B); //Scan line	
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xC1);
  ST7701_WriteData(0x0B);	//VBP
  ST7701_WriteData(0x02);

  ST7701_WriteCommand(0xC2);
  ST7701_WriteData(0x07);
  ST7701_WriteData(0x02);

  ST7701_WriteCommand(0xCC);
  ST7701_WriteData(0x10);

  ST7701_WriteCommand(0xCD);  //RGB format
  ST7701_WriteData(0x08);

  ST7701_WriteCommand(0xB0);  // IPS   
  ST7701_WriteData(0x00);     // 255 
  ST7701_WriteData(0x11);     // 251    
  ST7701_WriteData(0x16);     // 247  down
  ST7701_WriteData(0x0e);     // 239    
  ST7701_WriteData(0x11);     // 231    
  ST7701_WriteData(0x06);     // 203    
  ST7701_WriteData(0x05);     // 175 
  ST7701_WriteData(0x09);     // 147    
  ST7701_WriteData(0x08);     // 108    
  ST7701_WriteData(0x21);     // 80  
  ST7701_WriteData(0x06);     // 52   
  ST7701_WriteData(0x13);     // 24    
  ST7701_WriteData(0x10);     // 16    
  ST7701_WriteData(0x29);     // 8    down
  ST7701_WriteData(0x31);     // 4    
  ST7701_WriteData(0x18);     // 0   

  ST7701_WriteCommand(0xB1);  //  IPS	   
  ST7701_WriteData(0x00);     //  255 
  ST7701_WriteData(0x11);     //  251
  ST7701_WriteData(0x16);     //  247   down
  ST7701_WriteData(0x0e);     //  239
  ST7701_WriteData(0x11);     //  231
  ST7701_WriteData(0x07);     //  203    
  ST7701_WriteData(0x05);     //  175
  ST7701_WriteData(0x09);     //  147  
  ST7701_WriteData(0x09);     //  108  
  ST7701_WriteData(0x21);     //  80 
  ST7701_WriteData(0x05);     //  52   
  ST7701_WriteData(0x13);     //  24 
  ST7701_WriteData(0x11);     //  16 
  ST7701_WriteData(0x2a);     //  8  down 
  ST7701_WriteData(0x31);     //  4  
  ST7701_WriteData(0x18);     //  0  

  ST7701_WriteCommand(0xFF);
  ST7701_WriteData(0x77);
  ST7701_WriteData(0x01);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x11);

  ST7701_WriteCommand(0xB0);  // VOP  3.5375+ *x 0.0125
  ST7701_WriteData(0x6d);     // 5D
  
  ST7701_WriteCommand(0xB1); 	// VCOM amplitude setting  
  ST7701_WriteData(0x37);
  
  ST7701_WriteCommand(0xB2); 	// VGH Voltage setting  
  ST7701_WriteData(0x81);	    // 12V

  ST7701_WriteCommand(0xB3);
  ST7701_WriteData(0x80);

  ST7701_WriteCommand(0xB5); 	// VGL Voltage setting  
  ST7701_WriteData(0x43);	    //-8.3V

  ST7701_WriteCommand(0xB7);
  ST7701_WriteData(0x85);

  ST7701_WriteCommand(0xB8);
  ST7701_WriteData(0x20);

  ST7701_WriteCommand(0xC1);
  ST7701_WriteData(0x78);

  ST7701_WriteCommand(0xC2);
  ST7701_WriteData(0x78);

  ST7701_WriteCommand(0xD0);
  ST7701_WriteData(0x88);

  ST7701_WriteCommand(0xE0);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x02);

  ST7701_WriteCommand(0xE1);
  ST7701_WriteData(0x03);	
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x04);	
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x20);
  ST7701_WriteData(0x20);

  ST7701_WriteCommand(0xE2);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	  
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE3);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x11);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE4);
  ST7701_WriteData(0x22);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE5);		
  ST7701_WriteData(0x05);	
  ST7701_WriteData(0xEC);	
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x07);	
  ST7701_WriteData(0xEE);	
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE6);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x11);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE7);
  ST7701_WriteData(0x22);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xE8);		
  ST7701_WriteData(0x06);	
  ST7701_WriteData(0xED);	
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x08);	
  ST7701_WriteData(0xEF);	
  ST7701_WriteData(0xA0); 
  ST7701_WriteData(0xA0);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0xEB);
  ST7701_WriteData(0x00); 	
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x40);
  ST7701_WriteData(0x40);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);  

  ST7701_WriteCommand(0xED);  
  ST7701_WriteData(0xFF); 
  ST7701_WriteData(0xFF);  
  ST7701_WriteData(0xFF); 	
  ST7701_WriteData(0xBA); 		
  ST7701_WriteData(0x0A); 	
  ST7701_WriteData(0xBF); 	
  ST7701_WriteData(0x45); 	
  ST7701_WriteData(0xFF); 
  ST7701_WriteData(0xFF);  
  ST7701_WriteData(0x54); 	
  ST7701_WriteData(0xFB); 	
  ST7701_WriteData(0xA0); 	
  ST7701_WriteData(0xAB); 	
  ST7701_WriteData(0xFF); 
  ST7701_WriteData(0xFF); 
  ST7701_WriteData(0xFF); 

  ST7701_WriteCommand(0xEF);
  ST7701_WriteData(0x10); 
  ST7701_WriteData(0x0D); 
  ST7701_WriteData(0x04); 
  ST7701_WriteData(0x08); 
  ST7701_WriteData(0x3F); 
  ST7701_WriteData(0x1F);

  ST7701_WriteCommand(0xFF);
  ST7701_WriteData(0x77);
  ST7701_WriteData(0x01);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x13);

  ST7701_WriteCommand(0xEF);
  ST7701_WriteData(0x08);

  ST7701_WriteCommand(0xFF);
  ST7701_WriteData(0x77);
  ST7701_WriteData(0x01);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);
  ST7701_WriteData(0x00);


  ST7701_WriteCommand(0x36);
  ST7701_WriteData(0x00);

  ST7701_WriteCommand(0x3A);
  ST7701_WriteData(0x66);

  ST7701_WriteCommand(0x11);
  
  vTaskDelay(pdMS_TO_TICKS(480));

  ST7701_WriteCommand(0x20); //
  vTaskDelay(pdMS_TO_TICKS(120));
  ST7701_WriteCommand(0x29); 
  ST7701_CS_Dis();

  //  RGB
  esp_lcd_rgb_panel_config_t rgb_config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .timings =  {
      .pclk_hz = ESP_PANEL_LCD_RGB_TIMING_FREQ_HZ,
      .h_res = ESP_PANEL_LCD_HEIGHT,
      .v_res = ESP_PANEL_LCD_WIDTH,
      .hsync_pulse_width = ESP_PANEL_LCD_RGB_TIMING_HPW,
      .hsync_back_porch = ESP_PANEL_LCD_RGB_TIMING_HBP,
      .hsync_front_porch = ESP_PANEL_LCD_RGB_TIMING_HFP,
      .vsync_pulse_width = ESP_PANEL_LCD_RGB_TIMING_VPW,
      .vsync_back_porch = ESP_PANEL_LCD_RGB_TIMING_VBP,
      .vsync_front_porch = ESP_PANEL_LCD_RGB_TIMING_VFP,
      .flags = {
        .hsync_idle_low = 0,
        .vsync_idle_low = 0,
        .de_idle_high = 0,
        .pclk_active_neg = false,
        .pclk_idle_high = 0,
      },
    },
    .data_width = ESP_PANEL_LCD_RGB_DATA_WIDTH,
    .bits_per_pixel = ESP_PANEL_LCD_RGB_PIXEL_BITS,
    .num_fbs = ESP_PANEL_LCD_RGB_FRAME_BUF_NUM,
    .bounce_buffer_size_px = 10 * ESP_PANEL_LCD_HEIGHT,
    .psram_trans_align = 64,
    .hsync_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_HSYNC,
    .vsync_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_VSYNC,
    .de_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_DE,
    .pclk_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_PCLK,
    .disp_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_DISP,
    .data_gpio_nums = {
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA0,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA1,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA2,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA3,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA4,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA5,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA6,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA7,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA8,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA9,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA10,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA11,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA12,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA13,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA14,
      ESP_PANEL_LCD_PIN_NUM_RGB_DATA15,
    },
    .flags = {
      .disp_active_low = 0,
      .refresh_on_demand = 0,
      .fb_in_psram = true,
      .double_fb = true,
      .no_fb = 0,
      .bb_invalidate_cache = 0,
    },
  };
  esp_lcd_new_rgb_panel(&rgb_config, &panel_handle); 
  esp_lcd_panel_reset(panel_handle);
  esp_lcd_panel_init(panel_handle);
}

void LCD_Init() {
  ST7701_Reset();
  ST7701_Init();
  Backlight_Init();
}

void LCD_addWindow(uint16_t Xstart, uint16_t Xend, uint16_t Ystart, uint16_t Yend, uint8_t *color) {
  Xend = Xend + 1;
  Yend = Yend + 1;
  if (Xend >= ESP_PANEL_LCD_WIDTH)
    Xend = ESP_PANEL_LCD_WIDTH;
  if (Yend >= ESP_PANEL_LCD_HEIGHT)
    Yend = ESP_PANEL_LCD_HEIGHT;
   
  esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color);
}

void Backlight_Init() {
  ledcAttach(LCD_BACKLIGHT_PIN, frequency, resolution);    
}

void Set_Backlight(uint8_t light) {
  if (light > backlight_max || light < 0) {
    return;
  }

  uint32_t backlight = light * 10;
  //printf("backlight %d\n", backlight);
  ledcWrite(LCD_BACKLIGHT_PIN, backlight);
}



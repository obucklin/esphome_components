#include "waveshare_epaper.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "image_copy.h"
#include "vector"

// 4bit

namespace esphome
{
  namespace waveshare_epaper
  {

    void GoodDisplayGdep073e01::fill(Color color)
    {
      // flip logic
      const uint8_t fill = get_color(color);
      for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
        this->buffer_[i] = fill*36+fill*6+fill;
    }


    static const char *const TAG = "gdep073e01";

    uint32_t GoodDisplayGdep073e01::idle_timeout_() { return 20000; }

    void GoodDisplayGdep073e01::initialize()
    {
      ESP_LOGD("TAG", "initializing GoodDisplayGdep073e01");
      this->init_display_();

    }

    bool GoodDisplayGdep073e01::wait_until_idle_()
    {
      ESP_LOGD("TAG", "wait_until_idle_");
      if (this->busy_pin_ == nullptr)
      {
        return true;
      }

      const uint32_t start = millis();
      while (!this->busy_pin_->digital_read())
      {
        if (millis() - start > this->idle_timeout_())
        {
          ESP_LOGI(TAG, "Timeout while displaying image!");
          return false;
        }
        App.feed_wdt();
        delay(10);
      }
      delay(200); // NOLINT
      return true;
    };

    void GoodDisplayGdep073e01::init_display_()
    {
      ESP_LOGD("TAG", "init_display_");
      this->reset_();

      this->command(0xAA); // CMDH
      this->data(0x49);
      this->data(0x55);
      this->data(0x20);
      this->data(0x08);
      this->data(0x09);
      this->data(0x18);

      this->command(0x01); //
      this->data(0x3F);

      this->command(0x00);
      this->data(0x5F);
      this->data(0x69);

      this->command(0x03);
      this->data(0x00);
      this->data(0x54);
      this->data(0x00);
      this->data(0x44);

      this->command(0x05);
      this->data(0x40);
      this->data(0x1F);
      this->data(0x1F);
      this->data(0x2C);

      this->command(0x06);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x17);
      this->data(0x49);

      this->command(0x08);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x1F);
      this->data(0x22);
      this->command(0x30);
      this->data(0x08);
      this->command(0x50);
      this->data(0x3F);

      this->command(0x60);
      this->data(0x02);
      this->data(0x00);

      this->command(0x61);
      this->data(0x03);
      this->data(0x20);
      this->data(0x01);
      this->data(0xE0);

      this->command(0x84);
      this->data(0x01);

      this->command(0xE3);
      this->data(0x2F);

      this->command(0x04); // PWR on
      ESP_LOGD("TAG", "init end reached");

      this->wait_until_idle_();

      for (int i = 0; i < 128640; i++)
      {
        this->buffer_[i] = 43;
      }
    };

    void GoodDisplayGdep073e01::init_display_fast_()
    {
      this->reset_();

      this->command(0xAA); // CMDH
      this-> data(0x49);
      this-> data(0x55);
      this-> data(0x20);
      this-> data(0x08);
      this-> data(0x09);
      this-> data(0x18);

      this->command(0x01);
      this-> data(0x3F);
      this-> data(0x00);
      this-> data(0x32);
      this-> data(0x2A);
      this-> data(0x0E);
      this-> data(0x2A);

      this->command(0x00);
      this-> data(0x5F);
      this-> data(0x69);

      this->command(0x03);
      this-> data(0x00);
      this-> data(0x54);
      this-> data(0x00);
      this-> data(0x44);

      this->command(0x05);
      this-> data(0x40);
      this-> data(0x1F);
      this-> data(0x1F);
      this-> data(0x2C);

      this->command(0x06);
      this-> data(0x6F);
      this-> data(0x1F);
      this-> data(0x16);
      this-> data(0x25);

      this->command(0x08);
      this-> data(0x6F);
      this-> data(0x1F);
      this-> data(0x1F);
      this-> data(0x22);

      this->command(0x13); // IPC
      this-> data(0x00);
      this-> data(0x04);

      this->command(0x30);
      this-> data(0x02);

      this->command(0x41); // TSE
      this-> data(0x00);

      this->command(0x50);
      this-> data(0x3F);

      this->command(0x60);
      this-> data(0x02);
      this-> data(0x00);

      this->command(0x61);
      this-> data(0x03);
      this-> data(0x20);
      this-> data(0x01);
      this-> data(0xE0);

      this->command(0x82);
      this-> data(0x1E);

      this->command(0x84);
      this-> data(0x01);

      this->command(0x86); // AGID
      this-> data(0x00);

      this->command(0xE3);
      this-> data(0x2F);

      this->command(0xE0); // CCSET
      this-> data(0x00);

      this->command(0xE6); // TSSET
      this-> data(0x00);

      this->command(0x04); // PWR on
      this->wait_until_idle_();

    };

    unsigned char GoodDisplayGdep073e01::get_color(Color color)
    {
      int16_t red = color.r;
      int16_t green = color.g;
      int16_t blue = color.b;
      unsigned char cv7 = 0x01;

      if ((red < 0x80) && (green < 0x80) && (blue < 0x80))
        cv7 = 0x00; // black
      else if ((red >= 0x80) && (green >= 0x80) && (blue >= 0x80))
        cv7 = 0x01; // white
      else if ((green >= 0x80) && (blue >= 0x80))
        cv7 = green > blue ? 0x05 : 0x04; // green, blue
      else if ((red >= 0x80) && (green >= 0x80))
        cv7 = 0x02; // yellow
      else if (red >= 0x80)
        cv7 = 0x03; // red
      else if (green >= 0x80)
        cv7 = 0x05; // green
      else
        cv7 = 0x04; // blue

      return cv7;
    }

    void HOT GoodDisplayGdep073e01::draw_absolute_pixel_internal(int x, int y, Color color)
    {
      if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
        return;

      const uint32_t pos = (x/ 3u) + (y * 268) ;

      unsigned int cv = this->get_color(color);
      unsigned char temp_byte = this->buffer_[pos];

      uint8_t temp = uint8_t(temp_byte);

      uint8_t top = (temp / 36 != 4)?temp / 36:1;
      uint8_t mid = ((temp % 36) / 6!=4)?(temp % 36) / 6:1;
      uint8_t bot = (temp % 6 != 4)?temp % 6:1;


      switch (x % 3)
      {
      case 0:
        top = cv;
        break;
      case 1:
        mid = cv;
        break;
      case 2:
        bot = cv;
        break;
      }

      this->buffer_[pos] = (top * 36) + (mid * 6) + bot;
    }

    uint32_t GoodDisplayGdep073e01::get_buffer_length_(){return 128640;}

    void GoodDisplayGdep073e01::display_buffer_()
    {
      // Acep_color(White); //Each refresh must be cleaned first
      this->command(0x10);
      uint16_t row_bytes = 0;
      for (int i = 0; i < 128640; i+=2)
      {
        uint8_t bytes_in[6] = 
        {
          this->buffer_[i]/36, 
          (this->buffer_[i] % 36) / 6, 
          this->buffer_[i] % 6, 
          this->buffer_[i+1]/36, 
          (this->buffer_[i+1] % 36) / 6, 
          this->buffer_[i+1] % 6
        };
        
        for (int j = 0; j < 6; j++) bytes_in[j] = (bytes_in[j] > 3) ? bytes_in[j]+1 : bytes_in[j];


        for (int j = 0; j < 3; j++)
        {
          if (row_bytes < 400) {
            this->data(bytes_in[j]<<4 | bytes_in[j+1]);
            row_bytes++;}
          else
          {
            row_bytes = 0;
            break;
          }
        }


      }
      this->command(0x04);
      this->command(0x12); // Refresh command (DRF in W21)
      this->data(0x00);
      delay(1);
      this->wait_until_idle_();
    }

    void GoodDisplayGdep073e01::display_pic_(const unsigned char pic_data[], int size)
    {
      ESP_LOGD("TAG", "display_pic_");
      ESP_LOGD("TAG", "image size = %d", size);

      unsigned char temp1, temp2;
      unsigned char data;
      int i = 0;
      // Acep_color(White); //Each refresh must be cleaned first

      this->command(0x10);
      while (i < size)
      {
        if (i < 384000)
        {
          temp1 = pic_data[i++] << 4;
          temp2 = pic_data[i++];
          data = temp1 | temp2;
          this->data(data);
        }
      }
      this->command(0x12); // Refresh command (DRF in W21)
      this->data(0x00);
      delay(1);
      this->wait_until_idle_();
    }

    void GoodDisplayGdep073e01::display_fill_color_(const unsigned char color)
    {
      // #define Black   0x00
      // #define White   0x11
      // #define Green   0x66
      // #define Blue    0x55
      // #define Red     0x33
      // #define Yellow  0x22
      // #define Clean   0x77

      ESP_LOGD("TAG", "display_fill_color_");
      this->command(0x10);
      {
        for (unsigned long i = 0; i < 192000; i++)
        {
          this->data(color);
          // if (i%1000 == 0) ESP_LOGD("TAG", "i = %02X", color);
        }
      }
      this->command(0x04);
      this->wait_until_idle_();

      // 20211212
      // Second setting
      this->command(0x06);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x17);
      this->data(0x49);

      this->command(0x12);
      this->data(0x00);
      this->wait_until_idle_();

      this->command(0x02);
      this->data(0x00);
      this->wait_until_idle_();
    }

    void GoodDisplayGdep073e01::clear_screen()
    {
      ESP_LOGD("TAG", "clear_screen");

      this->display_fill_color_(0x00);
    }

    void HOT GoodDisplayGdep073e01::display()
    {
      this->init_display_fast_();
      ESP_LOGD("TAG", "display");
      // this->clear_screen();
      // delay((1000)); // NOLINT
      this->display_buffer_();

      // uint8_t data_in[] = {0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x02, 0x02};

      // std::vector<GoodDisplayGdep073e01::ColorCount> *compressed = this->compress_pixels_(data_in, 4, 5);
      
      // ESP_LOGD("TAG", "compressed size" = sizeof(*compressed));
      // this->display_compressed_(compressed, 4);

      


      //this->display_pic_(stock_image, sizeof(stock_image));
      // this->display_fill_color_(0x66);
      // delay(5000); // NOLINT
      this->deep_sleep();
    }

    int GoodDisplayGdep073e01::get_width_internal() { return 800; }

    int GoodDisplayGdep073e01::get_height_internal() { return 480; }

    void GoodDisplayGdep073e01::dump_config()
    {
      LOG_DISPLAY("", "GoodDisplay 6-color E-Paper", this);
      ESP_LOGCONFIG(TAG, "  Model: gdep073e01");
      LOG_PIN("  Reset Pin: ", this->reset_pin_);
      LOG_PIN("  DC Pin: ", this->dc_pin_);
      LOG_PIN("  Busy Pin: ", this->busy_pin_);
      LOG_UPDATE_INTERVAL(this);
    }
  }
}
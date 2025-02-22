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



    static const char *const TAG = "gdep073e01";

    uint32_t GoodDisplayGdep073e01::idle_timeout_() { return 20000; }

    void GoodDisplayGdep073e01::initialize()
    {
      ESP_LOGD("TAG", "initializing GoodDisplayGdep073e01");
      this->init_display_();
      for (int i = 0; i < get_buffer_length_(); i++)
      {
        this->buffer_[i] = 0x00;
      }
      ESP_LOGD("TAG", "sample buffer = %d", this->buffer_[342]);
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
        cv7 = green > blue ? 0x06 : 0x05; // green, blue
      else if ((red >= 0x80) && (green >= 0x80))
        cv7 = green > red ? 0x06 : 0x03; // yellow, orange
      else if (red >= 0x80)
        cv7 = 0x03; // red
      else if (green >= 0x80)
        cv7 = 0x06; // green
      else
        cv7 = 0x05; // blue

      return cv7;
    }

    void HOT GoodDisplayGdep073e01::draw_absolute_pixel_internal(int x, int y, Color color)
    {
      if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
        return;

      const uint32_t pos = (x + (y * (this->get_width_internal()))) / 2u;
      unsigned char cv = this->get_color(color);
      unsigned char temp_byte = this->buffer_[pos];
      if ((temp_byte & 0xF0) == 0xF0) //if top nibble is xF
        temp_byte &= 0x1F;            //set top nibble to default white 0x10
      if ((temp_byte & 0x0F) == 0x0F) //if bottom nibble is xF
        temp_byte &= 0xF1;            //set bottom nibble to default white 0x01

      if (x & 0x01)           //if x is odd
      {
        temp_byte &= 0xF0;  //clear bottom nibble
        temp_byte |= cv;    //set bottom nibble
      }
      else
      {
        temp_byte &= 0x0F;          //clear top nibble
        temp_byte |= (cv << 4);     //set top nibble
      }
      this->buffer_[pos] = temp_byte;

      // ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[pos]);
    }

    uint32_t GoodDisplayGdep073e01::get_buffer_length_()
    {
      uint32_t len = this->get_height_internal() * this->get_width_internal() / 2;
      return len;
    }

    void GoodDisplayGdep073e01::display_buffer_()
    {
      // Acep_color(White); //Each refresh must be cleaned first
      this->command(0x10);
      for (int i = 0; i < 192000; i++)
      {
        if (i < this->get_buffer_length_())
        {
          this->data(*(this->buffer_ + i));
        }
        else
          this->data(0x11);
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

    struct ColorCount
    {
      uint8_t color;
      int count;
    };

    std::vector<GoodDisplayGdep073e01::ColorCount> * GoodDisplayGdep073e01::compress_pixels_(const uint8_t *pixels, int rows, int cols)
    {
      std::vector<ColorCount> lines[rows];
      int pix = 0;
      for (int row = 0; row < rows; row++)
      {
        uint8_t color = pixels[pix];
        uint16_t count = 1;
        std::vector<GoodDisplayGdep073e01::ColorCount> line;
        for (int col = 0; col < cols; col++)
        {
          if (pixels[pix] == color)
          {
            count++;
          }
          else
          {
            ColorCount cc({color, count});
            lines[row].push_back(cc);
            color = pixels[pix];
            count = 1;
          }
          pix++;
        }
      }
    }

    void GoodDisplayGdep073e01::display_compressed_(std::vector<GoodDisplayGdep073e01::ColorCount> *lines, int rows)
    {
      for (uint16_t row = 0; row < rows; row++)
      {
        uint16_t cc_count = 0;
        uint16_t byte_count = 0;
        uint16_t val_ct = 0;
        bool low = false;
        uint8_t byte = 0;
        while (byte_count < 400)
        {
          if (low)
            byte = lines[row][cc_count].color;
          else
          {
            byte |= (lines[row][cc_count].color << 4);
            ESP_LOGD("TAG", "assembled decompressed byte = %02X", byte);

            // this->data(byte);
            byte = 0;
            byte_count++;
          }
          low = !low;
          val_ct++;
          if (val_ct >= lines[row][cc_count].count)
          {
            cc_count++;
            val_ct = 0;
          }
        }
      }
    }

    int GoodDisplayGdep073e01::get_width_internal() { return 800; }

    int GoodDisplayGdep073e01::get_height_internal() { return 320; }

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
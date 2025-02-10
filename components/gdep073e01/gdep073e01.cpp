// custom_epd_w21.cpp
#include "gdep073e01.h"
#include "esphome/components/display/display_buffer.h"

void GDEP073E01::setup()
{
  // Initialize SPI
  spi_ = new SPIClass(HSPI);
  spi_->begin(SCK, MISO, MOSI, SS);

  // Initialize pins
  reset_pin_->setup();
  dc_pin_->setup();
  cs_pin_->setup();
  busy_pin_->setup();

  // Hardware reset
  reset_pin_->digital_write(true);
  delay(10);
  reset_pin_->digital_write(false);
  delay(10);
  reset_pin_->digital_write(true);
  delay(10);

  initialize_display();
}

void GDEP073E01::dump_config() {
  LOG_DISPLAY("", "GDEP073E01", this);
  ESP_LOGCONFIG(TAG, "  GDEP073E01");
   
  ESP_LOGCONFIG(TAG, "  Full Update Every: %" PRIu32, this->full_update_every_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Busy Pin: ", this->busy_pin_);
  LOG_UPDATE_INTERVAL(this);
}

void GDEP073E01::initialize_display()
{

  EPD_W21_RST_0; // Module reset
  delay(10);     // At least 10ms delay
  EPD_W21_RST_1;
  delay(10); // At least 10ms delay

  send_command(0xAA); // CMDH
  send_data(0x49);
  send_data(0x55);
  send_data(0x20);
  send_data(0x08);
  send_data(0x09);
  send_data(0x18);

  send_command(PWRR); //
  send_data(0x3F);

  send_command(PSR);
  send_data(0x5F);
  send_data(0x69);

  send_command(POFS);
  send_data(0x00);
  send_data(0x54);
  send_data(0x00);
  send_data(0x44);

  send_command(BTST1);
  send_data(0x40);
  send_data(0x1F);
  send_data(0x1F);
  send_data(0x2C);

  send_command(BTST2);
  send_data(0x6F);
  send_data(0x1F);
  send_data(0x17);
  send_data(0x49);

  send_command(BTST3);
  send_data(0x6F);
  send_data(0x1F);
  send_data(0x1F);
  send_data(0x22);
  send_command(PLL);
  send_data(0x08);
  send_command(CDI);
  send_data(0x3F);

  send_command(TCON);
  send_data(0x02);
  send_data(0x00);

  send_command(TRES);
  send_data(0x03);
  send_data(0x20);
  send_data(0x01);
  send_data(0xE0);

  send_command(T_VDCS);
  send_data(0x01);

  send_command(PWS);
  send_data(0x2F);

  send_command(0x04); // PWR on
  wait_until_idle();  // waiting for the electronic paper IC to release the idle signal
}

void GDEP073E01::wait_until_idle()
{
  while (busy_pin_->digital_read() == LOW)
  {
    delay(1);
  }
}

void GDEP073E01::send_command(uint8_t command)
{
  dc_pin_->digital_write(LOW);
  cs_pin_->digital_write(LOW);
  spi_->transfer(command);
  cs_pin_->digital_write(HIGH);
}

void GDEP073E01::send_data(uint8_t data)
{
  dc_pin_->digital_write(HIGH);
  cs_pin_->digital_write(LOW);
  spi_->transfer(data);
  cs_pin_->digital_write(HIGH);
}


void GDEP073E01::sleep(void)
{
  send_command(0X02); // power off
  send_data(0x00);
  this->wait_until_idle(); // waiting for the electronic paper IC to release the idle signal
  /*send_command(0X07);   //deep sleep
  send_data(0xA5);*/
}

unsigned char GDEP073E01::get_color(Color color)
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
}
  return cv7;
}

uint16_t GDEP073E01::get_width_internal() const { return 800; }
uint16_t GDEP073E01::get_height_internal() const { return 480; }

void GDEP073E01::clear(void)
{
  send_command(0x10);
  for (int i = 0; i < 480; i++)
  {
    for (int j = 0; j < 800 / 2; j++)
    {
      send_data(0x00);
    }
  }
  // Refresh
  send_command(0x12); // DISPLAY REFRESH
  send_data(0x00);
  delay(1);          //!!!The delay here is necessary, 200uS at least!!!
  wait_until_idle(); // waiting for the electronic paper IC to release the idle signal
}

void HOT GDEP073E01::draw_absolute_pixel_internal(int x, int y, unsigned char color)
{
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

  const uint32_t pos = x + (y * (this->get_width_internal() / 2u));

if (x & 0x01) {
  this->buffer_[pos] = (this->buffer_[pos] & 0x0F) | (color << 4);
} else {
  this->buffer_[pos] = (this->buffer_[pos] & 0xF0) | color;
}

}

uint32_t GDEP073E01::get_buffer_length_()
{
  return this->get_width_internal() * this->get_height_internal() / 2u; // 4 bits per pixel
} // black and red buffer

void GDEP073E01::fill(Color color)
{
  char color_internal = this->get_color(color);
  char double_color = color_internal | (color_internal << 4);
  const uint8_t fill = color.is_on() ? 0x00 : 0xFF;
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
    this->buffer_[i] = double_color;
}

void GDEP073E01::display_image(const unsigned char *picData)
{
  unsigned int row, col, pixel_pair, pos;
  unsigned char left_pixel, right_pixel; 
  unsigned char data_H, data_L, data;
  uint16_t col_count = this->get_width_internal() / 2;

  for (row = 0; row < 480; row++)
  {
    pixel_pair = 0;
    for (col = 0; col < col_count; col++)
    {
      left_pixel = picData[row * 800 + pixel_pair++];
      right_pixel = picData[row * 800 + pixel_pair++];
      data_H = this->get_color(left_pixel) << 4;
      data_L = this->get_color(right_pixel);
      data = data_H | data_L;
      pos = row * col_count + col;
      this->buffer_[pos] = data;
    }
  }
}

void GDEP073E01::display()
{
  // Acep_color(White); //Each refresh must be cleaned first
  send_command(0x10);
  this->write_array(this->buffer_, this->get_buffer_length_());
  // Refresh
  send_command(0x12); // DISPLAY REFRESH
  send_data(0x00);
  delay(1);                //!!!The delay here is necessary, 200uS at least!!!
  this->wait_until_idle(); // waiting for the electronic paper IC to release the idle signal
  this->sleep();
}
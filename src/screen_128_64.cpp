#include <Adafruit_SSD1306.h>
#include "MemoryFree.h"
#include "common.h"
#include "screen_128_64.h"

#define BOX_TYPE_11 0   
#define BOX_TYPE_12 1
#define BOX_TYPE_21 2   // √
#define BOX_TYPE_22 3   // √
#define BOX_TYPE_LEN 4 

#define FONT_HEIGHT 7
#define FONT_WIDTH 5
#define FONT_INTERVAL 1

#define LINE_SPACING 4

const uint8_t BOX_SIZE_HEIGHT[BOX_TYPE_LEN] = {0, 0, 26, 53};
const uint8_t BOX_SIZE_WIDTH[BOX_TYPE_LEN] = {0,0, 63, 64};

uint16_t g_color = WHITE;
uint16_t g_color_bk = BLACK;

Adafruit_SSD1306 *screen_ssd1306 = new Adafruit_SSD1306(PIN_SSD1306_RESET);
Adafruit_GFX *screen = screen_ssd1306;

class Box {
  public:
    int8_t m_type;
    int16_t m_x = 0;
    int16_t m_y = 0;

    Box(){}

    // 初始化, 设置文本框位置和类型
    void init(int8_t type, int16_t x, int16_t y)
    {
        m_type = type;
        m_x = x;
        m_y = y;
    }

    // 设置文本框名称
    void set_name(const char name[])
    {
        screen->setTextSize(1);
        int16_t name_pixil_len = strlen(name) * (FONT_WIDTH + FONT_INTERVAL);
        int16_t x_name = m_x + (get_width() - name_pixil_len) / 2;
        int16_t y_name = m_y + get_spacing_height();
        Serial.print(F("m_y="));
        Serial.print(m_y);
        Serial.print(F(";y_name="));
        Serial.println(y_name);
        screen->setCursor(x_name, y_name);
        screen->print(name);
    }

    // 设置文本框数值
    void set_value(const char value[])
    {
        int8_t font_size = 0;
        if (m_type == BOX_TYPE_22)
            font_size = 2;
        else
            font_size = 1;

        screen->setTextSize(font_size);
        int16_t y_name = m_y + get_spacing_height();
        int16_t value_pixil_len = strlen(value) * font_size * (FONT_WIDTH + FONT_INTERVAL);
        int16_t x_value = m_x + (get_width() - value_pixil_len) / 2;
        int16_t y_value = y_name + FONT_HEIGHT + LINE_SPACING;
        screen->setCursor(x_value, y_value);
        screen->print(value);
        screen->setTextSize(1);
    }

    // 获取文字空白高度
    uint8_t get_spacing_height()
    {
        uint8_t sh = 0;
        switch (m_type)
        {
        case BOX_TYPE_21:
            sh = (get_height() - FONT_HEIGHT - LINE_SPACING - FONT_HEIGHT) / 2;
            break;
        case BOX_TYPE_22:
            sh = (get_height() - FONT_HEIGHT - LINE_SPACING - FONT_HEIGHT * 2) / 2;
            break;
        }
        return sh;
    }

    // 获取数值字体的高度
    uint8_t get_value_font_height()
    {
        uint8_t vh = 0;
        switch (m_type)
        {
        case BOX_TYPE_21:
            vh = FONT_HEIGHT;
            break;
        case BOX_TYPE_22:
            vh = FONT_HEIGHT * 2;
            break;
        }
        return vh;
    }

    // 获取数值字体的宽度
    uint8_t get_value_font_width()
    {
        uint8_t vh = 0;
        switch (m_type)
        {
        case BOX_TYPE_21:
            vh = FONT_WIDTH;
            break;
        case BOX_TYPE_22:
            vh = FONT_WIDTH * 2;
            break;
        }
        return vh;
    }

    // 获取容器高度
    uint8_t get_height()
    {
        return BOX_SIZE_HEIGHT[m_type];
    }

    // 获取容器宽度
    uint8_t get_width()
    {
        return BOX_SIZE_WIDTH[m_type];
    }

};
Box box[3];

// 设置屏幕颜色
void screen_set_color(uint16_t color) {
    if (color == WHITE) {
        g_color = WHITE;
        g_color_bk = BLACK;
    } else {
        g_color = BLACK;
        g_color_bk = WHITE;
    }
    screen->setTextColor(g_color, g_color_bk);
}

// 时间
void screen_set_time(const char time[])
{
    int16_t x_time = 80;
    screen->fillRect(80,0,(FONT_WIDTH+FONT_INTERVAL)*5,9,g_color_bk);
    screen->setCursor(x_time, 0);
    screen->print(time);
}

// 状态栏
// 128 * (9 + 1)
void screen_status_bar() {
    // Owner
    int16_t x_owner = 1;
    screen->setCursor(x_owner, 0);
    screen->print(F("SilvBike"));    

    // 时间
    int16_t x_time = 80;
    screen->setCursor(x_time, 0);
    screen->print(F("14:00"));    

    // 电量图标
    int16_t x_battery = x_time + 5 * 6 + 1;  
    int16_t battery_len = 12;
    int16_t battery_height = 7;
    int16_t battery_head_len = 2;
    int16_t battery_head_height = 5;
    screen->drawRect(x_battery, 0, battery_len, battery_height, g_color);
    screen->drawRect(x_battery+battery_len, 1, battery_head_len, battery_head_height, g_color);
    screen->drawLine(x_battery+battery_len-1, 1, x_battery+battery_len, 1+battery_head_height, g_color_bk);
    screen->drawLine(x_battery+battery_len, 2, x_battery+battery_len, 1+battery_head_height-2, g_color_bk);
    screen->fillRect(x_battery, 0, 10, 7, g_color);
}

// 211布局
void screen_layout_211() {
    screen_set_color(WHITE);
    screen_clear();

    screen->drawLine(0, 9, 127, 9, g_color);
    screen->drawLine(63, 10, 63, 63, g_color);
    screen->drawLine(63, 37, 127, 37, g_color);

    screen_status_bar();

    box[0].init(BOX_TYPE_22, 0, 11);
    box[1].init(BOX_TYPE_21, 64, 11);
    box[2].init(BOX_TYPE_21, 64, 39);

    box[0].set_name("Receive");
    box[0].set_value("");

    box[1].set_name("Distance");
    box[1].set_value("7.2");

    box[2].set_name("Mem");
}

// 初始化屏幕
void screen_init() {
    digitalWrite(PIN_SSD1306_GND, LOW);
    pinMode(PIN_SSD1306_GND, OUTPUT);

    digitalWrite(PIN_SSD1306_VCC, HIGH);
    pinMode(PIN_SSD1306_VCC, OUTPUT);

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    screen_ssd1306->begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    screen_ssd1306->clearDisplay();

    screen_layout_211();
    screen_display();
}

void screen_display() {
    screen_ssd1306->display();
}

// 清屏
void screen_clear() {
    screen->fillRect(0,0,128,64,g_color_bk);
}

char buffer[10]; // the code uses 70 characters.
void screen_loop() {
    screen_set_time(buffer);

    box[2].set_value((String(freeMemory()) + " bytes").c_str());

    screen_display();
}

void screen_print(const char text[])
{
    box[0].set_value(text);
}
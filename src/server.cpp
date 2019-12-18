#include "common.h"
#include "server.h"

long readVcc();

Server::Server(Stream *serial)
{
    m_serial = serial;
    m_last_print_time = -PRINT_INTERVAL;
    m_last_time = 0;
    m_last_measure_vcc_time = 0;
    m_vcc = 5000;

    m_last_throttle = 0;
    m_throttle = 0;
    m_last_output_throttle = 0;
    m_output_throttle = 0;

    m_break_status = true;

    m_last_cruise_button_status = false;
    m_last_cruise_button_cruise_status = false;
    m_cruise_force_stop_flag = false;
    m_cruise_status = false;
    m_cruise_throttle = 0;
    m_cruise_low_throttle = 1023;
    m_cruise_high_throttle = 0;
    m_cruise_last_trigger_time = 0;

    m_pedal_throttle = 0;
    m_pedal_trigger_time1 = 0;
    m_pedal_trigger_time2 = 0;
    m_pedal_trigger_time3 = 0;

    m_throttle_servo = new Servo;
}


void Server::start()
{
    // analogReference(INTERNAL);
    pinMode(PIN_THROTTLE_ADC, INPUT);
    pinMode(PIN_BATTERY_VOLTAGE_ADC, INPUT);
    pinMode(PIN_THRTTLE_OUTPUT_PWM, OUTPUT);
    pinMode(PIN_BUTTON_BREAK, INPUT_PULLUP);
    pinMode(PIN_BUTTON_CRUISE, INPUT_PULLUP);

    m_throttle_servo->attach(PIN_THRTTLE_OUTPUT_PWM);

    m_vcc = readVcc();
}

void Server::loop() {
    uint64_t time = millis();

    // print to serial
    if (SERIAL_PRINT && time - m_last_print_time >= PRINT_INTERVAL) {
        m_last_print_time = time;
        this->print();
    }

    // measure vcc
    if (time - m_last_measure_vcc_time >= VCC_MEASURE_INTERVAL) {
        m_last_measure_vcc_time = time;
        m_vcc = readVcc();
    }

    this->loopThrottle();

    #ifdef ENABLE_BREAK_SENSOR 
    this->loopBreak();
    #endif

    #ifdef ENABLE_PEDAL_SENSOR 
    this->loopPedal();
    #endif

    #ifdef ENABLE_CRUISE 
    this->loopCruise();
    #endif

    this->loopOutputThrottle();

    m_last_time = micros();
    m_last_time_mill = time;
}

// print info to serial
void Server::print() {
    this->printLoopTime();
    m_serial->print(F(" Input Throttle: "));
    // m_serial->print(map(m_last_throttle, 0, 1023, 0, 5000) / 1000.0);
     m_serial->print(this->adcRead(PIN_THROTTLE_ADC));
    m_serial->print(F(" Throttle: "));
    // m_serial->print(map(m_last_throttle, 0, 1023, 0, 5000) / 1000.0);
     m_serial->print(m_throttle);

    m_serial->print(F(" PedalThrottleInterval: "));
    m_serial->print(uint32_t(m_pedal_trigger_time1 - m_pedal_trigger_time2));
    m_serial->print(F(" PedalThrottle: "));
    m_serial->print(m_pedal_throttle);

    m_serial->print(F(" BTEAK: "));
    m_serial->print(m_break_status);

    // m_serial->print(F(" Vcc: "));
    // m_serial->print(m_vcc / 1000.0);

    // m_serial->print(F(" Battery Voltage: "));
    // m_serial->print(this->map(this->adcRead(PIN_BATTERY_VOLTAGE_ADC), 0, 1023, 0, 5000) / 1000.0);

    m_serial->print(F(" OutputThrottle: "));
    m_serial->print(m_output_throttle);

    m_serial->print(F(" Cruise: "));
    m_serial->print(m_cruise_status);
    m_serial->print(F(" "));
    m_serial->print(m_cruise_throttle);
    m_serial->print(F(" "));
    m_serial->print(m_cruise_low_throttle);
    m_serial->print(F(" "));
    m_serial->print(m_cruise_high_throttle);

    m_serial->print(F("\n"));
}

void Server::printLoopTime() {
    m_serial->print(F(" Loop Time: "));
    m_serial->print(uint32_t(micros() - m_last_time) / 1000.0);
    m_serial->print(F("ms"));
}

//================================== break ===================================
void Server::loopBreak() {
    m_break_status = digitalRead(PIN_BUTTON_BREAK) == 1;
    if (m_break_status)
        m_cruise_force_stop_flag = true;
}

//================================== pedal ===================================
void Server::loopPedal() {
    uint64_t time = millis();
    // pedal sensor
    uint64_t interval1 = time - m_pedal_trigger_time1;
    uint64_t interval2 = m_pedal_trigger_time1 - m_pedal_trigger_time2;
    uint64_t interval3 = m_pedal_trigger_time2 - m_pedal_trigger_time3;

    uint64_t interval_avg = 0.5 * interval1 + 0.3 * interval2 + 0.2 * interval3;

    float throttle = 0.0;

    throttle = this->map(interval1>interval_avg?interval1:interval_avg, 30, 250, 1023, 0) / 1024.0;
    throttle = throttle * throttle;
    throttle = throttle * 1024;
    m_pedal_throttle = PEDAL_SCALE * throttle;
    // m_pedal_throttle = this->map(interval2, 40, 2000, 255, 0);
}

void Server::pedalInterrupt() {
    uint64_t time = millis();
    if (time - m_pedal_trigger_time1 <= 20)
        return;

    m_serial->println(F("interrupt"));
    m_pedal_trigger_time3 = m_pedal_trigger_time2;
    m_pedal_trigger_time2 = m_pedal_trigger_time1;
    m_pedal_trigger_time1 = time;
}

//================================== curise ===================================
void Server::loopCruise() {
    bool cruise_button_press;

    if (CRUISE_IS_HOLD_BUTTON) {
        cruise_button_press = digitalRead(PIN_BUTTON_CRUISE) == LOW;
    } else {
        bool button_press = digitalRead(PIN_BUTTON_CRUISE) == LOW;
        bool button_new_press = button_press && !m_last_cruise_button_status;
        m_last_cruise_button_status = button_press;

        if (button_new_press && m_last_time_mill - m_cruise_last_trigger_time > 500) {
            m_last_cruise_button_cruise_status = !m_last_cruise_button_cruise_status;
            m_cruise_last_trigger_time = m_last_time_mill;
        }
        cruise_button_press = m_last_cruise_button_cruise_status;
    }

    // 如果处于'强制取消巡航状态', 并且巡航按钮被抬起来了, 则取消'强制取消巡航状态'
    if (m_cruise_force_stop_flag && !cruise_button_press)
        m_cruise_force_stop_flag = false;

    bool cruise_status = !m_cruise_force_stop_flag && cruise_button_press;

    // 如果刚刚进入巡航状态
    if (cruise_status && !m_cruise_status) {
        m_serial->println("New Cruise");
        m_cruise_throttle = m_throttle;
        m_cruise_low_throttle = m_throttle;
        m_cruise_high_throttle = m_throttle;
    }

    // 记录最低速度和最高速度
    m_cruise_low_throttle = min(m_throttle, m_cruise_low_throttle);
    m_cruise_high_throttle = max(m_throttle, m_cruise_high_throttle);

    // 如果最低速度小于巡航速度, 并且当前速度大于最低速度, 或者当前速度大于巡航速度, 则取消巡航状态.
    if (cruise_status && (
            (
                (int(m_cruise_low_throttle) < int(m_cruise_throttle) - CRUISE_MAX_JITTER) &&
                (int(m_cruise_low_throttle) < int(m_throttle) - CRUISE_MAX_JITTER)
            ) ||
            (m_throttle > m_cruise_throttle + CRUISE_MAX_JITTER)
        )
    ) {
        m_cruise_force_stop_flag = true;
        m_last_cruise_button_cruise_status = false;
    }

    m_cruise_status = cruise_status;
}

//================================== throttle ===================================
void Server::loopThrottle() {
    m_last_throttle = m_throttle;
    m_throttle = this->map(this->adcRead(PIN_THROTTLE_ADC), THROTTLE_MIN, THROTTLE_MAX, 0, 1023);
}

//================================== output throttle ===================================
void Server::loopOutputThrottle() {
    uint16_t throttle = 0;

    // if breaking, shutdown the motor.
    if (m_break_status) 
        throttle = 0;
    else if (m_cruise_status)
        throttle = m_cruise_throttle;
    else
        throttle = max(m_throttle, m_pedal_throttle);

    m_last_output_throttle = m_output_throttle;
    m_output_throttle = this->mapInputToOutPut(throttle);
    m_throttle_servo->writeMicroseconds(m_output_throttle);
}

//================================== tools ===================================

uint16_t Server::adcRead(int Pin) {
    return analogRead(Pin) * (m_vcc / 5000.0);
}

long Server::map(long x, long in_min, long in_max, long out_min, long out_max) {
    x = max(x, in_min);
    x = min(x, in_max);
    return (x - in_min) * float(out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t Server::mapInputToOutPut(uint16_t value) {
    // return Server::map(Server::map(value, 0, 1023, 1100, 1900), 0, 2040, 0, 255);
    // return Server::map(value, 0, 1023, 0, 180);
    return Server::map(value, 0, 1023, 1000, 2000);
    // return Server::map(value, 0, 1023, 1100, 1900);
    // return value * (255.0/1023);
}

#ifndef PSN_SERVER_H
#define PSN_SERVER_H

#include "common.h"
// #include <pb.h>
#include <Servo.h>

class Server 
{
    Stream *m_serial;
    Servo *m_throttle_servo;

    uint64_t m_last_print_time;
    uint64_t m_last_time;
    uint64_t m_last_time_mill;

    uint64_t m_last_measure_vcc_time;
    uint64_t m_vcc;

    bool m_break_status;

    // curise
    bool m_last_cruise_button_status;
    bool m_last_cruise_button_cruise_status;
    bool m_cruise_status;
    bool m_cruise_force_stop_flag;
    uint64_t m_cruise_last_trigger_time;
    uint16_t m_cruise_throttle;
    uint16_t m_cruise_low_throttle;
    uint16_t m_cruise_high_throttle;

    uint16_t m_last_throttle;
    uint16_t m_throttle;
    uint16_t m_last_output_throttle;
    uint16_t m_output_throttle;

    uint16_t m_pedal_throttle;
    uint64_t m_pedal_trigger_time1;
    uint64_t m_pedal_trigger_time2;
    uint64_t m_pedal_trigger_time3;

    void print();
    void printLoopTime();
    void loopBreak();
    void loopPedal();
    void loopCruise();
    void loopThrottle();
    void loopOutputThrottle();

public:
    Server(Stream *serial);
    uint16_t adcRead(int Pin);
    void pedalInterrupt();
    void start();
    void loop();

    static uint16_t mapInputToOutPut(uint16_t value);
    static long map(long x, long in_min, long in_max, long out_min, long out_max);
};

#endif
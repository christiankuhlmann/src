#include <Arduino.h>
#define SEED_XIAO_NRF52840

#ifdef ESP_32
#endif

#ifdef SEED_XIAO_NRF52840

// Internal button 1
bool internal_button1_interrupt_flag = false;
void ISR_internal_button1()
{
    internal_button1_interrupt_flag = true;
}

// // Internal button 2
// bool internal_button2_interrupt_flag = false;
// void ISR_internal_button2()
// {
//     internal_button2_interrupt_flag = true;
// }

// External button
bool external_button_interrupt_flag = false;
void ISR_external_button()
{
    external_button_interrupt_flag = true;
}

void init_interrupts()
{
    attachInterrupt(PIN_A0,ISR_external_button,FALLING);
    attachInterrupt(PIN_A1,ISR_internal_button1,FALLING);
    //attachInterrupt(PIN_A2,ISR_internal_button2,FALLING);
}

#endif
/******************************************************************************
 * @file LEDControl.h
 * @brief LED Control Object
 * 
 * Contains class definition for the control service object, which is used
 * by the LED Control Service to configure the LED output to the desired
 * pattern/intensity/etc.
 * 
 * @author Steve Rizor
 * @date 4/3/2021
 * 
 *****************************************************************************/
#pragma once

#include "logging.h"
#include "rpi_ws281x/ws2811.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              WS2811_STRIP_GBR

class LEDControl
{
public:
    // LED struct is formatted for easy interop with the rpi_ws2811 library
    typedef struct
    {
        union
        {
            uint32_t wrgb;
            struct
            {
                uint8_t white;
                uint8_t red;
                uint8_t green;
                uint8_t blue;
            };
        };
    } __attribute__ ((packed)) led_t;

    enum led_color_e
    {
        LED_W = 0x10000000,
        LED_R = 0x00200000,
        LED_G = 0x00002000,
        LED_B = 0x00000020
    } ;

    LEDControl(uint16_t led_count)
        :_count(led_count)
  {
        memset(&_ledstring, 0, sizeof(_ledstring));

        _ledstring.freq = TARGET_FREQ;
        _ledstring.dmanum = DMA;
        _ledstring.channel[0].gpionum = GPIO_PIN;
        _ledstring.channel[0].count = _count;
        _ledstring.channel[0].invert = 0;
        _ledstring.channel[0].brightness = 255;
        _ledstring.channel[0].strip_type = STRIP_TYPE;

        if (ws2811_init(&_ledstring) != WS2811_SUCCESS)
        {
            LOG(LOG_ERR, "ws2811_init failed");
            exit(1);
        }
    }

    ~LEDControl() = default;

    void setIntensity(uint8_t intensity)
    {
        LOG(LOG_INFO, "Setting intensity to %u", intensity);
        _intensity = intensity;
        // TODO: update output
        _ledstring.channel[0].brightness = _intensity;
    }

    void setPattern(led_t* leds)
    {
        LOG(LOG_INFO, "Setting pattern");
        // TODO update output
        _render();
    }

    void setAll(led_color_e color)
    {
        //LOG(LOG_DEBUG, "Setting all to color");
        // TODO update output
        for (int i = 0; i<_count; i++){
            _ledstring.channel[0].leds[i] = color;
        }
        _render();
    }

private:
    uint16_t _count;
    uint8_t _intensity = 0;
    ws2811_t _ledstring;

    void _render()
    {
        LOG(LOG_DEBUG, "Rendering");
        ws2811_render(&_ledstring);
        LOG(LOG_DEBUG, "Rendered");
        // TODO add error checking
    }

};

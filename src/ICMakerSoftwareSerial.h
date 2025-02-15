#ifndef ICMakerSoftwareSerial_h
#define ICMakerSoftwareSerial_h

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "pio_uart_tx.h"
#include "pio_uart_rx.h"
#include <Arduino.h>

//状态机块
#define SoftwareSerial_PIO0 pio0
#define SoftwareSerial_PIO1 pio1

/**
 * @brief Software Serial Arduino Stream which uses the Pico PIO.
 *
 * Based on https://github.com/pschatzmann/pico-arduino/tree/00f59dddd68a0683972345d4c56bd440e356aafe/Arduino/SoftwareSerial
 * @author Flamabalistic
 * @copyright GPLv3
 */

class SoftwareSerial : public HardwareSerial
{
public:
    /*构造函数 --自由适配 请勿和简洁版本混用
     * @param txPin 发送引脚
     * @param rxPin 接收引脚
     * @param stateMachineRxIndex 接收状态机索引 推荐0 or 1
     * @param stateMachineTxIndex 发送状态机索引 推荐2 or 3
     * @param pio 状态机块 pio0 or pio1
     *
     * @note 逻辑rp2040 有2个状态机块(pio0 or pio1)，每个状态机块有4个状态机索引(0~3)
     * @note 二个状态机索引对应一个软串口,一个用于接收，一个用于发送。
     * -------------------------------------------------------------
     * @note 请勿和简洁版本混用！只要使用自由适配版本，简洁版本将会失效。
     */
    SoftwareSerial(int txPin, int rxPin, uint stateMachineRxIndex, uint stateMachineTxIndex, PIO pio);
    
    /*构造函数 -- 简洁版本
     * @param txPin 发送引脚
     * @param rxPin 接收引脚
     *
     * @note 自动适配状态机索引和pio状态机块，最多支持4个软串口,请勿和自由适配版本混用!
     * @note 请勿和简洁版本混用！只要使用自由适配版本，简洁版本将会失效。
     */
    SoftwareSerial(int txPin, int rxPin);

    // 初始化串口
    void begin(unsigned long baudRate = 115200);
    // 初始化串口
    void begin(unsigned long baudRate, uint16_t config);

    // 更改引脚
    void setPins(int txPin, int rxPin);

    void end();
    // 获取可用字节数
    int available();
    // 读取一个字节
    int peek();
    // 读取一个字节
    int read();
    // 清空缓冲区
    void flush();

    operator bool()
    {
        return true;
    }

    size_t write(uint8_t c);

    using Print::print;   // pull in write(str) and write(buf, size) from Print
    using Print::println; // pull in write(str) and write(buf, size) from Print
    using Print::write;   // pull in write(str) and write(buf, size) from Print

protected:
    uint baudRate;
    int tx;
    int rx;

    PIO pio;
    uint sm_rx;
    uint sm_tx;
    int offset;
    int peekValue = -1;

    // static uint8_t num_serials = 0; // how many serial objects have been created - used to offset the pio num

    void setupRx(uint pin_rx);

    void setupTx(uint pin_tx);
};

#endif
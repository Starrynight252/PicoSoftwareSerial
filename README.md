## _ 原作者 [Flamabalistic](https://github.com/Flamabalistic)**_
## _ PicoSoftwareSerial 库文件作者 （https://github.com/beegee-tokyo/PicoSoftwareSerial）

该ICMakerSoftwareSerial库基于 "PicoSoftwareSerial",修改而得到,修改内容:

添加 ICMakerSoftwareSerial.cpp
把PicoSoftwareSerial.h 内容移植到ICMakerSoftwareSerial.h和ICMakerSoftwareSerial.cpp,并且加以修改.

# 程序内容：
添加中文注释（目前只是简洁介绍），修改构造函数:
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

----添加：
    /*构造函数 -- 简洁版本
     * @param txPin 发送引脚
     * @param rxPin 接收引脚
     *
     * @note 自动适配状态机索引和pio状态机块，最多支持4个软串口,请勿和自由适配版本混用!
     * @note 请勿和简洁版本混用！只要使用自由适配版本，简洁版本将会失效。
     */
    SoftwareSerial(int txPin, int rxPin);
    
    // 更改引脚
    void setPins(int txPin, int rxPin);
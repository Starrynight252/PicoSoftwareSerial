#include "ICMakerSoftwareSerial.h"

/*结构体存在数量
 * @note 用于判断是否存在多个结构体
 */
uint StructExistence_number = 0;
// 检查简洁版本和自由适配版本是否混用
bool isMix = false;

SoftwareSerial::SoftwareSerial(int txPin, int rxPin, uint stateMachineRxIndex, uint stateMachineTxIndex, PIO pio)
{
    isMix = true;

    if (StructExistence_number > 0)
    {
        printf("Error: Do not mix concise version and free adaptation version!\n");
        return;
    }

    this->pio = pio;
    this->sm_rx = stateMachineRxIndex; // index of the state machine running the rx program
    this->sm_tx = stateMachineTxIndex; // index of the state machine running the tx program
    this->tx = txPin;
    this->rx = rxPin;
}

SoftwareSerial ::SoftwareSerial(int txPin, int rxPin)
{
    // 检查是否混用
    if (isMix)
    {
        printf("Error: Do not mix concise version and free adaptation version!\n");
        return;
    }

    if (StructExistence_number >= 4)
    {
        printf("Error: The number of structures exceeds the limit!\n");
        return;
    }

    // 根据当前分配次数确定使用的 PIO 和状态机索引
    switch (StructExistence_number)
    {
    case 0:
        this->pio = pio0;
        this->sm_rx = 0;
        this->sm_tx = 1;
        break;
    case 1:
        this->pio = pio0;
        this->sm_rx = 2;
        this->sm_tx = 3;
        break;
    case 2:
        this->pio = pio1;
        this->sm_rx = 0;
        this->sm_tx = 1;
        break;
    case 3:
        this->pio = pio1;
        this->sm_rx = 2;
        this->sm_tx = 3;
        break;
    default:
        printf("Error: Invalid allocation request!\n");
        return;
    }

    ++StructExistence_number;

    this->tx = txPin;
    this->rx = rxPin;
}

void SoftwareSerial::begin(unsigned long baudRate)
{
    this->baudRate = baudRate;

    if (rx >= 0)
    {
        setupRx(rx);
    }

    if (tx >= 0)
    {
        setupTx(tx);
    }
}

void SoftwareSerial::begin(unsigned long baudRate, uint16_t config)
{
    begin(baudRate);
}


void SoftwareSerial::setPins(int txPin, int rxPin)
{
    this->tx = txPin;
    this->rx = rxPin;
    if (rx >= 0)
    {
        setupRx(rx);
    }

    if (tx >= 0)
    {
        setupTx(tx);
    }
}

void SoftwareSerial::end()
{
    // TODO - implement
}

int SoftwareSerial::available()
{
    return pio_sm_get_rx_fifo_level(pio, sm_rx);
}

int SoftwareSerial::peek()
{
    peekValue = read();
    return peekValue;
}

int SoftwareSerial::read()
{
    if (peekValue != -1)
    {
        int result = peekValue;
        peekValue = -1;
        return result;
    }
    // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
    io_rw_8 *rxfifo_shift = (io_rw_8 *)&pio->rxf[sm_rx] + 3;
    if (pio_sm_is_rx_fifo_empty(pio, sm_rx))
        return -1;

    tight_loop_contents();
    return (char)*rxfifo_shift;
}

void SoftwareSerial::flush()
{
    // should always be writable
    return;
}

size_t SoftwareSerial::write(uint8_t c)
{
    pio_sm_put_blocking(pio, sm_tx, (uint32_t)c);
    return 1;
}

void SoftwareSerial::setupRx(uint pin_rx)
{
    pio_sm_set_consecutive_pindirs(pio, sm_rx, pin_rx, 1, false);
    pio_gpio_init(pio, pin_rx);
    gpio_pull_up(pin_rx);

    int offset = pio_add_program(pio, &pio_uart_rx_program);
    pio_sm_config c = pio_uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_rx); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin_rx); // for JMP
    // Shift to right, autopull disabled
    sm_config_set_in_shift(&c, true, false, 32);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baudRate);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm_rx, offset, &c);
    pio_sm_set_enabled(pio, sm_rx, true);
}

void SoftwareSerial::setupTx(uint pin_tx)
{
    // Tell PIO to initially drive output-high on the selected pin, then map PIO
    // onto that pin with the IO muxes.
    pio_sm_set_pins_with_mask(pio, sm_tx, 1u << pin_tx, 1u << pin_tx);
    pio_sm_set_pindirs_with_mask(pio, sm_tx, 1u << pin_tx, 1u << pin_tx);
    pio_gpio_init(pio, pin_tx);

    int offset = pio_add_program(pio, &pio_uart_tx_program);
    pio_sm_config c = pio_uart_tx_program_get_default_config(offset);

    // OUT shifts to right, no autopull
    sm_config_set_out_shift(&c, true, false, 32);

    // We are mapping both OUT and side-set to the same pin, because sometimes
    // we need to assert user data onto the pin (with OUT) and sometimes
    // assert constant values (start/stop bit)
    sm_config_set_out_pins(&c, pin_tx, 1);
    sm_config_set_sideset_pins(&c, pin_tx);

    // We only need TX, so get an 8-deep FIFO!
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baudRate);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm_tx, offset, &c);
    pio_sm_set_enabled(pio, sm_tx, true);
}

#include "DShot.h"

#include <math.h>

/**
 * @brief Create a DShot-package
 *
 * @param value A throttle value nor command
 * @param telemetry Instruct the ESC to send telemetry data
 * @return uint16_t The created package
 */
static inline uint16_t createPackage(uint16_t value, bool telemetry)
{
    // Append telemetry to value
    value = (value << 1) | telemetry;
    // Calc crc and append to value
    return (value << 4) | ((value ^ (value >> 4) ^ (value >> 8)) & 0x0F);
}

/**
 * @brief Send a DShot-package via the defined HardwareSerial
 *
 * @param package Package created with createPackage()
 */
void DShot::sendPackage(uint16_t package)
{
    uint8_t buffer[16];

    for (uint8_t i = 0; i < 16; i++) {
        if (package & ((1 << 15) >> i)) {
            buffer[i] = DSHOT_ONE_BYTE;
        } else {
            buffer[i] = DSHOT_ZERO_BYTE;
        }
    }

    uart->write(buffer, 16);
}

/**
 * @brief Construct a new DShot object and initilize the HardwareSerial with a DShotType
 *
 * @param uart A HardwareSerial with a available TX
 */
DShot::DShot(HardwareSerial* uart, DShotType type)
    : uart(uart)
{
    switch (type) {
    case DShotType::DShot150:
        uart->begin((1'500'000), SERIAL_8N1_TXINV);
        break;

    case DShotType::DShot300:
        uart->begin((3'000'000), SERIAL_8N1_TXINV);
        break;

    case DShotType::DShot600:
        uart->begin((6'000'000), SERIAL_8N1_TXINV);
        break;

        // case DShotType::DShot1200:
        //     UART->begin((12'000'000 / 10 * (uint8_t)accuracy), format);
        //     break;

    default:
        break;
    }
}

/**
 * @brief Create a DShot-package and send it
 *
 * @param throttle A value between 0 and 1999
 * @param telemetry Instruct the ESC to send telemetry data
 */
void DShot::sendThrottle(int16_t throttle, bool telemetry)
{
    sendPackage(createPackage(fmax(fmin(throttle, DSHOT_MAX_THROTTLE), DSHOT_MIN_THROTTLE) + 48, telemetry));
}

/**
 * @brief Create a DShot-package and send it
 *
 * @param command A value between 0 and 47
 */
void DShot::sendCommand(uint8_t command, bool telemetry)
{
    sendPackage(createPackage(fmax(fmin(command, 47), 0), telemetry));
}

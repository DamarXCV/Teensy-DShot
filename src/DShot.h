#include <HardwareSerial.h>

constexpr uint8_t DSHOT_ZERO_BYTE = 0b1111'1000;
constexpr uint8_t DSHOT_ONE_BYTE = 0b1000'0000;

constexpr uint16_t DSHOT_MIN_THROTTLE = 0;
constexpr uint16_t DSHOT_MAX_THROTTLE = 1999;

enum DShotType {
    DShot150,
    DShot300,
    DShot600,
    // DShot1200,
};

class DShot {
private:
    HardwareSerial* uart;

private:
    void sendPackage(uint16_t package);

public:
    DShot(HardwareSerial* uart, DShotType type);
    virtual ~DShot() = default;

    void sendThrottle(int16_t throttle, bool telemetry = false);
    void sendCommand(uint8_t command, bool telemetry = false);
};

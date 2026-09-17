#ifndef DS1307_H
#define DS1307_H

/*Wiring
* DS1307 -> Arduino
*  VCC -> 5V
*  GND -> GND
*  SDA -> A4
*  SCL -> A5
*  BAT -> 3V coin cell battery or GND if not used
*  SQW -> Not connected (or can be used for square wave output)
*  DS -> Not connected (or can be used for 32kHz output)
*/

struct DateTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;      // 24-hour format
    uint8_t dayOfWeek; // 1 = Sunday, 7 = Saturday
    uint8_t day;
    uint8_t month;
    uint16_t year;     // e.g., 2026
};
typedef enum : uint8_t {
    SQW_OFF = 0xFF,
    SQW_1HZ = 0x00,
    SQW_4096HZ = 0x01,
    SQW_8192HZ = 0x02,
    SQW_32768HZ = 0x03
} SQMode;

class DS1307 {
private:
    static constexpr uint8_t I2C_ADDR = 0x68;

    static uint8_t decToBcd(uint8_t val) {
        return ((val / 10) << 4) | (val % 10);
    }

    static uint8_t bcdToDec(uint8_t val) {
        return ((val >> 4) * 10) + (val & 0x0F);
    }

public:
    bool begin() {
        Wire.begin();
        Wire.beginTransmission(I2C_ADDR);
        return (Wire.endTransmission() == 0); // Returns true if device ACKs
    }

    void setTime(const DateTime& dt) {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x00); // Start writing at address 0x00
        Wire.write(decToBcd(dt.second) & 0x7F); // Bit 7 = 0 (Clears CH bit to run clock)
        Wire.write(decToBcd(dt.minute));
        Wire.write(decToBcd(dt.hour) & 0x3F);   // 24-hour mode mask
        Wire.write(decToBcd(dt.dayOfWeek));
        Wire.write(decToBcd(dt.day));
        Wire.write(decToBcd(dt.month));
        Wire.write(decToBcd(static_cast<uint8_t>(dt.year % 100)));
        Wire.endTransmission();
    }

    DateTime getTime() {
        DateTime dt{};
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x00); // Set pointer to 0x00
        Wire.endTransmission();

        // Sequential read auto-increments register pointer from 0x00 to 0x06
        Wire.requestFrom(I2C_ADDR, static_cast<uint8_t>(7));
        if (Wire.available() >= 7) {
            dt.second = bcdToDec(Wire.read() & 0x7F);
            dt.minute = bcdToDec(Wire.read());
            dt.hour = bcdToDec(Wire.read() & 0x3F);
            dt.dayOfWeek = bcdToDec(Wire.read());
            dt.day = bcdToDec(Wire.read());
            dt.month = bcdToDec(Wire.read());
            dt.year = 2000 + bcdToDec(Wire.read());
        }
        return dt;
    }
    // modes: SQW_1HZ , SQW_4096HZ , SQW_8192HZ , SQW_32768HZ , SQW_OFF = Disable
    void setSquareWave(SQMode mode) {
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x07); // Control Register address

        if (mode == SQW_OFF) {
            Wire.write(0x00); // SQWE = 0 (disabled), OUT = 0
        }
        else {
            // Bit 4 (0x10) enables SQWE; lower 2 bits select frequency
            Wire.write(0x10 | ((uint8_t)mode & 0x03));
        }
        Wire.endTransmission();
    }

    bool writeRAM(uint8_t offset, uint8_t data) {
        if (offset > 55) return false; // Max 56 bytes (0x08 to 0x3F)
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x08 + offset);
        Wire.write(data);
        return (Wire.endTransmission() == 0);
    }

    uint8_t readRAM(uint8_t offset) {
        if (offset > 55) return 0;
        Wire.beginTransmission(I2C_ADDR);
        Wire.write(0x08 + offset);
        Wire.endTransmission();
        Wire.requestFrom(I2C_ADDR, static_cast<uint8_t>(1));
        return Wire.available() ? Wire.read() : 0;
    }
};

#endif
/**
 * \file    AX12Base.h
 * \author  Mickaël Thomas
 * \version 1.0
 * \date    02/04/2014
 * \brief   Platform-independent AX12 library.
 */
#ifndef AX12BASE_H
#define AX12BASE_H

#include <stdint.h>
#include <math.h>

/**
 * \brief Abstract class used to communicate with an AX12 servomotor.
 *
 * Subclasses must provide an implementation of the readBytes(), writeBytes() and flushInput() methods.
 */
class AX12Base {
    public:
        virtual ~AX12Base() { }

        /**
         * \brief The broadcast address
         */
        static const int AX12_BROADCAST = 0xFE;

        /**
         * \brief AX12 Instruction codes
         */
        enum AX12_Instr {
            AX12_INSTR_PING = 0x01,
            AX12_INSTR_READ_DATA = 0x02,
            AX12_INSTR_WRITE_DATA = 0x03,
            AX12_INSTR_RESET = 0x06,
        };

        /**
         * \brief AX12 registers address (lower byte address for multibytes registers)
         */
        enum AX12_Register {
            AX12_ROM_MODEL_NUMBER = 0x00,
            AX12_ROM_FIRMWARE_VERSION = 0x02,
            AX12_ROM_ID = 0x03,
            AX12_ROM_BAUD_RATE = 0x04,
            AX12_ROM_RETURN_DELAY_TIME = 0x05,
            AX12_ROM_CW_ANGLE_LIMIT = 0x06,
            AX12_ROM_CCW_ANGLE_LIMIT = 0x08,
            AX12_ROM_HIGH_TEMP_LIMIT = 0x0B,
            AX12_ROM_LOW_VOLTAGE_LIMIT = 0x0C,
            AX12_ROM_HIGH_VOLTAGE_LIMIT = 0x0D,
            AX12_ROM_MAX_TORQUE = 0x0E,
            AX12_ROM_STATUS_RETURN_LEVEL = 0x10,
            AX12_ROM_ALARM_LED = 0x11,
            AX12_ROM_ALARM_SHUTDOWN = 0x12,
            AX12_ROM_DOWN_CALIBRATION = 0x14,
            AX12_ROM_UP_CALIBRATION = 0x16,

            AX12_RAM_TORQUE_ENABLE = 0x18,
            AX12_RAM_LED = 0x19,
            AX12_RAM_CW_COMPILANCE_MARGIN = 0x1A,
            AX12_RAM_CCW_COMPILANCE_MARGIN = 0x1B,
            AX12_RAM_CW_COMPILANCE_SLOPE = 0x1C,
            AX12_RAM_CCW_COMPILANCE_SLOPE = 0x1D,
            AX12_RAM_GOAL_POSITION = 0x1E,
            AX12_RAM_MOVING_SPEED = 0x20,
            AX12_RAM_TORQUE_LIMIT = 0x22,
            AX12_RAM_PRESENT_POSITION = 0x24,
            AX12_RAM_PRESENT_SPEED = 0x26,
            AX12_RAM_PRESENT_LOAD = 0x28,
            AX12_RAM_PRESENT_VOLTAGE = 0x2A,
            AX12_RAM_PRESENT_TEMPERATURE = 0x2B,
            AX12_RAM_REGISTERED_INSTRUCTION = 0x2C,
            AX12_RAM_MOVING = 0x2E,
            AX12_RAM_LOCK = 0x2F,
            AX12_RAM_PUNCH = 0x30,
        };

        /**
         * \brief This enum contains the possible flags for the ERROR field of a status packet
         * \sa AX12Base::getErrors()
         */
        enum AX12_Error {
            AX12_ERROR_NONE          = 0,        /**< No error */
            AX12_ERROR_INPUT_VOLTAGE = (1 << 0), /**< The voltage is out of the operating voltage range */
            AX12_ERROR_ANGLE_LIMIT   = (1 << 1), /**< The Goal Position is set outside the range between CW limit and CCW limit */
            AX12_ERROR_OVERHEATING   = (1 << 2), /**< The internal temperature of the AX12 is outside of the operating temperature range */
            AX12_ERROR_RANGE         = (1 << 3), /**< The instruction sent is out of the defined range */
            AX12_ERROR_CHECKSUM      = (1 << 4), /**< The checksum of the instruction packet is incorrect */
            AX12_ERROR_OVERLOAD      = (1 << 5), /**< The specified maximum torque can't control the applied load */
            AX12_ERROR_INSTRUCTION   = (1 << 6), /**< An undefined instruction was sent */
        };

        /**
         * \brief Library-specific communication error codes
         */
        enum AX12_Comm_Error {
            AX12_COMM_ERROR_NONE     = 0,         /**< No error */
            AX12_COMM_ERROR_CHECKSUM = (1 << 0),  /**< The checksum of the status packet is incorrect */
            AX12_COMM_ERROR_TIMEOUT  = (1 << 1),  /**< There was a timeout reading the status packet */
            AX12_COMM_ERROR_FORMAT   = (1 << 2),  /**< The received status packet was malformed */
            AX12_COMM_ERROR_ID       = (1 << 3),  /**< The status packet ID did not match the one in the sent instruction packet */
            AX12_COMM_ERROR_SYSTEM   = (1 << 4),  /**< A system error occured */
        };

        /**
         * \brief Read \a len bytes starting at address \a reg_start
         * \param reg_start start address
         * \param len number of bytes to read
         * \return \a true if no communication error occured, \a false otherwise
         * \sa getCommError(), getErrors()
         */
        bool readData(uint8_t reg_start, uint8_t len);

        /**
         * \brief Write \a len bytes starting at address \a reg_start
         * \param reg_start start address
         * \param len number of bytes to read
         * \param data bytes to write
         * \return \a true if no communication error occured, \a false otherwise
         * \sa getCommError(), getErrors()
         */
        bool writeData(uint8_t reg_start, uint8_t len, const uint8_t data[]);

        /**
         * \brief Read a one-byte register at address \a reg_start
         * \param reg_start register address
         * \return the 8-bit value read, or -1 if an error occured
         * \sa getCommError(), getErrors()
         */
        int readRegister1(uint8_t reg_start);

        /**
         * \brief Read a two-byte register starting at address \a reg_start
         * \param reg_start lower register address
         * \return the 16-bit value read, or -1 if an error occured
         * \sa getCommError(), getErrors()
         */
        int readRegister2(uint8_t reg_start);

        /**
         * \brief Write to a one-byte register at address \a reg_start
         * \param reg_start register address
         * \param val the 8-bit value to write
         * \return \a true if no communication error occured, \a false otherwise
         * \sa getCommError(), getErrors()
         */
        bool writeRegister1(uint8_t reg_start, uint8_t val);

        /**
         * \brief Write a two-byte register starting at address \a reg_start
         * \param reg_start lower register address
         * \param val the 16-bit value to write
         * \return \a true if no communication error occured, \a false otherwise
         * \sa getCommError(), getErrors()
         */
        bool writeRegister2(uint8_t reg_start, uint16_t val);

        /**
         * \brief Ping the AX12 until it responds
         * \remarks This is equivalent to
         * \code
         * while (!ax12.ping());
         * \endcode
         * \sa ping()
         */
        virtual void pingWait();

        /**
         * \brief Send a RESET command to the AX12.
         *
         * This resets the AX12 EEPROM to factory defaults.
         *
         * The ID is set to 1 and the baud rate to 1,000,000.
         *
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool reset();

        /**
         * \brief Send a PING command to the AX12.
         * \return \a true if the AX12 responded, \a false if a communication error occured
         * \sa getCommError()
         */
        bool ping();

        /**
         * \brief Get the current angular position of the AX12.
         * \return the angular position in degrees (0 - 300) or -1 if a communication error occured
         * \sa getCommError()
         */
        float getPresentPosition();

        /**
         * \brief Get the current speed of the AX12.
         * \return the current speed in RPM (a positive value indicates a clockwise rotation, while a negative value indicates a counter-clockwise rotation) or -1 if a communication error occured.
         * \sa getCommError()
         */
        float getPresentSpeed();

        /**
         * \brief Set the goal position of the AX12.
         * \param angle the angle in degrees (0 - 300)
         * \param block block until the AX12 is done moving
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool setGoalPosition(float angle, bool block = false);

        /**
         * \brief Set the goal speed of the AX12.
         * \param speed the angle in degrees (0 - 300)
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool setGoalSpeed(float speed);

        /**
         * \brief Get the clockwise angle limit (lower limit).
         * \return the lower angle limit (0 - 300), or a negative value if a communication error occured
         */
        float getCWLimit();

        /**
         * \brief Set the clockwise angle limit (lower limit).
         * \remarks This value is stored in the EEPROM and is therefore non-volatile.
         * \param angle the lower angle limit (0 - 300)
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool setCWLimit(float angle);

        /**
         * \brief Get the counter-clockwise angle limit (upper limit).
         * \return the upper angle limit (0 - 300), or a negative value if a communication error occured
         */
        float getCCWLimit();

        /**
         * \brief Set the counter-clockwise angle limit (upper limit).
         * \remarks This value is stored in the EEPROM and is therefore non-volatile.
         * \param angle the upper angle limit (0 - 300)
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool setCCWLimit(float angle);

        /**
         * \brief Enable or disable the torque of the AX12.
         * \param enable \a true to enable the torque, \a false to enter in Torque Free Run Condition (zero torque).
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool setTorqueEnable(bool enable);

        /**
         * \brief Change the ID of the AX12.
         * \param new_id the new ID
         * \remarks The setCurrentID() method is automatically called with the new ID unless the message was sent in broadcast or a communication error occured.
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool changeID(uint8_t new_id);

        /**
         * \brief Change the baud rate of the AX12.
         * \param new_baud the new baud rate
         */
        bool changeBaud(int new_baud);

        /**
         * \brief Set the address of the AX12 we want to send messages to. This does not send anything to the AX12.
         *
         * If the special address \ref AX12_BROADCAST is used, every connected AX12 will interpret sent messages but no status packet will be sent.
         */
        void setCurrentID(uint8_t new_id) { id = new_id; }

        /**
         * \brief Set the baud rate of the AX12 we want to send messages to. This does not send anything to the AX12.
         */
        void setCurrentBaud(int new_baud) { baud = new_baud; }

        /**
         * \brief Check if the AX12 is moving
         *
         * \return \a true if the AX12 is moving, \a false if it's not moving or if a communication error occured.
         * \sa getCommError()
         */
        bool isMoving();

        /**
         * \brief Get the raw parameters in the latest status packet sent by the AX12. This corresponds to the parameters fields after the ERROR field of the status packet.
         * \return an array of bytes, use getDataSize() to get the number of elements.
         */
        uint8_t *getData() { return &recv_packet[5]; }

        /**
         * \brief Get the size of the array returned by getData()
         */
        uint8_t getDataSize() { return recv_packet[3] - 2; }

        /**
         * \brief Get the contents of the ERROR field of the latest status packet sent by the AX12. This is a combination of the values in the \ref AX12_Error enum.
         */
        int getErrors() { return (AX12_Error) recv_packet[4]; }

        /**
         * \brief Get the ID in the latest status packet sent by the AX12.
         */
        uint8_t getReceiverId() { return recv_packet[2]; }

        /**
         * \brief Get the communication error status of the last sent instruction as defined in the \ref AX12_Comm_Error enum
         */
        int getCommError() { return comm_error; }


        void setDebug(bool on) {
            debugOn = on;
        }

        /**
         * \brief Send an instruction packet to the AX12 and read the status packet (unless the broadcast ID is used).
         * \param instr The instruction to send (as defined in the \ref AX12_Instr enum).
         * \param len The number of parameters.
         * \param data The \a len parameters.
         * \return \a true if no communication error occured, \a false otherwise
         */
        bool writePacket(uint8_t instr, uint8_t len, uint8_t data[]);

        virtual void debug(const char *format, ...);
        virtual void dumpHex(const uint8_t *buffer, int len);

        /**
         * \brief Convert an angle value (0-300°) to a raw register value (0-3ff)
         */
        static uint16_t degToRegVal(float val) { return (uint16_t) (val * 0x3ff / 300.); }

        /**
         * \brief Convert a raw register value (0-3ff) to an angle value (0-300°)
         */
        static float regValToDeg(uint16_t deg) { return deg * 300. / 0x3ff; }

        /**
         * \brief Convert a raw register value to a signed float (-1.0 to +1.0). This is used in speed and load registers.
         */
        static float regValToFloat(uint16_t val) {
            float res = val & 0x3ff;
            if (val & 0x400)
                res = -res;
            return res / 0x3ff;
        }
        /**
         * \brief Convert a signed float (-1.0 to +1.0) to a raw register value. This is used in speed and load registers.
         */
        static uint16_t floatToRegVal(float val) {
            return ((val < 0) ? 0x400 : 0) | ((uint16_t) (fabs(val) * 0x3ff));
        }
    protected:
        /**
         * \brief Construct a new AX12 with the specified \a id, \a baud rate and \a timeout.
         * \param id ID of the AX12 (or AX12_BROADCAST)
         * \param baud baudrate of the serial port
         * \param timeout timeout in microseconds
         */
        AX12Base(int id, int baud = 1000000, int timeout = 1000);
        /**
         * \brief Read \a len bytes from the AX12 serial bus.
         * \param buffer the byte array to store the received bytes into
         * \param len the number of bytes to read
         * \param timeout the maximum time (in microseconds) to wait for a reply before aborting, or -1 to wait indefinitely.
         * \remarks This method must be implemented by subclasses. If an error occurs, the implementation must return -1 and comm_error should be set.
         * \return the number of bytes read or -1 if an error occured
         */
        virtual int readBytes(uint8_t *buffer, int len, int timeout) = 0;

        /**
         * \brief Write \a len bytes to the AX12 serial bus.
         * \param buffer the byte array to write
         * \param len the number of bytes to write
         * \remarks This method must be implemented by subclasses. If an error occurs, the implementation must return -1 and comm_error should be set.
         * \return the number of bytes written or -1 if an error occured
         */
        virtual int writeBytes(const uint8_t *buffer, int len) = 0;

        /**
         * \brief Called at the beginning of a communication (read + write)
         * This can be used to reserve a lock
         */
        virtual void beginComm() { };

        /**
         * \brief Called at the end of a communication (read + write)
         * This can be used to release a lock
         */
        virtual void endComm() { };

        /**
         * \brief Flush the serial input buffer
         * \remarks This method must be implemented by subclasses.
         */
        virtual void flushInput() = 0;

        void setCommError(int comm_error);

    private:
        uint8_t checksum(uint8_t data[], uint8_t len);
        bool readPacket();

        uint8_t recv_packet[64];
        int id;
        int baud;
        int comm_error;
        int timeout;
        bool debugOn;
};

#endif

// PZEM-004T Emulator
// Original example: https://wokwi.com/projects/396609649340376065
// Library: https://github.com/mandulaj/PZEM-004T-v30
// Fixed & Upgraded by AI

#include "wokwi-api.h"
#include <stdio.h>
#include <stdint.h>

typedef struct {
  uart_dev_t uart0;
  uint8_t rx_buffer[8];
  uint8_t rx_index;
} chip_state_t;

uint16_t calculate_crc(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)data[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void send_data(chip_state_t *chip) {
    uint8_t data[25];
    int idx = 0;

    // =========================================================
    // ⚙️ UBAH NILAI SIMULASI DI SINI ⚙️
    // Ganti angka di bawah ini sesuai dengan skenario Anda
    // =========================================================
    float set_voltage = 220;   // Tegangan (Volt)
    float set_current = 30;     // Arus (Ampere)
    float set_power   = 3400;   // Daya Aktif (Watt)
    uint32_t set_energy = ( 4000* 3600) / 1000;   // Energi (Watt-hour / Wh)
    float set_freq    = 60.0;    // Frekuensi (Hz)
    float set_pf      = 0.98;    // Power Factor (0.00 - 1.00)
    // =========================================================

    // Proses konversi otomatis ke format register PZEM-004T
    uint16_t v  = (uint16_t)(set_voltage * 10.0);
    uint32_t i  = (uint32_t)(set_current * 1000.0);
    uint32_t p  = (uint32_t)(set_power * 10.0);
    uint32_t e  = set_energy;
    uint16_t f  = (uint16_t)(set_freq * 10.0);
    uint16_t pf = (uint16_t)(set_pf * 100.0);

    // Header Modbus (Alamat dinamis mengikuti request ESP32)
    data[idx++] = chip->rx_buffer[0];  
    data[idx++] = 0x04;  // Function code (Read Input Registers)
    data[idx++] = 0x14;  // Byte count (20 bytes untuk 10 register)

    // 1. Voltage (16-bit)
    data[idx++] = (v >> 8) & 0xFF; data[idx++] = v & 0xFF;
    
    // 2. Current (32-bit: Low-Word lalu High-Word)
    data[idx++] = (i >> 8) & 0xFF;  data[idx++] = i & 0xFF;
    data[idx++] = (i >> 24) & 0xFF; data[idx++] = (i >> 16) & 0xFF;
    
    // 3. Power (32-bit: Low-Word lalu High-Word)
    data[idx++] = (p >> 8) & 0xFF;  data[idx++] = p & 0xFF;
    data[idx++] = (p >> 24) & 0xFF; data[idx++] = (p >> 16) & 0xFF;
    
    // 4. Energy (32-bit: Low-Word lalu High-Word)
    data[idx++] = (e >> 8) & 0xFF;  data[idx++] = e & 0xFF;
    data[idx++] = (e >> 24) & 0xFF; data[idx++] = (e >> 16) & 0xFF;
    
    // 5. Frequency (16-bit)
    data[idx++] = (f >> 8) & 0xFF; data[idx++] = f & 0xFF;
    
    // 6. Power Factor (16-bit)
    data[idx++] = (pf >> 8) & 0xFF; data[idx++] = pf & 0xFF;
    
    // 7. Alarm Status (16-bit) -> Dikosongkan (0)
    data[idx++] = 0x00; data[idx++] = 0x00;

    // Hitung CRC (Pemeriksaan keamanan data)
    uint16_t crc = calculate_crc(data, idx);
    data[idx++] = crc & 0xFF;
    data[idx++] = crc >> 8;

    // Kirim balasan ke ESP32
    uart_write(chip->uart0, data, idx);
}

void on_uart_rx(void *user_data, uint8_t byte) {
    chip_state_t *chip = user_data;
    chip->rx_buffer[chip->rx_index++] = byte;
    
    if (chip->rx_index >= 8) {
        if (chip->rx_buffer[1] == 0x04) {
            send_data(chip);
        }
        chip->rx_index = 0; 
    }
}

void chip_init(void) {
    static chip_state_t chip;
    chip.rx_index = 0;
    
    const uart_config_t uart_config = {
        .tx = pin_init("TX", OUTPUT),      
        .rx = pin_init("RX", INPUT),
        .baud_rate = 9600,
        .rx_data = on_uart_rx,             
        .user_data = &chip,
    };
    
    chip.uart0 = uart_init(&uart_config);
    printf("PZEM Emulator Ready\n");
}

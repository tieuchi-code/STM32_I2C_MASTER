#include "main.h"

#define GPIOA_BASE_ADDR	0x40020000
#define GPIOB_BASE_ADDR	0x40020400
#define GPIOC_BASE_ADDR	0x40020800
#define GPIOD_BASE_ADDR 0x40020C00
#define GPIOE_BASE_ADDR 0x40021000
#define GPIOH_BASE_ADDR 0x40021C00

#define USART1_BASE_ADDR 0x40011000
#define I2C1_BASE_ADDR 0x40005400
#define SPI1_BASE_ADDR 0x40013000
#define I2S1_BASE_ADDR 0x40013000

#define DMA1_BASE_ADDR 0x40026000


void I2C1_Master_Init()	//PB6,PB7	MASTER
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    // MODER: Alternate function (10)
    volatile uint32_t* MODER   = (uint32_t*)(GPIOB_BASE_ADDR + 0x00);
     *MODER &= ~((0b11 << (6 * 2)) | (0b11 << (7 * 2)));
     *MODER |=  ((0b10 << (6 * 2)) | (0b10 << (7 * 2)));

     // AFRL: AF4 for I2C
     volatile uint32_t* AFRL    = (uint32_t*)(GPIOB_BASE_ADDR + 0x20);
     *AFRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
     *AFRL |=  ((0x4 << (6 * 4)) | (0x4 << (7 * 4)));

     // OTYPER: Open-drain
     volatile uint32_t* OTYPER  = (uint32_t*)(GPIOB_BASE_ADDR + 0x04);
     *OTYPER |= (1 << 6) | (1 << 7);

     // PUPDR: Pull-up
     volatile uint32_t* PUPDR   = (uint32_t*)(GPIOB_BASE_ADDR + 0x0C);
     *PUPDR &= ~((0b11 << (6 * 2)) | (0b11 << (7 * 2)));
     *PUPDR |=  ((0b01 << (6 * 2)) | (0b01 << (7 * 2)));

     // OSPEEDR: High speed
     volatile uint32_t* OSPEEDR = (uint32_t*)(GPIOB_BASE_ADDR + 0x08);
     *OSPEEDR &= ~((0b11 << (6 * 2)) | (0b11 << (7 * 2)));
     *OSPEEDR |= (0b11 << (6 * 2)) | (0b11 << (7 * 2));

     //Cấu hình I2C1
     volatile uint32_t* CR1   = (uint32_t*)(I2C1_BASE_ADDR + 0x00);
     volatile uint32_t* CR2   = (uint32_t*)(I2C1_BASE_ADDR + 0x04);
     volatile uint32_t* CCR   = (uint32_t*)(I2C1_BASE_ADDR + 0x1C);

     *CR1 &= ~(1 << 0);     // Disable I2C (PE = 0)
     *CR2 = 16;             // APB1 = 16 MHz
     *CCR = 80;             // CCR = Fpclk / (2 * F_SCL) = 16MHz / (2*100kHz)
     *CR1 |= (1 << 0);      // Enable I2C
}
void I2C_master_transmit(uint8_t address, uint8_t data)
{
    volatile uint32_t* CR1 = (uint32_t*)(I2C1_BASE + 0x00);
    volatile uint32_t* SR1 = (uint32_t*)(I2C1_BASE + 0x14);
    volatile uint32_t* SR2 = (uint32_t*)(I2C1_BASE + 0x18);
    volatile uint32_t* DR  = (uint32_t*)(I2C1_BASE + 0x10);

    // 1. Gửi START
    *CR1 |= (1 << 8);
    while (!(*SR1 & (1 << 0))); // Wait SB = 1 (sent start)

    // 2. Gửi địa chỉ + Write
    *DR = (address << 1);
    while (!(*SR1 & (1 << 1)));	// ADDR = 1
    (void)*SR2;					// Clear cờ addr

    // 3. Gửi dữ liệu
    while (!(*SR1 & (1 << 7))); // TxE = 1 (DR empty)
    *DR = data;

    // 4. Đợi truyền xong
    while (!(*SR1 & (1 << 2))); // BTF = 1

    // 5. Gửi STOP
    *CR1 |= (1 << 9);
}

int main()
{
	HAL_Init();
	I2C1_Master_Init();
	I2C_master_transmit(0x22, 0x46);
	while(1)
	{

	}
}

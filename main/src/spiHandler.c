#include "spiHandler.h"

#define I2C_MASTER_SCL_IO           22          /* SCL */
#define I2C_MASTER_SDA_IO           21          /* SDA */
#define I2C_MASTER_NUM              0           /* I2C port peripheral number */
#define I2C_MASTER_FREQ_HZ          400000      /* I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0           /* I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0           /* I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define TLC59116_ADDR               0x60

#define TLC59116_MODE_1_REG         0x00
#define TLC59116_MODE_2_REG         0x01
#define TLC59116_PWR_00_ADDR_REG    0x02
#define TLC59116_PWR_01_ADDR_REG    0x03
#define TLC59116_PWR_02_ADDR_REG    0x04
#define TLC59116_PWR_03_ADDR_REG    0x05
#define TLC59116_PWR_04_ADDR_REG    0x06
#define TLC59116_PWR_05_ADDR_REG    0x07
#define TLC59116_PWR_06_ADDR_REG    0x08
#define TLC59116_PWR_07_ADDR_REG    0x09
#define TLC59116_PWR_08_ADDR_REG    0x0A
#define TLC59116_PWR_09_ADDR_REG    0x0B
#define TLC59116_PWR_10_ADDR_REG    0x0C
#define TLC59116_PWR_11_ADDR_REG    0x0D
#define TLC59116_PWR_12_ADDR_REG    0x0E
#define TLC59116_PWR_13_ADDR_REG    0x0F
#define TLC59116_PWR_14_ADDR_REG    0x10
#define TLC59116_PWR_15_ADDR_REG    0x11
#define TLC59116_GRPPWM_REG         0x12
#define TLC59116_GRPFREQ_REG        0x13
#define TLC59116_LEDOUT0_REG        0x14
#define TLC59116_LEDOUT1_REG        0x15
#define TLC59116_LEDOUT2_REG        0x16
#define TLC59116_LEDOUT3_REG        0x17
#define TLC59116_SUBADR1_REG        0x18
#define TLC59116_SUBADR2_REG        0x19
#define TLC59116_SUBADR3_REG        0x1A
#define TLC59116_ALLCALLADR_REG     0x1B
#define TLC59116_IREF_REG           0x1C
#define TLC59116_EFLAG1_REG         0x1D
#define TLC59116_EFLAG2_REG         0x1E

#define TLC59116_PWR_ADDR_INC_REG   0xA2

#define TLC59116_LEDOUTx_DEFAULT    0xAA
#define TLC59116_LEDOUTx_DISABLE    0x00

const uint8_t InitAnnimation[5][17] = {
/*    PWM0  PWM1  PWM2  PWM3  PWM4  PWM5  PWM6  PWM7  PWM8  PWM9  PWM10 PWM11 PWM12 PWM13 PWM14 PWM15 Delay(x 10ms) */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

int8_t Increment_PWM[16]; /* Used to create a fade between the frames */

/**
 * @brief Read a sequence of bytes from a TLC59116 registers
 */
static esp_err_t RegisterRead_TLC59116(uint8_t reg_addr, uint8_t *data, size_t len)
{
    esp_err_t ret = i2c_master_write_read_device(I2C_MASTER_NUM, TLC59116_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK)
        ESP_LOGE(TAG,"Register read: %s", esp_err_to_name(ret));
    return ret;
}

/**
 * @brief Write a byte to a TLC59116 register
 */
static esp_err_t RegisterWriteByte_TLC59116(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, TLC59116_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK)
        ESP_LOGE(TAG,"Register read: %s", esp_err_to_name(ret));
    return ret;
}

static esp_err_t RegisterWriteData_TLC59116(uint8_t reg_addr, uint8_t* data, size_t len)
{
    uint8_t write_buf[len + 1];
    size_t i;
    write_buf[0] = reg_addr;
    for (i = 0 ; i < len ; i++)
        write_buf[i+1] = data[i];

    esp_err_t ret = i2c_master_write_to_device(I2C_MASTER_NUM, TLC59116_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (ret != ESP_OK)
        ESP_LOGE(TAG,"Register read: %s", esp_err_to_name(ret));
    return ret;
}

static esp_err_t spiMasterInit(void)
{
    ESP_LOGI(TAG, "Set io");
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);  // Define como saída
    gpio_set_pull_mode(GPIO_NUM_23, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "spi int");
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    esp_err_t ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK)
        ESP_LOGE(TAG,"Register read: %s", esp_err_to_name(ret));
    return ret;
}

bool CheckError_TLC59116() {
    bool ret = true;
    uint8_t ErrorFlag_1;
    uint8_t ErrorFlag_2;
    RegisterRead_TLC59116(TLC59116_EFLAG1_REG, &ErrorFlag_1, 1);
    RegisterRead_TLC59116(TLC59116_EFLAG2_REG, &ErrorFlag_2, 1);
    if (ErrorFlag_1 != 0x00) {
        ESP_LOGE(TAG,"Error Flag 1: %x", ErrorFlag_1);
        ret = false;
    }
    if (ErrorFlag_2 != 0x00) {
        ESP_LOGE(TAG,"Error Flag 2: %x", ErrorFlag_2);
        ret = false;
    }
    return ret;
}

bool WriteAllPWM(uint8_t *values, size_t len) {
    return CheckError_TLC59116();
}

bool WritePWMNum(uint8_t pwm_num, uint8_t value) {
    return CheckError_TLC59116();
}

bool DisableOutput() {
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT0_REG, TLC59116_LEDOUTx_DISABLE);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT1_REG, TLC59116_LEDOUTx_DISABLE);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT2_REG, TLC59116_LEDOUTx_DISABLE);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT3_REG, TLC59116_LEDOUTx_DISABLE);
    return CheckError_TLC59116();
}

bool EnableOutput() {
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT0_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT1_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT2_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT3_REG, TLC59116_LEDOUTx_DEFAULT);
    return CheckError_TLC59116();
}

void spiHandler() {
    ESP_LOGI(TAG, "spiHandler");
    spiMasterInit();

    gpio_set_level(GPIO_NUM_23, 1);
    /* Read the TLC59116 WHO_AM_I register, on power up the register should have the value 0x71 */
    RegisterWriteByte_TLC59116(TLC59116_MODE_1_REG, 0x00);
    RegisterWriteByte_TLC59116(TLC59116_MODE_2_REG, 0x00);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT0_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT1_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT2_REG, TLC59116_LEDOUTx_DEFAULT);
    RegisterWriteByte_TLC59116(TLC59116_LEDOUT3_REG, TLC59116_LEDOUTx_DEFAULT);
    uint8_t i;
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_00_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_01_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_02_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_03_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_04_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_05_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_06_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_07_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_08_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_09_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_10_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_11_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_12_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_13_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (i = 0 ; i < 25 ; i++) {
        RegisterWriteByte_TLC59116(TLC59116_PWR_14_ADDR_REG, i*10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    RegisterWriteByte_TLC59116(TLC59116_PWR_00_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_01_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_02_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_03_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_04_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_05_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_06_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_07_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_08_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_09_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_10_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_11_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_12_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_13_ADDR_REG, 0);
    RegisterWriteByte_TLC59116(TLC59116_PWR_14_ADDR_REG, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
    uint8_t data[15];
    for (i = 0 ; i < 15 ; i++)
        data[i] = 0x0A;

    RegisterWriteData_TLC59116(TLC59116_PWR_ADDR_INC_REG, data, sizeof(data));

    //ESP_LOGI(TAG, "WHO_AM_I 1 led 0 = %X", data[0]);

    vTaskDelay(pdMS_TO_TICKS(5000));
    gpio_set_level(GPIO_NUM_23, 0);

}
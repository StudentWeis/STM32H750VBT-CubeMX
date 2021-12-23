#include "OV2640_config.h"
#include "OV2640.h"
#include "string.h"

#define pictureBufferLength 1024 * 10
static uint32_t JpegBuffer[pictureBufferLength];

OV2640_IDTypeDef OV2640ID;
uint16_t I2C_ADDRESS = 0x60;
I2C_HandleTypeDef *mI2C;
UART_HandleTypeDef *mHuart;
DCMI_HandleTypeDef *DCMI_hdcmi;

static uint8_t OV2640_ReadID(OV2640_IDTypeDef *OV2640ID);
static void i2c_sent(uint8_t sub_address, uint8_t data);
static uint8_t i2c_receive(uint8_t sub_address);
static void SCCB_WriteRegs(const uint8_t *pbuf);
static void OV2640_AutoExposure(uint8_t level);
static void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat);
static void OV2640_BrightnessConfig(uint8_t Brightness);
static void ov2640_config(void);

uint8_t OV2640_Init(I2C_HandleTypeDef *hi2c, DCMI_HandleTypeDef *hdcmi, UART_HandleTypeDef *huart)
{
    mI2C = hi2c;
    mHuart = huart;
    DCMI_hdcmi = hdcmi;
    if (OV2640_ReadID(&OV2640ID))
        return 1;
    ov2640_config();
    return 0;
}

uint8_t OV2640_ReadID(OV2640_IDTypeDef *OV2640ID)
{
    i2c_sent(OV2640_DSP_RA_DLMT, 0x01);

    OV2640ID->Manufacturer_ID1 = i2c_receive(OV2640_SENSOR_MIDH);
    if (OV2640ID->Manufacturer_ID1 != 0x7F)
        return 1;

    OV2640ID->Manufacturer_ID2 = i2c_receive(OV2640_SENSOR_MIDL);
    if (OV2640ID->Manufacturer_ID2 != 0xA2)
        return 2;

    OV2640ID->Version = i2c_receive(OV2640_SENSOR_PIDH);
    if (OV2640ID->Version != 0x26)
        return 3;

    OV2640ID->PID = i2c_receive(OV2640_SENSOR_PIDL);
    if (OV2640ID->PID != 0x42)
        return 4;

    return 0;
}

void i2c_sent(uint8_t sub_address, uint8_t data)
{
    uint8_t sent_data[2];
    sent_data[0] = sub_address;
    sent_data[1] = data;
    HAL_I2C_Master_Transmit(mI2C, (uint16_t)I2C_ADDRESS, (uint8_t *)sent_data, 2, 10000);
}

uint8_t i2c_receive(uint8_t sub_address)
{
    uint8_t data;
    HAL_I2C_Mem_Read(mI2C, (uint16_t)I2C_ADDRESS, sub_address, 1, (uint8_t *)&data, 1, 10000);
    return data;
}

void ov2640_config(void)
{
    OV2640_JPEGConfig(JPEG_320x240);
    OV2640_BrightnessConfig(0x20);
    OV2640_AutoExposure(0);
}

void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
    uint32_t i;

    // OV2640_Reset();
    // delay_ms(200);

    for (i = 0; i < (sizeof(OV2640_JPEG_INIT) / 2); i++)
    {
        i2c_sent(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
        // delay_ms(1);
    }

    for (i = 0; i < (sizeof(OV2640_YUV422) / 2); i++)
    {
        i2c_sent(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
        // delay_ms(1);
    }

    i2c_sent(0xff, 0x01);
    i2c_sent(0x15, 0x00);

    for (i = 0; i < (sizeof(OV2640_JPEG) / 2); i++)
    {
        i2c_sent(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
        // delay_ms(1);
    }

    // delay_ms(100);

    switch (ImageFormat)
    {
    case JPEG_160x120: {
        for (i = 0; i < (sizeof(OV2640_160x120_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
            // delay_ms(1);
        }
        break;
    }
    case JPEG_176x144: {
        for (i = 0; i < (sizeof(OV2640_176x144_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
        }
        break;
    }
    case JPEG_320x240: {
        for (i = 0; i < (sizeof(OV2640_320x240_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
            // delay_ms(1);
        }
        break;
    }
    case JPEG_352x288: {
        for (i = 0; i < (sizeof(OV2640_352x288_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
        }
        break;
    }
    case JPEG_800x600: {
        for (i = 0; i < (sizeof(OV2640_800x600_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_800x600_JPEG[i][0], OV2640_800x600_JPEG[i][1]);
            // delay_ms(1);
        }
        break;
    }

    default: {
        for (i = 0; i < (sizeof(OV2640_160x120_JPEG) / 2); i++)
        {
            i2c_sent(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
        }
        break;
    }
    }
}

void OV2640_BrightnessConfig(uint8_t Brightness)
{
    i2c_sent(0xff, 0x00);
    i2c_sent(0x7c, 0x00);
    i2c_sent(0x7d, 0x04);
    i2c_sent(0x7c, 0x09);
    i2c_sent(0x7d, Brightness);
    i2c_sent(0x7d, 0x00);
}

void OV2640_AutoExposure(uint8_t level)
{
    switch (level)
    {
    case 0:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
        break;
    case 1:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL1);
        break;
    case 2:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL2);
        break;
    case 3:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL3);
        break;
    case 4:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL4);
        break;
    default:
        SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
        break;
    }
}

void SCCB_WriteRegs(const uint8_t *pbuf)
{
    while (1)
    {
        if ((*pbuf == 0) && (*(pbuf + 1) == 0))
        {
            break;
        }
        else
        {
            i2c_sent((uint8_t)*pbuf++, (uint8_t)*pbuf++);
        }
    }
}

void StartOV2640()
{
    __HAL_DCMI_ENABLE_IT(DCMI_hdcmi, DCMI_IT_FRAME);                                               //使用帧中断
    memset(JpegBuffer, 0, sizeof(JpegBuffer));                                        //把接收BUF清空
    HAL_DCMI_Start_DMA(DCMI_hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)JpegBuffer, pictureBufferLength); //启动拍照
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{

    HAL_DCMI_Suspend(DCMI_hdcmi); // 拍照完成，挂起 DCMI
    HAL_DCMI_Stop(DCMI_hdcmi);    // 拍照完成，停止 DMA传输
    int pictureLength = pictureBufferLength;
    while (pictureLength > 0)     //循环计算出接收的JPEG的大小
    {
        if (JpegBuffer[pictureLength - 1] != 0x00000000)
        {
            break;
        }
        pictureLength--;
    }
    pictureLength *= 4;
    if (mHuart != NULL)
        HAL_UART_Transmit_DMA(mHuart, (uint8_t *)JpegBuffer, pictureLength);

    // StartOV2640();
}

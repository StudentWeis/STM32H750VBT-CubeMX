# STM32H750VBTx CubeMX 练习仓库

**环境**

CubeMX 版本：6.4.0

CubeMX STM32H7xx Firmware 版本：1.9.1

Keil 版本：5.32

开发板用到了两款：

- DevEBox750V2：带有摄像头接口。
- 凡客 FK750M2：精简版。

**仓库**

- https://gitee.com/studentwei/stm32h750vbt-cubmx
- https://github.com/StudentWeis/STM32H750VBT-CubeMX

---

本仓库以进阶 STM32 学习为目的，主要完成了以下几个任务：

### 任务 00

**简介**：后续任务的模板，使用 USB 打印。

**外设**：GPIO、USBFS CDC 

**参考**：https://www.sunev.cn/embedded/732.html

**心得**：

- CubeMX 时钟配置出错，显示：“Frequency searched for is out of range for this VOS range"。

  参考：https://blog.csdn.net/weixin_42165647/article/details/115325494

  解决：修改 Power Regular Voltage Scale，由最小的 3 改为 1 或 2。 

- USB CDC VPC 总是无法被 Windows 识别到。

  实验：调节主频至 100MHz 就没事了，200MHz 就不行。可是我用 H7 不就是为了高主频吗？低主频还有什么意义？

  猜想：USB 外设时钟有一个最高限速。

  解决：要在 Clock Configuration 中配置 USB Clock Mux 为 RC48，才可以获得精确的 48MHz，否则在其他频率时 USB 初始化会发生错误。

  参考：https://www.waveshare.net/study/article-664-1.html
  
  参考手册中也有说明。

### 任务 01

**简介**：

- 使用 DCMI+I2C 驱动 OV2640，通过 UART+DMA 将图像传到电脑上。
- 摄像头和开发板都是淘宝优信电子购买的。

**外设**：DCMI、I2C、UART

**心得**：

- 花了好久才把厂家给的例程跑通，但是例程不是 CubeMX 的，感觉不太方便，不好添加新功能，就想着移植到 CubeM 上。

- 刚开始想要移植核心板的例程，发现太难了，耗时一天都没有解决。后来发现了一个 Github 上开源的 STM32H750 的 CubeMX 的例程，下载下来发现非常简洁，就尝试着移植了一下，成功。

  参考：

  - https://github.com/McflyWZX/DCMItest
  - https://blog.csdn.net/StormsWzx/article/details/113858160

- 使用野火的串口摄像头可以直接完成 MCU >> 电脑的图传。

  参考：https://www.firebbs.cn/forum.php?mod=viewthread&tid=29790

### 任务 02

**简介**：使用 DCMI+I2C 驱动 OV2640，通过 USBFS CDC 将图像传到电脑上。

**外设**：DCMI、I2C、USBFS 

**心得**：

- 发送数据不精确，无法定位 JPEG 的头和尾。

  解决：设计程序扫描出 JPEG 头和尾，并计算出精确的 JPEG 大小，精确传输数据。

  参考：DevEBox750V2 核心板例程。

- 无法连续发送图像。

  解决：每次发送前加一点延时（50 ms），可能是因为无法连续开启 DCMI，中断需要缓冲。

### 任务 03

**简介**：通过 SDMMC + FatFS 读写 SD 卡，通过 UART 与电脑通信。

**外设**：UART、SDMMC、FatFs

**心得**：

- H7 的 SD 卡接口不叫 SDIO，叫作 SDMMC，需要注意，且内置 DMA，无需再设置。

- H7 在设置时要注意将 SDMMC 除了 SDMMC_CK 之外的所有引脚全部改为 Pull Up，否则无法读写。因为开发板上的 SD 卡引脚没有上拉。

  **参考**：https://blog.csdn.net/qq_40708778/article/details/117995976

- SDMMC 的时钟频率别设太大，不能超过 SD 卡的上限。

- 问题：编译的时候遇到报错：`No space in execution regions with .ANY selector matching`。

  **原因**：空间不够大。

  参考：https://blog.csdn.net/Mark_md/article/details/108386706

  分析：FatFs 中文文件名所占 Flash 很大 170 多 KB。

  解决：取消中文支持，选择 U.S.

- 文件名要加盘符，例如：`0:c`

### 任务 04

**简介**：通过 SDIO + FatFS 读写 SD 卡，通过 USBFS CDC 与电脑通信。

**外设**：USBFS CDC、SDIO、FatFs

**心得**：堆栈配置大一点。

### 任务 05

**简介**：使用 DCMI + I2C 驱动 OV2640 拍摄照片，通过 SDMMC + FatFs 将照片写到 SD 卡中；通过 USBFS CDC 与电脑通信。

**外设**：DCMI、I2C、SDMMC、FatFs、USBFS CDC

**心得**：

- 不能在 OV2640.c 的中断回调函数中加入 `printf` 函数，否则会卡死，不知道是为什么。可能是因为中断深度不够吧。
- 基本就是任务 02 和 04 的一个整合，没有什么特别需要注意的。

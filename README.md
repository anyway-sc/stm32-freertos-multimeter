# 基于 STM32 + FreeRTOS 的多功能测量仪器

基于 **STM32F103RCT6 + FreeRTOS V11.1.0** 实现的多功能测量仪器，在一块 ST7789 SPI 屏上集成**数字万用表、示波器、信号发生器、稳压电源监测**四类功能，各功能以 FreeRTOS 多任务方式并发运行。

<!-- 建议在这里补充演示素材，比文字更有说服力：
![实物照片](docs/board.jpg)
![界面截图](docs/ui.jpg)
-->

## 功能特性

| 功能 | 实现方式 |
| --- | --- |
| 数字万用表 | 拨挡开关选择 2V / 10V / 50V 电压挡与 10kΩ / 100kΩ / 1MΩ 电阻挡；ADC 注入序列由 TIM2 触发，测量结果经邮箱送到界面任务显示 |
| 示波器 | 外部触发信号（EXTI3）启动 ADC + DMA 采集 1024 点波形，支持光标测量、时基与幅度缩放 |
| 信号发生器 | DAC + DMA 循环输出正弦波、三角波、方波等波形，由 TIM5 触发 |
| 稳压电源监测 | 通过 ADC 采集电源电压并在界面显示 |

## 硬件平台

| 项目 | 参数 |
| --- | --- |
| 主控 | STM32F103RCT6（Cortex-M3，72 MHz，HSE 8 MHz + PLL×9） |
| 显示 | ST7789 SPI 屏，SPI1 主机、单工发送 |
| 实时系统 | FreeRTOS V11.1.0（内核源码位于 `code/FreeRTOS/`） |
| 调试 | SWD（PA13 / PA14） |

### 引脚分配

| 功能 | 引脚 | 说明 |
| --- | --- | --- |
| 屏 SCK / MOSI | PB3 / PB5 | SPI1_SCK / SPI1_MOSI |
| 屏 CS / DC / RST / BL | PD2 / PC12 / PB4 / PB6 | LCD_NSS / LCD_RS / LCD_RST / LCD_BL |
| 按键 KEY1 / KEY2 / KEY4 | PC9 / PA8 / PC0 | 内部上拉输入 |
| 按键 KEY3 / KEY_UP / KEY_DOWN | PC15 / PC13 / PC14 | |
| LED / 蜂鸣器 | PC3 / PC8 | 运行指示 / 提示音 |
| 采集触发 | PA3 | EXTI3 外部中断 |
| 万用表挡位 | PB12 / PB13 / PB14 / PB15 / PC6 / PC7 | DMM50V / DMM10V / DMM2V / DMM10kΩ / DMM100kΩ / DMM1MΩ |
| ADC 输入 | PA0 / PA1 / PB0 / PB1 | ADC_IN0 / IN1 / IN8 / IN9 |
| DAC 输出 | PA4 | DAC_OUT1 |
| 串口 | PA9 / PA10 | USART1，115200 |

### 外设触发链路

- ADC1 注入序列（4 通道 + VREFINT）由 **TIM2_TRGO** 触发
- ADC1 规则转换由 **TIM3_TRGO** 触发
- DAC 输出由 **TIM5_TRGO** 触发，配合 DMA 循环送数据
- 示波器采集由 **EXTI3**（外部触发引脚）启动
- TIM6 作为 HAL 时基，SysTick 交给 FreeRTOS

## 软件架构

### 任务

| 任务 | 优先级 | 栈 | 职责 |
| --- | --- | --- | --- |
| Key | 3 | 512 B | 按键扫描与分发 |
| Buzzer | 1 | 512 B | 蜂鸣器提示 |
| LCD | 1 | 1024 B | 界面绘制与按需刷新 |
| LED | 0 | 512 B | 运行指示 |
| 定时器服务任务 | 4 | 512 B | 软件定时器回调 |

### 任务间通信与同步

| 机制 | 用途 |
| --- | --- |
| 队列（长度 1，即邮箱） | 万用表测量结果传递到界面任务 |
| 计数信号量（上限 5） | 蜂鸣器提示次数 |
| 二进制信号量 | SPI DMA 发送完成同步 |
| 事件组 | LCD 分区按需重绘，避免整屏刷新 |
| 软件定时器 | 示波器触发禁止间隔（10 ms 单次）、强制触发（100 ms 周期） |

### 目录结构

```
.
├── README.md
├── LICENSE
├── FreeRTOS参考手册.pdf
├── FreeRTOS学习笔记.pdf
├── FreeRTOS开发板电路图.pdf
└── code/                           # 完整的 STM32CubeIDE 工程
    ├── App/                        # 应用层代码（与 CubeMX 生成代码分离）
    │   ├── Tasks/                  # FreeRTOS 任务
    │   ├── Tasks/Keys/             # 各按键的具体处理
    │   ├── GUI/                    # 界面控件与面板
    │   ├── Drivers/                # 按键驱动、LCD 接口
    │   ├── dmm.c                   # 数字万用表
    │   ├── waveform_capture.c      # 示波器采集与触发
    │   └── waveform_generator.c    # 信号发生器
    ├── Core/                       # CubeMX 生成的启动代码与 FreeRTOSConfig.h
    ├── Drivers/                    # STM32 HAL 与 CMSIS
    ├── FreeRTOS/                   # FreeRTOS 内核源码
    ├── p9.2.ioc                    # CubeMX 工程文件
    └── STM32F103RCTX_FLASH.ld      # 链接脚本
```

## 快速开始

### 环境要求

- **STM32CubeIDE**：工程由 CubeIDE 创建，`.cproject` / `.project` / `.ioc` 均已随仓库提交
- 目标板：STM32F103RCT6 + ST7789 SPI 屏
- ST-Link 调试器

### 导入与编译

1. 打开 STM32CubeIDE，选择 `File → Import → General → Existing Projects into Workspace`
2. `Select root directory` 选择本仓库的 **`code`** 目录，导入工程 `p9.2`
3. `Project → Build All`（默认 Debug 配置）
4. `Run → Debug`，通过 ST-Link 烧录运行

> **Windows 专属步骤**：`code/App/Drivers/lcd_tool.exe` 是随课程提供的预编译工具，被配置成构建前置步骤（`.cproject` 中的 prebuild）。在 Linux / macOS 上编译时需要先删除该 prebuild 步骤。

### 关于 FreeRTOS 的引入方式

FreeRTOS 内核以源码方式手动加入工程（没有走 CubeMX 中间件配置），所以 `.ioc` 里看不到 FreeRTOS 选项。用 CubeMX 重新生成代码时只会更新 HAL 与 GPIO 相关文件，`code/FreeRTOS/` 与 `FreeRTOSConfig.h` 不受影响。

## 主要工作

1. **软件架构**：基于 FreeRTOS 多任务模型划分功能模块，负责整体软件架构设计
2. **任务通信与同步**：队列/邮箱传递测量数据，二进制与计数信号量完成任务与中断同步
3. **界面刷新机制**：事件组实现 LCD 按需分区重绘，降低单片机无效刷新开销
4. **定时任务**：软件定时器实现示波器触发禁止间隔与强制触发
5. **显示驱动**：SPI + DMA 驱动 ST7789 液晶，DMA 传输完成回调同步界面任务
6. **数据采集**：ADC 注入序列多通道采集（万用表挡位测量、稳压电源电压监测）
7. **波形输出**：DAC + DMA 循环输出正弦波、三角波、方波等多种波形
8. **触发链路**：定时器 TRGO 触发 ADC/DAC，EXTI 外部中断触发波形采集

## 已知限制

- 示波器波形缓冲区由 ADC 采集完成回调（中断上下文）写入、界面任务读取，目前仅通过事件组做"采集完成"通知，缓冲区本身没有临界区保护，极端情况下界面可能读到更新中的半个缓冲区。
- `configTOTAL_HEAP_SIZE` 为 8 KB，后续继续增加功能时需要重新评估堆余量。

## 第三方组件与版权

- `code/App/Drivers/lcd.a`、`lcd_tool.exe`：随课程提供的预编译 LCD 驱动，仓库中不含其源码。
- `code/Drivers/`、`code/FreeRTOS/`：STMicroelectronics HAL/CMSIS 与 FreeRTOS 内核，版权归各自作者，遵循其原始许可。
- 根目录三份 PDF 为课程参考资料，版权归原作者，仅供学习使用。

## 许可证

本仓库中本人编写的代码采用 MIT 许可证，详见 [LICENSE](LICENSE)。

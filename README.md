# 基于STM32F103的LoRa网关的设计与实现

> 该文档还没有整理完成，如果你在浏览本项目的时候发现模糊不清的问题，或者你想知道项目更多的细节，欢迎提出issue！

<br>

本项目实现的LoRa网关能够实现收发LoRa协议数据包，并通过MQTT协议与上层应用进行通信。

## 主要硬件选型

|          | 型号          | 淘宝链接                                                     |
| -------- | ------------- | ------------------------------------------------------------ |
| MCU      | STM32F103RCT6 |                                                              |
| LoRa模块 | SX1278        | [SX1278 LORA扩频无线透传模块 433MHZ/SPI转直插/Ra-01 Ra-02-淘宝网 (taobao.com)](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.67002e8dO1zuVt&id=556229791298&_u=120shlcla8dd) |
| WiFi模块 | ATK-ESP8266   | [正点原子串口WIFI模块ATK-ESP8266透传转物联网无线通信开发板-tmall.com天猫](https://detail.tmall.com/item.htm?spm=a230r.1.14.7.6b74611dWfSpdl&id=609757779633&ns=1&abbucket=12) |
| OlED模块 | 0.96OLED      | [0.96寸OLED显示屏模块0.91 1.3寸液晶屏供原理图12864屏 IIC/SPI-tmall.com天猫](https://detail.tmall.com/item.htm?id=556732249552&spm=a1z09.2.0.0.75aa2e8dee837J&_u=120shlcl2851) |

## 开发平台

基于STM32cubeMX+keil5的开发平台，OS是Windows10 1904。

## 注意事项

TODO



## 附 1：LoRa节点的Demo	

为了工程的完整性和可演示性，制作了一个基于STM32L151的LoRa节点。开发的环境与网关相同。




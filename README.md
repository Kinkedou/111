# 轻量化UI交互框架（Lite-UI）
## 🚀 项目概述

> **基于 i.MX6ULL 的嵌入式 Linux 轻量化GUI交互框架，**  
> 支持**触摸屏本地操作 + UDP 网络远程指令**双输入方式，基于 FrameBuffer 驱动 LCD 显示，结合 FreeType 实现自定义按钮式交互界面，可灵活配置按钮触发指令，适用于嵌入式设备可视化控制场景

**核心功能**：
- 本地触摸屏交互（基于 tslib），支持触摸坐标 / 压力值识别
- UDP 网络远程指令控制，端口可配置，支持按钮状态远程控制
- FrameBuffer 底层驱动，像素级 LCD 画面绘制与区域刷新
- FreeType 字体渲染，支持文字自适应居中、动态字体大小调整
- 配置文件驱动按钮生成，支持按钮可触摸属性、触发系统指令自定义
- 多输入设备 / 显示设备 / 字体引擎注册机制，模块化易扩展

**技术栈**：`嵌入式Linux`, `i.MX6ULL`, `C`, `FrameBuffer`, `tslib`, `FreeType2`, `网络编程`, `多线程`, `配置解析`

---

## 📦 仓库结构
| 目录/文件 | 说明 |
|----------|------|
| business/ | 业务主逻辑模块 |
| config/ | 配置解析模块 |
| display/ | 显示设备模块 |
| font/ | 字体渲染模块 |
| include/ | 所有模块头文件 |
| input/ | 输入模块 |
| page/ | 页面管理模块 |
| ui/ | 按钮组件模块 |
| unitest/ | 网络指令测试模块 |
| Makefile | 项目说明文档 |
| gui.conf | 按钮配置文件（按钮名称，可触摸属性，触发命令） |
| led.sh | 按钮触发示例脚本 |
| simsun.ttc | 字体文件 |

---

## ❗ 快速开始
### 编译
```bash
#编译UI程序
make
#编译网络指令client端
arm-buildroot-linux-gnueabihf-gcc -o client unitest/client.c
```
### 运行
```bash
./LiteUI ./simsun.ttc &          # 后台启动
./client 127.0.0.1 "led ok"      # 指令格式“按钮名 ok/error/数字”，对应打开/关闭/特殊三种状态
``` 
---

## 📋 详细说明
以下为项目完整模块设计、编译运行等详情内容，供进一步了解。

---

## 🧩 模块详解

### 1. 显示管理模块
- **位置**：`display/`
- **层级&功能**：
  - 管理层（`disp_manager.c`）：封装底层接口，定义显示设备的统一注册规范，实现对显示设备的全局管理，为上层提供标准化接口，同时独立于上层业务逻辑；
  - 底层（`framebuffer.c`）：直接操作 FrameBuffer 底层驱动，完成设备打开、内存映射、色深适配、实现最基础的像素绘制、区域填充等原子化操作，独立于上层文件；
- **关键实现**：
  - 多色深适配：`PutPixel`接口自动适配 8/16/32bpp LCD 色深，无需针对不同硬件调整绘制逻辑；
  - 高效区域刷新：`DrawRegion`仅刷新指定区域像素，相比全屏刷新降低 80% 以上资源占用；
  - 自适应文字渲染：`DrawTextInRegionCentral`结合 FreeType 计算文字区域，实现文字在按钮 / 区域内自动居中，适配不同尺寸显示设备。


### 2. 输入管理模块
- **位置**：`input/`
- **层级&功能**：
  - 管理层（`input_manager.c`）：向上层提供统一的事件读取接口`GetInputEvent`，实现多输入设备的注册/管理，统一事件分发逻辑，封装底层事件采集接口，；
  - 底层（`touchscreen.c`、`netinput.c`）：touchscreen 调用 tslib 接口读取触摸屏原始坐标和压力值，netinput操作 UDP Socket接收并解析远程指令，最终输出标准化InputEvent结构体，不依赖上层文件，仅依赖系统库（tslib/socket）；
- **关键实现**：
  - 并发事件处理：环形队列缓冲区支持多输入设备并发写入，无事件丢失，响应延迟 < 10ms；
  - 跨输入类型兼容：上层仅通过GetInputEvent接口获取事件，无需区分是触摸还是网络输入，完全解耦输入类型；
  - 即插即用扩展：新增输入设备仅需实现标准采集接口，无需修改现有事件分发逻辑。
 

### 3. RPC 服务端（硬件控制核心）
- **位置**：`rpc_server/`
- **功能**：运行于开发板底层，负责与硬件交互，响应客户端的 RPC 请求。
- **线程与 socket**：
  - 主线程：监听新的客户端连接（socket），有连接到来时创建新线程处理该连接
  - 温湿度读取线程：独立线程，定期读取 DHT11 传感器数据，更新共享内存
  - 视频采集线程：独立线程，使用 V4L2 采集 USB 摄像头 MJPEG 帧，放入缓冲区
  - 客户端通信线程：客户端的每个连接对应一个线程，处理该客户端的 RPC 请求（读/写 LED、移动监测开关、获取温湿度、获取视频帧等）
  - socket连接：支持多个客户端同时连接（QT 客户端和 MQTT 客户端），为每个客户端的socket分别创建多个线程
- **技术实现**：
  - 数据交互：服务端与客户端之间的所有 RPC 请求/响应均采用 **cJSON** 格式封装。
  - LED 和 DHT11 温湿度传感器：由各自的驱动程序完成GPIO的读写功能，将数据发送到各客户端
  - 视频流：通过 V4L2 采集 MJPEG 格式，base64编码为字符串后，发送到 QT 客户端
  - 移动监测：帧差法，检测到运动时自动保存图片（保存到开发板存储）


### 4. 整体数据流
 - 用户（触摸屏）   <---------------> QT 客户端 <-----------------> RPC Server <---> 硬件（LED、DHT11、摄像头）
 - 用户（手机 APP） <--> OneNET 云平台 <--> MQTT 客户端 <---> RPC Server <---> 硬件（LED、DHT11、摄像头）

---

## 🔧 编译与运行

### 依赖库
项目依赖以下第三方库（已放置在 `lib/` 目录，或需自行编译）：
- **jsonrpc-c**：用于构建 RPC 消息，依赖于libev库
- **libev**：事件循环库
- **paho.mqtt.c**：MQTT 客户端 C 库

交叉编译时需确保工具链已安装。

### 编译步骤
1. **编译 RPC 服务端**
   ```bash
   cd rpc_server
   make
   ```
生成可执行文件 rpc_server

2. **编译 MQTT 客户端**
   ```bash
   cd mqtt_device_wechat
   make
   ```
生成可执行文件 mqtt_client

3. **编译 QT 客户端**

使用 Qt Creator 打开 LED_TempHumi_Video/LED_TempHumi_Video.pro，选择合适 kit（交叉编译工具链）进行编译。编译输出目录示例为 build-LED_TempHumi_Video-100ask-Debug/，生成的可执行文件为 LED_TempHumi_Video

### 运行步骤
- 确保开发板可以ping通外网
- 启动服务端`./rpc_server`
- 启动两个客户端（不分先后）`./LED_TempHumi_Video` `./mqtt_device_wechat`
- 建议编写开机自启动脚本（如 /etc/init.d/S99myqt），内容示例：
  ```bash
  #!/bin/sh
  
  start() {
  echo -e "\033[9;0]" > /dev/tty0
  export QT_QPA_GENERIC_PLUGINS=tslib:/dev/input/event1
  export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0
  export QT_QPA_FONTDIR=/usr/lib/fonts/

  psplash-write "PROGRESS 95"
  psplash-write "QUIT"
  /root/rpc_server &
  sleep 5
  /root/LED_TempHumi_Video  &
  sleep 5
  /root/mqtt_device_wechat
  }

  stop() {
      killall LED_TempHumi_Video
  }

  case "$1" in
      start)
          start
          ;;
      stop)
          stop
          ;;
      *)
          echo "Usage: $0 {start| stop|restart}"
          exit 1
  esac

  exit $?
  ```
---

## 📌 后续计划
- 增加更多传感器和外设（如烟雾、人体红外、GPS）
- 移动监测告警通知到手机APP
- 部署更智能的检测算法，增加目标识别等功能
- 优化视频帧采集和传输
- 编写简单的驱动程序，对现有驱动进行替换

---

## 许可证
- 本项目基于韦东山老师开源项目改进，仅供参考；
- 允许学习、参考、修改、非商业使用；
- 禁止商用；
- 引用请注明出处。
## 作者
- 阿卫
- GitHub：[Kinkedou](https://github.com/Kinkedou)

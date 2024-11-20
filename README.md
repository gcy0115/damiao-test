# damiao-linux下速度位置模式

## 1. 通讯设置
4310电机使用can通讯，串口部分用于连接上位机调试。上位机仅在windows平台上使用。详细的连接和调试说明，参考[使用说明书](调试助手使用说明书（达妙驱动控制协议）V1.4.pdf)。

代码运行平台使用Ubuntu22，can设备为[usb转can](http://e.tb.cn/h.T1gwEpVj8v1dp8S?tk=ocHw3sugto3)，我这里选了1.0隔离版本。
在 Ubuntu 或其他 Linux 发行版中，使用以下命令安装：
```shell
sudo apt update
sudo apt install can-utils  #确保 can-utils 已安装
```

分析仪默认刷了pcan的固件，在我这里一直没有用起来，于是选择刷canable的官方固件，参考[官方updater](https://canable.io/updater/canable1.html)，刷入固件后在Ubuntu下即可识别：

```shell
ifconfig -a

    can0: flags=193<UP,RUNNING,NOARP>  mtu 16
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 10  (未指定)
        RX packets 219792  bytes 1758336 (1.7 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 112701  bytes 901608 (901.6 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```
可测试如下内容：

```sh
sudo ip link set can0 type can bitrate 1000000  #设置can0通讯波特率
sudo ifconfig can0 up  #启用can0
cansend can0 123#1122334455667788  #发送一个 CAN 帧，ID 为 123，数据为 11 22 33 44 55 66 77 88
sudo ifconfig can0 down  #停用can0
```
可开启另一个终端使用`candump`命令监听发送的指令是否被接收：
```shell
candump can0
    can0  123   [8]  11 22 33 44 55 66 77 88  #应收到的内容
```

## 2. 控制流程
电机完成校准、标定以及参数设置后，即可以使用，控制使用 CAN 标准帧
格式，固定波特率为 1Mbps，按功能可分为接收帧和反馈帧，接收帧为接收到的控制数据，用于实现对电机的命令控制；反馈帧为电机向上层控制器发送电机的状态数据。根据电机选定的不同模式，其接收帧帧格式定义以及帧 ID 各不相同，但各种模式下的反馈帧是相同的。

注意：**4310的反馈帧只有接收can指令后才会发送，所以在电机使能后，必须持续发送控制帧才能得到持续的反馈帧。**

在实现思路上，我们在主函数中开启三个线程，

1. **接收线程**: 用于接收，无限循环，后续可加上ROS广播节点；
2. **控制线程**: 用于发送控制帧，需要维持一定的控制频率，过高可能导致can负载过大，我这里单电机可运行在1000hz，更多电机时需要适当降低控制频率或增加多路can；
3. **订阅线程**: 用于接收外界指令，并传递给控制线程。目前订阅来自键盘的输入，后续可加上ROS订阅节点。










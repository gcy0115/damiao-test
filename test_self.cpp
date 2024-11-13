// test socket CAN via cpp

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <linux/can.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <cstdlib>  // stdlib.h
#include <iostream>

#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "include/can_opration.hpp"
#include "include/dm4310.hpp"

int main(){


    const char* can_interface = "can0";  // CAN 接口名称
    std::string can_id = "can0";
    initial_can(can_id);

    int target_id = 0x123;               // 目标 CAN ID
    
    int sock = openCANSocket(can_interface);
    if (sock < 0) {
        return -1;
    }
    
    // SocketCAN 的套接字可以复用，即：
    // 一旦套接字被成功创建并绑定到指定的 CAN 接口上，你可以通过 write 函数不断向套接字发送新的 CAN 帧数据，而无需重新打开或绑定套接字。
    // 只需在 can_frame 结构体中更新 can_id 和 data，然后调用 write 函数即可将新的数据发送到 CAN 总线上。

    // 准备 CAN 帧数据
    struct can_frame frame;
    frame.can_id = 0x123;  // 设置 CAN ID
    frame.can_dlc = 8;     // 数据长度（0-8）
    frame.data[0] = 0x01;  // 填充数据
    frame.data[1] = 0x02;
    frame.data[2] = 0x03;
    frame.data[3] = 0x04;
    frame.data[4] = 0x05;
    frame.data[5] = 0x09;
    frame.data[6] = 0x07;
    frame.data[7] = 0x08;

    // 发送 CAN 帧
    // write(sock, &frame, sizeof(frame))
    // 使用 write 函数通过套接字 sock 发送 CAN 帧。
    // sock 是一个已经连接并绑定到 CAN 接口的套接字。
    // &frame 是一个指向 can_frame 结构体的指针，代表需要发送的 CAN 帧。
    // sizeof(frame) 表示 CAN 帧的大小（字节数），告诉 write 发送的数据量。

    // write 的返回值检查:
    // write 返回实际写入的字节数。
    // 如果 write 的返回值与 sizeof(frame) 不相等，说明发送过程中可能出现了错误（例如传输中断或套接字错误）。
    if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
        std::cerr << "Error in sending CAN frame: " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }

    std::cout << "CAN frame sent successfully!" << std::endl;

    int motor1 = 0x201;
    enable_motor(sock, motor1);

    // 关闭套接字
    close(sock);
    terminate_can(can_id);
    return 0;
}
// test socket CAN via cpp

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <linux/can.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <cstdlib>  // stdlib.h
#include <cstring>
#include <cerrno>
#include <net/if.h>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

#include "include/can_opration.hpp"
#include "include/dm4310.hpp"


void sendThread(int sock, int motor_id) {
    while (true) {
        enable_motor(sock, motor_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 控制发送频率
        disable_motor(sock, motor_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void receiveThread(int sock, int master_id) {
    while (true) {
        std::vector<uint8_t> received_data;
        if (receiveCANFrame(sock, master_id, received_data)) {
            // 处理接收到的反馈数据
            std::cout << "Received feedback from motor " << master_id << std::endl;
        }
    }
}

int main(){


    const char* can_interface = "can0";  // CAN 接口名称
    std::string can_id = "can0";
    initial_can(can_id);

    int sock = openCANSocket(can_interface);
    if (sock < 0) {
        return -1;
    }
    
    // SocketCAN 的套接字可以复用，即：
    // 一旦套接字被成功创建并绑定到指定的 CAN 接口上，你可以通过 write 函数不断向套接字发送新的 CAN 帧数据，而无需重新打开或绑定套接字。
    // 只需在 can_frame 结构体中更新 can_id 和 data，然后调用 write 函数即可将新的数据发送到 CAN 总线上。

    int motor1 = 0x002;
    int target1 = 0x000;

    // starting receving

    
    std::thread receiver(receiveThread, sock, target1);
    std::thread sender(sendThread, sock, motor1);

    // enable_motor(sock, motor1);

    // std::this_thread::sleep_for(std::chrono::microseconds(10000)); // 暂停 5 秒
    // disable_motor(sock, motor1);

    receiver.join();


    // 关闭套接字
    close(sock);
    terminate_can(can_id);
    return 0;
}
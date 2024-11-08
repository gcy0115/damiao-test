// test socket CAN via cpp
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



// #define ip_cmd_set_can0_params "sudo ip link set can0 type can bitrate 1000000"
// #define ip_cmd_can0_up         "sudo ifconfig can0 up"
// #define ip_cmd_can0_down       "sudo ifconfig can0 down"


void initial_can(std::string& can_name){
    const std::string cmd1 = "sudo ip link set " + can_name + " type can bitrate 1000000";
    const std::string cmd2 = "sudo ifconfig " + can_name + " up";
    const std::string cmd3 = "sudo ifconfig " + can_name + " down";
    //generate comands for can;

    const char* ip_cmd_set_can_params = cmd1.c_str();
    const char* ip_cmd_can_up = cmd2.c_str();
    const char* ip_cmd_can_down = cmd3.c_str();
    // string2char*

    system(ip_cmd_can_down); // shut down the possibly running can
    int CAN_ready = system(ip_cmd_set_can_params); // 设置参数   
    CAN_ready += system(ip_cmd_can_up);  // 开启can0接口
    // std::cout << "Output:" << CAN_ready << std::endl;

    // system(ip_cmd_can0_down);

    if (CAN_ready != 0) {
        std::cerr << "can0 failed to open" << std::endl;

    }
    else{
        std::cout << "can0 successfully opened" << std::endl;
    }
}

void terminate_can(std::string& can_name){
    const std::string cmd3 = "sudo ifconfig " + can_name + " down";
    const char* ip_cmd_can_down = cmd3.c_str();
    system(ip_cmd_can_down);
    std::cout << "can0 shutting down." << std::endl;
}

int main(){

    std::string can0 = "can0";
    initial_can(can0);

    // Opening a socket, CAN type.
    // PF_CAN：指定使用 CAN 协议族。
    // SOCK_RAW：设置为原始套接字，以便直接访问 CAN 帧数据。
    // CAN_RAW：使用原始 CAN 协议，用于标准的 CAN 帧通信。


    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        std::cerr << "Error while opening socket: " << strerror(errno) << std::endl;
        return -1;
    }

    // 指定 CAN 接口
    struct ifreq ifr;
    std::strcpy(ifr.ifr_name, "can0");  // 使用 can0 作为 CAN 接口
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "Error: could not locate CAN interface" << std::endl;
        close(sock);
        return -1;
    }

    // 绑定套接字到 CAN 接口
    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error in socket bind: " << strerror(errno) << std::endl;
        close(sock);
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
    // write(sock, &frame, sizeof(frame)
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

    // 关闭套接字
    close(sock);
    terminate_can(can0);
    return 0;
}
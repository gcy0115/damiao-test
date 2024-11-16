// test socket CAN via cpp
#include <iostream>
#include <thread>

#include "include/can_opration.hpp"
#include "include/dm4310.hpp"


void sendThread(int sock, dmMotor motor) {
    while (true) {
        enable_motor(sock, motor.getID());
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // 控制发送频率
        disable_motor(sock, motor.getID());
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void receiveThread(int sock, dmMotor motor) {
    while (true) {
        std::vector<uint8_t> received_data;
        if (receiveCANFrame(sock, motor.getMasterID(), received_data)) {
            // 处理接收到的反馈数据

            // printReceivedCANFrame(sock, motor.getMasterID());
            MotorFeedback feedback = parseCANFeedback(motor.getMasterID(), received_data);
            motor.setERR(feedback.ERR);
            motor.setPOS(feedback.POS);
            motor.setVEL(feedback.VEL);
            motor.setT(feedback.T);
            motor.setTMOS(feedback.T_MOS);
            motor.setTRotor(feedback.T_Rotor);
            std::cout << "Motor ID: " << feedback.motorID << std::endl;
            // std::cout << "ERR: " << feedback.ERR << std::endl;
            // std::cout << "POS: " << feedback.POS << std::endl;
            // std::cout << "VEL: " << feedback.VEL << std::endl;
            // std::cout << "T: " << feedback.T << std::endl;
            // std::cout << "T_MOS: " << static_cast<int>(feedback.T_MOS) << std::endl;
            // std::cout << "Received feedback from motor :" << motor.getMasterID() << std::endl;
        }
    }
}

int main(){


    // const char* can_interface = "can0";  // CAN 接口名称
    std::string can_id = "can0";
    initial_can(can_id);

    int sock = openCANSocket(can_id.c_str());
    // .c_str() 返回一个 const char* 指针，指向 std::string 中的字符数组，并在末尾加上空字符 \0。c_str() 返回的指针是只读的，不应被修改，否则会导致未定义行为。
    if (sock < 0) {
        return -1;
    }
    
    // SocketCAN 的套接字可以复用，即：
    // 一旦套接字被成功创建并绑定到指定的 CAN 接口上，你可以通过 write 函数不断向套接字发送新的 CAN 帧数据，而无需重新打开或绑定套接字。
    // 只需在 can_frame 结构体中更新 can_id 和 data，然后调用 write 函数即可将新的数据发送到 CAN 总线上。

    // 实例化一个dmMotor对象，使用ID和masterID初始化变量 
    dmMotor motorFR(0x002, 0x102);

    // starting receving
    std::thread receiver(receiveThread, sock, motorFR);

    // starting sending
    std::thread sender(sendThread, sock, motorFR);


    receiver.join();
    sender.join();


    terminate_can(can_id);
    // 关闭套接字
    close(sock);

    return 0;
}
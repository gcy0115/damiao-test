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
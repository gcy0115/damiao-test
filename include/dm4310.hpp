#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>


float uint_to_float(int x_int, float x_min, float x_max, int bits){
    /// converts unsigned int to float, given range and number of bits ///
    float span = x_max- x_min;
    float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}


int float_to_uint(float x,float x_min, float x_max, int bits){
    // Converts afloat to anunsigned int, given range and number ofbits

    float span = x_max-x_min;
    float offset =x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}


// void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* _hcan){

//     p_int=(_hcan->pRxMsg->Data[1]<<8)|_hcan->pRxMsg->Data[2];
//     v_int=(_hcan->pRxMsg->Data[3]<<4)|(_hcan->pRxMsg->Data[4]>>4);
//     t_int=((_hcan->pRxMsg->Data[4]&0xF)<<8)|_hcan->pRxMsg->Data[5];
//     position = uint_to_float(p_int, P_MIN, P_MAX, 16);// (-12.5,12.5)
//     velocity = uint_to_float(v_int, V_MIN, V_MAX, 12);// (-45.0,45.0)
//     torque = uint_to_float(t_int, T_MIN, T_MAX, 12); //(-18.0,18.0)
    
//     /*#### add enable can it again to solve can receive only one ID problem!!!####**/
//     __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
// }

class Motor {
private:
    int ID;                // 电机的 CAN ID
    float p_des;           // 期望位置
    float v_des;           // 期望速度
    float Kp;              // 位置比例系数
    float Kd;              // 位置微分系数
    float T_ff;            // 转矩给定值
    float POS;             // 实际位置
    float VEL;             // 实际速度
    float T;               // 电机的扭矩信息
    float T_MOS;           // 驱动器温度
    float T_Rotor;         // 电机线圈温度
    int ERR;               // 状态信息，
                                // 0——失能；
                                // 1——使能；
                                // 8——超压；
                                // 9——欠压；
                                // A——过电流；
                                // B——MOS过温；
                                // C——电机线圈过温；
                                // D——通讯丢失；
                                // E——过载；

    public:
    // 构造函数，初始化各个参数
    Motor(int id, float p_des = 0.0f, float v_des = 0.0f, float kp = 0.0f, float kd = 0.0f, float t_ff = 0.0f,
          float pos = 0.0f, float vel = 0.0f, float t = 0.0f, float t_mos = 0.0f, float t_rotor = 0.0f, int err = 0)
        : ID(id), p_des(p_des), v_des(v_des), Kp(kp), Kd(kd), T_ff(t_ff),
          POS(pos), VEL(vel), T(t), T_MOS(t_mos), T_Rotor(t_rotor), ERR(err) {}

    // Getter 和 Setter 方法
    int getID() const { return ID; }
    void setID(int id) { ID = id; }

    float getPDes() const { return p_des; }
    void setPDes(float p) { p_des = p; }

    float getVDes() const { return v_des; }
    void setVDes(float v) { v_des = v; }

    float getKp() const { return Kp; }
    void setKp(float kp) { Kp = kp; }

    float getKd() const { return Kd; }
    void setKd(float kd) { Kd = kd; }

    float getTFF() const { return T_ff; }
    void setTFF(float t_ff) { T_ff = t_ff; }

    float getPOS() const { return POS; }

    float getVEL() const { return VEL; }

    float getT() const { return T; }

    float getTMOS() const { return T_MOS; }

    float getTRotor() const { return T_Rotor; }

    int getERR() const { return ERR; }
    
};

// ------------------------DEMO----------------------------//
// int main() {
//     Motor motor(1);  // 创建一个 ID 为 1 的电机实例

//     motor.setPDes(100.0f);  // 设置期望位置
//     motor.setKp(1.2f);      // 设置位置比例系数

//     int E = 5;  // 假设状态信息最大值为 E = 5
//     try {
//         motor.setERR(3, E);  // 设置状态信息
//     } catch (const std::out_of_range& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     std::cout << "Motor ID: " << motor.getID() << std::endl;
//     std::cout << "Desired Position: " << motor.getPDes() << std::endl;
//     std::cout << "Kp: " << motor.getKp() << std::endl;
//     std::cout << "Error Status (ERR): " << motor.getERR() << std::endl;

//     return 0;
// }

// 解析 CAN 总线上的反馈信息


struct MotorFeedback {
    int motorID;      // 电机 ID
    int ERR;          // 错误信息
    int POS;          // 16 位实际位置
    int VEL;          // 12 位实际速度
    int T;            // 12 位扭矩
    uint8_t T_MOS;    // 8 位驱动器温度
    uint8_t T_Rotor;  // 8 位电机线圈温度
};
MotorFeedback parseCANFeedback(int can_id, const std::vector<uint8_t>& data) {
    if (data.size() < 8) {
        throw std::invalid_argument("CAN 数据长度不足 8 字节");
    }

    MotorFeedback feedback;

    // 第1位：低 8 位为电机 ID，高 8 位为 ERR 信息
    feedback.motorID = data[0] & 0x0F;  // 提取低 4 位的电机 ID
    feedback.ERR = data[0] >> 4;        // 提取ERR 信息

    // 第2、3位：POS 高 8 位和低 8 位，总长 16 位
    feedback.POS = (data[1] << 8) | data[2];

    // 第4、5位：VEL 的高 8 位和低 4 位，总长 12 位
    feedback.VEL = (data[3] << 4) | (data[4] >> 4);

    // 第5位（低 4 位）和第6位：T 的高 4 位和低 8 位，总长 12 位
    feedback.T = ((data[4] & 0x0F) << 8) | data[5];

    // 第7位：T_MOS（8 位）
    feedback.T_MOS = data[6];

    // 第8位：T_Rotor（8 位）
    feedback.T_Rotor = data[7];

    return feedback;
}


// 测试
// int main() {
//     // 模拟 CAN 数据
//     int can_id = 123;
//     std::vector<uint8_t> data = { 0x01, 0x00, 0x01, 0x12, 0x34, 0x56, 0x78, 0x9A };

//     try {
//         MotorFeedback feedback = parseCANFeedback(can_id, data);

//         std::cout << "Motor ID: " << feedback.motorID << std::endl;
//         std::cout << "ERR: " << feedback.ERR << std::endl;
//         std::cout << "POS: " << feedback.POS << std::endl;
//         std::cout << "VEL: " << feedback.VEL << std::endl;
//         std::cout << "T: " << feedback.T << std::endl;
//         std::cout << "T_MOS: " << static_cast<int>(feedback.T_MOS) << std::endl;
//         std::cout << "T_Rotor: " << static_cast<int>(feedback.T_Rotor) << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error parsing CAN feedback: " << e.what() << std::endl;
//     }

//     return 0;
// }
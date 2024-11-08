#include <iostream>
#include <stdexcept>

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
    int ERR;               // 状态信息，0——失能；
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

#include "Cont_SMotor.h"
#include <math.h>
#include <stdbool.h>
#include "AllHeader.h"

Coordinate laser_position = {0.0f, 0.0f}; // 激光雷达位置
Coordinate target_position = {0.0f, 0.0f}; // 目标位置
bool is_updated = false; // 是否更新了数据
bool is_new_mode = true; // 是否进入了新模式

void PID_SMotor_Cont(void)
{
    static PIDdata pid_x, pid_y;
    static uint32_t last_update_time = 0;
    PIDConfig config_x = {1.0f, 0.1f, 0.01f};
    PIDConfig config_y = {1.0f, 0.1f, 0.01f};
    uint32_t current_time = tick;
    if(is_new_mode) {
        PID_Init(&pid_x);
        PID_Init(&pid_y);
        is_new_mode = false; // 重置新模式标志
        last_update_time = current_time; // 更新上次更新时间
        return; // 如果是新模式则不进行PID计算
    }
    if(!is_updated) return; // 如果数据未更新则返回
    is_updated = false; // 重置更新标志
    float dt = (current_time - last_update_time) / 1000.0f; // 时间间隔
    PID_Update(&pid_x, target_position.x, laser_position.x, dt);
    PID_Update(&pid_y, target_position.y, laser_position.y, dt);
    last_update_time = current_time; // 更新上次更新时间
    // 计算PID输出
    float output_wyaw = -1 * PID_Compute(&pid_x, config_x.Kp, config_x.Ki, config_x.Kd);
    float output_wpitch = PID_Compute(&pid_y, config_y.Kp, config_y.Ki, config_y.Kd);
    // 发送控制命令
    YP_SMotor_SetSpeed(output_wyaw, output_wpitch); // 设置舵机速度
    YP_SMotor_UpdateState(); // 更新舵机状态
    #ifdef SMOTOR_DEBUG
    target_position.x = 1.0f; // 调试时将目标位置设置为激光雷达位置
    target_position.y = 1.0f; // 调试时将目标位置设置为激光雷达位置
    laser_position.x += output_wyaw * dt * (-0.01f); // 模拟激光雷达位置变化
    laser_position.y += output_wpitch * dt * (0.01f); // 模拟激光雷达位置变化
    is_updated = true; // 设置数据已更新标志
    #endif
}


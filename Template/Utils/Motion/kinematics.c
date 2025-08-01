#include "kinematics.h"
#include "AllHeader.h"


// --- ??????? ---
float sumTheta(float theta1, float theta2)
{
    if(theta1 < -180.0f || theta1 > 180.0f || theta2 < -180.0f || theta2 > 180.0f) {
        snprintf(error_message, sizeof(error_message), "Theta values out of range: theta1 = %.2f, theta2 = %.2f", theta1, theta2);
        error_handler(); 
    }
    float sum = theta1 + theta2;
    while (sum < -180.0f) sum += 360.0f;
    while (sum > 180.0f) sum -= 360.0f;
    return sum;
}

// --- ??????? ---
WheelSpeed SpeedToWheelSpeed(Speed speed)
{
    WheelSpeed wheel_speed;
    wheel_speed.left_wheel_speed = speed.linear_velocity - (speed.angular_velocity / 180.0f * PI * WHEEL_DIS / 2000.0f);
    wheel_speed.right_wheel_speed = speed.linear_velocity + (speed.angular_velocity / 180.0f * PI * WHEEL_DIS / 2000.0f);
    return wheel_speed;
}

// --- PID?????? ---
// ??????????current_data??dt??current_data.dt??????????UpdateData()?????
Speed PID_Move(float v, float w, short isreload)
{
    static PIDdata pidSpeed;
    static PIDdata pidAngular;
    static Speed last_target = {0.0f, 0.0f};

    // PID???????????????
    float K_p_v = 1.0f, K_i_v = 0.1f, K_d_v = 0.0f;
    float K_p_w = 1.0f, K_i_w = 0.1f, K_d_w = 0.1f;

    if (isreload) {
        PID_Init(&pidSpeed);
        PID_Init(&pidAngular);
        last_target.linear_velocity = v;
        last_target.angular_velocity = w;
        return current_data.speed;
    }

    // ??current_data.dt???dt
    float dt = current_data.dt;
    if (dt <= 0.0f) dt = 0.01f; // ???dt?0

    PID_Update(&pidSpeed, v, current_data.speed.linear_velocity, dt);
    PID_Update(&pidAngular, w, current_data.speed.angular_velocity, dt);

    Speed target_speed;
    target_speed.linear_velocity = v + PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v);
    target_speed.angular_velocity = w + PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w);

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed);

    LSet((int16_t)(STOD * wheel_speed.left_wheel_speed));
    RSet((int16_t)(STOD * wheel_speed.right_wheel_speed));

    return current_data.speed;
}

bool runCircle(float radius, float speed, float angle, DIR dir)
{
    static int first_run = 1;
    static float start_theta = 0.0f;
		static uint32_t last_time = 0;
		uint32_t now = tick;

    float linear_velocity = speed;
    float angular_velocity = linear_velocity / radius * (180.0f / PI);
    if (dir == RIGHT) angular_velocity = -angular_velocity;

    if (first_run || now - last_time > 1000) {
        first_run = 0;
        start_theta = car.pose.theta; // ��¼��ʼ�Ƕ�
        PID_Move(linear_velocity, angular_velocity, 1); // ��ʼ��PID
				last_time = now;
        return false;
    }
	last_time = now;

    Speed cur = PID_Move(linear_velocity, angular_velocity, 0);

    float delta_theta = sumTheta(car.pose.theta, -start_theta);
    float remain_angle = angle - fabs(delta_theta);

     if (fabs(remain_angle) < 2.0f) {
        first_run = 1;
        LSet(0);
        RSet(0);
        return true;
    }
    return false;
}

void track(float linear_velocity)
{
    static int first_run = 1;
    static uint32_t last_time = 0;
    uint32_t now = tick;
    int init = 0;
    if (first_run && now - last_time > 1000) {
        first_run = 0;
        init = 1; // ��ʼ����־
    }
    last_time = now;
    float k_w = 6.0f; // ���ٶȵı���ϵ��
    float angular_velocity = k_w * thetaGrayscale(); 
    if (angular_velocity > 100.0f) angular_velocity = 100.0f; // ���������ٶ�
    if (angular_velocity < -100.0f) angular_velocity = -100.0f; // ������С���ٶ�
    PID_Move(linear_velocity, angular_velocity, init);
    return;
}
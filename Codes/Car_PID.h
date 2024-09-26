#ifndef CAR_PID_H
#define CAR_PID_H

class PID
{
    private:
        float kp;
        float ki;
        float kd;
        float max_output;
        float min_output;
        float integral = 0.0f;
        float previous_error = 0.0f;
    
    public:
        PID(float kp, float ki, float kd, float max_output, float min_output)
        {
            this->kp = kp;
            this->ki = ki;
            this->kd = kd;
            this->max_output = max_output;
            this->min_output = min_output;
        }

        float compute(float error, float dt)
        {
            float P = error * kp;

            this->integral += error * dt;
            float I = ki * this->integral;

            float derivative = (error - previous_error) / dt;
            float D = kd * derivative;
            if (D < 0.0f) D = 0.0f;

            previous_error = error;

            float output = P + D + I;

            if (output > max_output)
                output = max_output;
            else if (output < min_output)
                output = min_output;

            if (output == max_output || output == min_output)
                this->integral -= error * dt;

            return output;
        }
};

#endif

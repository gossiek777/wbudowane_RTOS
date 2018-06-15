enum ServState {_CALLIBRATION, _IDDLE, _IN_PROGRESS, _WAITING};

struct ServoStatus{
	enum ServState eState;
	unsigned char uiPosition;
};

void ServoInit(void); 
void ServoGoTo(unsigned int);
void ServoCallib(void);
void ServoWait(unsigned int);
void ServoSpeed(unsigned int);

struct ServoStatus Servo_State(void);

enum TimeUnit {SECCONDS, MINUTES};

struct WatchEvent {
	enum TimeUnit eTimeUnit;
	char TmeValue;
};

void Watch_Init(void);
struct WatchEvent sWatch_Read(void);

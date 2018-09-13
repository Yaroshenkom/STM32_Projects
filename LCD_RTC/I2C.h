void InitialzieI2C_PB6_PB7();
void I2C1Initialize();
void I2C1LCDInitialization();
void I2C1LCDSendByte(int byte, int isdata);
void I2C1LCDSendNibble(int nibble);
void I2C1SendString(char *str);
//void I2C1LCDWaitForBusyFlagReset();
//void I2C1FixBusyFlagIssue();

void TIM2Delay(int us);
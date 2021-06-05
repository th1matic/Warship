#ifndef __PulseSensor_H
#define	__PulseSensor_H

extern int BPM;

void sendDataToProcessing(char symbol, int dat );
void SendDataSBQ(void);
void PulseSensor_CallBack(void);
#endif /* __PulseSensor_H */

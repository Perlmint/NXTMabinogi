#pragma once

#include <iostream>
#include <fantom/iNXT.h>

void sleep(clock_t ms);

class CNXTRobot
{
 public:

 protected:
  iNXT *m_lego;
};

class CNXTSensor
{
 public:
  CNXTSensor(unsigned char portNum, iNXT *lego);
  ~CNXTSensor(void);

  bool SetInputMode(unsigned char type, unsigned char mode);
  long GetValue();
  unsigned int GetRawValue();
  unsigned int GetNormalizedValue();
  int GetScaledValue();

 protected:
  iNXT *m_lego;

  unsigned char m_Port;
  unsigned char m_Type;
  unsigned char m_Mode;
};

class CNXTMotor
{
 public:
  CNXTMotor(unsigned char portNum, iNXT *lego);
  ~CNXTMotor(void);

  bool Run(int speed);
  bool Stop();

  int count();

  bool RunLimitLength(int speed, int length);

 protected:
  iNXT *m_lego;

  unsigned char m_Port;
  unsigned char m_Type;
  unsigned char m_Mode;
};

class CMabinogi
{
 public:
  CMabinogi(iNXT *lego);
  ~CMabinogi(void);

  bool play(int dt);
  bool play(string name, bool loop);

  bool stop();

  bool InsertMML(string &mml);
  int PlaySingleTone(char c[3], int i, int delay);

 protected:
  iNXT *m_lego;
  string m_Mml;

  int restWaitTime;
  int currentOctave;
  int currentTempo;
  double currentDefaultLength;
  int ptr;
};

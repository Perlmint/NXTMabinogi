#include "StdAfx.h"
#include "Mabinogi.h"

void sleep(clock_t ms)
{
  clock_t t = clock();
  while(t + ms >= clock())
  {
  }
}

inline int getIntLength(int i)
{
  int ret = 1;
  while (i >= 10)
  {
    i /= 10;
    ret++;
  }
  return ret;
}

CNXTSensor::CNXTSensor(unsigned char portNum, iNXT *lego)
{
  m_lego = lego;
  m_Port = portNum;
}

CNXTSensor::~CNXTSensor(void)
{
  m_lego = NULL;
  m_Port = 0;
}

bool CNXTSensor::SetInputMode(unsigned char type, unsigned char mode)
{
  tStatus status;
  unsigned char command[4] = {0x05, m_Port, m_Type = type, m_Mode = mode};
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  command[0] = 0x08;
  command[1] = m_Port;
  m_lego->sendDirectCommand(false, command, 2, NULL, 0, status);

  if (status.isNotFatal())
    {
      return true;
    }

  if (m_Type == 0x0A || m_Type == 0x0B)
    {
      unsigned char command[8] = {0x0F, m_Port, 0x03, 0x00, 0x02, 0x41, 0x02};
      m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

      if (status.isNotFatal())
        {
          return true;
        }
    }

  return false;
}

long CNXTSensor::GetValue()
{
  tStatus status;
  long ret = 0;
  if (m_Type == 0x0A || m_Type == 0x0B)
  {
    unsigned char command[7] = {0x0F, m_Port, 0x02, 0x01, 0x02, 0x42};
    m_lego->sendDirectCommand(false, command, 7, NULL, 0, status);

    if (status.isFatal())
    {
      return -1;
    }

    sleep(30);


    command[0] = 0x10;
    unsigned char result[19] = {0};
    m_lego->sendDirectCommand(true, command, 2, result, sizeof(result), status);

    ret = result[3];
    return ret;
  }
  else
  {
    unsigned char command[2] = {0x07, m_Port};
    unsigned char result[15] = {0};
    m_lego->sendDirectCommand(true, command, sizeof(command), result, sizeof(result), status);
    if (status.isNotFatal())
    {
      return result[12] * 256 + result[13];
    }
    else
    {
      return -1;
    }
  }

  if (status.isFatal())
  {
    return -1;
  }
}

unsigned int CNXTSensor::GetRawValue()
{
  tStatus status;
  unsigned char command[2] = {0x07, m_Port};
  unsigned char result[15] = {0x00};
  m_lego->sendDirectCommand(true, command, sizeof(command), result, sizeof(result), status);
  if (status.isNotFatal())
  {
    return result[8] * 256 + result[9];
  }

  return -1;
}

unsigned int CNXTSensor::GetNormalizedValue()
{
  tStatus status;
  unsigned char command[2] = {0x07, m_Port};
  unsigned char result[15] = {0x00};
  m_lego->sendDirectCommand(true, command, sizeof(command), result, sizeof(result), status);
  if (status.isNotFatal())
  {
    if (result[4])
    {
      return result[10] * 256 + result[11];
    }
    else
    {
      return -1;
    }
  }

  return -1;
}

int CNXTSensor::GetScaledValue()
{
  tStatus status;
  unsigned char command[2] = {0x07, m_Port};
  unsigned char result[15] = {0};
  m_lego->sendDirectCommand(true, command, sizeof(command), result, sizeof(result), status);

  return result[12] * 256 + result[13];
}

CNXTMotor::CNXTMotor(unsigned char portNum, iNXT *lego)
{
  m_lego = lego;
  m_Port = portNum;
  tStatus status;
  unsigned char command[3] = {0x0A, m_Port, 0x01};
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);
}

CNXTMotor::~CNXTMotor(void)
{
  m_lego = NULL;
}

int CNXTMotor::count()
{
  tStatus status;
  unsigned char command[2] = {0x06, m_Port};

  unsigned char result[24] = {0};

  m_lego->sendDirectCommand(true, command, sizeof(command), result, sizeof(result), status);

  if (status.isNotFatal())
  {
    return result[20] + result[17] * 0xFF + result[18] * 0xFF * 0xFF + result[19] * 0xFF * 0xFF * 0xFF;
  }

  return -1;
}

bool CNXTMotor::Run(int speed)
{
  tStatus status;
  unsigned char command[13] = {0x04, m_Port, speed, 0x01, 0x02, speed, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};

  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  if (status.isNotFatal())
  {
    return true;
  }

  command[0] = 0x0A;
  command[3] = 0x00;

  m_lego->sendDirectCommand(false, command, 3, NULL, 0, status);

  return false;
}

bool CNXTMotor::RunLimitLength(int speed, int length)
{
  tStatus status;
  unsigned char command[13] = {0x04, m_Port, speed, 0x01, 0x01, speed, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00};

  int i = 7;
  while(length > 0 && i < 11)
  {
    command[i] = length % 0xFF;
    length = length / 0xFF;
    i++;
  }

  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  if (status.isNotFatal())
  {
    return true;
  }

  return false;
}

bool CNXTMotor::Stop()
{
  tStatus status;
  unsigned char command[13] = {0x04, m_Port, 0x00, 0x02, 0x00, 0x00, 0x20, 0x00};
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  if (status.isNotFatal())
  {
    return true;
  }

  return false;
}

CMabinogi::CMabinogi(iNXT *lego)
{
  restWaitTime = 0;
  m_lego = lego;
  currentOctave = 4;
  currentTempo = 120;
  currentDefaultLength = 4.0;
  ptr = 0;
}

CMabinogi::~CMabinogi(void)
{
  m_lego = NULL;
}

bool CMabinogi::play(int dt)
{
  int nOctave;
  double i;
  char c, nt[3];
  if((restWaitTime -= dt) <= 0)
  {
    sscanf_s((m_Mml.c_str()) + ptr, "%c", &c);
    if (c >= 'a')
    {
      c -= ' ';
    }
    switch(c)
    {
      case ' ':
      case '&':
        ptr++;
        break;
      case 'T':
        ptr++;
        sscanf_s((m_Mml.c_str()) + ptr, "%d", &currentTempo);
        ptr += getIntLength(currentTempo);
        break;
      case 'V':
        ptr++;
        sscanf_s((m_Mml.c_str()) + ptr, "%d", &nOctave);
        ptr += getIntLength(nOctave);
        break;
      case 'O':
        ptr++;
        sscanf_s((m_Mml.c_str()) + ptr, "%d", &currentOctave);
        ptr += getIntLength(currentOctave);
        break;
      case 'L':
        ptr++;
        sscanf_s((m_Mml.c_str()) + ptr, "%lf", &currentDefaultLength);
        ptr += getIntLength((int)currentDefaultLength);
        if(m_Mml[ptr] == '.')
        {
          ptr++;
          currentDefaultLength /= 1.5;
        }
        break;
      case '>':
        ptr++;
        currentOctave++;
        break;
      case '<':
        ptr++;
        currentOctave--;
        break;
      case 'R':
        ptr++;
        if (m_Mml[ptr] >= '1' && m_Mml[ptr] <= '9')
        {
          sscanf_s((m_Mml.c_str()) + ptr, "%d", &nOctave);
          ptr+= getIntLength(nOctave);
          i = nOctave;
        }
        else i = currentDefaultLength;
        if (m_Mml[ptr] == _T('.'))
        {
          i /= 1.5; ptr++;
        }
        restWaitTime = int(240000.0 / (i * currentTempo));
        break;
      case 'N':
        ptr++;
        sscanf_s((m_Mml.c_str()) + ptr, "%d", &nOctave);
        ptr += getIntLength((int)nOctave);
        nOctave = 0;
        while (i >= 12)
        {
          i -= 12; nOctave++;
        }
        i = nOctave * 100;
        PlaySingleTone("\0", (int)i, int(210000.0 / (currentDefaultLength * currentTempo)));
        restWaitTime = int(240000.0 / (currentDefaultLength * currentTempo));
        break;
      default:
        nt[0] = c; nt[1] = 0;
        ptr++;
        if (m_Mml[ptr] == '-' || m_Mml[ptr] == '#' || m_Mml[ptr] == '+')
        {
          nt[1] = m_Mml[ptr];
          nt[2] = 0;
          ptr++;
        }
        if (m_Mml[ptr] >= '1' && m_Mml[ptr] <= '9')
        {
          sscanf_s((m_Mml.c_str()) + ptr, "%d", &nOctave);
          ptr+= getIntLength(nOctave);
          i = nOctave;
        }
        else i = currentDefaultLength;
        if (m_Mml[ptr] == '.')
        {
          i /= 1.5; ptr++;
        }

        PlaySingleTone(nt, currentOctave, int(210000.0 / (i * currentTempo)));
        restWaitTime = int(240000.0 / (i * currentTempo));
        break;
      }
    return true;
  }
  else
  {
    return true;
  }
}

bool CMabinogi::InsertMML(string &mml)
{
  if(mml.length() > 1)
    {
      m_Mml = mml;
      restWaitTime = 0;
      currentOctave = 4;
      currentTempo = 120;
      currentDefaultLength = 4.0;
      ptr = 0;
      return true;
    }
  return false;
}

int CMabinogi::PlaySingleTone(char c[3], int i, int delay)
{
  const double HzData[13] = {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25};
  const int mappingData[7] = {0,2,4,5,7,9,11};
  int key;
  if(c[0] == '\0')
  {
    key = i % 100;
    i /= 100;
  }
  else
  {
    key = (c[0] < 'C') ? (mappingData[c[0] - 'A' + 5]) : (mappingData[c[0] - 'C']);
    if (c[1] == '#' || c[1] == '+') key++;
    else if (c[1] == '-') key--;
    if (key < 0)
    {
      key += 12;
      i--;
    }
    else if (key > 13)
    {
      key -= 12;
      i++;
    }
  }
  i -= 4;
  double ret = HzData[key];
  if (i > 0)
  {
    for (int j = 0; j < i; j++)
    {
      ret *= 2.0;
    }
  }

  if (i < 0)
  {
    for (int j = 0; j < i; j++)
    {
      ret /= 2.0;
    }
  }

  int ret2 = (ret - int(ret) >= 0.5) ? (int(ret) + 1) : (int(ret) - 1);

  tStatus status;
  unsigned char command[5] = {0x03, ret2 % 256, ret2 / 256, delay % 256, delay / 256};
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  if (status.isNotFatal())
  {
    return ret2;
  }

  return -2;
}

bool CMabinogi::play(string name, bool loop)
{
  tStatus status;
  unsigned char command[22] = {0x02, loop?0x01:0x00, 0x00};
  for (int i = name.length(); i > 0; i--)
  {
    command[name.length() - i + 2] = name[i];
  }
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  return status.isNotFatal();
}

bool CMabinogi::stop()
{
  tStatus status;
  unsigned char command[1] = {0x0C};
  m_lego->sendDirectCommand(false, command, sizeof(command), NULL, 0, status);

  return status.isNotFatal();
}

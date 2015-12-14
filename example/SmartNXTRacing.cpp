// SmartNXTRacing.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <Windows.h>

void gotoxy(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int _tmain(int argc, _TCHAR* argv[])
{
    tStatus status;
    iNXTIterator *iterator = iNXT::createNXTIterator(true, 3, status);
    iNXT *nxtPtr;

    bool flag = true;

    if (status.isNotFatal())
    {
	   char *name = new char[15];
	   iterator->getName(name, status);
	   while(1)
	   {
		 
		  iterator->getName(name, status);
		  cout << name;
		  int i = 0;
		  cin >> i;
		  if (i == 1)
		  {
			  nxtPtr = iterator->getNXT(status);
			  break;
		  }
		  iterator->advance(status);
	   }
	  if (status.isFatal())
		 return -1;
	  iNXT::destroyNXTIterator(iterator);
    }
    else
    {
	   return -1;
    }

    CNXTMotor motorB(1, nxtPtr);
    CNXTMotor motorC(2, nxtPtr);

    CNXTSensor sona(0, nxtPtr);
    CNXTSensor light(1, nxtPtr);

    motorB.Run(85);
    motorC.Run(85);

    CMabinogi sound(nxtPtr);

    sound.InsertMML(string("T200V15L8RGGGE-1RFFFD1RGGGRA-A-A->RE-E-E-C2R<GGGRA-A-A->RFFFD2RGGFE-2DGGFE-2DGGFE-4R4C4R4G1<RA-A-A-F1RA-A-A-F1&F2E-A-A-A-F1&F2E-G>CCC2<BB>DDD2CCCE-E-DDFFEEGGFFA-A-GGB-B-A-A->CC<BB>DC16R16E-E-E-C<GGGE-C<GGE-CCC<B>>FDD<BGFFD<BGFD<B>CCC>>E-E-E-C<AAAG-E-E-E-C<AAAA2"));

    sona.SetInputMode(0x0B, 0x00);
    light.SetInputMode(0x05, 0x00);

    system("cls");

    int stat = 0;
    flag = true;

    clock_t last = clock();

    int sonaData = 0;
    int lightData = 0;
    int last2 = 800;
    while(flag)
    {
	   sound.play(clock() - last);
	   last = clock();
	   sonaData = sona.GetValue();
	   lightData = light.GetValue();

	   gotoxy(0, 0);

	   cout << "Light : " << lightData << endl;
	   cout << "Sona : " << sonaData << endl;
	   cout << "Stat : " << stat << endl;
	   cout << "B : " << motorB.count() << endl;
	   cout << "C : " << motorC.count() << endl;

	   switch(stat)
	   {
	   case 0:
		 if (motorB.count() > last2 || lightData < 205)
		 {
			 motorB.Stop();
			 motorC.Stop();
			 motorB.Run(100);
			 motorC.Run(100);
			 if(lightData > 205)
				sleep(200);
			 stat++;
			 last2 = motorB.count() + 1600;
		  }
		  break;
	   case 1:
		  if (motorB.count() > last2 || lightData < 205)
		  {
			 motorB.Stop();
			 motorC.Stop();
			 motorB.Run(100);
			 motorC.Run(86);
			 if(lightData > 205)
				sleep(200);
			 stat++;
			 last2 = motorB.count() + 2400;
		  }
		  break; 
	   case 2:
		  if (motorB.count() > last2 || lightData < 205)
		  {
			 motorB.Stop();
			 motorC.Stop();
			 motorB.Run(85);
			 motorC.Run(85);
			 if(lightData > 205)
				sleep(200);
			 stat++;
		  }
		  break;
	   case 3:
		  if (sonaData < 20)
		  {
			 stat ++;
		  }
		  break;
	   default:
		  flag = false;
	   }
    }
    sona.SetInputMode(0x00, 0x00);
    light.SetInputMode(0x00, 0x00);

    motorB.Stop();
    motorC.Stop();

    iNXT::destroyNXT(nxtPtr);

    return 0;
}


#include "../inc/UH_HAL.h"
#include "Sys/Sys.h"
void HAL_Delay(unsigned int Delay)
{
	zmSys->mDelay(Delay);
}
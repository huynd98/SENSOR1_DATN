#include "Turbidity.h"
extern float Turbidity;
void ReadValueTurbidity( void )
{
		Turbidity=(HAL_ADC_GetValue(&hadc2)/4096.0*3.3-2.9652)/-0.0008;
		if (Turbidity<0) Turbidity=0;
}
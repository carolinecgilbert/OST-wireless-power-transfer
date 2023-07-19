/*
 * my_DAC.c
 *
 *  Created on: 21.12.2020
 *      Author: Christoph Fehr
 */
#include "my_DAC.h"


void INIT_my_DAC(void){
        //Set VDAC as the DAC reference voltage.
        DAC_setReferenceVoltage(DACA_BASE, DAC_REF_ADC_VREFHI);     // Internal-Ref = 1.65V
        DAC_setReferenceVoltage(DACB_BASE, DAC_REF_ADC_VREFHI);     // Internal-Ref = 1.65V

        // Gain for 3.3V at the output
        DAC_setGainMode(DACA_BASE, DAC_GAIN_TWO);   // Ref*2 = 3.3V
        DAC_setGainMode(DACB_BASE, DAC_GAIN_TWO);   // Ref*2 = 3.3V

        // Defines the LoadMode --> as fast as possible
        DAC_setLoadMode(DACA_BASE, DAC_LOAD_SYSCLK);
        DAC_setLoadMode(DACB_BASE, DAC_LOAD_SYSCLK);


        //Set the DAC shadow output to 0
        DAC_setShadowValue(DACA_BASE, 0);   // Output Value will be one SYSCLK late
        DAC_setShadowValue(DACB_BASE, 0);   // Output Value will be one SYSCLK late

        // Enable the DAC output
        DAC_enableOutput(DACA_BASE);
        DAC_enableOutput(DACB_BASE);
}


void my_DAC_change_Value(uint32_t base, float voltage){ // set the DAC Value back calculated from the Voltage
    float DAC_Value = (voltage * 4096.0 / 3.3);
    if (4095 < DAC_Value){
        DAC_Value = 4095.0;
    }
    DAC_setShadowValue(base,(uint32_t)DAC_Value);
}


void my_DAC_set_Value(uint32_t base, uint32_t value){   // set the DAC Value directly
    uint32_t check_value = value;
    if (4095 < check_value){
        check_value = 4095.0;
    }
    DAC_setShadowValue(base,check_value);
}

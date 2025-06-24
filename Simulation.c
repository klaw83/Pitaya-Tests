#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

int main (int argc, char **argv) {
    int unsigned led;

    float excitation_duration_microseconds = 41.027;
    float excitation_amplitude_Volts = 0.19;
    float Larmor_frequency_Hertz = 24378040.422;
    if (argc > 1) {
        led = atoi(argv[1]);
    }

    printf("Blinking LED[0]\n");
    printf("Emulation started\n");

    led = RP_LED0;

    // Initialization of API
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }
    if (rp_AcqReset() != RP_OK) {
        fprintf(stderr, "rp_AcqReset failed!\n");
        return -1;
    }
    if (rp_AcqStart() != RP_OK) {
        fprintf(stderr, "rp_AcqStart failed!\n");
        return -1;
    }

    rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
    rp_AcqSetTriggerLevel(RP_T_CH_1,excitation_amplitude_Volts);
    rp_acq_trig_state_t state = RP_TRIG_STATE_WAITING;
    
    //Activation du port out1
    rp_GenReset();

    rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
    rp_GenFreq(RP_CH_1, 1000);
    rp_GenAmp(RP_CH_1, 2.0);

    rp_GenMode(RP_CH_1, RP_GEN_MODE_BURST);
    rp_GenBurstCount(RP_CH_1, Larmor_frequency_Hertz);
    rp_GenBurstRepetitions(RP_CH_1, 1000);
    rp_GenBurstPeriod(RP_CH_1, 1);
    
    //Attente
    while (1){
        rp_AcqGetTriggerState(&state);
        if(state == RP_TRIG_STATE_TRIGGERED){
            usleep(excitation_duration_microseconds);
            rp_GenOutEnable(RP_CH_1);
            rp_GenTriggerOnly(RP_CH_1); //déclenchement out1 NOW
            break;
        }

        
        //led indiquant que la simulation tourne
        rp_DpinSetState(led+1, RP_HIGH);
        usleep(1);
    }

    

    rp_DpinSetState(led, RP_LOW);
    rp_DpinSetState(led+1, RP_LOW);
    // Releasing resources
    rp_Release();

    return EXIT_SUCCESS;
}

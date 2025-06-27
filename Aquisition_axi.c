/* Red Pitaya C API example Acquiring a signal from a buffer
 * This application acquires a signal on a specific channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rp.h"

#define DATA_SIZE 524288
#define SAMPLE_RATE 125000000

int main(int argc, char **argv)
{
    int dsize = DATA_SIZE;
    uint32_t dec =1; 
    uint32_t size1 = dsize;
    float *buff1 = (float *)malloc(dsize * sizeof(float));
    uint32_t posChA;
    bool fillState = false;
    
    float excitation_duration_seconds = 35.027e-06; //41.027e-06
    float excitation_duration_microseconds = excitation_duration_seconds*1000000;
    float excitation_amplitude_Volts = 0.5;
    float Larmor_frequency_Hertz = 24378040.422;
    int excitation_burst_cycles_tot = Larmor_frequency_Hertz *excitation_duration_seconds;
    int number_of_files = 1;

    char* nomFichier = "data";
    if (argc >= 4){
        dsize = atoi(argv[1]);
        dec = atoi(argv[2]);
        //nomFichier = argv[3]; //à modifier pour être sur qu'on met bien le nom d'un fichier .csv"
        number_of_files = atoi(argv[3]);
    }

    if (rp_InitReset(false) != RP_OK) {
            fprintf(stderr, "Rp api init failed!\n");
            return -1;
        }

    
    int i=0;
    char numFichier[256];
    for (i=0;i<=number_of_files;i++){
        
        sprintf(numFichier,"_%d.csv",i);
        strcat(nomFichier, numFichier);
        puts(nomFichier);
        /* Creation du ficher pour recolter les resultats*/
        FILE *fichier = fopen(nomFichier, "w");
        //printf("fichier crée\n");

        if (fichier == NULL) {
            perror("Erreur lors de l'ouverture du fichier\n");
            return EXIT_FAILURE; // Quitter le programme avec un code d'erreur
        }


        //Initialisation
        rp_AcqResetFpga();
        uint32_t g_adc_axi_start,g_adc_axi_size;
        rp_AcqAxiGetMemoryRegion(&g_adc_axi_start,&g_adc_axi_size);
        //printf("Reserved memory start 0x%X size 0x%X bytes\n",g_adc_axi_start,g_adc_axi_size);
        
        //INITIALISATION AQUISITION 
        if(rp_AcqReset()!=RP_OK){
            fprintf(stderr, "rp_AcqReset failed!\n");
            return -1;
        }
        if (rp_AcqAxiSetDecimationFactor(dec) != RP_OK) {
            fprintf(stderr, "rp_AcqAxiSetDecimationFactor failed!\n");
            return -1;
        }
        if (rp_AcqAxiSetTriggerDelay(RP_CH_1, dsize)  != RP_OK) { 
            fprintf(stderr, "rp_AcqAxiSetTriggerDelay RP_CH_1 failed!\n");
            return -1;
        }
        if (rp_AcqAxiSetBufferSamples(RP_CH_1,g_adc_axi_start, dsize) != RP_OK) {
            fprintf(stderr, "rp_AcqAxiSetBuffer RP_CH_1 failed!\n");
            return -1;
        }
        if (rp_AcqAxiEnable(RP_CH_1, true)) {
            fprintf(stderr, "rp_AcqAxiEnable RP_CH_1 failed!\n");
            return -1;
        }

        
    //INITIALISATION GENERATION
        if(rp_GenReset() != RP_OK){
            fprintf(stderr, "rp_GenReset failed!\n");
            return -1;
        }
        if(rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE) != RP_OK){
            fprintf(stderr, "rp_GenWaveform RP_CH_1 SINE failed!\n");
            return -1;
        }
        if(rp_GenFreq(RP_CH_1, Larmor_frequency_Hertz) != RP_OK){
            fprintf(stderr, "rp_GenFreq RP_CH_1 failed!\n");
            return -1;
        }
        if(rp_GenAmp(RP_CH_1, excitation_amplitude_Volts) != RP_OK){
            fprintf(stderr, "rp_GenAmp RP_CH_1 failed!\n");
            return -1;
        }
        if(rp_GenMode(RP_CH_1, RP_GEN_MODE_BURST) != RP_OK){
            fprintf(stderr, "rp_GenMode RP_CH_1 BURST failed!\n");
            return -1;
        }
        
        
        if(rp_GenBurstCount(RP_CH_1, excitation_burst_cycles_tot) != RP_OK){
            fprintf(stderr, "rp_GenBurstCount RP_CH_1 failed!\n");
            return -1;
        }
        //valeur max pour GenBurstCount = 50 000
        if(rp_GenBurstRepetitions(RP_CH_1, 1) != RP_OK){
            fprintf(stderr, "rp_GenBurstRepetitions RP_CH_1 failed!\n");
            return -1;
        }//Répété 1 fois pour que le burst dure qq usecondes
        
        if(rp_GenBurstPeriod(RP_CH_1, 1) != RP_OK){
            fprintf(stderr, "rp_GenBurstPeriod RP_CH_1 failed!\n");
            return -1;
        }//une micro seconde entre chaque répétition
        
        if( rp_GenOutEnable(RP_CH_1) != RP_OK){
            fprintf(stderr, "rp_GenOutEnable RP_CH_1 failed!\n");
            return -1;
        }
        if( rp_GenTriggerOnly(RP_CH_1) != RP_OK){
            fprintf(stderr, "rp_GenTriggerOnly RP_CH_1 failed!\n");
            return -1;
        }
        
        //LANCEMENT DE L'AQUISITION
        if (rp_AcqStart() != RP_OK) {
        fprintf(stderr, "rp_AcqStart failed!\n");
        return -1;
        }
        usleep(excitation_duration_microseconds);
        if( rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW) != RP_OK){
            fprintf(stderr, "rp_AcqSetTriggerSrc RP_TRIG_SRC_NOW failed!\n");
            return -1;
        }

        rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
        while(1){
            rp_AcqGetTriggerState(&state);
            if(state == RP_TRIG_STATE_TRIGGERED){
                sleep(1);
                break;
            }
        }
    
        
        printf ("wait to be filled\n");
        while (!fillState) {
            if (rp_AcqAxiGetBufferFillState(RP_CH_1, &fillState) != RP_OK) {
                fprintf(stderr, "rp_AcqAxiGetBufferFillState RP_CH_1 failed!\n");
                return -1;
            }
        }
        
        if(  rp_AcqStop() != RP_OK){
                fprintf(stderr, "rp_AcqStop failed!\n");
                return -1;
            }
        
        rp_AcqAxiGetWritePointerAtTrig(RP_CH_1,&posChA);
        fprintf(stderr,"Tr pos1: 0x%X\n",posChA);
        
        if(rp_AcqAxiGetDataV(RP_CH_1, 0, &size1, buff1)!=RP_OK){
            fprintf(stderr, "rp_AcqAxiGetDataV failed\n");
        }
        
    
        printf("ecriture dans %s\n",nomFichier);
        for (int i = 0; i < dsize; i++) {
            // printf("[%d]\t%f\n",i,buff1[i]);
            fprintf(fichier, "%f", buff1[i]);
            if (i!= dsize -1) fprintf(fichier, ",");
        }
        fprintf(fichier, "\n");

        for (int i = 0; i < dsize; i++) {
            float time = (float)i/(float)SAMPLE_RATE*dec;
        fprintf(fichier, "%f", time);
            if (i!= dsize -1) fprintf(fichier, ",");
        }

        fprintf(fichier, "\n");
        fclose(fichier);
        sleep(10);

}

    /* Releasing resources */
    rp_AcqAxiEnable(RP_CH_1, false);
    rp_Release();
    free(buff1);
    return 0;
}

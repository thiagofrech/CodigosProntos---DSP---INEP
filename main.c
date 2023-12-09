//********************* DUVIDAS ************************
// PARA ALTERAR OS VALORES DE (FREQ)(DUTYCYCLE) TENHO QUE FAZER A ALTERAÇAO COM O VETOR VOLTAGEX NO REGISTRADOR, E SEM USAR A PROTECAO EALLOW/EDIS? ONDE COLOCARIA ISSO?
//



// ----- ARQUIVOS DE BIBLIOTECA E DEFINE -----------

// todos da pasta (common > incluce)
// todos da pasta (headers > include)
// pasta (headers > cmd) Headers_nonBIOS
// pasta (headers > source) GlobalVariableDefs

// ----- ARQUIVOS PARA ADC -------------------------
//

// ----- ARQUIVOS PARA PWM -------------------------

// RAM_ink.cmd
// Adc.c
// Defaultsr.c
// EPwm.c
// Gpio.c
// PieCtrl.c
// PieVect.c
// SysCtrl.c
// Usb.h
// usbWrapper.c
// usbWrapper.h
// usDelay.asm

// ----------------------------------------------

#include "DSP28x_Project.h"                         // Arquivo de cabecalho do dispositivo e exemplos incluem arquivo
                                                    //
__interrupt void adc_isr(void);                     // declaracao da funcao de interrupcao
void Adc_Config(void);                              // declaracao de funcao
void freq (float);                                 //
void dutty (float);                                //
                                                    //
float LoopCount = 0;                               // (inteiro sem sinal de 16 bits) feito com typedef -- ??????????????????????
Uint16 ConversionCount = 0;                         // (inteiro sem sinal de 16 bits) feito com typedef -- ??????????????????????
float Voltage1;                                    // (inteiro sem sinal de 16 bits) feito com typedef -- buffer com 10 posicoes
float Voltage2;                                    // (inteiro sem sinal de 16 bits) feito com typedef -- buffer com 10 posicoes
float bufffer[100];                                // (inteiro sem sinal de 16 bits) feito com typedef -- buffer com 100 posicoes para guardar valores do ADC
Uint16 flag=0;                                      // variavel de controle para tratar cada interrupcao
float freq_PWM=0;
float duty_PWM=0;                                             //

int main(void)                                      //
{                                                   //
                                                    //
    InitSysCtrl();                                  //  inicializa o sistema (clk, ...)
    InitEPwm1Gpio();                                //  inicializa o PWM
    DINT;                                           //  desabilita interrupcoes
    InitPieCtrl();                                  //  inicializa o tabela de interrupcoes (acessa pievect)
    IER = 0x0000;                                   //  desabilita interrupcoes da CPU
    IFR = 0x0000;                                   //  limpa as flags de interrupcao
    InitPieVectTable();                             //  trabalha junto com initpiectrl  (configura priorirdades de interrupcoes)
                                                    //
    EALLOW;                                         //
    PieVectTable.ADCINT1 = &adc_isr;                // interrupcao associada ao endereço da função adc_isr
    EDIS;                                           //
                                                    //
    InitAdc();                                      // inicializacao o ADC
    AdcOffsetSelfCal();                             // ??????????????????????
                                                    //
                                                    // Habilita ADCINT1 no PIE
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;              // Habilita INT 1.1 no PIE
    IER |= M_INT1;                                  // Habilita interrupcao de CPU 1
    EINT;                                           // Habilita interrupcao global INTM
    ERTM;                                           // Habilita DBGM de interrupcao global em tempo real
                                                    //
                                                    //
    EALLOW;                                         //
    EPwm1Regs.TBPRD = 3000;                         // 30K Hz                                                                           //  ciclos de clock ate o pico da onda portadora , freq = freq_clk / n_ciclos (freq_clk = 90MHz)
    EPwm1Regs.CMPA.half.CMPA = 1500;                // Duty cycle de 50% (3000/2)                                                       //  configura o valor de comparacao com canal A
    EPwm1Regs.CMPB = 0;                             // Não utilizado neste exemplo                                                      //  configura o valor de comparacao com canal B
    EPwm1Regs.TBCTR = 0;                            //                                                                                  //  incrementa ate alcançar o TBPRD
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;      // Modo de contagem: Up-Down mode                                                   //  contagem ascendente, descendente ou ascendente-descendente
    EPwm1Regs.TBCTL.bit.PHSEN = 0;                  //                                                                                  //  sincronização de fase, permitindo que o sinal PWM seja sincronizado com outro sinal
    EPwm1Regs.TBCTL.bit.PRDLD = 1;                  // Carregar o valor do período imediatamente // fazer apresentação para explicação  //  Determina quando o valor do período (PRD) deve ser carregado. Se for 1, o carregamento é imediato
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 0;               // Sem sincronização                                                                //  habilita ou desabilita a sincronização do PHSEN
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Divisor de clock em alta velocidade       // fazer apresentação para explicação  //  Define o divisor de clock para o timer do PWM em alta velocidade
    EPwm1Regs.TBCTL.bit.CLKDIV = 0;                 // Divisor de clock                          // pesquisar dps                       //  Define o divisor de clock para o timer do PWM
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Modo de sombra: Atualização direta                                               //  guarda a informação do CMPA para comparar com TBCTR
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Modo de sombra: Atualização direta                                               //  Configura o modo de sombra para o registrador CMPB
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   // Carregar CMPA imediatamente                                                      //  Configura quando o valor de comparação para o canal A é carregado. imediato ou no próximo ciclo
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   // Não utilizado neste exemplo                                                      //  Configura quando o valor de comparação para o canal B é carregado
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;              // Set no evento de contagem zerada                                                 //  Configura a ação que ocorre quando o contador do timer atinge zero para o canal A
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // Clear no evento de contagem igual a CMPA                                         //  Configura a ação que ocorre quando o contador do timer atinge o valor de comparação para o canal A
    EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;              //                                                                                  //  Configura a ação que ocorre quando o contador do timer atinge zero para o canal B
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;            //                                                                                  //  Configura a ação que ocorre quando o contador do timer atinge o valor de comparação para o canal B
    EPwm1Regs.ETSEL.bit.SOCAEN   = 1;        // Habilitar SOC em um grupo
    EPwm1Regs.ETSEL.bit.SOCASEL  = 4;        // Selecione SOC do CMPA na contagem crescente
    EPwm1Regs.ETPS.bit.SOCAPRD   = 1;        // Gerar pulso no 1Âº evento
    EDIS;                                           //
                                                    //
    EALLOW;                                         //
        SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      // inicia todos os timers sincronizados
    EDIS;                                           //
                                                    //
                                                    //
    EALLOW;                                         //
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;          // Ativar modo de sobreposicao                              (1 sem sobreposicao , 0 com sobreposicao)
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;            // interrupcao gerada pelo ADC                              (-- no incio da conversao , -- no final da conversao) ??????????????????????
    AdcRegs.INTSEL1N2.bit.INT1E     = 1;            // interrupcao do ADCINT1                                   (1 habilita interrupcao , 0 desabilita interrupcao)
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;            // interrupcao do ADCINT1 continua ou uma vez               (1 continuo , 0 uma vez)
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 1;            // qual conversor inicia a interrupcao do ADCINT1           (1 EOC1 , 0 --) ??????????????????????
    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 4;            // canal SOC0 analogico sera convertido para qual ADCIN     (4 ADCINA4)
    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 2;            // canal SOC1 analogico sera convertido para qual ADCIN     (2 ADCINA2)
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;            // fonte de disparo(trigger) do SOC0 no EPWM1A              (devido ao round-robin, o SOC0 converte primeiro e depois o SOC1)
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;            // fonte de disparo(trigger) do SOC1 no EPWM1A              (devido ao round-robin, o SOC0 converte primeiro e depois o SOC1)
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;            // define a janela SOC0 S/H para 7 ciclos de relogio ADC    (6 ACQPS mais 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;            // define a janela SOC1 S/H para 7 ciclos de relogio ADC    (6 ACQPS mais 1)
    EDIS;

                                                            //
    while(1)                                                // loop infinito para enviar ao led o sinal de ADC
    {                                                       //

        if (flag == 1)                                      // se ocorrer a interrupcao
        {                                                   //
            flag = 0;                                       // reseta a variavel de controle (que indica se ocorreu interrupcao)

            freq(Voltage1);
            dutty(Voltage2);

            EPwm1Regs.TBPRD = freq_PWM;
            EPwm1Regs.CMPA.half.CMPA = duty_PWM;
            //EPwm1Regs.TBPRD = Voltage1;                     // registrador destinado a frequecia do PWM recebe um valor qualquer do ADC
            //EPwm1Regs.CMPA.half.CMPA = Voltage2;            // regsitrador destinado a DutyCycle do PWM recebe um valor qualquer do ADC

        }                                                   //
    }                                                       //
}                                                           // end da main
                                                            //
                                                            //
__interrupt void  adc_isr(void)                             // função usada apos a interrupcao do ADC
{                                                           //
                                                            //
   Voltage1 = AdcResult.ADCRESULT0;                         // variavel recebe o valor da saida do conversor ADC
   Voltage2 = AdcResult.ADCRESULT1;                         // variavel recebe o valor da saida do conversor ADC
   bufffer[ConversionCount] = AdcResult.ADCRESULT0;         // o valor de saida do ADC0 sera guardado em uma posicao do vetor
                                                            //
                                                            //
   if(ConversionCount == 100)                               // parte do codigo que incrementa a variavel que indica a posicao no vetor
   {                                                        //
      ConversionCount = 0;                                  //
   }                                                        //
   else ConversionCount++;                                  //
                                                            //
   AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;                    // Limpar a flag ADCINT1 e reinicializar para o próximo SOC
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;                  // Reconhecer interrupção para PIE
   flag = 1;                                                // indicador de que ocorreu interupcao
   return;
}


void freq (float Voltage1)                                 // freq = freq_clk / n_ciclos (freq_clk = 90MHz)
{                                                           // (0V = 8Hz)       (3.3V = 30Hz)
    float aux1 = 0;                                        // variavel auxiiar que guarda o valor da frequencia desejada
    aux1 = 8 + Voltage1 * 22/4096 ;                         // frequencia desejada
    freq_PWM = 90000000/aux1;                                // calculo do valor a ser colocado no TBPRD

}

void dutty (float Voltage2)                                // DutyCycle = freq * (% do dutycycle)
{                                                           //
    float aux2 = 0;                                        // variavel auxiiar que guarda o valor da porcentagem do dutycycle
    aux2 =  Voltage2 * 1/4096 ;                            //
    duty_PWM = 1 + freq_PWM * aux2;                             //

}

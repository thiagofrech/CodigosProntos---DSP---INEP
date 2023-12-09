
#include "DSP28x_Project.h"                     //
                                                //
                                                //
                                                //
int main(void)                                  //
{                                               //
                                                //
InitSysCtrl();                                  //  inicializa o sistema (clk, ...)
InitEPwm1Gpio();                                //  inicializa o PWM
DINT;                                           //  desabilita interrupcoes
InitPieCtrl();                                  //  inicializa o tabela de interrupcoes (acessa pievect)
IER = 0x0000;                                   //  desabilita interrupcoes da CPU
IFR = 0x0000;                                   //  limpa as flags de interrupcao
InitPieVectTable();                             //  trabalha junto com initpiectrl  (configura priorirdades de interrupcoes)
                                                //
                                                //
EALLOW;                                         //
                                                //
EPwm1Regs.TBPRD = 3000;                         // 30K Hz                                                                           //  ciclos de clock ate o pico da onda portadora , freq = freq_clk / n_ciclos
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
                                                //
EDIS;                                           //
                                                //
EALLOW;                                         //
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      // Start all the timers synced
EDIS;                                           //
                                                //
    while (1)                                   //
    {                                           //
                                                // Inverter o estado do LED a cada segundo (aproximadamente)


    }


}

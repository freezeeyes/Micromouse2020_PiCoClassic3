/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#include "iodefine.h"

#define IO_OUTPUT 1
#define IO_INPUT 0
#define MMPP (0.377)

volatile float v;   //  速度
volatile float a;   //  加速度
volatile unsigned int step_l;   //  左モータのパルスカウンター
volatile unsigned int step_r;   //  右モータのパルスカウンター
volatile short r_sen;           //  右センサの反射光値
volatile short l_sen;           //  左センサの反射光値
volatile short fr_sen;          //  右前センサの反射光値
volatile short fl_sen;          //  左前センサの反射光値

void init_clock(void);
void init_motor(void);
void init_sensor(void);
void init_led(void);
void init_buzzer(void);
void init_cmt0(void);
void init_cmt1(void);
void int_cmt0(void);
void int_cmt1(void);
void set_led(unsigned char data);
void buzzer_on(void);
void buzzer_off(void);
void int_motor_r(void);
void int_motor_l(void);
void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif


void main(void)
{
  init_clock();

}


/*
 * Clockの初期設定
 */
void init_clock(void)
{
  SYSTEM.PRCR.WORD = 0xA50B;
  SYSTEM.PLLCR.WORD = 0x0F00;
  SYSTEM.PLLCR2.BYTE = 0x00;
  SYSTEM.SCKCR.LONG = 0x21C21211;
  SYSTEM.SCKCR2.WORD = 0x0032;
  SYSTEM.BCKCR.BYTE = 0x01;
  SYSTEM.SCKCR3.WORD = 0x0400;
}


/*
 * Motorの初期設定
 */
void init_motor(void)
{
  //  I/Oの設定
  PORT1.PODR.BIT.B5 = 0;
  PORT1.PDR.BIT.B5 = 1;
  PORTC.PDR.BIT.B5 = 1;
  PORTC.PDR.BIT.B6 = 1;
  MPC.PWPR.BIT.B0WI = 0;
  MPC.PWPR.BIT.PFSWE = 1;
  MPC.P17PFS.BIT.PSEL = 1;
  MPC.PE2PFS.BIT.PSEL = 1;
  MPC.PWPR.BYTE = 0x80;
  PORT1.PMR.BIT.B7 = 1;
  PORTE.PMR.BIT.B2 = 1;
  
  //  MTUの設定
  SYSTEM.PRCR.WORD = 0xA502;
  MSTP(MTU) = 0;
  SYSTEM.PRCR.WORD = 0xA500;
  MTU.TSTR.BYTE = 0;
  
  //  右モータの設定（MTU3）
  MTU3.TCR.BIT.CCLR = 1;
  MTU3.TCR.BIT.TPSC = 2;
  MTU3.TMDR.BIT.MD = 2;
  MTU3.TIORH.BIT.IOA = 2;
  MTU3.TIORH.BIT.IOB = 1;
  MTU3.TGRA = 6297;
  MTU3.TGRB = 50;
  MTU3.TGRC = 6297;
  MTU3.TMDR.BIT.BFA = 1;
  MTU3.TIER.BIT.TGIEB = 1;
  IEN(MTU3,TGIB3) = 1;
  IPR(MTU3,TGIB3) = 13;
  IR(MTU3,TGIB3) = 0;
  
  //  左モータの設定（MTU4）
  MTU.TOER.BIT.OE4A = 1;
  MTU4.TCR.BIT.CCLR = 1;
  MTU4.TCR.BIT.TPSC = 2;
  MTU4.TMDR.BIT.MD = 2;
  MTU4.TIORH.BIT.IOA = 2;
  MTU4.TIORH.BIT.IOB = 1;
  MTU4.TGRA = 6297;
  MTU4.TGRB = 50;
  MTU4.TGRC = 6297;
  MTU4.TMDR.BIT.BFA = 1;
  MTU4.TIER.BIT.TGIEB = 1;
  IEN(MTU4,TGIB4) = 1;
  IPR(MTU4,TGIB4) = 12;
  IR(MTU4,TGIB4) = 0;
  
  MTU3.TCNT = 0;
  MTU4.TCNT = 0;
}


/*
 * Sensorの初期設定
 */
void init_sensor(void)
{
  //  I/Oの設定
  PORT5.PDR.BIT.B4 = IO_OUTPUT;
  PORT0.PDR.BIT.B5 = IO_OUTPUT;
  PORT2.PDR.BIT.B7 = IO_OUTPUT;
  PORTB.PDR.BIT.B5 = IO_OUTPUT;
  PORT5.PODR.BIT.B4 = 0;
  PORT0.PODR.BIT.B5 = 0;
  PORT2.PODR.BIT.B7 = 0;
  PORTB.PODR.BIT.B5 = 0;
  PORTE.PMR.BIT.B1 = 1;
  PORT4.PMR.BIT.B4 = 1;
  PORT4.PMR.BIT.B6 = 1;
  PORT4.PMR.BIT.B2 = 1;
  MPC.PWPR.BIT.B0WI = 0;
  MPC.PWPR.BIT.PFSWE = 1;
  MPC.PE1PFS.BIT.ASEL = 1;
  MPC.P44PFS.BIT.ASEL = 1;
  MPC.P46PFS.BIT.ASEL = 1;
  MPC.P42PFS.BIT.ASEL = 1;
  MPC.PWPR.BYTE = 0x80;
  
  //  A/Dの設定
  SYSTEM.PRCR.WORD = 0xA502;
  MSTP(S12AD) = 0;
  SYSTEM.PRCR.WORD = 0xA500;
  S12AD.ADCSR.BIT.CKS = 3;
  S12AD.ADCSR.BIT.ADCS = 0;
}


/*
 * LEDの初期設定
 */
void init_led(void)
{
  //  各ポートを出力ポートに設定する
  PORTB.PDR.BIT.B0 = IO_OUTPUT;   //  左端から１番目
  PORTA.PDR.BIT.B6 = IO_OUTPUT;   //  左端から２番目
  PORTA.PDR.BIT.B4 = IO_OUTPUT;   //  左端から３番目
  PORTA.PDR.BIT.B0 = IO_OUTPUT;   //  左端から４番目
}


/*
 * Buzzerの初期設定（MTU0）
 */
void init_buzzer(void)
{
  //  I/Oの設定
  PORTB.PODR.BIT.B3 = 1;
  PORTB.PDR.BIT.B3 = 1;
  PORTB.PMR.BIT.B3 = 0;
  MPC.PWPR.BIT.B0WI = 0;
  MPC.PWPR.BIT.PFSWE = 1;
  MPC.PB3PFS.BIT.PSEL = 1;
  MPC.PWPR.BYTE = 0x80;
  
  //  ブザーの設定（MTU0）
  SYSTEM.PRCR.WORD = 0xA502;
  MSTP(MTU) = 0;
  SYSTEM.PRCR.WORD = 0xA500;
  MTU.TSTR.BYTE = 0;
  MTU0.TCR.BIT.CCLR = 2;
  MTU0.TCR.BIT.TPSC = 1;
  MTU0.TMDR.BIT.MD = 2;
  MTU0.TIORH.BIT.IOA = 5;
  MTU0.TIORH.BIT.IOB = 2;
  MTU0.TGRA = (13636 - 1);
  MTU0.TGRB = (27272 - 1);
  MTU0.TCNT = 0;
}


/*
 * 光センサ制御の初期設定
 */
void init_cmt0(void)
{
  //  割り込み周期：0.25ms(4kHz)
  //  (1)48MHz / 32分周 = 1.5MHz ≒ 0.667us
  //  (2)1.5MHz / 4kHz = 375カウント

  SYSTEM.PRCR.WORD = 0xA502;    //  プロテクト解除
  MSTP(CMT0) = 0;               //  CMT0の有効化
  SYSTEM.PRCR.WORD = 0xA500;    //  プロテクト
  CMT.CMSTR0.BIT.STR0 = 0;      //  CMT0のカウント停止
  CMT0.CMCR.BIT.CKS = 1;        //  周波数を32分周設定
  CMT0.CMCR.BIT.CMIE = 1;       //  割込み許可
  CMT0.CMCNT = 0;               //  カウント値のリセット
  CMT0.CMCOR = 375 - 1;         //  割込み周期を0.25ms設定
  IEN(CMT0,CMI0) = 1;
  IPR(CMT0,CMI0) = 15;
  IR(CMT0,CMI0) = 0;
  CMT.CMSTR0.BIT.STR0 = 1;      //  CMT0のカウント開始
}


/*
 * 速度制御の初期設定（CMT1）
 */
void init_cmt1(void)
{
  //  割り込み周期：1ms(1kHz)
  //  (1)48MHz / 32分周 = 1.5MHz ≒ 0.667us
  //  (2)1.5MHz / 1kHz = 1500カウント
  
  SYSTEM.PRCR.WORD = 0xA502;    //  プロテクト解除
  MSTP(CMT1) = 0;               //  CMT1の有効化
  SYSTEM.PRCR.WORD = 0xA500;    //  プロテクト
  CMT.CMSTR0.BIT.STR1 = 0;      //  CMT1のカウント停止
  CMT1.CMCR.BIT.CKS = 1;        //  周波数を32分周設定
  CMT1.CMCR.BIT.CMIE = 1;       //  割込み許可
  CMT1.CMCNT = 0;               //  カウント値のリセット
  CMT1.CMCOR = 1500 - 1;        //  割込み周期を1ms設定
  IEN(CMT1,CMI1) = 1;
  IPR(CMT1,CMI1) = 15;
  IR(CMT1,CMI1) = 0;
  CMT.CMSTR0.BIT.STR1 = 1;      //  CMT1のカウント開始
}


/*
 * 光センサの値をA/D変換で取得する
 */
void int_cmt0(void)
{
  static char state = 0;
  short i;
  
  switch(state)
  {
  case 0:
    PORT0.PODR.BIT.B5 = 1;            //  右センサ用のLED点灯開始
    for(i=0; i<300; i++);             //  トランジスタが反応するまで待機
    S12AD.ADANS0.BIT.ANS0 = 0x0040;   //  AN006(右のセンサ)
    S12AD.ADCSR.BIT.ADST = 1;         //  A/D変換の開始
    while(S12AD.ADCSR.BIT.ADST);      //  A/D変換が終了するまで待機
    PORT0.PODR.BIT.B5 = 0;            //  右センサ用のLED消灯
    r_sen = S12AD.ADDR6;
    break;
  case 1:
    PORT5.PODR.BIT.B4 = 1;            //  左センサ用のLED点灯開始
    for(i=0; i<300; i++);             //  トランジスタが反応するまで待機
    S12AD.ADANS0.BIT.ANS0 = 0x0004;   //  AN002(左のセンサ)
    S12AD.ADCSR.BIT.ADST = 1;         //  A/D変換の開始
    while(S12AD.ADCSR.BIT.ADST);      //  A/D変換が終了するまで待機
    PORT5.PODR.BIT.B4 = 0;            //  左センサ用のLED消灯
    l_sen = S12AD.ADDR2;
    break;
  case 2:
    PORTB.PODR.BIT.B5 = 1;            //  右前センサ用のLED点灯開始
    for(i=0; i<300; i++);             //  トランジスタが反応するまで待機
    S12AD.ADANS0.BIT.ANS0 = 0x0200;   //  AN009(右前の光センサ)
    S12AD.ADCSR.BIT.ADST = 1;         //  A/D変換の開始
    while(S12AD.ADCSR.BIT.ADST);      //  A/D変換が終了するまで待機
    PORTB.PODR.BIT.B5 = 0;            //  右前センサ用のLED消灯
    fr_sen = S12AD.ADDR9;
    break;
  case 3:
    PORT2.PODR.BIT.B7 = 1;            //  左前センサ用のLED点灯開始
    for(i=0; i<300; i++);             //  トランジスタが反応するまで待機
    S12AD.ADANS0.BIT.ANS0 = 0x0010;   //  AN004(左前の光センサ)
    S12AD.ADCSR.BIT.ADST = 1;         //  A/D変換の開始
    while(S12AD.ADCSR.BIT.ADST);      //  A/D変換が終了するまで待機
    PORT2.PODR.BIT.B7 = 0;            //  左前センサ用のLED消灯
    fl_sen = S12AD.ADDR4;
    break;
  }
  
  state++;
  if(state>3) {
    state = 0;
  }
}


/*
 * モータの速度を制御する
 */
void int_cmt1(void)
{
  v = v + a;
  //  速度から速度に必要なパルス数に変換する
  MTU3.TGRC = 3000000 / ((v / MMPP));
  MTU4.TGRC = 3000000 / ((v / MMPP));
}


/*
 * LEDを点灯消灯する
 */
void set_led(unsigned char data)
{
  //  各LEDの出力ポートに出力値を設定する
  PORTB.PODR.BIT.B0 = data & 0x01;      //  左端から１番目
  PORTA.PODR.BIT.B6 = (data>>1)&0x01;   //  左端から２番目
  PORTA.PODR.BIT.B4 = (data>>2)&0x01;   //  左端から３番目
  PORTA.PODR.BIT.B0 = (data>>3)&0x01;   //  左端から４番目
}


/*
 * Buzzerを開始する
 */
void buzzer_on(void)
{
  MTU0.TCNT = 0;            //  ブザーのカウント値をリセットする
  PORTB.PMR.BIT.B3 = 1;     //  汎用ポートから機能ポートに切り替える
  MTU.TSTR.BIT.CST0 = 1;    //  ブザーのカウントを開始する
}


/*
 * Buzzerを停止する
 */
void buzzer_off(void)
{
  PORTB.PMR.BIT.B3 = 0;     //  機能ポートから汎用ポートに切り替える
  MTU.TSTR.BIT.CST0 = 0;    //  ブザーのカウントを停止する
}


/*
 * 右モータのパルスをカウントする
 */
void int_motor_r(void)
{
  step_r++;
}


/*
 * 左モータのパルスをカウントする
 */
void int_motor_l(void)
{
  step_l++;
}


#ifdef __cplusplus
void abort(void)
{

}
#endif

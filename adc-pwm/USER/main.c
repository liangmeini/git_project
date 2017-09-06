/**
  ******************************************************************************
  * @file ADC2_TriggerConversion\main.c
  * @brief This file contains the main function for the ADC2 Trigger Conversion example.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.1.1
  * @date 06/05/2009
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  * 
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"
#include "stm8s_gpio.h"
#include "stm8s_adc2.h"
#include "type_def.h"


#define LIUSHUIDELAYTIME 500
#define DELAYTIME1 1000
#define ALARMTIME 500

long int value_2[5]={0};
long int value_3[5]={0};
long int value_4[5]={0};
long int day_count=0;//天数计数
long int time_count=0;//秒钟计数
char boot_check_flag=0,boot_count=0;//开机自检标志
char check_flag=0;//进入检测标志
char fault_flag=0;//故障标志
char once_check=0;
long int adcvalue_2=0,AverageData_2=0,adcvalue_3=0,AverageData_3=0,adcvalue_4=0,AverageData_4=0;

void DelayMS(unsigned int ms);

void Clk_conf(void)
{
  CLK_CKDIVR= 0x00; //内部16M时钟不分频
}

void CloseLed(void)
{
	GPIO_WriteHigh(GPIOC, LED_PIN_3);
	GPIO_WriteHigh(GPIOC, LED_PIN_5);
	GPIO_WriteHigh(GPIOC, LED_PIN_6);
	GPIO_WriteHigh(GPIOC, LED_PIN_7);
}
/*
void green_light_high(void)//绿灯闪烁
{
  GPIO_WriteHigh(GPIOC, LED_PIN_5);
  DelayMS(250);
  GPIO_WriteLow(GPIOC, LED_PIN_5);
  DelayMS(250);
  
  GPIO_WriteHigh(GPIOC, LED_PIN_5);
  DelayMS(250);
  GPIO_WriteLow(GPIOC, LED_PIN_5);
  DelayMS(250);
  
}
*/
void green_light_high(void)//绿灯亮
{
  GPIO_WriteLow(GPIOC, LED_PIN_5);
}
void green_light_low(void)//绿灯灭
{
  GPIO_WriteHigh(GPIOC, LED_PIN_5);
}

void yellow_light_high(void)//黄灯亮
{
  GPIO_WriteLow(GPIOC, LED_PIN_6);
}
void yellow_light_low(void)//黄灯灭
{
  GPIO_WriteHigh(GPIOC, LED_PIN_6);
}

void delay_time(void)
{
  DelayMS(250);
}

void Gpio_Init(void)
{
	/*  Init GPIO for ADC2 */
	//GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_IT);
	GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
    /* Init GPIO for LED  */
	GPIO_Init(LEDS_PORT, (GPIO_PIN_3 |GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7), GPIO_MODE_OUT_PP_LOW_FAST);
}

void ADC_Init(void)
{
   //ADC2_DeInit();
   /**< 连续转换模式 */
    /**< 使能通道 */
    /**< ADC时钟：fADC2 = fcpu/18 */
    /**< 这里设置了从TIM TRGO 启动转换，但实际是没有用到的*/
    /**  不使能 ADC2_ExtTriggerState**/
    /**< 转换数据右对齐 */
    /**< 不使能通道10的斯密特触发器 */
    /**  不使能通道10的斯密特触发器状态 */
   ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS,(ADC2_CHANNEL_2),ADC2_PRESSEL_FCPU_D2,ADC2_EXTTRIG_TIM, DISABLE,ADC2_ALIGN_RIGHT, (ADC2_SCHMITTTRIG_CHANNEL2),DISABLE);//初始化3通道

   ADC2_ConversionConfig(ADC2_CONVERSIONMODE_CONTINUOUS,ADC2_CHANNEL_2,ADC2_ALIGN_RIGHT);
   ADC2_ITConfig(DISABLE);//关闭中断功能
   ADC2_Cmd(ENABLE);//启用ADC2
}

int GetAdcValue(ADC2_Channel_TypeDef ADC2_Channel,ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel)
{
	int adcvalue;
	
	ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS,ADC2_Channel,ADC2_PRESSEL_FCPU_D2,ADC2_EXTTRIG_TIM, DISABLE,ADC2_ALIGN_RIGHT, ADC2_SchmittTriggerChannel,DISABLE);
       
   ADC2_Cmd(ENABLE);//启用ADC2
   ADC2_StartConversion();//开始转换
   adcvalue = ADC2_GetConversionValue();
   return adcvalue;
}

BitStatus readvalue=0;
long int VoltageValue_2=0,VoltageValue_3=0,VoltageValue_4=0;
int charge_statu = 0;
void auto_check(void)
{
  int i;
  for(i=0;i<5;i++)//检测5次取平均值
	  {
		  value_2[i] = GetAdcValue(ADC2_CHANNEL_2,ADC2_SCHMITTTRIG_CHANNEL2);//2通道电压采集
		  adcvalue_2 = adcvalue_2 +value_2[i];
		  DelayMS(1);
		  value_3[i] = GetAdcValue(ADC2_CHANNEL_3,ADC2_SCHMITTTRIG_CHANNEL3);//3通道电压采集
		  adcvalue_3 = adcvalue_3 +value_3[i];
		  DelayMS(1);
		  value_4[i] = GetAdcValue(ADC2_CHANNEL_4,ADC2_SCHMITTTRIG_CHANNEL4);//4通道电压采集
		  adcvalue_4 = adcvalue_4 +value_4[i];
		  DelayMS(1);
	  }
	  
	  AverageData_2 = adcvalue_2/5;
	  adcvalue_2 = 0;
	  VoltageValue_2 = (330*AverageData_2)/1024; //2通道采集的实际电压值
	  
	  AverageData_3 = adcvalue_3/5;
	  adcvalue_3 = 0;
	  VoltageValue_3 = (330*AverageData_3)/1024; //3通道采集的实际电压值
		
	  AverageData_4 = adcvalue_4/5;
	  adcvalue_4 = 0;
	  VoltageValue_4 = (330*AverageData_4)/1024; //4通道采集的实际电压值
}

void state_check(void)//当前充电状态的检测
{
  if(readvalue==1&&VoltageValue_4<=175&&VoltageValue_4>=130) { //充电中
	charge_statu = 1;
  } else if(readvalue==1&&VoltageValue_4>190) {//已充满
	charge_statu = 2;
  }
}

int state_mode=1;
int auto_statu = 0;
int work_statu = 0;

void auto_flashlight_function(void)
{
  if(state_mode == 1) {
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(4000);
	//if(VoltageValue_3-VoltageValue_4<10)
	  //work_statu = work_statu | 0x01;
	//else
	//  work_statu = work_statu & 0x0e;
	state_mode = 2;
	once_check = 0;
  } else if(state_mode == 2) {
	green_light_high();  
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(4000);
	if(VoltageValue_4<120)
	  work_statu = work_statu | 0x02;
	else
	  work_statu = work_statu & 0x0d;
	state_mode = 3;
  } else if(state_mode == 3) {
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(4000);
	if(VoltageValue_3<120)
	  work_statu = work_statu | 0x04;
	else
	  work_statu = work_statu & 0x0b;
	state_mode = 4;
  } else if(state_mode == 4) {
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();  
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();  
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(450);
	green_light_high();
	DelayMS(450);
	green_light_low();
	DelayMS(4000);
	if(VoltageValue_2<50 || VoltageValue_2>100)
	  work_statu = work_statu | 0x08;
	else
	  work_statu = work_statu & 0x07;
	state_mode = 1;
	auto_statu = 1;
	once_check = 1;
  }
}

void fault_flashlight_function(void)
{
  if(work_statu == 1) {
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(4000);
  } else if(work_statu == 2) {
	yellow_light_high();  
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(4000);
  } else if(work_statu == 4) {
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(4000);
  } else if(work_statu == 8) {
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();  
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();  
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(450);
	yellow_light_high();
	DelayMS(450);
	yellow_light_low();
	DelayMS(4000);
  } else {
	yellow_light_high();
  }
}

void normal_flashlight_function(void)
{
  if(state_mode == 1) {
	//if(VoltageValue_3-VoltageValue_4<10)
	  //work_statu = work_statu | 0x01;
	//else
	  //work_statu = work_statu & 0x0e;
	state_mode = 2;
	once_check = 0;
  } else if(state_mode == 2) {
	if(VoltageValue_4<120)
	  work_statu = work_statu | 0x02;
	else
	  work_statu = work_statu & 0x0d;
	state_mode = 3;
  } else if(state_mode == 3) {
	if(VoltageValue_3<120)
	  work_statu = work_statu | 0x04;
	else
	  work_statu = work_statu & 0x0b;
	state_mode = 4;
  } else if(state_mode == 4) {
	if(VoltageValue_2<50 || VoltageValue_2>100)
	  work_statu = work_statu | 0x08;
	else
	  work_statu = work_statu & 0x07;
	state_mode = 1;
	once_check = 1;
  } 
}

void main(void)
{
	long int i;
	
	/*设置内部高速时钟16M为主时钟*/
	Clk_conf();
	Gpio_Init();//LED  GPIO初始化
	CloseLed();//初始化后默认LED亮,关闭LED灯
	ADC_Init();
	green_light_low();//绿灯亮
	
	while(1)
	{
	  readvalue=GPIO_ReadInputPin(GPIOD,GPIO_PIN_4);//停电检测
	  auto_check();//进行检测
	  state_check();
	  if(readvalue == 0) {
		//所有的标志位都清除
		state_mode = 1;
		auto_statu = 0;
		if(work_statu != 0) {
		  fault_flashlight_function();
		}
		GPIO_WriteLow(GPIOC, LED_PIN_3);//点亮红灯
		GPIO_WriteHigh(GPIOC, LED_PIN_7);//打开应急LED灯
	  } else {
		GPIO_WriteHigh(GPIOC, LED_PIN_3);//关闭红灯
		GPIO_WriteLow(GPIOC, LED_PIN_7);//关闭应急LED灯
		if(auto_statu == 0) {
		  readvalue=GPIO_ReadInputPin(GPIOD,GPIO_PIN_4);//停电检测
		  if(readvalue == 0) {
			//所有的标志位都清除
			state_mode = 1;
			auto_statu = 0;
			if(work_statu != 0) {
			  fault_flashlight_function();
			}
			GPIO_WriteLow(GPIOC, LED_PIN_3);//点亮红灯
			GPIO_WriteHigh(GPIOC, LED_PIN_7);//打开应急LED灯
		  } else {
			auto_flashlight_function();
		  }
		} else {
		  readvalue=GPIO_ReadInputPin(GPIOD,GPIO_PIN_4);//停电检测
		  if(readvalue == 0) {
			//所有的标志位都清除
			state_mode = 1;
			auto_statu = 0;
			if(work_statu != 0) {
			  fault_flashlight_function();
			}
			GPIO_WriteLow(GPIOC, LED_PIN_3);//点亮红灯
			GPIO_WriteHigh(GPIOC, LED_PIN_7);//打开应急LED灯
		  } else {
			normal_flashlight_function();
			if(work_statu != 0 && once_check == 1) {
			  fault_flashlight_function();
			} else {
			  if(charge_statu == 1) {
				green_light_high();
				DelayMS(450);
				green_light_low();
				DelayMS(450);
				time_count++;
			  } else if(charge_statu == 2) {
				green_light_high();
			  }
			  DelayMS(1000);
			  time_count++;
			  if(time_count>=86400)//一天的时间
			  {
				day_count++;
				time_count=0;
			  }
			  if(day_count>=30) {
				auto_statu = 0;
				day_count = 0;
			  }
			}
		  }
		}
	  }
	}

/*
	while(1)
	{
	  auto_check();//进行检测
	  state_check();//状态显示
	  if(boot_check_flag==0&&boot_count<6&&readvalue!=0&&fault_flag==0)//开机自检三秒
	  {
		green_light_high();//绿灯亮
		delay_time();
		green_light_low();//绿灯灭
		delay_time();
		boot_count++;
	  }
	  
	  green_light_high();//绿灯亮
	  
	  if(readvalue==0)//停电了 所有的操作都不检测。
	  {
		//所有的标志位都清除
		boot_check_flag=0;
		boot_count=0;
		day_count=0;
		time_count=0;
		check_flag=0;
		once_check=0;
		GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
	  }
	  else if(fault_flag==0)//没有停电
	  {
		GPIO_WriteLow(GPIOC, LED_PIN_7);//关闭LED指示�
		DelayMS(1);
		time_count++;
		if(time_count>=86400)//一天的时间
		{
		  day_count++;
		  time_count=0;
		}
		if(day_count%30==0){
		  check_flag=1;
		} else {
		  check_flag=0;
		  once_check=0;
		}
		if(day_count>=30&&day_count<=180)
		{
		  if(check_flag==1&&once_check==0)
		  {
			for(i=0;i<600;i++)
			{
			  green_light_high();//绿灯亮
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  green_light_low();//绿灯灭
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  state_check();//状态显示
			  if(fault_flag==1)
				break;
			}
			time_count=time_count+600;
			once_check=1;
		  }
		}
		else if(day_count>180&&day_count<360)
		{
		  if(check_flag==1&&once_check==0)
		  {
			for(i=0;i<1800;i++)
			{
			  green_light_high();//绿灯亮
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  green_light_low();//绿灯灭
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  state_check();//状态显示
			  if(fault_flag==1)
				break;
			}
			time_count=time_count+1800;
			once_check=1;
		  }
		}
		else if(day_count>=360)
		{
		  if(check_flag==1&&once_check==0)
		  {
			for(i=0;i<10800;i++)
			{
			  green_light_high();//绿灯亮
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  green_light_low();//绿灯灭
			  auto_check();//进行检测
			  if(readvalue==0)//停电了 所有的操作都不检测。
			  {
				//所有的标志位都清除
				boot_check_flag=0;
				day_count=0;
				time_count=0;
				check_flag=0;
				once_check=0;
				GPIO_WriteHigh(GPIOC, LED_PIN_7);//点亮led灯
				break;
			  }
			  delay_time();
			  state_check();//状态显示
			  if(fault_flag==1)
				break;
			}
			time_count=time_count+10800;
			once_check=1;
		  }
		}
		state_check();//状态显示
	  }
  }
*/
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval 
  * None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void DelayMS(unsigned int ms)
{
  unsigned int i;
  while(ms != 0)
  {
    for(i=0;i<1000;i++)
    {
    }
	for(i=0;i<75;i++)
	{
	}
	ms--; 
  }    
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

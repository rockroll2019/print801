/**
******************************************************************************
  * @file    x.h
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   This file contains all the functions prototypes for the firmware 
  *          library.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macro Definition ----------------------------------------------------------*/
#define	CUTTER_ENABLE
#define	ERROR_EMERGENCY_PROTECT_ENABLE	//�Ƿ��ڴ���ʱ���н�����������
#define	ERROR_STATUS_ADC_UPDATE_ENABLE	//�Ƿ���и���ADC����״̬������:�ڱ꣬ȱֽ,ֽ����������,��ѹ,Ƿѹ,ֽ��ȡ�ߵ�
#define	REALTIME_STATUS_UPDATE_ENABLE		//�Ƿ���и���ADC����״̬������:ֽ�ָǣ��Ƿ�ȱֽ��������״̬

#define	PAPER_OUT_RE_PRINT_ENABLE										  //ȱֽ�ش������� 2016.10.08
//#define	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE					//���ش�ӡ�ٶȽ��ʹ���
//#define	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE	//��ͬ�Ŵ���ʱ��ӡ����
//#define	PRT_SPEED_LIMIT_ENABLE

//#define	HEAT_TIME_BY_POWER_ADJUST_ENABLE						//���ݹ����ѹ��������ʱ��
#define	HEAT_TIME_BY_TEMPERATURE_ADJUST_ENABLE			//�����¶ȵ�������ʱ��
//#define	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE						//���Դ�ӡ����ʱ����ٴ���
#define	HEAT_TIME_START_STEP_ADJUST_ENABLE					//��ʼ��ӡʱ��������ʱ��
#define	HEAT_TIME_DRV_PRT_ADJUST_ENABLE							//������ӡ����ʱ�����

#define	AUTO_FEED_AFTER_CUT_ENABLE								//������ֽ���Զ���ֽ 2016.081.02  ȥ��
#define	UNDER_LINE_PRT_ENABLE												//�������»���
#define	MACRO_DEFINITION_ENABLE											//������궨��
#define	WH_SELFDEF_CUT_CMD_ENABLE										//������쿻��Զ����е�ָ��
#define	MAINTAIN_ENABLE														  //������ͳ��ָ��

#define	SPI_PRT_TX_BY_DMA_ENABLE	//����о����ȫ��ʹ��DMA��ʽ����
#define	PNE_ENABLE								//������ֽ����ȱֽ�ź�
#define	PARALLEL_PNE_OUT_ENABLE		//��������ֽ��������ź�
// #define	EXT_SRAM_ENABLE				//�����ⲿSRAM��չ
// #define	CPCL_CMD_ENABLE

//#define	UART_MAX_BAUD							(115200)
//#define	DRIVE_PRT_MODE_RX_DELAY_ENABLE	//������ӡʱ������ʱ
//#define	BARCODE_PRT_AS_BITMAP_ENABLE
//#define	FEED_MOTOR_MONITOR_ENABLE
// #define	HEAT_ONE_TIME_ENABLE				  //�������ݶ���ͳͳһ�μ��� 2016.06.09


//#define	DBG_NEW_METHOD_SELFTEST
//#define	DBG_RCV_DATA_LOSE
//#define	DBG_INT_RCV
//#define	DBG_UART_VISIBLE_ENABLE
//#define	DBG_RX_TOTAL_THEN_PRT		//����ȫ������֮���ٴ�ӡ

//#define	DBG_MODE_SELECT

//#define	DUMMY
#ifdef	DUMMY
	#define	DBG_DUMMY_FEED	//������ֽ
	#define	DBG_DUMMY_HEAT	//�������
	#define	DBG_DUMMY_CUT		//������ֽ
#endif
//#define	DBG_DUMMY_CUT		//������ֽ
//#define	DBG_DUMMY_HEAT	//�������

//#define	DBG_TIM					//���Զ�ʱ��
#ifdef	DBG_TIM
//	#define	DBG_TIM1
//	#define	DBG_TIM2
//	#define	DBG_TIM3
//	#define	DBG_TIM4
//	#define	DBG_TIM5
//	#define	DBG_TIM6
//	#define	DBG_TIM7
//	#define	DBG_TIM8
//	#define	DBG_TIM9
//	#define	DBG_TIM10
//	#define	DBG_TIM11
//	#define	DBG_TIM12
//	#define	DBG_TIM13
//	#define	DBG_TIM14
//	#define	DBG_TIM15
//	#define	DBG_TIM16
//	#define	DBG_TIM17
#endif

//#define	DBG_BUTTON			//��ֽ����̫����λ����
//#define	DBG_BUTTON_USE_EXTI
//#define	DBG_POWER_CTRL	//���ȵ�ѹ�������
//#define	DBG_USART				//�����жϽ���,��ѯ����
//#define	USE_USART1_AS_NET
//#define	UART_MAX_SPEED_ENABLE
//#define	UART_MAX_BAUD						(250000)
//#define	UART_MAX_BAUD						(1000000)

//#define	DBG_ADC					//ADC
#ifdef	DBG_ADC
//	#define	DBG_ADC_PE					//ȱֽ�ź�
//	#define	DBG_ADC_TEMPERATURE	//��о�¶��ź�
//	#define	DBG_ADC_POWER				//��Դ��ѹADC
//	#define	DBG_ADC_PNE					//ֽ�����ź�
//	#define	DBG_ADC_BM					//�ڱ��ź�
#endif
//#define	DBG_ADC_PRECISION	//����ADC�ľ���

//#define	DBG_INBUF_RCV		//InBuf��صĲ���

//#define	DBG_SPI					//SPI
#ifdef	DBG_SPI
//	#define	DBG_SPI_ZIKU	//�ֿ��д,��ѯ����,д��
//	#define	DBG_SPI_ROM		//�ֿ��д,��ѯ����,д��
//	#define	DBG_SPI_HEAT	//һ�������ݷ��͵���о,MDA�жϷ���
#endif

//#define	DBG_LPB_INT_COUNT		//���Դ�ӡ�����д�ӡ���ƶ�ʱ������Ĵ���
//#define	DBG_HEAT_INT_COUNT	//���Դ�ӡ�����м��ȿ��ƶ�ʱ������Ĵ���

//���Ը������ں�ʱ
//#define	DBG_PROCESS_TIME
#ifdef	DBG_PROCESS_TIME
//	#define	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//��������ȡһ�����ֵĵ����ֿ��ַ����Ϣ��ʱ
//	#define	DBG_PROCESS_TIME_READ_ZIKU_ONE_ASCII		//��һ��ASCII�ĵ����ֿ����ݺ�ʱ
//	#define	DBG_PROCESS_TIME_READ_ZIKU_ONE_HZ				//��һ�����ֵĵ����ֿ����ݺ�ʱ
//	#define	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB			//����һ���ַ��ı༭��������ʱ,ֻ�����༭���ʱ��
//	#define	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB			//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
//	#define	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB			//��ձ༭�д�ӡ��������ʱ
//	#define	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ,DMA��ʽ
//	#define	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB				//��ӡһ�к�ʱ
//	#define	DBG_PROCESS_TIME_WAIT_LPB_ILDE					//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
//	#define	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB	//����һ�еı༭��������ʱ,�������ֿ�ͱ༭���ʱ��
//	#define	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ,��ѯ����
#endif
//#define	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���

//#define	DBG_ONE_LINE_TIME			//���Գ���������ڴ����ʱ
#ifdef	DBG_ONE_LINE_TIME
//	#define	DBG_DEAL_ONE_LINE_TIME	//��������һ�д�ӡ���ݺ�ʱ
//	#define	DBG_COPY_ONE_LINE_TIME	//���Ը��ƴ�ӡ���������ݺ�ʱ
//	#define	DBG_COPY_AND_CLEAR_ONE_LINE_TIME	//���Ը��ƴ�ӡ���������ݲ����ԭ��������ʱ
//	#define	DBG_PRT_SPEED						//���Դ�ӡ�ٶ�,�޷�����
//	#define	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
#endif

//#define	DBG_LEAST_MEM_OPERATE
//#define	NORMAL_MEM_OPERATE
//#define	DBG_ZIKU_SPI_RX_DMA			//�����ֿ�SPIʹ��DMA��ʽ����
//#define	DBG_LATCH								//���Ի�о�����������
//#define	DBG_MOTOR_HOLD					//���Ե����ʼ�ͽ�����ס����Ч��
//#define	DBG_SPI_HEAT_DATA_SET		//���Ի�оSPI3���ͽ����Ƿ���ȷ

//#define	DBG_FEED				//������ֽ,��ʱ��ʽ
//#define	DBG_CUT					//�е�,��ʱ��ʽ
//#define	DBG_HEAT				//����,�����ܽŵ�ƽ,����ֱ�ӽӻ�о
//#define	DBG_PRT					//��ӡ

//#define	DBG_USB						//USB
//#define	DBG_ETH_USART			//��̫������
//#define	DBG_ETH_SPI				//��̫��SPI
//#define	DBG_UPDATE
//#define	DBG_ZIKU_SPI_WRITE
//#define	DBG_SELF_DEF_CMD_ENABLE
//#define	DBG_025
//#define	DBG_SELF_DEF_CODEPAGE
//#define	DBG_MOTOR_STOP_COUNT
//#define	DBG_REALTIME_1B76_ENABLE
//#define	DBG_GB18030_ASCII	//���ֿ���ָ��λ�ÿ�ʼ��ָ����Ŀ���ַ�ȡ������ӡ����
//#define	DBG_TEMPERATURE_AND_HEAT_TIME
//#define	DBG_EXT_SRAM
#define	DBG_ESC_RELEASE

//#define	DBG_CPCL_TEXT		//�������CPCLָ���ʵ���ı���ӡ



/* Funtion Declare -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/

/**********************************************
 * Configuration of AiThinker LoRa module(ra-01)
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/	

#ifndef __SX1276_CONFIG_H__
#define __SX1276_CONFIG_H__
     
/* user defination */

#define RX_TIMEOUT_VALUE                            10000      /* 接收超时时间 (ms) */
#define LORA_BUFFER_SIZE                            128         /* Define the payload size here */

#define USE_BAND_433	            /* 选择一个频率 */

#define USE_MODEM_LORA	          /* 选择LoRa模式 */


#if defined( USE_BAND_433 )

#define RF_FREQUENCY                                434000000 // Hz
																														 
#elif defined( USE_BAND_780 )                                 
																														 
#define RF_FREQUENCY                                780000000 // Hz
																														 
#elif defined( USE_BAND_868 )                                 
																														 
#define RF_FREQUENCY                                868000000 // Hz
																														 
#elif defined( USE_BAND_915 )                                 
																														 
#define RF_FREQUENCY                                915000000 // Hz

#else
    #error "Please define a frequency band in the compiler options."
#endif


#define TX_OUTPUT_POWER                             20        // dBm

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         /* 带宽       [0: 125 kHz,
                                                               *             1: 250 kHz,
                                                               *             2: 500 kHz,
                                                               *             3: Reserved]  */
																																 			    
#define LORA_SPREADING_FACTOR                       7         /* 扩频因子   [SF7..SF12]    */
																																 			    
#define LORA_CODINGRATE                             1         /* 编码率     [1: 4/5, 
                                                               *             2: 4/6,
                                                               *             3: 4/7,
                                                               *             4: 4/8]       */
																															 
#define LORA_PREAMBLE_LENGTH                        8         /* 前导码长度 Tx 和 Rx 相同  */
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols

#define LORA_FIX_LENGTH_PAYLOAD_ON                  true
#define LORA_FIX_LENGTH_PAYLOAD_OFF                 false

#define LORA_PAYLOAD_LENGTH                             0

#define LORA_CRC_ON                                 true
#define LORA_CRC_OFF                                false

#define LORA_FREQ_HOP_ON                            true
#define LORA_FREQ_HOP_OFF                           false
#define LORA_HOP_PERIOD                             0

#define LORA_RX_CONTINUOUS                          1
#define LORA_RX_SINGLE                              0

#define LORA_IQ_INVERSION_ON                        true
#define LORA_IQ_INVERSION_OFF                       false

/* FSK mode config*/
#elif defined( USE_MODEM_FSK )

#define FSK_FDEV                                    25e3      // Hz
#define FSK_DATARATE                                50e3      // bps
#define FSK_BANDWIDTH                               50e3      // Hz
#define FSK_AFC_BANDWIDTH                           83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#else
    #error "Please define a modem in the compiler options."
#endif

#endif // __SX1276_CONFIG_H__

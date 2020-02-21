/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dma.h"
#include "DAC_GeradorDeSinais.h"
#include "loop.h"

/* Private variables ---------------------------------------------------------*/
DAC_InitTypeDef DAC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

const uint16_t Sine12bit32[32] =
{
	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263,  909,
	599,   344,  155,   38,    0,   38,  155,  344,  599,  909,
	1263, 1647
};


const uint16_t Sine12Bit256[256] =
{
	2047,1997,1946,1896,1846,1795,1745,1696,1646,1597,
	1548,1499,1451,1402,1355,1308,1261,1214,1169,1123,
	1079,1034, 991, 948, 906, 864, 823, 783, 744, 706,
	 668, 631, 595, 560, 526, 492, 460, 429, 398, 369,
	 341, 313, 287, 262, 238, 215, 193, 172, 152, 134,
	 116, 100,  85,  71,  59,  47,  37,  28,  21,  14,
	   9,   5,   2,   0,   0,   1,   3,   7,  11,  17,
	  24,  33,  42,  53,  65,  78,  93, 108, 125, 143,
	 162, 182, 203, 226, 250, 274, 300, 327, 355, 384,
	 413, 444, 476, 509, 543, 577, 613, 649, 687, 725,
	 764, 803, 844, 885, 927, 969,1013,1056,1101,1146,
	1191,1238,1284,1331,1379,1426,1475,1523,1572,1621,
	1671,1721,1770,1820,1871,1921,1971,2022,2072,2123,
	2173,2223,2274,2324,2373,2423,2473,2522,2571,2619,
	2668,2715,2763,2810,2856,2903,2948,2993,3038,3081,
	3125,3167,3209,3250,3291,3330,3369,3407,3445,3481,
	3517,3551,3585,3618,3650,3681,3710,3739,3767,3794,
	3820,3844,3868,3891,3912,3932,3951,3969,3986,4001,
	4016,4029,4041,4052,4061,4070,4077,4083,4087,4091,
	4093,4094,4094,4092,4089,4085,4080,4073,4066,4057,
	4047,4035,4023,4009,3994,3978,3960,3942,3922,3901,
	3879,3856,3832,3807,3781,3753,3725,3696,3665,3634,
	3602,3568,3534,3499,3463,3426,3388,3350,3311,3271,
	3230,3188,3146,3103,3060,3015,2971,2925,2880,2833,
	2786,2739,2692,2643,2595,2546,2497,2448,2398,2349,
	2299,2248,2198,2148,2097,2047
};

const uint16_t Sine12Bit127[128] =
{
	2047,1946,1845,1744,1645,1546,1448,1352,1258,1165,
	1075, 987, 902, 819, 740, 664, 591, 521, 456, 394,
	 336, 283, 234, 189, 149, 113,  82,  56,  35,  19,
	   8,   1,   0,   4,  13,  26,  45,  69,  97, 130,
	 168, 211, 258, 309, 365, 424, 488, 555, 627, 701,
	 779, 860, 944,1031,1120,1211,1305,1400,1497,1595,
	1694,1794,1895,1996,2098,2199,2300,2400,2499,2597,
	2694,2789,2883,2974,3063,3150,3234,3315,3393,3467,
	3539,3606,3670,3729,3785,3836,3883,3926,3964,3997,
	4025,4049,4068,4081,4090,4094,4093,4086,4075,4059,
	4038,4012,3981,3945,3905,3860,3811,3758,3700,3638,
	3573,3503,3430,3354,3275,3192,3107,3019,2929,2836,
	2742,2646,2548,2449,2350,2249,2148,2047
};

const uint16_t Sine12Bit64[64] =
{
	2047,1846,1648,1453,1264,1082, 910, 748, 600, 465,
	 345, 242, 156,  88,  39,  10,   0,  10,  39,  88,
	 156, 242, 345, 465, 600, 748, 910,1082,1264,1453,
	1648,1846,2047,2248,2446,2641,2830,3012,3184,3346,
	3494,3629,3749,3852,3938,4006,4055,4084,4094,4084,
	4055,4006,3938,3852,3749,3629,3494,3346,3184,3012,
	2830,2641,2446,2248,2047
};

const uint16_t Sine12Bit32[32] =
{
	2047,1648,1264, 910, 600, 345, 156,  39,   0,  39,
	 156, 345, 600,	910,1264,1648,2047,2446,2830,3184,
	3494,3749,3938,4055,4094,4055,3938,3749,3494,3184,
	2830,2446,2047
};

const uint16_t Sinc12Bit32[32] =
{
	2128,2010,1928,2006,2156,2174,2009,1864,1958,2215,
	2296,2009,1642,1753,2565,3613,4094,3613,2565,1753,
	1642,2009,2296,2215,1958,1864,2009,2174,2156,2006,
	1928,2010,2128
};

const uint16_t Sinc12Bit64[64] =
{
	2128,2076,2010,1953,1928,1947,2006,2085,2156,2191,
	2174,2107,2009,1916,1864,1877,1958,2085,2215,2299,
	2296,2192,2009,1801,1642,1609,1753,2085,2565,3110,
	3613,3967,4094,3967,3613,3110,2565,2085,1753,1609,
	1642,1801,2009,2192,2296,2299,2215,2085,1958,1877,
	1864,1916,2009,2107,2174,2191,2156,2085,2006,1947,
	1928,1953,2010,2076,2128
};

const uint16_t Sinc12Bit128[128] =
{
	2128,2105,2076,2043,2010,1979,1953,1936,1928,1932,
	1947,1972,2006,2044,2085,2123,2156,2179,2191,2190,
	2174,2146,2107,2060,2009,1960,1916,1883,1864,1861,
	1877,1909,1958,2018,2085,2153,2215,2266,2299,2310,
	2296,2256,2192,2108,2009,1904,1801,1710,1642,1606,
	1609,1657,1753,1897,2085,2311,2565,2836,3110,3374,
	3613,3814,3967,4062,4094,4062,3967,3814,3613,3374,
	3110,2836,2565,2311,2085,1897,1753,1657,1609,1606,
	1642,1710,1801,1904,2009,2108,2192,2256,2296,2310,
	2299,2266,2215,2153,2085,2018,1958,1909,1877,1861,
	1864,1883,1916,1960,2009,2060,2107,2146,2174,2190,
	2191,2179,2156,2123,2085,2044,2006,1972,1947,1932,
	1928,1936,1953,1979,2010,2043,2076,2105,2128
};

const uint16_t Sinc12Bit256[256] =
{
	2128,2117,2105,2091,2076,2060,2043,2026,2010,1994,
	1979,1965,1953,1943,1936,1931,1928,1929,1932,1938,
	1947,1959,1972,1988,2006,2024,2044,2064,2085,2104,
	2123,2140,2156,2169,2179,2187,2191,2192,2190,2184,
	2174,2162,2146,2128,2107,2084,2060,2035,2009,1984,
	1960,1937,1916,1898,1883,1871,1864,1860,1861,1867,
	1877,1891,1909,1932,1958,1987,2018,2051,2085,2119,
	2153,2185,2215,2242,2266,2285,2299,2307,2310,2306,
	2296,2279,2256,2227,2192,2152,2108,2060,2009,1957,
	1904,1851,1801,1753,1710,1673,1642,1620,1606,1602,
	1609,1628,1657,1699,1753,1819,1897,1986,2085,2194,
	2311,2435,2565,2699,2836,2973,3110,3244,3374,3497,
	3613,3719,3814,3897,3967,4022,4062,4086,4094,4086,
	4062,4022,3967,3897,3814,3719,3613,3497,3374,3244,
	3110,2973,2836,2699,2565,2435,2311,2194,2085,1986,
	1897,1819,1753,1699,1657,1628,1609,1602,1606,1620,
	1642,1673,1710,1753,1801,1851,1904,1957,2009,2060,
	2108,2152,2192,2227,2256,2279,2296,2306,2310,2307,
	2299,2285,2266,2242,2215,2185,2153,2119,2085,2051,
	2018,1987,1958,1932,1909,1891,1877,1867,1861,1860,
	1864,1871,1883,1898,1916,1937,1960,1984,2009,2035,
	2060,2084,2107,2128,2146,2162,2174,2184,2190,2192,
	2191,2187,2179,2169,2156,2140,2123,2104,2085,2064,
	2044,2024,2006,1988,1972,1959,1947,1938,1932,1929,
	1928,1931,1936,1943,1953,1965,1979,1994,2010,2026,
	2043,2060,2076,2091,2105,2117,2128
};

const uint8_t Escalator8bit[6] =
{
	0x00, 0x33, 0x66,
	0x99, 0xCC, 0xFF
};

u8 opcao = 1;

vu8 pushButtonPA0 = 0;
vu8 SelectedWavesForm = 0;

void DAC_Main(void)
{
	while (1)
	{

		if(pushButtonPA0)
		{

			pushButtonPA0 = 0;
			DAC_DeInit();

			if(SelectedWavesForm)
			{
				DAC_Ch1_NoiseConfig();
				DAC_Ch2_TriangleConfig();
			}
			else
			{
				DAC_Ch1_EscalatorConfig();
				DAC_Ch2_SineWaveConfig();
			}
		}
		delay_ms(200);
		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
	}
}

/**
 * @brief  DAC  Channel2 SineWave Configuration
 * @param  None
 * @retval None
 */
void DAC_Ch2_SineWaveConfig(void)
{
	/* DAC channel2 Configuration */
	DAC_InitStructure.DAC_Trigger        = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer   = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* DMA1_Stream6 channel7 configuration **************************************/
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_Channel            = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) DAC_DHR12R2_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t) &Sine12bit32;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize         = 32;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

	/* Enable DMA1_Stream6 */
	DMA_Cmd(DMA1_Stream6, ENABLE);

	/* Enable DAC Channel2 */
	DAC_Cmd(DAC_Channel_2, ENABLE);

	/* Enable DMA for DAC Channel2 */
	DAC_DMACmd(DAC_Channel_2, ENABLE);
}

/**
 * @brief  DAC Channel1 Escalator Configuration
 * @param  None
 * @retval None
 */
void DAC_Ch1_EscalatorConfig(void)
{
	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger        = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer   = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* DMA1_Stream5 channel7 configuration **************************************/
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel            = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t) &Escalator8bit;
	DMA_InitStructure.DMA_BufferSize         = 6;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);

	/* Enable DMA1_Stream5 */
	DMA_Cmd(DMA1_Stream5, ENABLE);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	/* Enable DMA for DAC Channel1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/**
 * @brief  DAC Channel2 Triangle Configuration
 * @param  None
 * @retval None
 */
void DAC_Ch2_TriangleConfig(void)
{
	/* DAC channel2 Configuration */
	DAC_InitStructure.DAC_Trigger                      = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration               = DAC_WaveGeneration_Triangle;
	//DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
	DAC_InitStructure.DAC_OutputBuffer                 = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel2 */
	DAC_Cmd(DAC_Channel_2, ENABLE);

	/* Set DAC channel2 DHR12RD register */
	DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
}

/**
 * @brief  DAC  Channel1 Noise Configuration
 * @param  None
 * @retval None
 */
void DAC_Ch1_NoiseConfig(void)
{
	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger                      = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration               = DAC_WaveGeneration_Noise;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0;
	DAC_InitStructure.DAC_OutputBuffer                 = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);

	/* Set DAC Channel1 DHR12L register */
	DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
}

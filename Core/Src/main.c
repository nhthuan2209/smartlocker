/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7789.h"
#include "rc522.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct 
{
	uint8_t uid[4];
	char name[30];
	char ID[10];
}Card;

Card list_user[] = {
	{{0x1A,0x01,0x02,0x02}, "Nguyen Minh", "NV01" }, {{0x07,0xF6,0x29,0x07}, "Nguyen Hoc", "NV02"}
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ST7789_RST_PORT 		GPIOB
#define ST7789_RST_PIN  		GPIO_PIN_1
#define ST7789_DC_PORT  		GPIOB
#define ST7789_DC_PIN   		GPIO_PIN_0

#define ST7789_CS_PORT  		GPIOA
#define ST7789_CS_PIN   		GPIO_PIN_4
#define FONT_MENU						Font_16x26
#define LETTER_MENU					YELLOW
#define BACKGROUND_MENU			BLACK
#define FORMAT_RTC					RTC_FORMAT_BIN
#define FONT_RTC						Font_11x18
#define LINE_COLOR					WHITE
#define BUTTON_UP_PORT			GPIOA
#define BUTTON_UP_PIN				GPIO_PIN_11
#define BUTTON_DOWN_PORT		GPIOA
#define BUTTON_DOWN_PIN			GPIO_PIN_10
#define BUTTON_SELECT_PORT	GPIOA
#define BUTTON_SELECT_PIN		GPIO_PIN_15
#define SELECT_COLOR				GREEN
#define LED_PORT						GPIOA
#define LED_PIN							GPIO_PIN_12
#define BUTTON_BACK_PORT		GPIOB
#define BUTTON_BACK_PIN			GPIO_PIN_3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
uint8_t uid[4];
uint8_t card = 0;
uint8_t select_closet = 0;
uint8_t in_main_case = 1;
uint8_t in_waiting_case = 0;
uint8_t in_confirm_case = 0;
uint8_t in_locker_case = 0;
uint8_t in_return_case = 0;
uint8_t select_confirm = 0;
uint8_t select_choose = 0;
uint8_t select_yn     = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Button_Up()
{
	return HAL_GPIO_ReadPin(BUTTON_UP_PORT, BUTTON_UP_PIN) == GPIO_PIN_RESET;
}
uint8_t Button_Down()
{
	return HAL_GPIO_ReadPin(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN) == GPIO_PIN_RESET;
}
uint8_t Button_Select()
{
	return HAL_GPIO_ReadPin(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN) == GPIO_PIN_RESET;
}
uint8_t Button_Back()
{
	return HAL_GPIO_ReadPin(BUTTON_BACK_PORT, BUTTON_BACK_PIN) == GPIO_PIN_RESET;
}
void Led_On_Off(uint8_t state)
{
	if(state)
	{
		HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
	}
}

void Select_Closet()
{
	if(select_closet == 0)
	{
		ST7789_WriteString(10, 70, "[1]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(10, 90, "Empty", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
	}
	else if(select_closet == 1)
	{
		ST7789_WriteString(120, 70, "[2]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(120, 90, "Empty", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);		
	}
	else if(select_closet == 2)
	{
		ST7789_WriteString(230, 70, "[3]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(230, 90, "Empty", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);		
	}
	else if(select_closet == 3)
	{
		ST7789_WriteString(10, 120, "[4]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(10, 140, "Empty", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);	
	}
	else if(select_closet == 4)
	{
		ST7789_WriteString(120, 120, "[5]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(120, 140, "Used", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);	
	}
	else
	{
		ST7789_WriteString(230, 120, "[6]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
		ST7789_WriteString(230, 140, "Empty", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);	
	}
}
void Deselect_Closet(uint8_t choose_closet)
{
	switch(choose_closet)
	{
		case 1:
			ST7789_WriteString(10, 70, "[1]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(10, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);		
			break;
		case 2:
			ST7789_WriteString(120, 70, "[2]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(120, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
		case 3:
			ST7789_WriteString(230, 70, "[3]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(230, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
		case 4:
			ST7789_WriteString(10, 120, "[4]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(10, 140, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);	
			break;
		case 5:
			ST7789_WriteString(120, 120, "[5]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(120, 140, "Used", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);	
			break;
		case 6:
			ST7789_WriteString(230, 120, "[6]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			ST7789_WriteString(230, 140, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);	
			break;
	}
}
void Move_To_Confirm()
{
	if(Button_Down())
	{
		Deselect_Closet(select_closet + 1);
		if(select_closet < 5)
		{
			select_closet++;
		}
		Select_Closet();
		HAL_Delay(100);
	}
	if(Button_Up())
	{
		Deselect_Closet(select_closet + 1);
		if(select_closet > 0)
		{
			select_closet--;
		}
		Select_Closet();
		HAL_Delay(100);
	}
}

int8_t Find_Uid(uint8_t *uid)
{	
	for(int i = 0; i < 2; i++)
	{
		if(list_user[i].uid[0] == uid[0] && list_user[i].uid[1] == uid[1] && list_user[i].uid[2] == uid[2] && list_user[i].uid[3] == uid[3])
		{
			return i;
		}
	}
	return -1;
}

uint8_t Read_Card()
{
	if(MFRC522_Request(PICC_REQIDL, uid) == MI_OK)
	{
		if(MFRC522_Anticoll(uid) == MI_OK)
		{
			return 1;
		}
	}
	return 0;
}


void Start_Menu_Page()
{
	ST7789_WriteString(60, 10, "SMART LOCKER", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(40, 60, "RFID LOCK SYSTEM", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(60, 110, "SYSTEM READY", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	HAL_Delay(2000);
	ST7789_Fill_Color(BLACK);
	in_main_case = 1;
	in_waiting_case = 0;
	in_confirm_case = 0;
}
void Main_Menu_Page()
{
	ST7789_WriteString(60, 10, "SMART LOCKER", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	
	char time_save[15];
	char date_save[15];
	
	HAL_RTC_GetTime(&hrtc, &time, FORMAT_RTC);
	HAL_RTC_GetDate(&hrtc, &date, FORMAT_RTC);
	
	sprintf(time_save, "%02d:%02d:%02d", time.Hours, time.Minutes, time.Seconds);
	sprintf(date_save, "%02d/%02d/20%02d", date.Date, date.Month, date.Year);
	
	ST7789_WriteString(10, 35, time_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(200, 35, date_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_DrawLine(10, 60, 300, 60, LINE_COLOR);
	
	ST7789_WriteString(10, 70, "[1]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);

	ST7789_WriteString(120, 70, "[2]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(120, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_WriteString(230, 70, "[3]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(230, 90, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_WriteString(10, 120, "[4]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 140, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);

	ST7789_WriteString(120, 120, "[5]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(120, 140, "Used", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_WriteString(230, 120, "[6]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(230, 140, "Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
}

void Closet_Confirm(uint8_t locker_ps, uint8_t card)
{
	char name_save[20];
	char locker_save[10];
	
	sprintf(name_save, "%s", list_user[card].name);
	sprintf(locker_save, "LOCKER:%02d", locker_ps);
	
	ST7789_WriteString(30, 10, locker_save, FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_DrawLine(10, 30, 300, 30, LINE_COLOR);
	ST7789_WriteString(10, 40, "Status: Empty", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 60, "Name: ", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 80, name_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(100, 100, "Confirm ?", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
}
void Select_Yes_Or_No(void)
{
	if(select_confirm == 0)
	{
		ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
	}
	else
	{
		ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);	
	}
}
void Deselect_Yes_No(uint8_t confirm)
{
	switch(confirm)
	{
		case 1:
			ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
		case 2:
			ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
	}
}

void Move_Yes_Or_No(void)
{
	if(Button_Down())
	{
		Deselect_Yes_No(select_confirm + 1);
		if(select_confirm < 1)
		{
			select_confirm++;
		}
		Select_Yes_Or_No();
	}
	
	if(Button_Up())
	{
		Deselect_Yes_No(select_confirm + 1);
		if(select_confirm > 0)
		{
			select_confirm--;
		}
		Select_Yes_Or_No();
	}
}
void Opening_Closet_Page(uint8_t locker, uint8_t t)
{
	ST7789_Fill_Color(BLACK);
	char locker_save[10];
	sprintf(locker_save, "LOCKER:%02d", locker);
	
	ST7789_WriteString(30, 10, locker_save, FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_DrawLine(10, 30, 300, 30, LINE_COLOR);
	
	ST7789_WriteString(30, 60, "ACCESS GRANTED", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(30, 100, "OPENING....", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(30, 150, "PLEASE WAIT", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	
	HAL_Delay(2000);
	ST7789_Fill_Color(BLACK);
	
	ST7789_WriteString(40, 10, "USER LOCKER", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	
	char name_save[20];
	
	sprintf(name_save, "Name:%s", list_user[t].name);
	ST7789_WriteString(20, 40, name_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 60, locker_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 80, "Status: Closed", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 100, " [OPEN LOCKER]", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 130, "[RETURN LOCKER]", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
}
void Opening_Locker_On_Led()
{
	Led_On_Off(1);
	ST7789_Fill_Color(BLACK);
	HAL_Delay(3000);
	in_main_case = 1;
	in_waiting_case = 0;
	in_locker_case = 0;
}
void Return_Locker(uint8_t locker, uint8_t t)
{
	ST7789_Fill_Color(BLACK);
	ST7789_WriteString(40, 10, "RETURN LOCKER", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	
	char locker_save[10];
	sprintf(locker_save, "LOCKER:%02d", locker);

	char name_save[20];
	sprintf(name_save, "Name:%s", list_user[t].name);
	ST7789_WriteString(20, 60, name_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 40, locker_save, FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	
	ST7789_WriteString(20, 100, "ARE YOU SURE ?", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);	
}

void Select_Open_Or_Return(void)
{
	if(select_choose == 0)
	{
		ST7789_WriteString(20, 100, " [OPEN LOCKER]", FONT_MENU, SELECT_COLOR, BACKGROUND_MENU);
	}
	else
	{
		ST7789_WriteString(20, 130, "[RETURN LOCKER]", FONT_MENU, SELECT_COLOR, BACKGROUND_MENU);
	}
}
void Deselect_Open_Return(uint8_t confirm)
{
	switch(confirm)
	{
		case 1:
			ST7789_WriteString(20, 100, " [OPEN LOCKER]", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
			break;
		case 2:
			ST7789_WriteString(20, 130, "[RETURN LOCKER]", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
			break;
	}
}
void Move_Open_Or_Return(void)
{
	if(Button_Down())
	{
		Deselect_Open_Return(select_choose + 1);
		if(select_choose < 1)
		{
			select_choose++;
		}
		Select_Open_Or_Return();
	}
	
	if(Button_Up())
	{
		Deselect_Open_Return(select_choose + 1);
		if(select_choose > 0)
		{
			select_choose--;
		}
		Select_Open_Or_Return();
	}
}

void Confirm_Yes_No()
{
	if(Button_Select())
	{
		switch(select_confirm)
		{
			case 0:
				ST7789_Fill_Color(BLACK);
				Opening_Closet_Page(select_closet + 1, card);
				in_main_case = 0;
				in_waiting_case = 0;
				in_confirm_case = 0;
				in_locker_case = 1;
				break;
			case 1:
				ST7789_Fill_Color(BLACK);
				Main_Menu_Page();
				in_main_case = 1;
				in_waiting_case = 0;
				in_confirm_case = 0;
				break;
		}
	}
}

void Check_Card()
{
	if(Read_Card())
	{
		int8_t card_check = Find_Uid(uid);
		if(card_check != -1)
		{
			card = card_check;
			in_waiting_case = 0;
			in_confirm_case = 1;
			ST7789_Fill_Color(BLACK);
			Closet_Confirm(select_closet + 1, card_check);
		}
		else
		{
			
		}
	}
}

void Select_Yes_Or_No_Return(void)
{
	if(select_yn == 0)
	{
		ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);
	}
	else
	{
		ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, SELECT_COLOR, BACKGROUND_MENU);	
	}
}
void Deselect_Yes_No_Return(uint8_t confirm)
{
	switch(confirm)
	{
		case 1:
			ST7789_WriteString(20, 125, "[ YES ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
		case 2:
			ST7789_WriteString(220, 125, "[ NO ]", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
			break;
	}
}

void Move_Yes_Or_No_Return(void)
{
	if(Button_Down())
	{
		Deselect_Yes_No_Return(select_yn + 1);
		if(select_yn < 1)
		{
			select_yn++;
		}
		Select_Yes_Or_No_Return();
	}
	
	if(Button_Up())
	{
		Deselect_Yes_No_Return(select_yn + 1);
		if(select_yn > 0)
		{
			select_yn--;
		}
		Select_Yes_Or_No_Return();
	}
}

void Return_Locker_Done(uint8_t locker)
{
	ST7789_Fill_Color(BLACK);
	
	char locker_save[20];
	sprintf(locker_save, "Locker:%02d", locker);
	
	ST7789_WriteString(20, 20, locker_save, FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 70, "RELEASE SUCCESS", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 140, "Locker is EMPTY now", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	HAL_Delay(2000);
	ST7789_Fill_Color(BLACK);
	in_main_case = 1;
	in_waiting_case = 0;
	in_confirm_case = 0;
}

void Confirm_Yes_No_Return()
{
	if(Button_Select())
	{
		switch(select_yn)
		{
			case 0:
				ST7789_Fill_Color(BLACK);
				Return_Locker_Done(select_closet + 1);
				in_locker_case = 0;
				in_return_case = 0;
				break;
			case 1:
				ST7789_Fill_Color(BLACK);
				Opening_Closet_Page(select_closet + 1, card);
				select_choose = 0;
				Select_Open_Or_Return();
				in_main_case = 0;
				in_locker_case = 1;
				in_return_case = 0;
				break;
		}
	}
}

void Confirm_Open_Return()
{
	if(Button_Select())
	{
		switch(select_choose)
		{
			case 0:
				ST7789_Fill_Color(BLACK);
				Opening_Locker_On_Led();
				break;
			case 1:
				ST7789_Fill_Color(BLACK);
				Return_Locker(select_closet + 1, card);
				select_yn = 0; 
				Select_Yes_Or_No_Return();
				in_locker_case = 0;
				in_return_case = 1; 
				break;
		}
	}
}

void Check_Card_Page(void)
{
	ST7789_Fill_Color(BLACK);
	ST7789_WriteString(20, 20, "RFID SCANNING", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(140, 60, "[RFID]", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(20, 100, "PLEASE SCAN CARD", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(50, 140, "WAITING....", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
}
void Used_Close_Page(void)
{
	ST7789_Fill_Color(BLACK);
	ST7789_WriteString(30, 10, "ACCESS DENIED", FONT_MENU, LETTER_MENU, BACKGROUND_MENU);
	ST7789_DrawLine(10, 40, 300, 30, LINE_COLOR);
	ST7789_WriteString(10, 60, "Locker: 05", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 90, "Owner: ", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 120, "Nguyen Nguyen", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	ST7789_WriteString(10, 150, "---Back Home Menu---", FONT_RTC, LETTER_MENU, BACKGROUND_MENU);
	HAL_Delay(2000);
	ST7789_Fill_Color(BLACK);
	in_main_case = 1;
	in_waiting_case = 0;
}
void First_Closet(void)
{
	Check_Card_Page();
}
void Second_Closet(void)
{
	Check_Card_Page();
}
void Third_Closet(void)
{
	Check_Card_Page();
}
void Fourth_Closet(void)
{
	Check_Card_Page();
}
void Fifth_Closet(void)
{
	Used_Close_Page();
}
void Sixth_Closet(void)
{
	Check_Card_Page();
}
void Confirm_Locker()
{
	if(Button_Select())
	{
		HAL_Delay(100);
		switch(select_closet)
		{
			case 0:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				First_Closet();
				break;
			case 1:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				Second_Closet();
				break;
			case 2:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				Third_Closet();
				break;
			case 3:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				Fourth_Closet();
				break;
			case 4:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				Fifth_Closet();
				break;
			case 5:
				in_main_case = 0;
				in_waiting_case = 1;
				in_confirm_case = 0;
				in_locker_case = 0;
				Sixth_Closet();
				break;
		}
	}
}
void Back_Home()
{
	if(Button_Back())
	{
		ST7789_Fill_Color(BLACK);
		Main_Menu_Page();
		
		in_main_case = 1;
		in_waiting_case = 0;
		in_confirm_case = 0;
		in_locker_case = 0;
		in_return_case = 0;

		select_confirm = 0;
		select_choose = 0;
		select_yn = 0;		
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	ST7789_Fill_Color(BLACK);
	ST7789_Init();
	MFRC522_Init();
	Start_Menu_Page();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(in_main_case)
		{
			Move_To_Confirm();
			Select_Closet();
			Main_Menu_Page();
			Confirm_Locker();
		}
		else if(in_waiting_case)
		{
			Check_Card();
		}
		else if(in_confirm_case)
		{
			Move_Yes_Or_No();
			Select_Yes_Or_No();
			Confirm_Yes_No();
			Back_Home(); 
		}
		else if(in_locker_case)
		{
			Move_Open_Or_Return();
			Select_Open_Or_Return();
			Confirm_Open_Return();
			Back_Home(); 
		}
		else if(in_return_case)
		{
			Move_Yes_Or_No_Return();
			Select_Yes_Or_No_Return();
			Confirm_Yes_No_Return();
			Back_Home(); 
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x8;
  sTime.Minutes = 0x30;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_AUGUST;
  DateToUpdate.Date = 0x13;
  DateToUpdate.Year = 0x26;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 PA8 PA9
                           PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

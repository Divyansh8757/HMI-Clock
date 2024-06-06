// FUNCTION DEFINITIONS

/* USER CODE BEGIN PFP */
void mdelay(uint16_t tim);
void lcd_Init();
void lcd_send_cmd(char data);
void lcd_send_data(char data);
void lcd_send_string (char *str);
void lcd_set_cursor(int row, int col);
void lcd_clear();
void set_time(uint8_t hour, uint8_t* minute, char tm, bool temp_set_t, bool temp_set_a);
void display_time(uint8_t hout, uint8_t* minute, char tm);
void enter(uint8_t new_hour, uint8_t* new_minute, char new_tm, bool new_set_t, bool new_set_a);
void sound_alarm();
/* USER CODE END PFP */

// VARIABLE DECLARATIONS

/* USER CODE BEGIN 0 */
uint8_t hour=0;
uint8_t minute[2]={0,0};
int row = 1;
char tm='A';
char Time[7];
bool set_t = false;
bool set_a = false;
bool alarm = false;
uint8_t a_hour=0;
uint8_t a_minute[2]={0,0};
char a_tm;
int sec=0;
/* USER CODE END 0 */

// MAIN FUNCTION

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  SystemPower_Config();
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();             //For milliseconds delay
  MX_TIM7_Init();             //To count in seconds

  HAL_TIM_Base_Start(&htim6);
  HAL_TIM_Base_Start(&htim7);
  lcd_Init();
  set_time(hour,minute,tm,set_t,set_a);

// WHILE LOOP
  
while (1)
  {
	  if(set_t==true){
		  set_time(hour,minute,tm,set_t,set_a);
	  }
	  else if(set_a==true){
		  set_time(hour,minute,tm,set_t,set_a);
	  }
	  else if((alarm==true)&&(hour==a_hour)&&(minute[0]==a_minute[0])&&(minute[1]==a_minute[1])&&(tm==a_tm)){
		  sound_alarm();
	  }
	  else{
		  if(TIM7->CNT>=999){
			  sec++;
		  }
		  else if(sec>59){
			  sec=0;
			  minute[1]++;
		  }
		  else if(minute[1]>9){
			  minute[1]=0;
			  minute[0]++;
			  lcd_Init();
		  }
		  else if(minute[0]>5){
			  hour++;
			  minute[0]=0;
			  if((hour>11) && (minute[1]==0) && (minute[0]=0)){
				  if(tm=='A')
					  tm='P';
				  else if(tm=='P')
					  tm='A';
			  }
		  }
		  else if(hour>12){
			  hour=1;
		  }

		  display_time(hour,minute,tm);
	  }
  }
}

// DELAY FUNCTION

void mdelay(uint16_t tim){
	__HAL_TIM_SET_COUNTER(&htim6,0);
	while(__HAL_TIM_GET_COUNTER(&htim6)<tim);
}

// LCD INTERFACING FUNCTIONS

void lcd_send_cmd(char data){
	uint8_t cmd[2];
	cmd[0] &= 0x00;
	cmd[1] &= 0x00;
	cmd[1] |= data;
	HAL_I2C_Master_Transmit(&hi2c1, slvaddr, cmd, 2, 100);
}

void lcd_send_data(char data){
	uint8_t cmd[2];
	cmd[0] &= 0x00;
	cmd[0] |= 0x40;
	cmd[1] &= 0x00;
	cmd[1] |= data;
	HAL_I2C_Master_Transmit(&hi2c1, slvaddr, cmd, 2, 100);
}

void lcd_clear(){
	lcd_send_cmd(0x01);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void lcd_set_cursor(int row, int col){
	switch(row){
	case 1:
		col |= 0x80;
		break;
	case 2:
		col |= 0xC0;
		break;
	}
	lcd_send_cmd(col);
	col=0;
}

void lcd_Init(){
	lcd_send_cmd(0x2C);
	mdelay(1);
	lcd_send_cmd(0x0C);
	mdelay(1);
	lcd_send_cmd(0x01);
	mdelay(1);
	lcd_send_cmd(0x06);
	mdelay(1);
}

// FUNCTION TO DISPLAY TIME

void display_time(uint8_t hour, uint8_t* minute, char tm){
	lcd_set_cursor(1,0);
	if(hour<10){
		sprintf(Time, "0%d:%d%d%c", hour, minute[0],minute[1],tm);
		lcd_send_string(Time);
	}
	else{
		sprintf(Time, "%d:%d%d%c", hour, minute[0],minute[1],tm);
		lcd_send_string(Time);
	}
}

// FUNCTION TO SOUND ALARM AND USE OF SNOOZE AND SLEEP KEYS

void sound_alarm(){
	while((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0) || (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0)){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1);
		lcd_Init();
		HAL_Delay(1);
		lcd_send_string("ALARM!");
		HAL_Delay(1000);
		lcd_clear();
		HAL_Delay(500);
	}
	if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==1)){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
		a_minute[1]+=5;
		if(a_minute[1]>9){
			a_minute[1]=a_minute[1]-10;
			a_minute[0]++;
		}
		else if(a_minute[0]>5){
			a_minute[0]=0;
			a_hour++;
		}
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==1){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
		alarm=false;
		a_hour=0;
		a_minute[0]=0;
		a_minute[1]=0;
	}
}

// FUNCTION TO SET TIME ***MOST IMPORTANT PART***

void set_time(uint8_t hour, uint8_t* minute,char tm, bool temp_set_t, bool temp_set_a){
	lcd_Init();
	mdelay(1);
	display_time(hour,minute,tm);
	uint8_t temp_hour = hour;
	uint8_t temp_minute[2]={'0'};
	char temp_tm=tm;
	temp_minute[0]=minute[0];
	temp_minute[1]=minute[1];
	lcd_send_cmd(0x0f);
	mdelay(1);
	lcd_set_cursor(1,1);
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1){
		if(row==1){
			while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
				// Wait for arrow keys to be pressed
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1 && (row==1)){
				if(temp_hour==12){
					temp_hour=1;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_hour++;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
			else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==1)){
				if(temp_hour==1){
					temp_hour=12;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_hour--;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
		}
		else if(row==3){
			while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
				// Wait for arrow keys to be pressed
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1 && (row==3)){
				if(temp_minute[0]==5){
					temp_minute[0]=0;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_minute[0]++;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
			else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==1 && (row==3)){
				if(temp_minute[0]==0){
					temp_minute[0]=5;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				else{
					temp_minute[0]--;
					display_time(temp_hour,temp_minute,temp_tm);
					lcd_set_cursor(1,row);
				}
				mdelay(200);
			}
		}
			else if(row==4){
				while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
					// Wait for arrow keys to be pressed
				}
				if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1) && (row==4)){
					if(temp_minute[1]==9){
						temp_minute[1]=0;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else{
						temp_minute[1]++;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
				else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==4)){
					if(temp_minute[1]==0){
						temp_minute[1]=9;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else{
						temp_minute[1]--;
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
			}
			else if(row==5){
				while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0) && (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==0) && (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)!=1)){
					// Wait for arrow keys to be pressed
				}
				if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1) && (row==5)){
					if(temp_tm=='A'){
						temp_tm='P';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else if(temp_tm=='P'){
						temp_tm = 'A';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
				else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1 && (row==5)){
					if(temp_tm=='A'){
						temp_tm='P';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					else if(temp_tm=='P'){
						temp_tm = 'A';
						display_time(temp_hour,temp_minute,temp_tm);
						lcd_set_cursor(1,row);
					}
					mdelay(200);
				}
			}
	}
	enter(temp_hour, temp_minute, temp_tm, temp_set_t, temp_set_a);
}

// FUNCTION FOR ENTER KEY 

void enter(uint8_t new_hour, uint8_t* new_minute, char new_tm, bool new_set_t, bool new_set_a){
	lcd_Init();
	row=1;
	mdelay(500);
	if(new_set_t){
		hour=new_hour;
		minute[0]=new_minute[0];
		minute[1]=new_minute[1];
		tm = new_tm;
		display_time(hour,minute,tm);
		set_t = false;
	}
	else if(new_set_a){
		a_hour=new_hour;
		a_minute[0]=new_minute[0];
		a_minute[1]=new_minute[1];
		a_tm=new_tm;
		set_a=false;
		alarm=true;
	}
	else{
		hour=new_hour;
		minute[0]=new_minute[0];
		minute[1]=new_minute[1];
		tm = new_tm;
		display_time(hour,minute,tm);
}
}

// INTERRUPT CALLBACK FUNCTION

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==GPIO_PIN_0){
		if(row==1){
			row=5;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if (row==3){
			row-=2;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if (row==4){
			row-=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==5){
			row-=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		mdelay(100);
	}
	else if((GPIO_Pin==GPIO_PIN_1)){
		if(row==1){
			row+=2;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==3){
			row+=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==4){
			row+=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		else if(row==5){
			row=1;
			lcd_set_cursor(1,row);
			lcd_send_cmd(0x0f);
		}
		mdelay(100);
	}
	else if(GPIO_Pin==GPIO_PIN_8){
		set_t = true;
		set_a = false;
	}
	else if(GPIO_Pin==GPIO_PIN_9){
		set_t = false;
		set_a = true;
		alarm = true;
	}
}

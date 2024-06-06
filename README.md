# HMI Clock
## I.) Purpose of the project.
The purpose of this project is to develop a Human Machine Interaction Interface. 
Hello all. Welcome to my project of HMI Clock. The intension behind this project was to implement and demonstrate the concepts that I have learned thus far since I started on this wonderful and exciting journey of becoming a product developer.
So I thought of creating a simple machine with which any user can interact in real-time. And what other than an Alarm Clock would be a simpler machine!

Earlier when I started with this project, I was calling it an 'Alarm Clock'. But then I thought that the name itself should be self-explainatory of the intent of the project, and hence the name - 'HMI Clock'.


## II.) Scope of the Project.
The aim of this project is to develop a simple machine that will run independently on a firmware application that will be developed on STM32 nucleo board. The main objective is to create the machine interactive to users.

This project can also act as a guide to those who are learning embedded firmware development like me. It can be very helpful with learning perspective as it deals with various concepts like interrupts, I2C communication, timers, push buttonms, LCD interfacing and a few more.


## III.) Components Used
1. STM32U545 Nucleo development board.
2. Push buttons (9 in total) to interact with the clock.
3. I2C LCD to display time and information.
4. A buzzer for alarm sound.
5. Bread board, connecting wires and jumper cables.


## IV.) Product Images

1. Complete Produuct.
![WhatsApp Image 2024-06-05 at 17 47 42](https://github.com/Divyansh8757/HMI-Clock-/assets/166917600/8d5b4d16-5fd1-47cf-a16a-bab81fc60e69)

2. Buttons arrangement.
![WhatsApp Image 2024-06-05 at 17 47 42 (2)](https://github.com/Divyansh8757/HMI-Clock-/assets/166917600/7e8967dc-b974-40ae-9c9b-b168d8e71c8e)

3. Arrow keys.
![WhatsApp Image 2024-06-05 at 17 47 43 (1)](https://github.com/Divyansh8757/HMI-Clock-/assets/166917600/dd130ab1-c7f4-4f0b-bf7f-aff110a447e5)
The rectangle push buttons are arrow keys. I hope the placement of the keys are self explainatory. The push button to the left of the arrow keys is 'Enter' key. The functions of the keys are explained later in this document.

4. Function Keys.
![WhatsApp Image 2024-06-05 at 17 47 42 (1)](https://github.com/Divyansh8757/HMI-Clock-/assets/166917600/5110f0f1-a173-4fe6-b2c3-42cb5f038df7)
The push button to the left most is 'Set Time' key, then next one is 'Set Alarm' key, the next to it is 'Snooze' key, and the rightmost one is 'Sleep' key. Functions of the keys are explained later in the document.


## V.) Pin Mappings and their modes.

| Keys            | Pin    | Mode   |
| ----------------|:-------| -----: |
| Left arrow key  | PA0    | EXTI   |
| Right arrow key | PA1    | EXTI   |
| Up arrow key    | PA2    | Input  |
| Down arrow key  | PA3    | Input  |
|                 |        |        |
| Enter key       | PC0    | Input  |
| Set time key    | PC8    | EXTI   |
| Set alarm key   | PC9    | EXTI   |
| Snooze key      | PB1    | Input  |
| Sleep key       | PB2    | Input  |
|                 |        |        |
| Buzzer          | PB5    | Output |

## VI.) Key Functions.

1. The left and right arrow keys are used to shift the cursor position to left or right. Cursor position shifting is cyclic, i.e. if the the cursor is at the leftmost position and we press the left arrow key, it will shift to the rightmost position.
2. The Up and down arrow keys are used to increase and decrease the value of hour and minutes corresponding to the cursor position.
3. The enter key is used to set and exit the set time mode and set alarm mode.
4. The Set time key is used to set the time on the clock. And once enter is pressed, the time is set and the clock exits from set time mode.
5. The Set alarm mode is used to set an alarm for desired hour and minute. Once enter is pressed the alarm is set and the clock exits from set alarm mode.
6. Snooze button shifts the alarm by 5 minutes, once a set alarm goes off.
7. Sleep button switches off the alarm.


## VII.) Flow Structure
1. Initially when the clock is powered on, it goes into set time mode. Then the user sets the time and presses enter after which  the clock starts normal functioning.
2. Then the required function key can be pressed to set time and alarm further.



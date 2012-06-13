/*********************************
* Name: control.h
* Desc: Header for Control Law
* Date: 2009-10-30
* Author: stanbaek
*********************************/

unsigned char Control_IsAutoPilot(void);
void Control_Steering(Target* target);
void Control_ManualOperation(void);



/**************************************************************************
 *Authour	:	Ben Haubrich																									*
 *File		:	handlers.c																										*
 *Synopsis:	fault handlers																								*
 *Date		: May 18th, 2019																									*
 **************************************************************************/
#include <tm4c123gh6pm.h>

void nmi_handler() {
	while(1);
}
void hfault_handler() {
	int intnum = (NVIC_DBG_XFER_REG_FLAGS & 0xFF);
	/* Pg.183, datasheet */
	int statdbg = (NVIC_HFAULT_STAT_R & NVIC_HFAULT_STAT_DBG);
	int statf = (NVIC_HFAULT_STAT_R & NVIC_HFAULT_STAT_FORCED);
	int statv = (NVIC_HFAULT_STAT_R & NVIC_HFAULT_STAT_VECT);
	while(1);
}
void mm_handler() {
	while(1);
}
void b_handler() {
	while(1);
}
void u_handler() {
	while(1);
}
void svc_handler() {
	while(1);
}
void dm_handler() {
	while(1);
}
void psv_handler() {
	while(1);
}
void syst_handler() {
	while(1);
}

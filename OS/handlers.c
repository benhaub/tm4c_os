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
	int faultstat_vect, faultstat_forced, faultstat_dbg;
	faultstat_vect  = (NVIC_HFAULT_STAT_R & 1 << 1);
	faultstat_forced = (NVIC_HFAULT_STAT_R & 1 << 30);
	faultstat_dbg = (NVIC_HFAULT_STAT_R & 1 << 31);
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

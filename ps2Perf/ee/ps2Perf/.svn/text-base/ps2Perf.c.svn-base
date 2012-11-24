/*
  _____     ___ ____
   ____|   |    ____|      PS2 OpenSource Project
  |     ___|   |____       (C)2002,Mavy & Bigboss ( mavy@ps2reality.net, 
												    bigboss@ps2reality.net )
  ------------------------------------------------------------------------
 Ps2Perf.c  PS2 Performance  Some functions to use performance counters of EE
 
  All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  provided that the above copyright notice(s) and this permission notice appear
  in all copies of the Software and that both the above copyright notice(s)
  and this permission notice appear in supporting documentation.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
  LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
  IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
  OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

  Except as contained in this notice, the name of a copyright holder shall not
  be used in advertising or otherwise to promote the sale, use or other dealings
  in this Software without prior written authorization of the copyright holder.  
*/



#include <ps2Perf.h>
#include <stdio.h>
#include <malloc.h>
#include <fileio.h>
#define SENDTOPCCR(x)	__asm __volatile (" mtps %0,0 \n\t SYNC.P" : : "r"(x) : "memory");
#define GETFROMPCCR(x)	__asm __volatile (" mfps %0,0 " : "=r"(x) :"r"(x) : "memory"); 
#define CLEARPCCR	__asm __volatile (" mtps $0,0 \n\t SYNC.P" : : : "memory");
#define CLEARPC0	__asm __volatile (" mtpc $0,0 \n\t SYNC.P" : : : "memory");
#define GETPC0(x)	__asm __volatile (" mfpc %0,0 \n\t SYNC.P" :"=r"(x) :"r"(x) : "memory");
#define CLEARPC1	__asm __volatile (" mtpc $0,1 \n\t SYNC.P" : : : "memory");
#define GETPC1(x)	__asm __volatile (" mfpc %0,1 \n\t SYNC.P" :"=r"(x) :"r"(x) : "memory");

void initPs2Perf ()
{
				
		CLEARPCCR; // Clean PCCR Control Register (stop it)
		
	
}

void startPs2Perf ()
{ 
	PCCR_Register reg_temp;
	
	GETFROMPCCR (reg_temp);
	reg_temp.CTE = 1;		// Send to Start
	SENDTOPCCR (reg_temp);
}


void stopPs2Perf ()
{ 
	PCCR_Register reg_temp;
	
	GETFROMPCCR (reg_temp);
	reg_temp.CTE = 0;		// Send to Stop
	SENDTOPCCR (reg_temp);
}


void initCounter (void * counters,char type)
{

	if (type == PC0)
		memset (counters,0,sizeof(Counters0));
	else if (type == PC1)
		memset (counters,0,sizeof(Counters1));
}


/**
	Stop PC's, read PCx, add to correct counter, put PCx = 0, and restart PC's
	Type = PC0 or PC1
*/
void flushCounter (void * counters,char type,char reStart)
{
	Counters0 *c0;
	Counters1 *c1;
	long value;
	PCCR_Register reg_temp;
	GETFROMPCCR (reg_temp);
	reg_temp.CTE = 0;  		// Stop Counters
	SENDTOPCCR (reg_temp);

	if (type == PC0)
	{
		c0 = counters;
		GETPC0(value);
		
		CLEARPC0;
		switch (reg_temp.EVENT0)
		{
		case EV0_ProcessorCycle 			:	c0->ProcessorCycle				+=value;break;
		case EV0_SingleInstructionIssue		:	c0->SingleInstructionIssue		+=value;break;
		case EV0_BranchIssued				:	c0->BranchIssued				+=value;break;
		case EV0_BTACmiss					:	c0->BTACmiss					+=value;break;
		case EV0_ITLBmiss					:	c0->ITLBmiss					+=value;break;	
		case EV0_InstructionCacheMiss		:	c0->InstructionCacheMiss		+=value;break;
		case EV0_AccessToDTLB				:	c0->AccessToDTLB				+=value;break;
		case EV0_nonBlockingLoad			:	c0->nonBlockingLoad				+=value;break;
		case EV0_WBBSingleRequest			:	c0->WBBSingleRequest			+=value;break;
		case EV0_WBBBurstRequest			:	c0->WBBBurstRequest				+=value;break;		
		case EV0_CPUAdressBusBusy			:	c0->CPUAdressBusBusy			+=value;break;	
		case EV0_InstructionCompleted		:	c0->InstructionCompleted		+=value;break;	
		case EV0_NonBDSInstrucionCompleted	: 	c0->NonBDSInstrucionCompleted	+=value;break;		
		case EV0_COP2InstructionCompleted	: 	c0->COP2InstructionCompleted	+=value;break;
		case EV0_LoadCompleted				: 	c0->LoadCompleted				+=value;break;
		default : CLEARPC0;
		}	
		
	}	
	else if (type == PC1)
	{
		c1 = counters;
		GETPC1(value);
		
		CLEARPC1;
		switch (reg_temp.EVENT1)
		{
		case EV1_LowOrderBranchIssued		:	c1->LowOrderBranchIssued			+=value; break;
		case EV1_ProcessorCycle   			:	c1->ProcessorCycle					+=value;break;
		case EV1_DualInstructionIssue		:	c1->DualInstructionIssue			+=value;break;
		case EV1_BranchMispredicted			:	c1->BranchMispredicted				+=value;break;
		case EV1_TLBmiss					:	c1->TLBmiss							+=value;break;	
		case EV1_DTLBmiss					:	c1->DTLBmiss						+=value;break;
		case EV1_DataCacheMiss				:	c1->DataCacheMiss					+=value;break;
		case EV1_WBBSingleRequestUnavailable:	c1->WBBSingleRequestUnavailable		+=value;break;
		case EV1_WBBBurstRequestUnavailable	:	c1->WBBBurstRequestUnavailable		+=value;break;
		case EV1_WBBBurstRequestAlmostFull	:	c1->WBBBurstRequestAlmostFull		+=value;break;		
		case EV1_WBBBurstRequestFull		:	c1->WBBBurstRequestFull				+=value;break;	
		case EV1_CPUDataBusBusy				:	c1->CPUDataBusBusy					+=value;break;	
		case EV1_InstructionCompleted		: 	c1->InstructionCompleted			+=value;break;		
		case EV1_NonBDSInstrucionCompleted	: 	c1->NonBDSInstrucionCompleted		+=value;break;
		case EV1_COP1InstructionCompleted	: 	c1->COP1InstructionCompleted		+=value;break;
		case EV1_StoreCompleted				:   c1->StoreCompleted					+=value;break;
		default : CLEARPC1;
		}	
	}
	if (reStart==1) reg_temp.CTE = 1;  		// ReStart Counters
	SENDTOPCCR (reg_temp);
	
}

/* It will stop counters, and clear PC0, (re)Start Manually */
void event0Select 	(char event0)
{
	PCCR_Register reg_temp;
	GETFROMPCCR (reg_temp);
	reg_temp.CTE = 0;  		// Stop Counters
	SENDTOPCCR (reg_temp);
	
	reg_temp.EVENT0 = event0;
	CLEARPC0;
	SENDTOPCCR (reg_temp);
}

/* It will stop counters, and clear PC1, (re)Start Manually */
void event1Select 	(char event1)
{
	PCCR_Register reg_temp;
	GETFROMPCCR (reg_temp);
	reg_temp.CTE = 0;  		// Stop Counters
	SENDTOPCCR (reg_temp);
	
	reg_temp.EVENT1 = event1;
	CLEARPC1;
	SENDTOPCCR (reg_temp);
}

/* It will stop counters, (re)Start Manually */
void mode0Select 	(char level1,char kernel,char supervisor, char user)
{
	PCCR_Register reg_temp;
	GETFROMPCCR (reg_temp);	
	reg_temp.CTE = 0;  		// Stop Counters
	SENDTOPCCR (reg_temp);
	reg_temp.EXL0 = level1;
	reg_temp.K0 = kernel;
	reg_temp.S0 = supervisor;
	reg_temp.U0 = user;
	
	SENDTOPCCR (reg_temp);
}


/* It will stop counters, (re)Start Manually */
void mode1Select 	(char level1,char kernel,char supervisor, char user)
{
	PCCR_Register reg_temp;
	GETFROMPCCR (reg_temp);	
	reg_temp.CTE = 0;  		// Stop Counters
	SENDTOPCCR (reg_temp);
	reg_temp.EXL1 = level1;
	reg_temp.K1 = kernel;
	reg_temp.S1 = supervisor;
	reg_temp.U1 = user;
	
	SENDTOPCCR (reg_temp);
}

char * DumpInfo (Counters0 *c0,Counters1 *c1)
{
	char * output;
	output = (char *) malloc (sizeof(char)*1024);
	sprintf (output,"\n[PC0] C1: %d C2: %d C3: %d C4: %d C5: %d C6: %d C7: %d\nC8: %d C9: %d C10: %d C11: %d C12: %d C13: %d C14: %d C15: %d \n[PC1] C0: %d C1: %d C2: %d C3: %d C4: %d C5: %d C6: %d C7: %d \nC8: %d C9: %d C10: %d C11: %d C12: %d C13: %d C14: %d C15: %d\n",
	c0->ProcessorCycle,   				
	c0->SingleInstructionIssue,		
	c0->BranchIssued,					
	c0->BTACmiss,						
	c0->ITLBmiss,						
	c0->InstructionCacheMiss,			
	c0->AccessToDTLB,					
	c0->nonBlockingLoad,				
	c0->WBBSingleRequest,					
	c0->WBBBurstRequest,				
	c0->CPUAdressBusBusy,				
	c0->InstructionCompleted,		
	c0->NonBDSInstrucionCompleted,		
	c0->COP2InstructionCompleted,		
	c0->LoadCompleted,
	c1->LowOrderBranchIssued,			
	c1->ProcessorCycle,   				
	c1->DualInstructionIssue,			
	c1->BranchMispredicted,			
	c1->TLBmiss,					
	c1->DTLBmiss,						
	c1->DataCacheMiss,					
	c1->WBBSingleRequestUnavailable,	
	c1->WBBBurstRequestUnavailable,	
	c1->WBBBurstRequestAlmostFull,		
	c1->WBBBurstRequestFull,			
	c1->CPUDataBusBusy,				
	c1->InstructionCompleted,		
	c1->NonBDSInstrucionCompleted,		
	c1->COP1InstructionCompleted,		
	c1->StoreCompleted);			

	return (output);
	
}
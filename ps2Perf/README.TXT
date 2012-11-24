
 Ps2Perf	 PS2 Performance Counters Functions
 ====================================================
 
 Some functions to use performance counters of EE 
 
 
* Why Use Them ?
 	
 		- If our program is using some kind of synchronization, 
 		we need something to know if we're making our functions faster or slower when we are modifying them.
 		Just use ProcessorCycle Event and you will know it :D.
 		
* What isn't for ?
		- Finetuning, for some events or exact count, counters should be initialized to something different than 0.

 
* How Use Them ?
		Well, it's no a patching utility , we need to put some instructions before&after functions.
		
		1 - Include in your project ps2Perf.h & ps2Perf.c
		2 - Set a Counters0, Counters1 structs in your code to save ps2Perf data.
		3 - initPs2Perf (); --> Just clear & stop Perfomance Counter Control Register (PCCR)
		4 - initCounter (&counter,PC0 or PC1); --> initialize counter's struct to 0
		5 - Select what you want to count 
			mode0Select 	(1,1,1,1);  Count on { Level 1 mode, kernel mode , supervisor mode, user mode }
			mode1Select 	(1,1,1,1);  
			
			event0Select 	(EVENT0);
			event1Select 	(EVENT1);
			
			Where EVENT0 can be (1..15) :
				EV0_ProcessorCycle 				
				EV0_SingleInstructionIssue			
				EV0_BranchIssued					
				EV0_BTACmiss						
				EV0_ITLBmiss						
				EV0_InstructionCacheMiss			
				EV0_AccessToDTLB					
				EV0_nonBlockingLoad				
				EV0_WBBSingleRequest				
				EV0_WBBBurstRequest				
				EV0_CPUAdressBusBusy				
				EV0_InstructionCompleted			
				EV0_NonBDSInstrucionCompleted		
				EV0_COP2InstructionCompleted		
				EV0_LoadCompleted					
 			And EVENT1 (0..15) :
	 			EV1_LowOrderBranchIssued		
				EV1_ProcessorCycle   			
				EV1_DualInstructionIssue		
				EV1_BranchMispredicted			
				EV1_TLBmiss						
				EV1_DTLBmiss					
				EV1_DataCacheMiss				
				EV1_WBBSingleRequestUnavailable	
				EV1_WBBBurstRequestUnavailable	
				EV1_WBBBurstRequestAlmostFull	
				EV1_WBBBurstRequestFull			
				EV1_CPUDataBusBusy				
				EV1_InstructionCompleted		
				EV1_NonBDSInstrucionCompleted	
				EV1_COP1InstructionCompleted	
				EV1_StoreCompleted				

		6 - startPs2Perf () ;   --> Just Start 	(no clear counters)
		7 - Execute code to count
		8 - stopPs2Perf  () ;   --> Stop Counting 	(no clear counters)
            
		9 - You can repeat 6-7-8
         
		10 - Eventually you can flushCounter (&counter,PC0,reStart)
         		It stops Counters, add current PC0 event to &counter->event, clear PC0, and reStart Counters (if reStart == 1 )

		11 - Finally you can access directly to counter->event.
            
		
		
		
* Any Examples?
	- Yes, just look & compile main.c
	  Performance counters are used to see differences between different array storage locations.
	  
* It would be very interesting to integrate it with future ps2link in pc side like a profiling tool 	  

CHANGELOG
----------------------------------------------------------------------------

26 January 2002 - Version 1.0

- First Release.

Greets
-----------------------------------------------------------------------------

To Oobles, Sjeep. Pukko and all ps2reality crew

COPYRIGHT AND PERMISSION NOTICE
----------------------------------------------------------------------------

Copyright 2002 (C) Mavy & Bigboss (mavy@ps2reality.net bigboss@ps2reality.net)
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

</End>
	
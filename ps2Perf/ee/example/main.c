/* 
 Ps2Perf   Sample Program
 ============================
 
 A simple program to show how to use Ps2Perf 
 
 Mavy & Bigboss (PS2Reality.net)
 

*/



#include <stdlib.h>
#include <stdio.h>
#include <tamtypes.h>
#include <sifcmd.h>
#include <kernel.h>
#include <fileio.h>
#include <sifrpc.h>
#include <math.h>
#include <stdarg.h>
#include <ps2Perf.h>

Counters0 myCounter0;
Counters1 myCounter1;

void loadModules(void)
{
    int ret;

    ret = SifLoadModule("rom0:LIBSD",0,NULL);
    if ( ret< 0 ) {
       return;
    }

}
long VeryVeryHardFunction (int iterations)
{
	char *array;
	int i;
	long output=0;
	array =  (char*) malloc (iterations); 
	
	for (i=0; i < iterations; i++)
		array[i] = i;
		
	for (i=0; i < iterations; i++)
		output += array[i];
	
	return (output);
}

long VeryVeryHardFunctionScratch (int iterations)
{
	char *array;
	int i;
	long output=0;
	
	array =  (char*) malloc (iterations);  // Just to equal algo's
	
	array = (char*)0x70000000;
	for (i=0; i < iterations; i++)
		array[i] = i;
		
	for (i=0; i < iterations; i++)
		output += array[i];
	
	return (output);
}

long VeryVeryHardFunctionNoCached (int iterations)
{
	char *array;
	int i;
	long output=0;
	array =  (char*) malloc (iterations) + 0x20000000; 

	for (i=0; i < iterations; i++)
		array[i] = i;
		
	for (i=0; i < iterations; i++)
		output += array[i];
	
	return (output);
}

long VeryVeryHardFunctionUnCachedAccelerated (int iterations)
{
	char *array;
	int i;
	long output=0;
	array =  (char*) malloc (iterations) + 0x30000000; 

	for (i=0; i < iterations; i++)
		array[i] = i;
		
	for (i=0; i < iterations; i++)
		output += array[i];
	
	return (output);
}



int main()
{
  	long output = 0;
  	
  	SifInitRpc(0);
	loadModules();
//	SjPCM_Init(0);
	printf ("Yeah... I'm Here\n");
	
		initPs2Perf 	();   		// Init The Machine :D
		mode0Select 	(1,1,1,1);  // We Count on any mode (kernel, user...)
		mode1Select 	(1,1,1,1);
		initCounter		(&myCounter0,PC0); // Well... just a shortcut 
		initCounter		(&myCounter1,PC1);
		event0Select 	(EV0_ProcessorCycle);		// PC0 Event Counter
		event1Select    (EV1_DataCacheMiss);	// PC1 Event Counter
		startPs2Perf 	();   	// Start counters!!! 
		output = VeryVeryHardFunction (10000);
		stopPs2Perf 	();		// Stop counters!!!
		printf ("Output Equals %d\n",output);
		
		// Here counters aren't counting 
		
		flushCounter 	(&myCounter0,PC0,0);  // += counter, and counter = 0
		flushCounter 	(&myCounter1,PC1,0);  // ...
		printf ("Very Very Hard Test 1\n");
		printf (DumpInfo (&myCounter0,&myCounter1)); // A simple dumping function...
/* ===========================================================================================================*/
		/* Test Another VeryVeryHard Function now using scratchpad */

		initCounter		(&myCounter0,PC0); 
		initCounter		(&myCounter1,PC1);
		event0Select 	(EV0_ProcessorCycle);		
		event1Select    (EV1_DataCacheMiss);	
		printf ("Second Test\n");
		startPs2Perf 	();   	
		output = VeryVeryHardFunctionScratch (10000);
		stopPs2Perf 	();		
		printf ("Output Equals %d\n",output);
		
		// Here counters aren't counting 
		
		flushCounter 	(&myCounter0,PC0,0);  
		flushCounter 	(&myCounter1,PC1,0);  
		printf ("Very Very Hard Test 2\n");
		printf (DumpInfo (&myCounter0,&myCounter1)); 
 
 /* ===========================================================================================================*/ 
  /* Test Another VeryVeryHard Function now using no cached memory */

		initCounter		(&myCounter0,PC0); 
		initCounter		(&myCounter1,PC1);
		event0Select 	(EV0_ProcessorCycle);		
		event1Select    (EV1_DataCacheMiss);	
		printf ("Third Test\n");
		startPs2Perf 	();   	
		output = VeryVeryHardFunctionNoCached (10000);
		stopPs2Perf 	();		
		printf ("Output Equals %d\n",output);
		
		// Here counters aren't counting 
		
		flushCounter 	(&myCounter0,PC0,0);  
		flushCounter 	(&myCounter1,PC1,0);  
		printf ("Very Very Hard Test 3\n");
		printf (DumpInfo (&myCounter0,&myCounter1)); 
  
  /* ===========================================================================================================*/ 
  /* Test Another VeryVeryHard Function now using uncached accelerated memory */

		initCounter		(&myCounter0,PC0); 
		initCounter		(&myCounter1,PC1);
		event0Select 	(EV0_ProcessorCycle);		
		event1Select    (EV1_DataCacheMiss);	
		printf ("Fourth Test\n");
		startPs2Perf 	();   	
		output = VeryVeryHardFunctionUnCachedAccelerated (10000);
		stopPs2Perf 	();		
		printf ("Output Equals %d\n",output);
		
		// Here counters aren't counting 
		
		flushCounter 	(&myCounter0,PC0,0);  
		flushCounter 	(&myCounter1,PC1,0);  
		printf ("Very Very Hard Test 4\n");
		printf (DumpInfo (&myCounter0,&myCounter1)); 

  	// Zzzzzz...
  	SleepThread(); 
}

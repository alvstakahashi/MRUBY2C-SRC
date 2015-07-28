/***********************************************************************/
/*                                                                     */
/*  FILE        :gcctestb.c                                            */
/*  DATE        :Mon, Jun 29, 2015                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :RX63N                                                 */
/*                                                                     */
/*  This file is generated by KPIT GNU Project Generator.              */
/*                                                                     */
/***********************************************************************/
                    
#include <stdint.h>	
#include <stdio.h>
#include <setjmp.h>
																							
#include "iodefine.h"
#include "kernel_impl.h"
#include "target_kernel.h"

int init_flag;

jmp_buf once_ctx;
jmp_buf save_ctx;


extern const uint8_t
#if defined __GNUC__
__attribute__((aligned(4)))
#elif defined _MSC_VER
__declspec(align(4))
#endif
rb_main[];

static int mruby_started = 0;



extern void mruby_call(const uint8_t *mrb_p );

#ifdef CPPAPP
//Initialize global constructors
// The GNURX uses a different method to initialize global constructors
// The below call is not invoked in case of RX target
extern "C" void __main()
{
  static int initialized;
  if (! initialized)
    {
      typedef void (*pfunc) ();
      extern pfunc __ctors[];
      extern pfunc __ctors_end[];
      pfunc *p;

      initialized = 1;
      for (p = __ctors_end; p > __ctors; )
	(*--p) ();

    }
}
#endif 

int main(void)
{
    // TODO: add application code here

	printf("hello\n");
//	return;
//	dly_tsk(1000);

//	PORT2.PDR.BIT.B5 = 0;		//IN
//	PORT2.PCR.BIT.B5 = 1;		//PULLUP

	uint8_t *prg_data = (uint8_t *)rb_main;
	
#if !defined HEWSIM
	if (PORT2.PIDR.BIT.B5 != 0)		//IO5��GND�����ŃX�L�b�v
	{
		prg_data = mrb_read_ui();
		if (prg_data == NULL)
		{
			prg_data = (uint8_t *)rb_main;
		}
	}
#endif
	mruby_call((const uint8_t *)prg_data);
	
	printf("mruby_call End\n");
	mruby_started = 1;
//	act_tsk(RUBY_TSK5);

//	PORTA.PODR.BIT.B0 = 1; 				// Output
//	PORT2.PODR.BIT.B2 = 1; 				// Output
//	sta_cyc(RUBY_CYC1);
//
//    while (1) {
//    }
  return 0;
}

void task2(intptr_t arg)
{
//	printf("task2\n");
//	PORTA.PDR.BIT.B0 ^= 1;				// Reverse (LED)
//	PORT2.PDR.BIT.B2 ^= 1;				// Reverse  P22 IO2
	
}

void CMI0()
{
	static int count=0;
	printf("CMI0 here \n");

	ipl_maskClear();
	t_unlock_cpu();


	if (mruby_started !=0)
	{
		call4mruby2("handler");
//		chandler();
	}
//	_kernel_signal_time();
	if (++count > 100)		// 10us�Ȃ̂ŁA�Ԉ���
	{
		count = 0;
//		isig_tim();
	}
}

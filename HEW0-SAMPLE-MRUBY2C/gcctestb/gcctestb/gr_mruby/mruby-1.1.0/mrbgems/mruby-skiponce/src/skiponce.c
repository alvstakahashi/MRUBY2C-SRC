#include "mruby.h"
#include "mruby/string.h"
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

extern jmp_buf once_ctx;
extern jmp_buf save_ctx;
extern int init_flag;

mrb_value
mrb_skiponce_init(mrb_state *mrb, mrb_value self)
{
	init_flag = 0;
	memset(&once_ctx,0,sizeof(jmp_buf));
	return self;
}

mrb_value
mrb_skiponce_actionDEBUG(mrb_state *mrb, mrb_value self);

mrb_value
mrb_skiponce_action(mrb_state *mrb, mrb_value self)
{
	int retval;
	mrb_int	tout;
    mrb_value argv;
    volatile int count =0;
    
    if (init_flag == 0)		//‰‰ñ‚ÍAƒRƒ“ƒeƒLƒXƒgÝ’è‚µ‚Ä
    {						// ‹A‚ç‚È‚¢
		init_flag = 1;
		if (setjmp(once_ctx) == 0)
		{
			/*“o˜^‚µ‚½ê‡*/
			for(;;)
			{
				count++;
			}
		}
		else
		{
			//•Ô‚éê‡
			return self;
		}
	}
	else
	{
		//2‰ñ–ÚˆÈ~
		memcpy(&save_ctx,&once_ctx,sizeof(jmp_buf));
		if (setjmp(once_ctx) == 0)
		{
			/*“o˜^‚µ‚½ê‡*/
			longjmp(save_ctx,1);	//ˆêŒÂ‘O‚Å‹A‚é
		}
		else
		{
			//•Ô‚éê‡
			return self;
		}
	}
	return self;	//‚±‚±‚É‚Í‚±‚È‚¢
}

void
mrb_mruby_skiponce_gem_init(mrb_state* mrb)
{
  	struct RClass *krn;
  	krn = mrb->kernel_module;
  	mrb_define_method(mrb, krn, "Skiponce_action", mrb_skiponce_actionDEBUG, ARGS_NONE());
  	mrb_define_method(mrb, krn, "Skiponce_init", mrb_skiponce_init, ARGS_NONE());
}

//  struct RClass *krn;
//  krn = mrb->kernel_module;
//  mrb_define_module_function(mrb, mrb->kernel_module, "eval", f_eval, MRB_ARGS_ARG(1, 3));
//  	mrb_define_method(mrb, mrb->kernel_module, "delay", mrb_ssp_delay,ARGS_NONE());
//  	 MRB_ARGS_REQ(1));
//    mrb_define_module_function(mrb, mrb->kernel_module, "Ssp_delay", mrb_ssp_delay, MRB_ARGS_REQ(1));
//  mrb_define_method(mrb, krn, "Ssp_delay", mrb_ssp_delay, MRB_ARGS_REQ(1));

void
mrb_mruby_skiponce_gem_final(mrb_state* mrb)
{
}

extern mrb_state *mrb_global;

void call4mruby(char *name)
{
	mrb_funcall(mrb_global, mrb_top_self(mrb_global), name,0);
}

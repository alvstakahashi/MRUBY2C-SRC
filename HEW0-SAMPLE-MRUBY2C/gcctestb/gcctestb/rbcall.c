#include <stdint.h>
#include "mruby.h"
#include "mruby/proc.h"
#include "mruby/data.h"
#include "mruby/compile.h"
#include "mruby/string.h"
#include "mruby/numeric.h"
#include "mruby/dump.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <setjmp.h>

#include <kernel.h>
#include "kernel_cfg.h"
#include "kernel_impl.h"
#include "target_kernel.h"

mrb_state *mrb_global;
mrb_irep *idx_code1 = 0;

extern int init_flag;

extern jmp_buf once_ctx;
extern jmp_buf save_ctx;

void mruby_call(const uint8_t *mrb_p )
{
    mrb_global = mrb_open();
	if (mrb_global == NULL) 
	{
		return ;
	}
	mrb_value return_value1;
	if (idx_code1 == 0) 
	{
		idx_code1 = mrb_read_irep(mrb_global, mrb_p);
		if (idx_code1 == 0) 
		{
			// irep_error(mrb, idx_code1);
			return ;
		}
	}
	if (idx_code1 != 0) 
	{
//		int ai = mrb_gc_arena_save(mrb_global);
		t_lock_cpu();
		return_value1 =  mrb_run(mrb_global, mrb_proc_new(mrb_global, idx_code1),mrb_top_self(mrb_global));
		t_unlock_cpu();
//		mrb_gc_arena_restore(mrb_global, ai);
	}
}
void call4mruby2(char *name)
{
	t_lock_cpu();
	mrb_funcall(mrb_global, mrb_top_self(mrb_global), name,0);
	t_unlock_cpu();
}
mrb_value
mrb_skiponce_actionDEBUG(mrb_state *mrb, mrb_value self)
{
	int retval;
	mrb_int	tout;
    mrb_value argv;
    volatile int count =0;
    

    if (init_flag == 0)		//初回は、コンテキスト設定して
    {						// 帰らない
		init_flag = 1;
		if (setjmp(once_ctx) == 0)
		{
			/*登録した場合*/
			for(;;)
			{
				count++;
				t_unlock_cpu();
				t_lock_cpu();
			}
		}
		else
		{
			//返る場合
			return self;
		}
	}
	else
	{
		init_flag = 0;
		longjmp(once_ctx,1);	//一個前で帰る
	}
	return self;	//ここにはこない
}

void chandler(void)
{
	static int toggle = 0;
	
	if (toggle == 0)
	{
		toggle = 1;
		printf("0 c call\n");
		mrb_skiponce_actionDEBUG(mrb_global, mrb_true_value());
		printf("0 c call end\n");
	}
	else
	{
		toggle = 0;
		printf("1 c call\n");
		mrb_skiponce_actionDEBUG(mrb_global,mrb_true_value());
		printf("1 c call end \n");
	}
}
	
#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/string.h"

#include <kernel.h>
#include "kernel_cfg.h"

#include <string.h>


extern mrb_value alarm_self_tbl[];

extern mrb_state *mrb_global;


static mrb_value
mrb_ssp_alarm_initialize(mrb_state *mrb, mrb_value self)
{
	  mrb_value id;
	  mrb_value name;
	  char *name_cstr;	
	  mrb_int	id_num;

#if 1  	  
  	  printf("alarm_initialize &self = %08x self = %08x %08x tbl=%08x\n",
  	  		  (int)&self,*((int*)&self),*(((int*)&self)+1),(int)alarm_self_tbl);
#endif

	  mrb_get_args(mrb, "So",&name, &id);

	  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@alarm_id"),id );
	  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@alarm_name"),name );

	  id_num = mrb_fixnum(id);
	  
	  alarm_self_tbl[id_num-1] = self;

  	  name_cstr = mrb_str_to_cstr(mrb, name);
#if MRB_DEBUG1
	  printf("alarm init here id=%d name=%s \n",id_num,name_cstr);
#endif
	  return self;
}
static mrb_value
mrb_ssp_alarm_sta(mrb_state *mrb, mrb_value self)
{
	ER retval;
	mrb_int	tout;
	mrb_value id   = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@alarm_id"));
	mrb_int id_num = mrb_fixnum(id);

  	mrb_get_args(mrb, "i", &tout);

	t_unlock_cpu();
	retval = sta_alm(id_num,tout);
	t_lock_cpu();
	if (retval == E_OK)
	{
		return(mrb_false_value());
	}
	return(mrb_true_value());
}
static mrb_value
mrb_ssp_alarm_ista(mrb_state *mrb, mrb_value self)
{
	ER retval;
	mrb_int	tout;
	mrb_value id   = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@alarm_id"));
	mrb_int id_num = mrb_fixnum(id);

  	mrb_get_args(mrb, "i", &tout);

#if MRB_DEBUG1
	printf("ssp_alarm_ista tout = %d\n",tout);
#endif

	t_unlock_cpu();
	retval = ista_alm(id_num,tout);
	t_lock_cpu();
	if (retval == E_OK)
	{
		return(mrb_false_value());
	}
	return(mrb_true_value());
}

static mrb_value
mrb_ssp_alarm_stp(mrb_state *mrb, mrb_value self)
{
	ER retval;
	mrb_value id   = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@alarm_id"));
	mrb_int id_num = mrb_fixnum(id);

	t_unlock_cpu();
	retval = stp_alm(id_num);
	t_lock_cpu();
	if (retval == E_OK)
	{
		return(mrb_false_value());
	}
	return(mrb_true_value());
}

// ssp_thread で登録されるtask本体
void
mrb_ssp_alarm_call(intptr_t exf)
{
	mrb_int alarm_id = (mrb_int)exf;		// alarmID
	mrb_value self  = alarm_self_tbl[alarm_id-1];
	char name_cstr[20];

#if MRB_DEBUG1
  	  printf("alarm_call &self = %08x self = %08x %08x\n",
  	  		  (int)&self,*((int*)&self),*(((int*)&self)+1));
#endif
	mrb_value name = mrb_iv_get(mrb_global, self, mrb_intern_lit(mrb_global, "@alarm_name"));
	mrb_value id   = mrb_iv_get(mrb_global, self, mrb_intern_lit(mrb_global, "@alarm_id"));
// 	char *name_cstr = mrb_str_to_cstr(mrb_global, name);	
    strncpy(name_cstr, RSTRING_PTR(name), RSTRING_LEN(name));
    name_cstr[RSTRING_LEN(name)]='\0';
	
#if MRB_DEBUG1
	printf("mrb_alarm_call exf = %d cstr=%s id = %d\n",
			exf,name_cstr,mrb_fixnum(id));
#endif

	t_lock_cpu();	

#if 0
	mrb_funcall(mrb_global, mrb_top_self(mrb_global), name_cstr,1, id);
#else
	mrb_funcall(mrb_global, self, name_cstr,1, id);
#endif
	t_unlock_cpu();

}
	
void
mrb_mruby_ssp_alarm_gem_init(mrb_state* mrb) {


	struct RClass *alm;
	alm = mrb_define_class(mrb, "Ssp_alarm", mrb->object_class);

	/* methods */
	mrb_define_method(mrb, alm, "initialize", mrb_ssp_alarm_initialize, ARGS_REQ(2));
	mrb_define_method(mrb, alm, "start", mrb_ssp_alarm_sta, ARGS_REQ(1));
	mrb_define_method(mrb, alm, "istart", mrb_ssp_alarm_ista, ARGS_REQ(1));
	mrb_define_method(mrb, alm, "stop" , mrb_ssp_alarm_stp, ARGS_NONE());

}

void
mrb_mruby_ssp_alarm_gem_final(mrb_state* mrb) {
  // finalizer
}

/* 
 tracer.h : trace kernel activety

*/
#ifndef __TRACER_H
#define __TRACER_H

#define __TRACER_VERSION 0x00010000

/* this macro enable trace code */
//#define __TRACER_ENABLE
#define __TRACER_LEVEL1
#define __TRACER_LEVEL2
#define __TRACER_LEVEL3


#ifdef __TRACER_ENABLE
/* 16byte per entry */
/* first 4byte is time stamp TBL*/
/* next 1byte is id */
/* next 3bytes are user define */
struct tracelog_tag {
  unsigned long data[4]; /* 4byte */
};

//#define MAX_TRACELOG 1024
#define MAX_TRACELOG 2048
/* defined in ./kernel/softirq.c */
extern struct tracelog_tag _tracelog[MAX_TRACELOG];
extern unsigned short _tracelog_index;
extern unsigned short _trace_log_flag;
extern unsigned long _trace_a;
extern unsigned long _trace_b;
extern unsigned long _trace_c;
extern unsigned long _trace_d;
extern unsigned long _trace_e;
extern unsigned long _trace_f;
extern unsigned long _trace_g;

/* Initialization macro
 *
 */
#define TRACE_INIT() struct tracelog_tag _tracelog[MAX_TRACELOG] ____cacheline_aligned; \
  unsigned short _tracelog_index = 0;                                   \
  unsigned short _trace_log_flag = 1;                                   \
  unsigned long _trace_a = 0;                                           \
  unsigned long _trace_b = 0;                                           \
  unsigned long _trace_c = 0;                                           \
  unsigned long _trace_d = 0;                                           \
  unsigned long _trace_e = 0;                                           \
  unsigned long _trace_f = 0;                                           \
  unsigned long _trace_g = 0;


/*
 * Macro for put value 
 */
#ifdef __TRACER_LEVEL1
#define trace_log_L1(_ina,_inb,_inc) trace_log(_ina,_inb,_inc)
#define trace_set_L1(_var,_ina) trace_set(_var,_ina)
#else 
#define trace_log_L1(_ina,_inb,_inc)
#define trace_set_L1(_var,_ina)
#endif
#ifdef __TRACER_LEVEL2
#define trace_log_L2(_ina,_inb,_inc) trace_log(_ina,_inb,_inc)
#define trace_set_L2(_var,_ina) trace_set(_var,_ina)
#else 
#define trace_log_L2(_ina,_inb,_inc)
#define trace_set_L2(_var,_ina)
#endif
#ifdef __TRACER_LEVEL3
#define trace_log_L3(_ina,_inb,_inc) trace_log(_ina,_inb,_inc)
#define trace_set_L3(_var,_ina) trace_set(_var,_ina)
#else 
#define trace_log_L3(_ina,_inb,_inc)
#define trace_set_L3(_var,_ina)
#endif

#define trace_log(_ina,_inb,_inc) do{\
    if( _trace_log_flag == 0 ) break; \
    _tracelog[_tracelog_index].data[0]=mfspr(SPRN_TBWL);\
    _tracelog[_tracelog_index].data[1]=_ina;\
    _tracelog[_tracelog_index].data[2]=_inb;\
    _tracelog[_tracelog_index].data[3]=_inc;\
    _tracelog_index = (_tracelog_index + 1) & (MAX_TRACELOG-1);\
  }while(0)

#define trace_tb mfspr(SPRN_TBWL)
#define trace_start() do{ _trace_log_flag = 1; } while(0)
#define trace_stop()  do{ _trace_log_flag = 0; } while(0)
#define trace_test(__ina)  do{ if( __ina ) trace_stop(); } while(0)
#define trace_message(str) do{ printk(str); } while(0)
#define trace_message1(str,str2) do{ printk(str,str2); } while(0)
#define trace_var(_var) _trace_##_var
#define trace_set(_var,_ina) do{ _trace_##_var = _ina; } while(0)
#define trace_inc(_var,_ina) do{ _trace_##_var += _ina; } while(0)
#define trace_clear() do{ int i;\
		_tracelog_index = 0;\
		for( i=0; i< MAX_TRACELOG; i++ ) {	\
			_tracelog[i].data[0] = 0;	\
			_tracelog[i].data[1] = 0;\
			_tracelog[i].data[2] = 0;	\
			_tracelog[i].data[3] = 0;	\
		} } while(0)

#define printk_tracelog() do{ int i; \
    for( i=0; i< MAX_TRACELOG; i++ ) {\
      printk(KERN_INFO"%04d,%08lx,%08lx,%08lx,%08lx\n",   \
             i, _tracelog[(_tracelog_index+i)&(MAX_TRACELOG-1)].data[0], \
             _tracelog[(_tracelog_index+i)&(MAX_TRACELOG-1)].data[1], \
             _tracelog[(_tracelog_index+i)&(MAX_TRACELOG-1)].data[2], \
             _tracelog[(_tracelog_index+i)&(MAX_TRACELOG-1)].data[3] ); \
    } } while(0)

#else /* If not define __TRACER_ENABLE */
/* empty macro */
#define TRACE_INIT() 
#define trace_log(_ina,_inb,_inc) do{;}while(0)
#define trace_start() do{ ; } while(0)
#define trace_stop()  do{ ; } while(0)
#define trace_test(__ina)  do{ ; } while(0)
#define trace_clear(__ina)  do{ ; } while(0) 
#define trace_message(str) do{ ; } while(0)
#define trace_message1(str,str2) do{ ; } while(0)
#define trace_get(_var)  do{ ; } while(0)
#define trace_set(_var,_ina) do{ ; } while(0)
#define trace_set_tb(_var) do{ ; } while(0)
#define trace_inc(_var,_ina) do{ ; } while(0)
#define printk_tracelog() do{ ; } while(0)

#define trace_log_L1(_ina,_inb,_inc)
#define trace_set_L1(_var,_ina)
#define trace_log_L2(_ina,_inb,_inc)
#define trace_set_L2(_var,_ina)
#define trace_log_L3(_ina,_inb,_inc)
#define trace_set_L3(_var,_ina)



#endif /* __TRACER_ENABLE */

#endif /* __TRACER_H */
/* end of file */


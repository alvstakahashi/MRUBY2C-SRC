/*
 *  TOPPERS/SSP Kernel
 *      Smallest Set Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 */

/*
 *  カーネルのターゲット依存定義（FRK-RX62N用）(GR-SAKURA GCC用)
 */

/*
 *  このインクルードファイルは，kernel.hでインクルードされる．他のファ
 *  イルから直接インクルードすることはない．このファイルをインクルード
 *  する前に，t_stddef.hがインクルードされるので，それらに依存してもよ
 *  い．
 */

#ifdef TOPPERS_KERNEL_IMPL_H

#ifndef TOPPERS_TARGET_KERNEL_H
#define TOPPERS_TARGET_KERNEL_H


#if defined __GNUC__
#define Inline	static __inline__	/* インライン関数 */
#endif

/*
 *  プロセッサで共通な定義
 */
//#include "rx_rxc/prc_kernel.h"

/*
 *  Iフラグ, IPLを取得するためのマクロ定義
 */
#define PSW_I_MASK		( 0x00010000UL )
#define PSW_IPL_MASK	( 0x07000000UL )

/*
 *  サポートする機能の定義
 */
#define	TOPPERS_TARGET_SUPPORT_GET_UTM		/* get_utmをサポートする */


/*
 *  タイムティックの定義
 */
#define	TIC_NUME		( 1U )		/* タイムティックの周期の分子 */
#define	TIC_DENO		( 1U )		/* タイムティックの周期の分母 */


#if defined __GNUC__
Inline void ipl_maskClear( void )
{
	__asm__("push	r1");
	__asm__("mvfc	psw,r1");
	__asm__("and	#0xf0ffffff,r1");
	__asm__("mvtc	r1,psw");
	__asm__("pop	r1");
}
#else
#pragma inline_asm	ipl_maskClear
static void 
ipl_maskClear(void)
{
	push	r1
	mvfc	psw,r1
	and		#0xf0ffffff,r1
	mvtc	r1,psw
	pop		r1
}
#endif



/*
 *  NMIを除くすべての割込みの禁止
 */
#if defined __GNUC__
static inline void disint( void )
{
	__asm__("clrpsw	i");
}
#else
#pragma inline_asm	disint
static void
disint( void )
{
	clrpsw	i
}
#endif

/*
 *  NMIを除くすべての割込みの許可
 */
#if defined __GNUC__
static inline void enaint( void )
{
	__asm__("setpsw	i");
}
#else
#pragma inline_asm	enaint
static void
enaint( void )
{
	setpsw	i
}
#endif



/*
 *  CPUロック状態への移行
 *
 *  IPM（ハードウェアの割込み優先度マスク）を，saved_iipmに保存し，カー
 *  ネル管理外のものを除くすべての割込み要求をマスクする値（TIPM_LOCK）
 *  に設定する．また，lock_flagをTRUEにする．
 *
 *  IPMが，最初からTIPM_LOCKと同じかそれより高い場合には，それを
 *  saved_iipmに保存するのみで，TIPM_LOCKには設定しない．これは，モデル
 *  上の割込み優先度マスクが，TIPM_LOCKと同じかそれより高いレベルに設定
 *  されている状態にあたる．
 *
 *  この関数は，CPUロック状態（lock_flagがTRUEの状態）で呼ばれることは
 *  ないものと想定している．
 */
#if defined __GNUC__
static inline
#else
#pragma inline  (x_lock_cpu)
static 
#endif
void x_lock_cpu( void ) 
{	
	disint();
}

#define t_lock_cpu()	x_lock_cpu()
#define i_lock_cpu()	x_lock_cpu()


/*
 *  CPUロック状態の解除
 *
 *  lock_flagをFALSEにし，IPM（ハードウェアの割込み優先度マスク）を，
 *  saved_iipmに保存した値に戻す．
 *
 *  この関数は，CPUロック状態（lock_flagがtrueの状態）でのみ呼ばれるも
 *  のと想定している．
 */
#if defined __GNUC__
static inline
#else
#pragma inline (x_unlock_cpu)
static 
#endif
void x_unlock_cpu( void )
{
	enaint();
}

#define t_unlock_cpu()	x_unlock_cpu()
#define i_unlock_cpu()	x_unlock_cpu()

/*
 *  デフォルトの非タスクコンテキスト用のスタック領域の定義
 *
 *  スタックサイズを変更する場合は, sectrx.incの_DEFAULT_ISTACK
 *  ラベルの位置にある.blkb命令のサイズと, 以下DEFAULT_ISTKSZの
 *  サイズを合わせて同じ値に変更すること.
 */
extern uint32_t	DEFAULT_ISTACK[];
#define DEFAULT_ISTKSZ		( 0x500U )
#define DEFAULT_ISTK		( (void *)&DEFAULT_ISTACK[0] )

#if defined __GNUC__
static inline
#else
#pragma inline (idle_loop)
static  
#endif
void idle_loop(void)
{
	t_unlock_cpu();
	ipl_maskClear();
	t_lock_cpu();
}

#if defined __GNUC__
static inline
#else
#pragma inline  (goto_taskMode)
static 
#endif
void goto_taskMode( void ) 
{	
	ipl_maskClear();
	t_unlock_cpu();
}

/*
 *  プロセッサステータスレジスタ(PSW)の現在値の読出し
 */
#if defined __GNUC__
Inline int current_psw(void)
{
	int status;
	__asm__("mvfc	psw,%[Rd]":[Rd]"=r"(status));
	return(status);
}

#else
#pragma inline_asm	current_psw
static uint32_t
current_psw( void )
{
	mvfc	psw, r1
}
#endif
/*
 *  CPUロック状態の参照
 */
#if defined __GNUC__
static inline
#else
#pragma inline (x_sense_lock)
static  
#endif
bool_t
x_sense_lock( void )
{
	return (( bool_t )(( current_psw() & PSW_I_MASK) == 0 ));
}

#define t_sense_lock()	x_sense_lock()
#define i_sense_lock()	x_sense_lock()


/*
 *  コンテキストの参照
 *
 *  RXでは，割込みの戻り先がタスクかどうかを判断するために intnest
 *  を使用している．これを用いてコンテキストを判断する．
 */
#if defined __GNUC__
static inline
#else
#pragma inline (sense_context) 
static 
#endif
bool_t sense_context( void )
{
	/*  ネストカウンタ0より大なら非タスクコンテキスト  */
	return ( intnest > 0U );
}
#if defined __GNUC__
#define	set_task_stack(x)	__asm__( "MVTC %[Rs1],isp"::[Rs1]"r"(x))
#else
#pragma inline_asm	set_task_stack
static void
set_task_stack( intptr_t stkp )
{
	MVTC	R1,isp
}
#endif

#endif /* TOPPERS_TARGET_KERNEL_H */
#endif
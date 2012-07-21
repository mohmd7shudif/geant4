/*
 * Generated by dtrace(1M).
 */

#ifndef	_SIMPLE_PROBES_H
#define	_SIMPLE_PROBES_H

#include <unistd.h>

#ifdef	__cplusplus
extern "C" {
#endif

#if _DTRACE_VERSION

#define	SIMPLE_POPTRACK(arg0, arg1, arg2, arg3, arg4) \
	__dtrace_simple___poptrack(arg0, arg1, arg2, arg3, arg4)
#ifndef	__sparc
#define	SIMPLE_POPTRACK_ENABLED() \
	__dtraceenabled_simple___poptrack()
#else
#define	SIMPLE_POPTRACK_ENABLED() \
	__dtraceenabled_simple___poptrack(0)
#endif
#define	SIMPLE_PUSHTRACK(arg0, arg1, arg2, arg3, arg4) \
	__dtrace_simple___pushtrack(arg0, arg1, arg2, arg3, arg4)
#ifndef	__sparc
#define	SIMPLE_PUSHTRACK_ENABLED() \
	__dtraceenabled_simple___pushtrack()
#else
#define	SIMPLE_PUSHTRACK_ENABLED() \
	__dtraceenabled_simple___pushtrack(0)
#endif


extern void __dtrace_simple___poptrack(int, int, int, int, int);
#ifndef	__sparc
extern int __dtraceenabled_simple___poptrack(void);
#else
extern int __dtraceenabled_simple___poptrack(long);
#endif
extern void __dtrace_simple___pushtrack(int, int, int, int, int);
#ifndef	__sparc
extern int __dtraceenabled_simple___pushtrack(void);
#else
extern int __dtraceenabled_simple___pushtrack(long);
#endif

#else

#define	SIMPLE_POPTRACK(arg0, arg1, arg2, arg3, arg4)
#define	SIMPLE_POPTRACK_ENABLED() (0)
#define	SIMPLE_PUSHTRACK(arg0, arg1, arg2, arg3, arg4)
#define	SIMPLE_PUSHTRACK_ENABLED() (0)

#endif


#ifdef	__cplusplus
}
#endif

#endif	/* _SIMPLE_PROBES_H */

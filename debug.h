#ifndef DEBUG_H_
#define DEBUG_H_

// debugging macros so we can pin down message origin at a glance
#define WHERESTR  "#[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__)
#define DEBUGPRINT(_fmt, ...)  DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#define DEBUGLOG(X) DEBUGPRINT2(WHERESTR X, WHEREARG)

#define PF(X) printf("%s = %f\n", #X, X)
#define PE(X) printf("%s = %e\n", #X, X)
#define PD(X) printf("%s = %d\n", #X, X)
#define PZ(X) printf("%s = %zu\n", #X, X) // for size_t
#define PU(X) printf("%s = %u\n", #X, X)
#define PP(X) printf("%s = %p\n", #X, (void*)X)
#define PS(X) printf("%s = %s\n", #X, X)
#define PL(X) DEBUGLOG(X);


#endif /* !DEBUG_H_ */

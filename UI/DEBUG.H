#ifdef DEBUG
#define DEBUGGER(Statements) Statements
#define WHERE(Statements) fprintf(stderr,"\n%s|%d\n",__FILE__,__LINE__);\
			Statements
#else
#define DEBUGGER(Statements) /* empty */
#define WHERE(Statements) /* empty */
#endif


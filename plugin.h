#ifndef _PLUGINS_H
#define _PLUGINS_H

#include "utils.h"


typedef enum __supported_plugins_t {
#ifdef _PYTHON_PLUGIN
	PYTHON,
#endif
  
#ifdef _C_PLUGIN
	C,
#endif
    
  END_VALUE
} supported_plugins_t;


const static UNUSED char* supported_plugins_str[] = {
#ifdef _PYTHON_PLUGIN
	"Python",
#endif

#ifdef _C_PLUGIN
	"C",
#endif
 
  NULL
};


const static UNUSED char* plugins_extensions_str[] = {
#ifdef _PYTHON_PLUGIN  
	".py",
#endif
  
#ifdef _C_PLUGIN      
	".so",
#endif 
  
  NULL
};

enum {
	REQUEST	 = 0,
	RESPONSE = 1
};

typedef struct _interpreter_type {
		unsigned short id;
		supported_plugins_t type;
		pthread_mutex_t mutex;
		void *vm;
		boolean ready;
} interpreter_t;

#define MAX_VMS 2
interpreter_t vms[MAX_VMS];



typedef struct _plugin_type {
		unsigned short id;
		char* filename;
		char* name;
		supported_plugins_t type;
		unsigned short priority;
		struct _plugin_type* next;
		unsigned char state;

		interpreter_t *interpreter;
		void *pre_function;
		void *post_function;
		
} plugin_t;


int	proxenet_create_list_plugins(char*);
int 	proxenet_plugin_list_size();
void 	proxenet_delete_list_plugins();
void 	proxenet_print_plugins_list();
int	count_plugins_by_type(int);
char*	get_plugin_path(char*);
int	count_initialized_plugins_by_type(int);

#endif /* _PLUGINS_H */
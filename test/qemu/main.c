#include <stdio.h>

extern void initialise_monitor_handles(void);

int main(void) {
  	initialise_monitor_handles();

    printf("hello world!\n");
    printf("hello world!\n");

    // Wait forever ...
    while(1);

  	return 0;
}
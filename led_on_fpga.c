#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define HW_REGS_BASE ( 0xFF200000 )     
#define HW_REGS_SPAN ( 0x00200000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define HEX_PIO_BASE 0x28

int main(void){
        volatile unsigned int *reg32_intface = NULL;
        void* virtual_base;
        int fd;

        if( (fd = open("/dev/mem", (O_RDWR | O_SYNC) ) ) == -1){
                printf("Could not open \"/dev/mem\"\n");
                return 1;
        }

        virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
        if(virtual_base == MAP_FAILED) {
                printf("mmap failed\n");
                close (fd);
                return 1;
        }

        reg32_intface = (unsigned int *) (virtual_base + ( (HEX_PIO_BASE) & (HW_REGS_MASK) ));

        char buffer[25];
        int number;

        while(1){
                printf("Enter an integer or 'exit': ");
                fgets(buffer, 25, stdin); // Get input from stdin
                sscanf(buffer, "%d\n", &number);
                if (!strcmp(buffer, "exit\n"))
                        break;

                printf("You entered %x\n", number);
                fflush(stdin);
                *(reg32_intface) = number; // Try the hex bus
        }

        munmap(virtual_base, HW_REGS_SPAN); 
        close(fd);
        return 0;
}

#include "shell.h"
#include <stddef.h>
#include "clib.h"
#include <string.h>
#include "fio.h"
#include "filesystem.h"

#include "FreeRTOS.h"
#include "task.h"
#include "host.h"

typedef struct {
	const char *name;
	cmdfunc *fptr;
	const char *desc;
} cmdlist;

void ls_command(int, char **);
void man_command(int, char **);
void cat_command(int, char **);
void ps_command(int, char **);
void host_command(int, char **);
void help_command(int, char **);
void host_command(int, char **);
void mmtest_command(int, char **);
void test_command(int, char **);
void _command(int, char **);
void new_command(int, char**);
void new_task();
void sysinfo_command(int, char**);
void sysinfo_task();

#define MKCL(n, d) {.name=#n, .fptr=n ## _command, .desc=d}

cmdlist cl[]={
	MKCL(ls, "List directory"),
	MKCL(man, "Show the manual of the command"),
	MKCL(cat, "Concatenate files and print on the stdout"),
	MKCL(ps, "Report a snapshot of the current processes"),
	MKCL(host, "Run command on host"),
	MKCL(mmtest, "heap memory allocation test"),
	MKCL(help, "help"),
	MKCL(test, "test new function"),
	MKCL(, ""),
	MKCL(new, "new a task"),
	MKCL(sysinfo, "sysinfo saved"),
};

int parse_command(char *str, char *argv[]){
	int b_quote=0, b_dbquote=0;
	int i;
	int count=0, p=0;
	for(i=0; str[i]; ++i){
		if(str[i]=='\'')
			++b_quote;
		if(str[i]=='"')
			++b_dbquote;
		if(str[i]==' '&&b_quote%2==0&&b_dbquote%2==0){
			str[i]='\0';
			argv[count++]=&str[p];
			p=i+1;
		}
	}
	/* last one */
	argv[count++]=&str[p];

	return count;
}

void ls_command(int n, char *argv[]){
    fio_printf(1,"\r\n"); 
    int dir;
    if(n == 0){
        dir = fs_opendir("");
    }else if(n == 1){
        dir = fs_opendir(argv[1]);
        //if(dir == )
    }else{
        fio_printf(1, "Too many argument!\r\n");
        return;
    }
(void)dir;   // Use dir
}

int filedump(const char *filename){
	char buf[128];

	int fd=fs_open(filename, 0, O_RDONLY);

	if( fd == -2 || fd == -1)
		return fd;

	fio_printf(1, "\r\n");

	int count;
	while((count=fio_read(fd, buf, sizeof(buf)))>0){
		fio_write(1, buf, count);
    }
	
    fio_printf(1, "\r");

	fio_close(fd);
	return 1;
}

void ps_command(int n, char *argv[]){
	signed char buf[1024];
	vTaskList(buf);
        fio_printf(1, "\n\rName          State   Priority  Stack  Num\n\r");
        fio_printf(1, "*******************************************\n\r");
	fio_printf(1, "%s\r\n", buf + 2);	
}

void cat_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

    int dump_status = filedump(argv[1]);
	if(dump_status == -1){
		fio_printf(2, "\r\n%s : no such file or directory.\r\n", argv[1]);
    }else if(dump_status == -2){
		fio_printf(2, "\r\nFile system not registered.\r\n", argv[1]);
    }
}

void man_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: man <command>\r\n");
		return;
	}

	char buf[128]="/romfs/manual/";
	strcat(buf, argv[1]);

    int dump_status = filedump(buf);
	if(dump_status < 0)
		fio_printf(2, "\r\nManual not available.\r\n");
}

void host_command(int n, char *argv[]){
    int i, len = 0, rnt;
    char command[128] = {0};

    if(n>1){
        for(i = 1; i < n; i++) {
            memcpy(&command[len], argv[i], strlen(argv[i]));
            len += (strlen(argv[i]) + 1);
            command[len - 1] = ' ';
        }
        command[len - 1] = '\0';
        rnt=host_action(SYS_SYSTEM, command);
        fio_printf(1, "\r\nfinish with exit code %d.\r\n", rnt);
    } 
    else {
        fio_printf(2, "\r\nUsage: host 'command'\r\n");
    }
}

void help_command(int n,char *argv[]){
	int i;
	fio_printf(1, "\r\n");
	for(i = 0;i < sizeof(cl)/sizeof(cl[0]) - 1; ++i){
		fio_printf(1, "%s - %s\r\n", cl[i].name, cl[i].desc);
	}
}

//Transform argv[2] string to integer
int stoi(char *str)
{
        int i;
        int result = 0;
        int count = strlen(str);//How many bits in "str"? string length
        for (i = 0; i < count; ++i)
        {
                result = result*10+(str[i]-'0'); //transform every bit
        }
        return result;
}

void test_command(int n, char *argv[]) {
    //Test print hello
    //fio_printf(1,"\r\n Hello world!");
    
    //Compute Fibonacci
    if(n ==3 )	//e.g. input is "test fib 10"
	{
		if(strcmp(argv[1],"fib")==0)
		{
			if(argv[2]!=0)
			{
			    int i;   
                            int count = stoi(argv[2]); 
                            int result = 1;
                            int prev = 0;
                            int temp;
                            
                            for (i = 1; i < count; ++i)
                            {
                                    temp = result;
                                    result = result + prev; //f(n+1)=f(n)+f(n-1)
                                    prev = temp ;
                            }
                            //print the result to the screen.
                            fio_printf(1,"\r\nFibonacci number at %d is %d",count,result) ;
			}
		}
	} 
 

 	fio_printf(1, "\r\n");
 /*
    int handle;
    int error;

    
    
    handle = host_action(SYS_SYSTEM, "mkdir -p output");
    handle = host_action(SYS_SYSTEM, "touch output/syslog");

    handle = host_action(SYS_OPEN, "output/syslog", 8);
    if(handle == -1) {
        fio_printf(1, "Open file error!\n\r");
        return;
    }

    char *buffer = "Test host_write function which can write data to output/syslog\n";
    error = host_action(SYS_WRITE, handle, (void *)buffer, strlen(buffer));
    if(error != 0) {
        fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\n\r", error);
        host_action(SYS_CLOSE, handle);
        return;
    }

    host_action(SYS_CLOSE, handle);
    */
}

void _command(int n, char *argv[]){
    (void)n; (void)argv;
    fio_printf(1, "\r\n");
}

cmdfunc *do_command(const char *cmd){

	int i;

	for(i=0; i<sizeof(cl)/sizeof(cl[0]); ++i){
		if(strcmp(cl[i].name, cmd)==0)
			return cl[i].fptr;
	}
	return NULL;	
}

// Create a new FreeRTOS task using "new" command
xTaskHandle newHandle = NULL; 
void new_command(int n,char *argv[]){

	if(n!=2){
		fio_printf(1,"\r\n argument error! \r\n");
		return;
	}

	//Creat a task
	xTaskCreate(new_task,(signed portCHAR *)"task",128,NULL,tskIDLE_PRIORITY, &newHandle);
    

    fio_printf(1, "\r\n new task created \r\n");

}

void new_task(void *pvParameters){
	vTaskSuspend(newHandle);
} 		

// Create a new FreeRTOS task using "new" command
xTaskHandle sysinfoHandle = NULL; 
void sysinfo_command(int n,char *argv[]){

	if(n!=2){
		fio_printf(1,"\r\n argument error! \r\n");
		return;
	}

	//Creat a task
	xTaskCreate(sysinfo_task,(signed portCHAR *)"info",1024,NULL,tskIDLE_PRIORITY, &sysinfoHandle);
    

    fio_printf(1, "\r\n sysinfo saved! \r\n");

}

void sysinfo_task(void *pvParameters){
	signed char buf[128];
    char output[512] = {0};
    char *tag = "\nName          State   Priority  Stack  Num\n*******************************************\n";
    int handle, error;
    const portTickType xDelay = 100000 / 5000;

    handle = host_action(SYS_OPEN, "output/sysinfo", 4);
    if(handle == -1) {
        fio_printf(1, "Open file error!\n");
        return;
    }

    while(1) {
        memcpy(output, tag, strlen(tag));
        error = host_action(SYS_WRITE, handle, (void *)output, strlen(output));
        if(error != 0) {
            fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\n\r", error);
            host_action(SYS_CLOSE, handle);
            return;
        }
        vTaskList(buf);

        memcpy(output, (char *)(buf + 2), strlen((char *)buf) - 2);

        error = host_action(SYS_WRITE, handle, (void *)buf, strlen((char *)buf));
        if(error != 0) {
            fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\n\r", error);
            host_action(SYS_CLOSE, handle);
            return;
        }

        vTaskDelay(xDelay);
    }
    
    host_action(SYS_CLOSE, handle);
} 	











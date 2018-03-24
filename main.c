#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define printf pspDebugScreenPrintf

/* Define the module info section */
PSP_MODULE_INFO("kirk_brute", 0x1000, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

//offset 0,1,2,3 for header hash
//offset 4,5,6,7 for data hash
//CHANGE here for each new word

#define DELAY_THREAD_SEC 1000000

void DisableKirk();
void EnableKirk();
//make sure to set the correct command in kirk.S
unsigned int dlap2();
unsigned int db_lap;
extern unsigned char buf_src[0x1000]__attribute__((aligned(0x40)));
extern unsigned char buf_dest[0x1000]__attribute__((aligned(0x40)));

SceCtrlData pad;
u32 fsize;
u32 *hash;
u32 hash_offset = 0;//config
u32 current_hash[8];//config
u32 hash_start = 0, hash_end = 0;//config
char str_buf[300*1024];

void CheckExit()
{
    u32 fout;

    sceCtrlPeekBufferPositive(&pad, 1);
    if(pad.Buttons)
    {
        if(pad.Buttons & PSP_CTRL_LTRIGGER) //save current hash+quit
        {
            fout = sceIoOpen("ms0:/kirk/brute.bin",PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC,0777);
            sceIoWrite(fout,buf_src,fsize);
            sceIoClose(fout);

            DisableKirk();
            sceKernelExitGame();
        }
    }
}

char *GetConfigValue(char *config, char *option)
{
char line[300];
char *str = 0;
u32 pos = 0;
u32 eol = 0;

    str = strstr(config,option);
    if(str)
    {
        eol = strcspn(str,"\n\r#");
        strncpy(line, str, eol); //copy across the line
        line[eol] = '\0';

        pos = strlen(option);
        pos += strspn(&line[pos]," =\t");
        return str+pos;
    }
    else
        return NULL;

}

int ParseConfig(char *fname)
{
SceUID fd;
u32 filesize;
char *buf;
char *str;
int i,x;

    fd = sceIoOpen(fname,PSP_O_RDONLY,0777);

    if(fd < 0)
        return fd;

    //read file into buffer
    filesize = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);

    buf = malloc(filesize);

    sceIoRead(fd, buf, filesize);

    buf[filesize] = '\0'; //make the file a null-terminated string
    sceIoClose(fd);


    //first filter out comments
    for(i = 0; i < filesize; i++)
    {
        if(buf[i] == '#')
        {
            for(x = strcspn(&buf[i], "\n\r"); x > 0; x--)
                buf[i+x-1] = ' ';
        }
    }

    //parse config values
    str = GetConfigValue(buf, "HASH_START"); 
    if(str)
        hash_start = strtoul(str, NULL, 0);
    else
        hash_start = 0;

    str = GetConfigValue(buf, "HASH_END"); 
    if(str)
        hash_end = strtoul(str, NULL, 0);
    else
        hash_end = 0;

    for (i = 0; i < 8; i++)
    {
        sprintf(str_buf, "CURRENT_HASH%1X", i);//used for found hashes

        str = GetConfigValue(buf, str_buf); 

        if(str)
            current_hash[i] = strtoul(str, NULL, 0);
        else
            current_hash[i] = 0;

        if (current_hash[i] == 0)
        {
            hash_offset = i;
            break;
        }
     }

    free(buf);
    return 0;
}

void EnableKirk()
{
    asm("    lui   $t1, 0xbc10\n");
    asm("    lw   $v1, 0x50($t1)\n");
    asm("    li   $v0, 0x80\n");
    asm("    or   $v0, $v1\n");
    asm("    sw   $v0, 0x50($t1)\n");
}

void DisableKirk()
{
    asm("   lui   $t1, 0xbc10\n");
    asm("   lw   $v1, 0x50($t1)\n");
    asm("   li   $a0, 0xff7f\n");
    asm("   and   $a0, $v1\n");
    asm("   sw   $a0, 0x50($t1)\n");
}

int main(int argc, char *argv[])
{
    SceUID fin,fout;
    u32 hash_time;//hash_time contains time for kirk to return an invalid value
    int i;

    sceKernelChangeThreadPriority(sceKernelGetThreadId(),4);

    pspDebugScreenInit();
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    EnableKirk();

    fin = sceIoOpen("ms0:/kirk/brute.bin",PSP_O_RDONLY,0777);
    fsize = sceIoRead(fin,buf_src,0x1000);
    sceIoClose(fin);

    if (ParseConfig("ms0:/kirk/config.txt") < 0)
    {
        printf("Error reading config\n");
        sceKernelDelayThread(DELAY_THREAD_SEC);
        sceKernelExitGame();
        return 0;
    }

    printf("Starting bruteforce with hash offset: %1X\n", hash_offset);
    printf("hash_start: %08X \nhash_end : %08X\n", hash_start, hash_end);

    for (i = 0; i < hash_offset; i++)//setup any hashes stored in the config file
    {
        hash = (u32*)&buf_src[0x20+(4*i)];
        hash[0] = current_hash[i];
        printf("current_hash[%08X] = %08x\n",i,current_hash[i]);
    }

    hash = (u32*)&buf_src[0x20+(4*hash_offset)];

    if ((hash[0] < hash_start) || (hash[0] > hash_end))//if we're not in range its because the file wasn't from a saved run
        hash[0] = hash_start;
    else if(hash[0] != 0)//test the previously saved hash
        hash[0]--;

    printf("hash_starting at :%08X\n", hash[0]);

    sceKernelDcacheWritebackInvalidateAll();
    dlap2(); //get initial hash time
    dlap2(); //must be done twice to get correct timing
    hash_time = db_lap;

    printf("starting main loop\n");
/*
for 0x1000000 without checkExit(); it takes: 9:10:44 to 9:17:00
so 376s or ~6.3 min

for 0x1000000 with checkExit it takes 9:18:50 to 9:26:10
                                      9:19:00 to 9:26:00

*/
    do
    {
        dlap2();
//        CheckExit();
        hash[0]++;
        sceKernelDcacheWritebackInvalidateAll();
    }while ((db_lap <= hash_time) && (hash[0] != hash_end));
    printf("end main loop\n");
    sceKernelDelayThread(100000);

    if (hash[0] != hash_end)
    {
        hash[0]--;//correct hash
        fout = sceIoOpen("ms0:/kirk/hash_done.txt",PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC,0777);
        sprintf(str_buf, "Hash found at offset :%1X\n hash is: 0x%08X\n Please put hash after HASH_OFFSET%1X\n", hash_offset, hash[0], hash_offset);
        sceIoWrite(fout,str_buf,strlen(str_buf));
        sceIoClose(fout); 
    }
    else
    {
        fout = sceIoOpen("ms0:/kirk/hash_not_done.txt",PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC,0777);
        sprintf(str_buf, "Hash not found at offset :%1X\n hash is not between: 0x%08X and 0x%08X\n", hash_offset, hash_start, hash_end);
        sceIoWrite(fout,str_buf,strlen(str_buf));
        sceIoClose(fout); 
    }

    DisableKirk();   

    sceKernelDelayThread(DELAY_THREAD_SEC*10);

    sceKernelExitGame();
	return 0;
}

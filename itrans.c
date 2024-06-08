#include<stdio.h>
#include<locale.h>
#include<wchar.h>
#include<string.h>

#include"converter.c"

// prototypes

int isnotvalid(wchar_t ch);
int findbound(wchar_t *ch);
int isnotvalidwc(wchar_t ch);
int searchindex(wchar_t ch, wchar_t *itrans);

void mapping(char *map, wchar_t *itrans);
void englishtohindi(char *file1, char *file2, wchar_t *itrans);
void hinditoeng(char *file1,char *file2, wchar_t *itrans);


//***********************************************************************************

// invalid characters for storing in buffer
int isnotvalid(wchar_t ch)
{
        int i=0;
        if(ch == ' ' || ch == '\t' || ch == '\n')
        {
                i=1;
        }

        return i;
}

// filter devnagari chars
int isnotvalidwc(wchar_t ch)
{
        int i=0;
        if(ch ==L' ' || ch == L'\t' || ch == L'\n')
        {
                i=1;
        }

        return i;
}

//************************************************************************************

int searchindex(wchar_t ch, wchar_t *itrans)
{
    setlocale(LC_ALL,"");
    int i=0;
    int f=-2;
    int count=0;
    if(ch<2305 || ch>2416)  //unicode range of devnagari characters
    {
        f=-3;
    }

    while(count!=3 && f==-2)
    {
        if(itrans[i]=='<')
        {
            count++;
        }

        if(ch==itrans[i] && count!=3)
        {
            f=i;
        }
        i++;
    }

    return f+1;
}

//**************************************************************************************

int anothersearchindex(wchar_t ch,wchar_t *itrans,int i)
{
    setlocale(LC_ALL,"");
    int f=-2;
    int count=0;

    while(count!=1 && f==-2)
    {
        if(itrans[i]=='<')
        {
            count++;
        }

        if(ch==itrans[i] && count!=3)
        {
            f=i;
        }
        i++;
    }

    return f+1;
}


//***************************************************************************************


void englishtohindi(char *file1, char *file2, wchar_t *itrans)
{
        setlocale(LC_ALL,"");
        int errno=0;
        FILE *input = fopen(file1,"r");
        if(input==NULL)
        {
                errno=1;
                fprintf(stderr,"%s file does not exits. Ensure that given input file is made correctly.\n",file1);
        }

        if(file2==NULL)
        {
                file2 = "output";
        }
        FILE *output = fopen(file2,"w");

        char ch;
        int f=0;
        int i=0;
        int c=0;
        char eword[100];
        wchar_t dword[100];

        if(errno==0)
        {
        while((ch=fgetc(input))!=EOF)
        {
                if(isnotvalid(ch))
                {
                        fprintf(output,"%c",ch);
                }

                if(!isnotvalid(ch) && ch!=EOF)
                {
                        i=0;
                        while(!isnotvalid(ch) && ch!=EOF)
                        {
                                eword[i]=ch;
                                i++;
                                ch = fgetc(input);
                        }
                        eword[i]='\0';
                        converter(eword,dword,itrans);
                        i=0;
                        while(dword[i]!=L'\0')
                        {
                                fwprintf(output,L"%lc",dword[i]);
                                i++;
                        }
                }
        }
        }

        fclose(input);
        fclose(output);
}

//**************************************************************************************



void hinditoeng(char *file1,char *file2, wchar_t *itrans)
{
        int errno=0;

        FILE *input = fopen(file1,"r");

        if(file2 == NULL)
        {
                file2="output";
        }

        FILE *output = fopen(file2,"w");

        if(input==NULL)
        {
                errno=1;
                fprintf(stderr,"%s file does not exits. Ensure that given input file is made correctly.\n",file1);
        }

        wchar_t ch;
        int i;

        int bound = findbound(itrans);  //@vowels

        if(errno==0)
        {
        while((ch=fgetwc(input))!=WEOF)
        {


                if(isnotvalid(ch))
                {
                        fprintf(output,"%c",ch);
                }

                if(!isnotvalidwc(ch) && errno==0)
                {

                        i=searchindex(ch,itrans);

                        if(i==-1)
                        {
                                i=anothersearchindex(ch,itrans,bound);
                        }



                        if(i>bound && i!=-1)
                        {
                                //push the pointer back by 1 character
                                fseek(output,-1,SEEK_CUR);
                        }

                        while(itrans[i]!=';'&& itrans[i]!='/' && i!=-1)
                        {
                                fprintf(output,"%c",itrans[i]);
                                i++;
                        }

                }
        }

        fclose(input);
        fclose(output);
        }
}



//**************************************************************************************



void mapping(char *map, wchar_t *itrans)
{
        setlocale(LC_ALL,"");
        FILE *file = fopen(map,"r");
        int errno=0;

        if(file==NULL)
        {
                errno=1;
                fprintf(stderr,"%s file does not exits. Ensure that given mapping file is made correctly.\n",map);
        }

        wchar_t ch;
        int i=0;

        while((ch = fgetwc(file))!=WEOF && errno==0)
        {
                if(!isnotvalid(ch))
                {
                        itrans[i]=ch;
                        i++;
                }

        }

        itrans[i]=L'\0';
        fclose(file);

	i=0;
/*
	while(itrans[i]!= '\0')
	{
		wprintf(L"   %lc   |",itrans[i]);
		i++;
	}
*/	
}


//**************************************************************************************


int main(int arg, char *argv[])
{
        int cmp;
        if(arg<2)
        {
                fprintf(stderr,"Flag is not Provided {-eh/-he}\n");
        }

        if(arg<3)
        {
                fprintf(stderr,"Mapping File is not Provided\n");
        }

        if(arg<4)
        {
                fprintf(stderr,"Input File is not provided\n");
        }



        if(arg>2)
        {
                cmp = strcmp("-eh",argv[1]);
                if(cmp==0)
                {
                wchar_t itrans[800];
                mapping(argv[2],itrans);
                englishtohindi(argv[3],argv[4],itrans);
                }

                cmp = strcmp("-he",argv[1]);
                if(cmp==0)
                {
                wchar_t itrans[800];
                mapping(argv[2],itrans);
                hinditoeng(argv[3],argv[4],itrans);
                }
        }
        return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_STR 100+1
#define BUFF_SIZE MAX_STR+20

void izlistaj_meni (void); 

int main ()
{
	while(1)
	{
		FILE *fp = NULL;
		char *stred = NULL, *str = NULL;
		size_t num_stred =MAX_STR;
		size_t num_str = BUFF_SIZE;
		size_t procitano;
		char unos;

		izlistaj_meni();
		do
		unos = getchar();
		while (unos==EOF||(unos!='1'&&unos!='2'&&unos!='3'&&unos!='4'&&unos!='5'&&unos!='6'&&unos!='7'&&unos!='Q'));
		//printf("%c\n",unos);
		switch(unos)
		{
			case '1':
			fp = fopen("/dev/stred","r");
			if (fp==NULL)
			{
			puts("Problem pri otvaranju /dev/stred");
			break;
			}

			stred = (char*)malloc(num_stred);
			do{
			procitano = getline(&stred, &num_stred, fp);}
			while(procitano<=1||procitano==-1);
			printf("Trenutno stanje stringa: %s\n",stred);
			//printf("%d\n",procitano);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			free(stred);
			break;

			case '2':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			puts("Unesite novi string: ");
			str = (char*)malloc(BUFF_SIZE);
			do
			procitano = getline(&str,&num_str,stdin);
			while (procitano <=1);
			fprintf(fp,"string=%s",str);
			puts("String upisan.");
			free(str);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;

			case '3':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			puts("Unesite string za konkatinaciju: ");
			str = (char*)malloc(BUFF_SIZE);
			do
			procitano = getline(&str,&num_str,stdin);
			while (procitano <=1);
			fprintf(fp,"append=%s",str);
			puts("String konkatiniran.");
			free(str);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;
			case '4':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}
			fputs("clear\n",fp);
			puts("String obrisan.");
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;
			case '5':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			fprintf(fp,"shrink\n");
			puts("Izbrisani vodeci i prateci space karakteri.");
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;
			case '6':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			puts("Unesite izraz: ");
			str = (char*)malloc(BUFF_SIZE);
			do
			procitano = getline(&str,&num_str,stdin);
			while (procitano <=1);
			fprintf(fp,"remove=%s",str);
			puts("Izraz izbrisan.");
			free(str);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;
			case '7':
			fp = fopen("/dev/stred","w");
			if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			puts("Unesite n: ");
			fflush(stdin);
			do
			unos = getchar();
			while(unos==EOF||(unos=='\n'||unos==' '));
			fprintf(fp,"truncate=%c\n",unos);
			printf("Poslednjih %c karaktera uspesno izbrisano.\n",unos);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				break;
			}
			break;

			case 'Q':
			return 0;
			default:
			break;
		}
		sleep(1);

	}
	return 0;

}

void izlistaj_meni (void){
	puts("********************************************\n");
	puts("<<<<<<<<<<<<<<<<<< MENI >>>>>>>>>>>>>>>>>>>>");
	puts("(unesite broj ispred komande za pokretanje)");
	puts("\n1 Procitaj trenutno stanje stringa");
	puts("2 Upisi novi string");
	puts("3 Konkataniraj string na trenutni");
	puts("4 Izbrisi citav string");
	puts("5 Izbrisi vodece i pratece space karaktere");
	puts("6 Izbrisi izraz iz stringa");
	puts("7 Izbrisi poslednjih n karaktera iz stringa");
	puts("Q Izlaz iz programa\n");
	puts("********************************************");
}



#include <stdio.h>
#include <stdlib.h>
#define MAX_STR 100+1
#define BUFF_SIZE MAX_STR+20

void izlistaj_meni (void); 

int main ()
{
	FILE *fp;
	char *stred, *str;
	size_t num_of_bytes =MAX_STR;
	size_t num = BUFF_SIZE;
	char unos;

	while(unos!='Q') // Vrti se dok ne pritisnes Q
	{
		izlistaj_meni();
		unos = getchar();
		switch(unos)
		{
			case '1':
				fp = fopen("/dev/stred","r");
				if (fp==NULL)
			{
				puts("Problem pri otvaranju /dev/stred");
				return -1;
			}

			stred = (char*)malloc(num_of_bytes+1);
			getline(&stred, &num_of_bytes, fp);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
			}
			printf("Trenutno stanje stringa: %s\n",stred);
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
			str = (char*)malloc(BUFF_SIZE+1);
			fflush(stdin);
			while (getline(&str,&num,stdin)!=0);
			fprintf(fp,"string=%s\n",str);
			puts("String upisan.");
			free(str);
			if (fclose(fp))
			{
				puts("Problem pri zatvaranju /dev/stred");
				return -1;
			}
			break;

			case '3':
			break;
		}

	}

}

void izlistaj_meni (void){
	puts("Meni:");
	puts("(unesite broj ispred komande za pokretanje)");
	puts("\n1. Procitaj trenutno stanje stringa");
	puts("2. Upisi novi string");
	puts("3. Konkataniraj string na trenutni");
	puts("4. Izbrisi citav string");
	puts("5. Izbrisi vodece i pratece space karaktere");
	puts("6. Izbrisi izraz iz stringa");
	puts("7. Izbrisi poslednjih n karaktera iz stringa");

}



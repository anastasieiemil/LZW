#pragma once
#include "stdafx.h"
#define BackSlashN 255		//codez \n deoarece functia fread imi pune \r
#define Modulo 254				//cu cat impart
#define Dimensiune 65276			//+256
#define dimensiune 64513			//numar prim
struct Table {
	int index;
	char* cod;
};
int hash(char* sir)
{
	int key = 0;
	int i = 0;
	while (sir[i] != 0) {
		key = key * 33 + sir[i];
		key = key % dimensiune;
		i++;
	}
	return abs(key) + 256;
}
void Init(Table*& a)
{
	for (int i = 0; i < Dimensiune; i++) {
		a[i].index = NULL;
	}
}
void Free(Table* a)
{
	for (int i = 255; i < Dimensiune; i++)
		if (a[i].index != NULL)
			free(a[i].cod);
	free(a);
}
void SwitchAddress(char* adresa)
{
	int i = strlen(adresa);
	while (adresa[i--] != '.');
	adresa[i + 2] = NULL;
	strcat(adresa, "eb");
	//adresa[++i]='_';


}
int Check(char* sir, Table* a, bool& bine, int& verificat)
{
	bine = true;
	if (strlen(sir) == 1) {
		verificat = sir[0];
		return sir[0];
	}
	int key = hash(sir);
	if (a[key].index != 0)
	{
		if (strcmp(sir, a[key].cod) != 0) {
			bine = false;
			return 0;
		}
		verificat = key;
		return key;
	}
	return 0;
}
int Add(Table*& a, char* sir)
{
	int key = hash(sir);
	a[key].cod = (char*)malloc((strlen(sir) + 1) * sizeof(char));
	a[key].index = key;
	strcpy_s(a[key].cod, strlen(sir) + 1, sir);
	return key;
}
void Compress(FILE* input, char* foutput, HWND hWnd)
{
	Table* a = (Table*)malloc(Dimensiune * sizeof(Table));
	Init(a);
	FILE* output;
	fopen_s(&output, foutput, "w");
	char buffer[1000], s[2], charArray[1000];
	int i, k = 0;
	int cat;
	bool ok = true;				//daca am coliziune in dictionar imi spune sa nu suprascriu dictionarul
	int checked;			//imi retine codarea pentru verificare
	charArray[0] = NULL;
	while (i = fread(buffer, 1, 1000, input))
	{
		k = 0;
		while (i > 0)
		{
			s[0] = buffer[k];
			if (s[0] == '\r')			//sar caracterul \r
			{
				s[0] = buffer[++k];
				i--;
			}
			s[1] = NULL;	//citesc caracter cu caracter
			strcat_s(charArray, s);//concateneaza
			switch (Check(charArray, a, ok, checked))		//functia verifica imi returneaza 0 daca nu s-a gasit si imi retine in verificat ultimul cod
			{
			case 0:										//daca sir + caracter nu  se afla in tabela
				cat = checked / Modulo;
				checked = checked % Modulo;			//verificat este restu
				if ((char)cat == '\n')
					cat = BackSlashN;
				if ((char)checked == '\n')
					checked = BackSlashN;
				fprintf(output, "%c%c", cat, checked);
				if (ok)
					Add(a, charArray);		//imi adauga la tabela sirul
				charArray[0] = charArray[strlen(charArray) - 1];		//retin caracaterul ce legat cu sirul initial nu se gasete in dictionar
				charArray[1] = NULL;
				checked = charArray[0];				//daca sirul nu este in tabela imi va retine mereu acelasi cod. Mereu va fi caracter
				break;
			}
			k++;
			i--;
		}

	}
	Free(a);
	fclose(input);
	fclose(output);
}
void Uncompress(FILE* input, char* foutput)
{
	Table* a = (Table*)malloc(Dimensiune * sizeof(Table));
	Init(a);
	FILE* output;
	fopen_s(&output, "decompresat.txt", "w");
	char sir[100], buffer[1010];
	int s, i, k, contor;
	int cat;
	int n = 255;
	int indec;
	fread(buffer, 1, 2, input);		//citsc primele doua caractere  functia fscanf_s este proasta 
	fprintf(output, "%c", buffer[1]);			//afisez primul caracter
	sir[0] = buffer[1];			//retin caracterul intr-un sir
	sir[1] = NULL;
	while ((contor = fread(buffer, 1, 1000, input)) != 0) {		//functia fread imi returneaza un numar fals de caractere citite
		k = 0;
		i = 0;
		while (contor > 0) {
			int aux = strlen(sir);
			if (k + 1 == 1000)				//am o conditie care imi mai sare caractere si se strica citirea doua cu cate doua caractere
				goto sar;
			cat = buffer[k];
			s = buffer[k + 1];
		jump:			// eroare din cauza functiei fread, deoarece imi pune \r dupa fiecare \n si im deregleaza ordinea de citire
			cat = (char)cat;			//face conversie
			s = (char)s;
			if (cat < 0)				//!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!
				cat = 256 + cat;
			if (s < 0)
				s = s + 256;
			if (s == BackSlashN) {
				s = '\n';	//este posibil sa am si un numar negativ, pentru ca am diacritice
			}
			if (cat == BackSlashN) {
				cat = '\n';
			}
			indec = Modulo * cat + s;
			if (indec > 255 && a[indec].index == NULL) {			//algoritmul de decompresie are un mic bug 
				sir[aux] = sir[0];
				sir[aux + 1] = NULL;
				indec = Add(a, sir);
				goto sari;			//sar cativa pasi
			}
			if (indec <= 255) {			//daca este un singur caracter
				sir[aux] = s;				//il adaug la sirul anterior
				sir[aux + 1] = NULL;
			}
			else {
				sir[aux] = a[indec].cod[0];		//adaug la sirul anterior primul caracter din sirul citit
				sir[aux + 1] = NULL;
			}
			if (a[hash(sir)].index == 0)
				Add(a, sir);		//pun in dictionar
		sari:
			if (indec <= 255) {				//actualizez sirul care imi retine precedentul
				fprintf(output, "%c", s);		//afisez
				sir[0] = indec;					//pun caracterul
				sir[1] = NULL;
			}
			else {
				fprintf(output, "%s", a[indec].cod);//afisez sirul
				strcpy_s(sir, strlen(a[indec].cod) + 1, a[indec].cod);
			}
			contor -= 2;
			k += 2;
		}
	sar:
		if (i % 2 != 0)	//daca este impar avem problema
		{
			s = fgetc(input);
			if (s == -1)		//eof
				break;
			i = 0;
			goto jump;
		}

	}
	Free(a);
	fclose(input);
	fclose(output);
}
FILE* InOpen(HWND hWnd, HWND caseta, char* adresaa)
{
	OPENFILENAME ofn;
	FILE* finput;
	ZeroMemory(&ofn, sizeof(ofn));
	char adress[MAX_PATH] = "";

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = _T("Text Files (*.txt)\0*.eb;*.txt\0All Files (*.*)\0*.*\0");
	ofn.lpstrFile = (LPWSTR)adress;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("txt");

	if (GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hWnd, ID_OPEN);
		SetWindowText(caseta, (LPWSTR)adress);
		wcstombs(adresaa, (LPWSTR)adress, MAX_PATH);
		fopen_s(&finput, adresaa, "rb");
		return finput;
	}
	return NULL;
}
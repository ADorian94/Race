#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "be.txt"
#define OUTPUT_FILE "ki.txt"
#define READ_TIER_PATTERN "%c %f %d %f %d %f[^\n]"

typedef struct TierChange
{
    int ChangeLap;          //mikor cserélünk kereket
    char Tier;              //milyen kerékre cserélünk
}TierChange;

typedef struct TireDetails {
   char TypeName;
   float Time;              //új köridő
   int BeginningOfErosion;  //kopás kezdete (hányadik kör)
   float ExtentOfErosion;   //kopás mértéke
   int BeginningOfDamge;    //elhasználódás kezdete (hányadik kör)
   float ExtentOfDamage;    //elhasználódás mértéke
} TireDetails;

typedef struct RaceData 
{
    TireDetails DetailsOfTire[3];   //kerekek részletei
    unsigned Laps;                  //hány körből állt a verseny
    float TimeInPit;                //mennyi időt töltünk a box-ban
    TierChange Strategy[256];       //kerék stratégia
    unsigned NumberOfChanges;       //hányszor cserélünk kereket -> Strategy tömb elemeinek száma
}RaceData;

TireDetails GetTierDetailsByName(RaceData *raceData, char tierName);
TireDetails GetCurrentTierByLap(RaceData *raceData, int lap, int *indexOfTier, int *roundWithTier);
float GetCurrentLapTime(TireDetails *currentTier, int roundWithTier);
void WriteResultToFile(float result);
int ReadDataFromFile(RaceData *raceData);
float ProcessRaceData(RaceData *raceData, int isDebugMode);
void PrintRaceData(RaceData *raceData);
int CheckArgument(char *expArg, char *currArg);

int main(int argc, char *argv[])
{
    //ha debug mode-ban indítottuk programot, akkor kiírjuk a számításokat később a konzolra
    int isDebugMode = 1;

    if(argc > 1 && CheckArgument("debug", argv[1]))
    {
        isDebugMode = 0;
    }

    //Fájl beolvasása
    RaceData raceData; 
    int result = ReadDataFromFile(&raceData);

    if(isDebugMode == 0)
    {
        PrintRaceData(&raceData);
    }

    //todo: validálni a stratégiát, hogy megfelelő e 
    // A stratégiai hibák (kevesebb, mint két keverék használata; és kevesebb, mint egy kiállás) esetén a
    // kimenetbe csak a ”HIBA” szöveg kerüljön.

    //Adatok feldolgozása
    float sumTime = ProcessRaceData(&raceData, isDebugMode);
    //Eredmény kiírása fájlba
    WriteResultToFile(sumTime);

    return 0;
}

TireDetails GetTierDetailsByName(RaceData *raceData, char tierName)
{
    TireDetails currentTier;

    for(int i = 0; i < 3; ++i)
    {
        if(raceData->DetailsOfTire[i].TypeName == tierName)
        {
            currentTier = raceData->DetailsOfTire[i];
        }
    }

    return currentTier;
}

TireDetails GetCurrentTierByLap(RaceData *raceData, int lap, int *indexOfTier, int *roundWithTier)
{
    if((*indexOfTier) + 1 < raceData->NumberOfChanges && lap > raceData->Strategy[(*indexOfTier) + 1].ChangeLap)
    {
        ++(*indexOfTier);
        roundWithTier = 0;
    }

    return GetTierDetailsByName(raceData, raceData->Strategy[*indexOfTier].Tier);
}

float GetCurrentLapTime(TireDetails *currentTier, int roundWithTier)
{
    if(roundWithTier >= currentTier->BeginningOfErosion + currentTier->BeginningOfDamge)
    {
        currentTier->Time += (currentTier->ExtentOfDamage * (1 + (roundWithTier - (currentTier->BeginningOfErosion + currentTier->BeginningOfDamge)))) + currentTier->ExtentOfErosion;
    }
    else if(roundWithTier >= currentTier->BeginningOfErosion)
    {
        currentTier->Time += currentTier->ExtentOfErosion;
    }

    return currentTier->Time;
}

int ReadDataFromFile(RaceData *raceData)
{
    FILE *filePtr;
    filePtr = fopen(INPUT_FILE, "r");

    //ha a file nem nyitható meg
    if(filePtr == NULL)
    {
        printf("Hiba a fájl megnyitása közben!\n");
    }

    TireDetails detail; 
    int indexOfLines = 0; 

    while(!feof(filePtr) && indexOfLines < 6)
    {
        if(indexOfLines < 3)
        {
            int numberOfElements = fscanf(filePtr, READ_TIER_PATTERN, &detail.TypeName, &detail.Time, &detail.BeginningOfErosion, &detail.ExtentOfErosion, &detail.BeginningOfDamge, &detail.ExtentOfDamage);

            //nem biztos, hogy így van értelme, de ha nem 6 adatot tartalmaz a file első 3 sora, akkor azt nem adjuk hozzá
            if(numberOfElements == 6)
            {
                raceData->DetailsOfTire[indexOfLines] = detail;
                ++indexOfLines;
            }
        }
        else if(indexOfLines == 3)
        {
            int numberOfLaps;
            fscanf(filePtr, "%d%*[^\n]", &numberOfLaps);
            ++indexOfLines;
            raceData->Laps = numberOfLaps;
        }
        else if(indexOfLines == 4)
        {
            float timeInOPit;
            fscanf(filePtr, "%f%*[^\n]", &timeInOPit);
            ++indexOfLines;
            raceData->TimeInPit = timeInOPit;
        }
        else if(indexOfLines == 5)
        {
            int i, index = 0;
            char ch;

            while(fscanf(filePtr, "%d%c[^\n]", &i, &ch) == 2)
            {
               raceData->Strategy[index].ChangeLap = i;
               raceData->Strategy[index].Tier = ch;
               ++index;
            }

            raceData->NumberOfChanges = index;
            ++indexOfLines;    
        }
        else
        {
            printf("Something went wrong.");
            ++indexOfLines;
        }
    }

    //file bezárása
    fclose(filePtr);

    return 0;
}

void WriteResultToFile(float result)
{
    FILE *filePtr;
    filePtr = fopen(OUTPUT_FILE, "w");

    if(filePtr == NULL)
    {
        printf("Hiba a fájl kiírása közben.\n");
    }

    fprintf(filePtr, "%0.3f\n", result);
    fclose(filePtr);
}

float ProcessRaceData(RaceData *raceData, int isDebugMode)
{
    int indexOfTier = 0, roundWithTier = 0;
    float sumTime = (raceData->NumberOfChanges-1) * raceData->TimeInPit; 
    TireDetails currentTier = GetTierDetailsByName(raceData, raceData->Strategy[indexOfTier].Tier);

    for(int i = 0; i < raceData->Laps; ++i)
    {
        if((indexOfTier) + 1 < raceData->NumberOfChanges && i >= raceData->Strategy[(indexOfTier) + 1].ChangeLap)
        {
            ++(indexOfTier);
            roundWithTier = 0;
            currentTier = GetTierDetailsByName(raceData, raceData->Strategy[indexOfTier].Tier);
        }

        float x = GetCurrentLapTime(&currentTier, roundWithTier);
        sumTime += x;

        if(isDebugMode == 0)
        {
            printf("Lap:= %d, LapWithTier:= %d, sumTime:= %0.3f, currentLapTime:= %0.1f, currentTier:= %c\n", i+1, roundWithTier+1, sumTime, x, currentTier.TypeName);
        }

        ++roundWithTier;
    }

    if(isDebugMode == 0)
    {
        printf("----------\nTime %0.3f\n", sumTime);
    }

    return sumTime;
}

void PrintRaceData(RaceData *raceData)
{
    for(int i = 0; i < 3; ++i)
    {
        printf("%c %0.3f %d %0.3f %d %0.3f\n", raceData->DetailsOfTire[i].TypeName, raceData->DetailsOfTire[i].Time, raceData->DetailsOfTire[i].BeginningOfErosion, raceData->DetailsOfTire[i].ExtentOfErosion, raceData->DetailsOfTire[i].BeginningOfDamge, raceData->DetailsOfTire[i].ExtentOfDamage);
    }

    printf("%d\n", raceData->Laps);
    printf("%0.3f\n", raceData->TimeInPit);

    for (int i = 0; i < raceData->NumberOfChanges; ++i)
    {
        printf("%d%c ", raceData->Strategy[i].ChangeLap, raceData->Strategy[i].Tier);
    }

    printf("\n----------\n");
}

int CheckArgument(char *expArg, char *currArg)
{
    while (*expArg && *currArg && *expArg == *currArg) {
		++expArg;
		++currArg;
	}

	return *expArg == *currArg;
}
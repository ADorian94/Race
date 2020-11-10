#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INPUT_FILE "be.txt"
#define OUTPUT_FILE "ki.txt"
#define READ_TIER_PATTERN "%c %f %d %f %d %f[^\n]"

typedef struct Strategy
{
    int ChangeLap;          //mikor cserélünk kereket
    char Tier;              //milyen kerékre cserélünk
}Strategy;

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
    float TimeInPit;               //mennyi időt töltünk a box-ban
    Strategy Strategy[256];         //kerék stratégia
    unsigned NumberOfChanges;
}RaceData;

void SetPrecision(TireDetails *detail);
TireDetails GetTierDetailsByName(RaceData *raceData, char tierName);
TireDetails GetCurrentTierByLap(RaceData *raceData, int lap, int *indexOfTier, int *roundWithTier);
float GetCurrentLapTime(TireDetails *currentTier, int roundWithTier);

//todo: javítani mindent mert ez egy fos :)
//todo: hibakezelés
//todo: feladat megoldása
int main()
{
    //file megnyitása
    FILE *filePtr;
    filePtr = fopen(INPUT_FILE, "r");

    //ha a file nem nyitható meg
    if(filePtr == NULL)
    {
        printf("Hiba a fájl megnyitása közben!\nNem létezik a be.txt.");
        return 1;
    }

    RaceData raceData;
    TireDetails dataFromFile[3];
    TireDetails detail; 
    int indexOfLines = 0; 

    while(!feof(filePtr) && indexOfLines < 6)
    {
        if(indexOfLines < 3)
        {
            int numberOfElements = fscanf(filePtr, READ_TIER_PATTERN, &detail.TypeName, &detail.Time, &detail.BeginningOfErosion, &detail.ExtentOfErosion, &detail.BeginningOfDamge, &detail.ExtentOfDamage);

            SetPrecision(&detail);

            //nem biztos, hogy így van értelme, de ha nem 6 adatot tartalmaz a file első 3 sora, akkor azt nem adjuk hozzá
            if(numberOfElements == 6)
            {
                raceData.DetailsOfTire[indexOfLines] = detail;
                printf("%c %0.1f %d %0.1f %d %0.1f\n", detail.TypeName, detail.Time, detail.BeginningOfErosion, detail.ExtentOfErosion, detail.BeginningOfDamge, detail.ExtentOfDamage);
                ++indexOfLines;
            }
        }
        else if(indexOfLines == 3)
        {
            int numberOfLaps;
            fscanf(filePtr, "%d%*[^\n]", &numberOfLaps);
            printf("%d\n", numberOfLaps);
            ++indexOfLines;
            raceData.Laps = numberOfLaps;
        }
        else if(indexOfLines == 4)
        {
            float timeInOPit;
            fscanf(filePtr, "%f%*[^\n]", &timeInOPit);
            timeInOPit = floor(10*timeInOPit)/10;
            printf("%0.1f\n", timeInOPit);
            ++indexOfLines;
            raceData.TimeInPit = timeInOPit;
        }
        else if(indexOfLines == 5)
        {
            int i, index = 0;
            char ch;

            while(fscanf(filePtr, "%d%c[^\n]", &i, &ch) == 2)
            {
               printf("%d%c ", i, ch);
               raceData.Strategy[index].ChangeLap = i;
               raceData.Strategy[index].Tier = ch;
               ++index;
            }
            printf("\n");
            raceData.NumberOfChanges = index;
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

    //ha idáig eljutottunk, akkor beolvastuk a fájlt és a raceData tartalmaz mindent amire szükségünk van -> fel kell dolgozni azt a sok szart amit beolvastunk neki!!!
    TireDetails currentTier;
    int indexOfTier = 0, roundWithTier = 0;
    float sumTime = (raceData.NumberOfChanges-1) * raceData.TimeInPit; 

    currentTier = GetTierDetailsByName(&raceData, raceData.Strategy[indexOfTier].Tier);

    printf("%c\n", currentTier.TypeName);

    for(int i = 0; i < raceData.Laps; ++i)
    {
        if((indexOfTier) + 1 < raceData.NumberOfChanges && i >= raceData.Strategy[(indexOfTier) + 1].ChangeLap)
        {
            ++(indexOfTier);
            roundWithTier = 0;
            currentTier = GetTierDetailsByName(&raceData, raceData.Strategy[indexOfTier].Tier);
        }


        float x = GetCurrentLapTime(&currentTier, roundWithTier);
        sumTime += x;
        printf("Lap:= %d, LapWithTier:= %d, sumTime:= %0.3f, currentLapTime:= %0.1f, currentTier:= %c\n", i+1, roundWithTier+1, sumTime, x, currentTier.TypeName);

        ++roundWithTier;
    }

    return 0;
}

void SetPrecision(TireDetails *detail)
{
    detail->Time = floor(10*detail->Time)/10;
    detail->ExtentOfDamage = floor(10*detail->ExtentOfDamage)/10;
    detail->ExtentOfErosion = floor(10*detail->ExtentOfErosion)/10;
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


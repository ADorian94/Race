#include <stdio.h>
#include <stdlib.h>

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
    float TimeInPit;                //mennyi időt töltünk a box-ban
    Strategy Strategy[256];         //kerék stratégia
}RaceData;

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
            int numberOfElements = fscanf(filePtr, READ_TIER_PATTERN, &detail.TypeName, &detail.Time, &detail.BeginningOfErosion, &detail.ExtentOfErosion, &detail.BeginningOfDamge, &detail.ExtentOfErosion);

            //nem biztos, hogy így van értelme, de ha nem 6 adatot tartalmaz a file első 3 sora, akkor azt nem adjuk hozzá
            if(numberOfElements == 6)
            {
                raceData.DetailsOfTire[indexOfLines] = detail;
                printf("%c %f %d %f %d %f\n", detail.TypeName, detail.Time, detail.BeginningOfErosion, detail.ExtentOfErosion, detail.BeginningOfDamge, detail.ExtentOfErosion);
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
            printf("%f\n", timeInOPit);
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
    return 0;
}


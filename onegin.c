#include <stdio.h>
#include <malloc.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "onegin.h"

static char   GetArrElem       (char* arr, size_t elemIndex);
static char*  GetArrElemPtr    (char* arr, size_t elemIndex);
static size_t CalcFileLen      (const char* const fileName);
static void   SetWord          (char** split_buffer, size_t* word_i, char* SetWord);
static int    IsPassSymbol     (const char c);
static void   FindFirstNotPass (char* buffer, size_t* buffer_i);
static void   Fread            (char* buffer, size_t bufferLen, FILE* filePtr);
static void   ReadBufRealloc   (char*** split_buffer, size_t splitBufSize);
static int    IsInt            (const char* const str, const char* const strEnd);

//---------------------------------------------------------------------------------------------------------------------------------------------------

int strtoi(const char* const str)
{
    assert(str);

    long res = 0;
    char* strEnd = NULL;
    res = strtol(str, &strEnd, 10);

    if (!IsInt(str, strEnd))
    {
        printf("\n\n'%s' - IS NOT A NUMBER.\n\n", str);
        assert(0 && "try to convert not int str to int.");
    }

    return (int) res;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static int IsInt(const char* const str, const char* const strEnd)
{
    assert(str);
    assert(strEnd);

    return (int) strlen(str) == (strEnd - str);
}

//============================ Read File ==============================================================================================================

char** ReadBufferFromFile(const char* const file, size_t* bufSize)
{
    assert(file);
    assert(bufSize);

    FILE* filePtr = fopen(file, "rb");
    assert(filePtr);

    size_t bufferLen = CalcFileLen(file);

    char*  buffer       = (char*)  calloc(bufferLen + 2, sizeof(*buffer));
    char** split_buffer = (char**) calloc(bufferLen + 2, sizeof(*split_buffer));

    assert(buffer);
    assert(split_buffer);

    Fread(buffer, bufferLen, filePtr);
    fclose(filePtr);

    buffer[bufferLen]     = ' ';
    buffer[bufferLen + 1] = '\0';

    size_t word_i = 0;
    SetWord(split_buffer, &word_i, GetArrElemPtr(buffer, 0));
    split_buffer++;

    size_t buffer_i = 0;
    FindFirstNotPass(buffer, &buffer_i);

    word_i = 0;
    SetWord(split_buffer, &word_i, GetArrElemPtr(buffer, buffer_i));

    for (; buffer_i <= bufferLen + 1; buffer_i++)
    {
        char bufElem = GetArrElem(buffer, buffer_i);
        if (IsPassSymbol(bufElem))
        {
            do
            {
                buffer[buffer_i] = '\0';
                buffer_i++;
                bufElem = GetArrElem(buffer, buffer_i);
            }
            while (IsPassSymbol(bufElem) && buffer_i <= bufferLen);
            SetWord(split_buffer, &word_i, GetArrElemPtr(buffer, buffer_i));
        }
    }

    *bufSize = word_i - 1;

    ReadBufRealloc(&split_buffer, *bufSize);

    assert(split_buffer);

    return split_buffer;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

void BufferDtor(char** buffer)
{
    assert(buffer);
    assert(*buffer);

    buffer--;
    assert(buffer);
    assert(*buffer);

    FREE(*buffer)
    FREE(buffer);

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static void Fread(char* buffer, size_t bufferLen, FILE* filePtr)
{   
    assert(buffer);
    assert(filePtr);

    size_t freadReturn = fread(buffer, sizeof(char), bufferLen, filePtr);
    assert(freadReturn == bufferLen);
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static void ReadBufRealloc(char*** split_buffer, size_t splitBufSize)
{
    assert(split_buffer);
    assert(*split_buffer);
    assert(**split_buffer);

    (*split_buffer)--;
    assert(split_buffer);

    *split_buffer = (char**) realloc(*split_buffer, (splitBufSize + 1) * sizeof(char*));

    assert(split_buffer);
    (*split_buffer)++;

    assert(split_buffer);
    assert(*split_buffer);
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static size_t CalcFileLen(const char* const fileName)
{
    assert(fileName);
    struct stat buf = {};
    stat(fileName, &buf);
    return (size_t) buf.st_size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static char GetArrElem(char* arr, size_t elemIndex)
{
    assert(arr);
    return arr[elemIndex];
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static char* GetArrElemPtr(char* arr, size_t elemIndex)
{
    assert(arr);
    return &arr[elemIndex];
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static void SetWord(char** split_buffer, size_t* word_i, char* SetWord)
{
    assert(split_buffer);
    assert(SetWord);
    assert(word_i);

    split_buffer[*word_i] = SetWord;

    assert(*(split_buffer + *word_i));

    (*word_i)++;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static int IsPassSymbol(const char c)
{
    return (c == ' ') || (c == '\n') || (c == '\r');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static void FindFirstNotPass(char* buffer, size_t* buffer_i)
{
    assert(buffer);
    assert(buffer_i);

    while(IsPassSymbol(GetArrElem(buffer, *buffer_i)))
    {
        buffer[*buffer_i] = '\0';
        (*buffer_i)++;
    }

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

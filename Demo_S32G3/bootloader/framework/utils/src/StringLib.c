/**
*   @file           StringLib.c
*   @version        24.12.0
*
*   @brief          AUTOSAR AR_MODULE_NAME - String Operations Library interface implementation.
*   @details        String Library Operations interface implementation.
*
*This file contains sample code only. It is not part of the production code deliverables.
*/
/*==================================================================================================
*   Project              : AUTOSAR 4.4 GOLDVIP
*   Platform             : CORTEXM
*   Peripheral           : USED_PERIPHERAL
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 24.12.0
*   Build Version        : S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
==================================================================================================*/
/*==================================================================================================
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif


/*===================================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
====================================================================================================================*/

#include "StringLib.h"
/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/
/*================================================================================================*/
/**
@brief   Function converts a char within the range '0' - '9' to their unsiged integer counterparts
         If the given Char is not a number, the function will return the char itself.

@param[in]  DataChar    Char to be converted

@return Converted Value
*/
/*================================================================================================*/
uint8  StrLib_Ctoi(char DataChar)
{
    uint8 ConvertedNumber = DataChar;
    
    if((DataChar >= '0') && (DataChar <= '9'))
    {
        /*value = prev_value * 10 */
        ConvertedNumber = DataChar - '0';
    }

    return ConvertedNumber;
}




/*================================================================================================*/
/**
@brief   Function is used to calculate the lenght of a given string of chars

@param[in]  DataChar    Data String

@return  length of string
*/
/*================================================================================================*/
uint32 StrLib_StringLen(char * DataStr)
{
   uint32 CharIndex = 0;
 
   while(*(DataStr + CharIndex) != '\0' )
      CharIndex++;
 
   return CharIndex;
}

/*================================================================================================*/
/**
@brief   Reverse the sub-string of DataStr given by the first length characters.

@param[in]  DataChar    Data String
@param[in]  length      Data String length

@return None
*/
/*================================================================================================*/
void StrLib_ReverseStringN(char *DataStr, uint32 length) 
{
   uint32 CharIndex = 0; 
   char *StrRev;
   char  TempStr;
 
   StrRev = DataStr;
 
   for(CharIndex = 0; CharIndex < length - 1U; CharIndex++)
      StrRev++;
 
   for(CharIndex = 0; CharIndex < length / 2U; CharIndex++)
   {        
      TempStr   = *StrRev;
      *StrRev   = *DataStr;
      *DataStr = TempStr;
 
      DataStr++;
      StrRev--;
   } 
}

/*================================================================================================*/
/**
@brief   Reverses a String given as parameter after the null terminator.

@param[in]  DataChar    Data String

@return None
*/
/*================================================================================================*/
void StrLib_ReverseString(char *DataStr) 
{
    char *StrRev;
    char TempStr;

    StrRev = DataStr;
    while (*++StrRev != 0);

    StrRev--;
    while (DataStr < StrRev)
    {
        TempStr = *StrRev;
        *StrRev = *DataStr;
        *DataStr = TempStr;

        DataStr++;
        StrRev--;
    }
}

/*================================================================================================*/
/**
@brief   funtion used to convert an integer in a given numeration base to a string of ascii chars

@param[in]  num    Number to be converted
@param[in]  base   Base for numeration
@param[out] DataStr    Data string for the converted number

@return None

*/
/*================================================================================================*/
int StrLib_Itoa(uint32 num, char * DataStr, uint8 base)
{
    int idx = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if(num == 0)
    {
        DataStr[idx++] = '0';
        DataStr[idx] = '\0';
    }
    else
    {
        /* Process individual digits*/
        while(num != 0)
        {
            int rem = num % base;
            DataStr[idx++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
            num = num/base;
        }
    
        DataStr[idx] = '\0';

        /*Reverse the string*/
        StrLib_ReverseStringN(DataStr, idx);
    }
    
    return idx;
}

/*================================================================================================*/
/**
@brief   function used to convert an ascii string to (unsigned) integer

@param[in] DataChar    Data string for the converted number

@return  converted value of the string
*/
/*================================================================================================*/
uint32 StrLib_Atoi(char * DataChar)
{
    uint32 ConvertedNumber = 0;
    uint32 multiplyer = 1U;
    
    while(*DataChar)
    {
        if(((*DataChar) >= '0') && ((*DataChar) <= '9'))
        {
            ConvertedNumber += ((*DataChar) - '0') * multiplyer;
            multiplyer *= 10U; 
            DataChar++;
        }
    }
    
    return ConvertedNumber;
}

/*================================================================================================*/
/**
@brief   function used to get the position of a certain char withing the given datastring

@param[in] DataChar     Data char whos position needs found
@param[in] DataString   Data string

@return  index in string
*/
/*================================================================================================*/
uint8 StrLib_GetCharIndex(char * DataString, char DataChar)
{
    uint8 Index = 0;

    
    while(DataString[Index] != DataChar)
    {                
        Index++;
    }
    
    return Index;
}


/*================================================================================================*/
/**
@brief   function used to copy the content of one string to an other address

@param[in] InputStr    Source of the copy
@param[out] OututStr    Destination for the data copy
@param[in] StrLen    data length

@return none
*/
/*================================================================================================*/
void StrLib_StrCpy(char* InputStr, char * OututStr, uint8 StrLen)
{
    uint8 StrIndex = 0;
    
    while(StrLen > StrIndex)
    {
        if(InputStr[StrIndex] != '\0')
        {    
            OututStr[StrIndex] = InputStr[StrIndex];
        }    
        StrIndex++;
    }
}


/*================================================================================================*/
/**
@brief   function used to insert a char to a given string. It is assumed that full lengt of the string
          is at least one char larger than StrLen
@details

@param[in] InputStr    Pointer to the Data string
@param[in] DataChar    Char to be inserted
@param[in] CharPosition    Index in string where the inserted char should be placed
@param[in] StrLen    data length before insertion

@return none
*/
/*================================================================================================*/
void StrLib_InsertCharToStr(char * InputStr, char DataChar, uint8 CharPosition, uint8 StrLen)
{
    uint8 StrIndex = 0;
        
    if(CharPosition != StrLen)    
    {    
        for(StrIndex = StrLen; StrIndex > CharPosition; StrIndex--)
        {        
            InputStr[StrIndex + 1U] = InputStr[StrIndex];       
        }
    }
     
    InputStr[CharPosition] = DataChar;
}


/*================================================================================================*/
/**
@brief   function used to insert compare to strings

@param[in] DataStr1    Pointer to the fisrt Data string
@param[in] DataStr2    Pointer to the second Data string

@return  1 if String1 is greater than String2
          - if equal
         -1 if String2 is greater than String1
*/
/*================================================================================================*/
uint8 StrLib_StrCmp(char * DataStr1, char * DataStr2)
{
    uint8 index=0;
    uint8 ReturnValue = 0;
    while((DataStr1[index] != '\0') || (DataStr2[index] != '\0'))
    {
        if((DataStr1[index] > DataStr2[index]) || (DataStr1[index] < DataStr2[index]))
        {    
            ReturnValue = 1;
            break;
        }            
        index++;
    }

    return ReturnValue;
}

/*================================================================================================*/
/**
@brief   function used to concatenate two strings

@param[in] SrcStr    Pointer to the source Data string
@param[in] DestStr    Pointer to the destination string

*/
/*================================================================================================*/
void StrLib_StrCat(char *DestStr, const char *SrcStr)
{
    while(*DestStr)
      DestStr++;
    while(*DestStr++ == *SrcStr++)
      ;
}

/*================================================================================================*/
/**
@brief   function used to remove the first Count characters from string

@param[in] DataString  Pointer to the source Data string
@param[in] Count       Number of characters to remove

*/
/*================================================================================================*/
void StrLib_RemoveFirstN(char *DataString, uint32 Count)
{
    uint32 Length = StrLib_StringLen(DataString);
    uint32 CurrIndex = 0;
    uint32 StrIndex = 0;
    
    if (Count > Length)
    {
        Count = Length;
    }
    
    for (StrIndex = Count; StrIndex < Length; StrIndex++)
    {
        DataString[CurrIndex++] = DataString[StrIndex];
    }

    DataString[CurrIndex] = '\0';
}

/*================================================================================================*/
#ifdef __cplusplus
}
#endif

#include "../signatureparser.h"

#include <ctype.h>
#include <stdio.h>

#include "../../libs/ACUtils/include/ACUtils/astring.h"

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE

    static void private_APluginSDK_replaceParameterDelimiters(struct AString *str, char newDelimiter)
    {
        size_t i, brackets = 0, braces = 0, squareBrackets = 0, templateBrackets = 0, strSize = AString_size(str);
        for(i = 0; i < strSize; ++i) {
            char c = AString_get(str, i);
            switch(c) {
                case '(': ++brackets; break;
                case ')': --brackets; break;
                case '{': ++braces; break;
                case '}': --braces; break;
                case '[': ++squareBrackets; break;
                case ']': --squareBrackets; break;
                case '<': ++templateBrackets; break;
                case '>': --templateBrackets; break;
                case ',':
                    if(brackets == 0 && braces == 0 && squareBrackets == 0 && templateBrackets == 0)
                        AString_set(str, i, newDelimiter);
                    break;
            }
        }
    }

    static void private_APluginSDK_removeDoubleSpaces(struct AString *str)
    {
        size_t i, spaceCount = 0;
        for(i = 0; i < AString_size(str); ++i) {
            char current = AString_get(str, i);
            if(isspace(current)) {
                ++spaceCount;
            } else if(i == spaceCount || spaceCount-- > 1) {
                i -= spaceCount;
                AString_remove(str, i, spaceCount);
                spaceCount = 0;
            } else {
                spaceCount = 0;
            }
        }
        AString_remove(str, AString_size(str) - spaceCount, spaceCount);
    }

    static void private_APluginSDK_separateSpecialChars(struct AString *parameter)
    {
        size_t i;
        for(i = 0; i < AString_size(parameter); ++i) {
            char current = AString_get(parameter, i);
            if(ispunct(current)) {
                AString_insertCString(parameter, i, "  ", 2);
                AString_set(parameter, ++i, current);
                AString_set(parameter, ++i, ' ');
            } else if(isspace(current) && current != ' ') {
                AString_set(parameter, i, ' ');
            }
        }
    }


    /* ====================================== private_APluginSDK_splitParameterList ===================================== */

    static char** private_APluginSDK_splitParameterList(const char* parameterList)
    {
        struct ASplittedString *splittedParameterList;
        size_t parameterCount, i, j, parameterListSize;
        struct AString *types, *names, *parameterListString, *tmpString;
        char **returnArray;
        const char *testStr = "     int *     a,     double     *const**b     ";
        tmpString = AString_construct();
        AString_appendCString(tmpString, testStr, strlen(testStr));
        private_APluginSDK_separateSpecialChars(tmpString);
        private_APluginSDK_removeDoubleSpaces(tmpString);
        printf("%s\n\n", AString_buffer(tmpString));
        AString_destruct(tmpString);
        types = AString_construct();
        names = AString_construct();
        parameterListString = AString_construct();
        AString_appendCString(parameterListString, parameterList, strlen(parameterList));
    /*        private_APluginSDK_separateSpecialCharsRemoveDoubleSpaces(parameterListString); */
        splittedParameterList = AString_split(parameterListString, ',', false);
        parameterCount = ADynArray_size(splittedParameterList);
        for(i = 0; i < parameterCount; ++i) {
            struct AString *parameter = ADynArray_get(splittedParameterList, i);
            struct ASplittedString *splittedParameter = AString_split(parameter, ' ', true);
            size_t partCount = ADynArray_size(splittedParameter);
            if(i != 0) {
                AString_appendCString(types, ", ", 2);
                AString_appendCString(names, ", ", 2);
            }
            if(partCount == 1) {
                AString_appendAString(types, ADynArray_get(splittedParameter, 0));
            } else if(partCount > 1) {
                bool lastWasPointer = false;
                for(j = 0; j < partCount - 1; ++j) {
                    struct AString *tmp = ADynArray_get(splittedParameter, j);
                    if(AString_equalsCString(tmp, "*")) {
                        if(!lastWasPointer)
                            AString_append(types, ' ');
                        lastWasPointer = true;
                    } else {
                        if(!lastWasPointer && j != 0)
                            AString_append(types, ' ');
                        lastWasPointer = false;
                    }
                    AString_appendAString(types, tmp);
                }
                AString_appendAString(names, ADynArray_get(splittedParameter, partCount - 1));
            }
            AString_freeSplitted(splittedParameter);
        }
        AString_freeSplitted(splittedParameterList);
        returnArray = (char**) malloc(2 * sizeof(void*));
        returnArray[0] = (char*) malloc((AString_size(types) + 1) * sizeof(char));
        memcpy(returnArray[0], AString_buffer(types), AString_size(types) + 1);
        returnArray[1] = (char*) malloc((AString_size(names) + 1) * sizeof(char));
        memcpy(returnArray[1], AString_buffer(names), AString_size(names) + 1);
        AString_destruct(types);
        AString_destruct(names);
        AString_destruct(parameterListString);
        return returnArray;
    }

PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE
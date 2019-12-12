#ifndef APLUGINSDK_INFOMANAGER_IPP
#define APLUGINSDK_INFOMANAGER_IPP

// =============================================== splitParameterList =============================================== //

std::pair<char*, char*> apl::detail::splitParameterList(const char* parameterList)
{
    std::string tmpParameterList(parameterList), typeString, tmpString, tmpTypesString, tmpNamesString;
    bool typeFront = true, firstRun = true;
    char last = ' ';
    tmpParameterList.push_back(','); // terminate with ',' to flush last type and name
    for(char current : tmpParameterList) {
        if(typeFront) {
            if(current != ',' && (std::isalnum(current) || std::ispunct(current))) {
                typeString.push_back(current);
            } else if(std::isspace(current) && typeString == "const") {
                typeString.push_back(' ');
            } else if(current == '*' || current == '&') {
                typeString.push_back(current);
                typeFront = false;
            } else if(std::isspace(current) && !std::isspace(last)) {
                typeFront = false;
            }
        } else {
            if(current != ',' && (std::isalnum(current) || std::ispunct(current))) {
                tmpString.push_back(current);
            } else if(current == '*' || current == '&') {
                if(!tmpString.empty() && typeString.back() != '*' && typeString.back() != '&') {
                    typeString.append(" ");
                }
                typeString.append(tmpString).push_back(current);
                tmpString.clear();
            } else if((std::isspace(current) || current == ',') && tmpString == "const") {
                typeString.append(tmpString);
                tmpString.clear();
            }
            if(current == ',') {
                tmpTypesString.append(firstRun ? "" : ", ").append(typeString);
                tmpNamesString.append(firstRun ? "" : ", ").append(tmpString);
                typeString.clear();
                tmpString.clear();
                typeFront = true;
                firstRun = false;
                current = ' '; // 'last' should be ' '
            }
        }
        last = current;
    }
    char* types = new char[tmpTypesString.size() + 1];
    char* names = new char[tmpNamesString.size() + 1];
    types[tmpTypesString.size()] = '\0';
    names[tmpNamesString.size()] = '\0';
    tmpTypesString.copy(types, std::string::npos);
    tmpNamesString.copy(names, std::string::npos);
    return {types, names};
}

#endif //APLUGINSDK_INFOMANAGER_IPP

/*
  ==============================================================================

    MailAPIsManager.h
    Created: 9 Feb 2023 9:11:12pm
    Author:  Robin

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <vector>

class MailAPIsManager 
{
private:
    char* username = nullptr;
    char* password = nullptr;
    char* popsAccount = nullptr;
    
public:
    MailAPIsManager();
    ~MailAPIsManager();

    void OpenGmail(char* _username, char* _password);
    static size_t WriteMemoryCallback(char *contents, size_t size, size_t nmemb, void *userp);
};
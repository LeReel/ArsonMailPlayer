/*
  ==============================================================================

	MailAPIsManager.cpp
	Created: 9 Feb 2023 9:11:12pm
	Author:  Robin

  ==============================================================================
*/

#include "MailAPIsManager.h"

#include <include/curl/curl.h>

struct MemoryStruct
{
    char* memory;
    size_t size;
};

MailAPIsManager::MailAPIsManager()
{
     // CURL* curl;
     // CURLcode res;
     //
     // curl = curl_easy_init();
     // if (curl)
     // {
     // 	// Set the URL for the request
     // 	curl_easy_setopt(curl, CURLOPT_URL, "https://graph.microsoft.com/v1.0/me/messages?$filter=hasAttachments eq true");
     //
     // 	// Set the authorization header
     // 	struct curl_slist* headers = NULL;
     // 	headers = curl_slist_append(headers, "Authorization: Bearer [ACCESS_TOKEN]");
     // 	headers = curl_slist_append(headers, "Content-Type: application/json");
     // 	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
     //
     // 	// Set the function to handle the response data
     // 	std::string responseData;
     // 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunction);
     // 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
     //
     // 	// Perform the request
     // 	res = curl_easy_perform(curl);
     // 	if (res != CURLE_OK)
     // 	{
     // 		std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
     // 	}
     // 	else
     // 	{
     // 		// Parse the JSON response
     // 		json responseJson = json::parse(responseData);
     //
     // 		// Get the list of emails
     // 		std::vector<json> emails = responseJson["value"];
     //
     // 		// Iterate through the emails
     // 		for (const json& email : emails)
     // 		{
     // 			// Get the ID of the email
     // 			std::string emailId = email["id"];
     //
     // 			// Get the list of attachments for the email
     // 			std::string attachmentsUrl = "https://graph.microsoft.com/v1.0/me/messages/" + emailId + "/attachments";
     //
     // 			curl_easy_setopt(curl, CURLOPT_URL, attachmentsUrl.c_str());
     // 			std::string attachmentsResponseData;
     // 			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &attachmentsResponseData);
     //
     // 			res = curl_easy_perform(curl);
     // 			if (res != CURLE_OK)
     // 			{
     // 				std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
     // 			}
     // 			else
     // 			{
     // 				json attachmentsResponseJson = json::parse(attachmentsResponseData);
     // 				std::vector<json> attachments = attachmentsResponseJ;
     // 			}
     // 		}
     // 	}
     // }
}

MailAPIsManager::~MailAPIsManager()
{
}

void MailAPIsManager::OpenGmail(char* _username, char* _password)
{
     CURL* curl;
     CURLcode res;
    
     curl = curl_easy_init();
     if (curl)
     {
         username = _username;
         password = _password;
         struct MemoryStruct chunk;
    
         chunk.memory = (char*)malloc(1);
         chunk.size = 0;
    
         //initialisation
         curl_global_init(CURL_GLOBAL_ALL);
         curl = curl_easy_init();
    
    
         //login
         curl_easy_setopt(curl, CURLOPT_USERNAME, _username);
         curl_easy_setopt(curl, CURLOPT_PASSWORD, _password);
    
         popsAccount = "pop3s://pop.gmail.com:995/"; //Adding '1' retrieves attachments
    
         curl_easy_setopt(curl, CURLOPT_URL, popsAccount);
         curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
         curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    
         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->WriteMemoryCallback);
         curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    
         curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    
         //some servers needs this validation
         curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
         /* Perform the request, res will get the return code */
         res = curl_easy_perform(curl);
    
         /* Check for errors */
         if (res != CURLE_OK)
         {
             fprintf(stderr, "curl_easy_perform() failed: %s\n",
                     curl_easy_strerror(res));
         }
         else
         {
             /*
             here is where you can work with the data inside the chunk
             */
             printf("%s\n", chunk.memory); //here is the information
             printf("%lu bytes retrieved\n", (long)chunk.size);
         }
    
         /* always cleanup */
         curl_easy_cleanup(curl);
    
         if (chunk.memory)
             free(chunk.memory);
         /* always cleanup */
    
         curl_global_cleanup();
     }
}

size_t MailAPIsManager::WriteMemoryCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

/*
  ==============================================================================

	MailAPIsManager.cpp
	Created: 9 Feb 2023 9:11:12pm
	Author:  Robin

  ==============================================================================
*/

#include "MailAPIsManager.h"

const juce::String MAILBOX_URL =
    "https://graph.microsoft.com/v1.0";
juce::String ACCESS_TOKEN = "";

struct MemoryStruct
{
    char* memory;
    size_t size;
};

MailAPIsManager::MailAPIsManager() : Thread("ArsonPlayer")
{
    addAndMakeVisible(&userBox);
    addAndMakeVisible(&passwordBox);

    addAndMakeVisible(&confirmButton);
    confirmButton.setButtonText("Confirm");
    confirmButton.onClick = [this] { confirmButtonClicked(); };
    confirmButton.setColour(juce::TextButton::buttonColourId, juce::Colours::rebeccapurple);
    confirmButton.setEnabled(true);

    addAndMakeVisible(&urlBox);
    addAndMakeVisible(&resultsBox);

    //startThread();
}

MailAPIsManager::~MailAPIsManager()
{
    stopThread(0);
}

void MailAPIsManager::run()
{
    //TODO: Input as an influence
    //const juce::String result = GetResultText(urlBox.getText());
    const juce::String result = GetResultText(MAILBOX_URL + "/me/messages");

    RetrieveAttachments(result);

    juce::MessageManagerLock mml(this);

    if (mml.lockWasGained())
        resultsBox.loadContent(result);
}

void MailAPIsManager::OpenOutlook(char* _username, char* _password)
{
    //CURL* curl;
    //CURLcode res;
    //
    //curl = curl_easy_init();


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

void MailAPIsManager::OpenGmail(char* _username, char* _password)
{
    //CURL* curl;
    //CURLcode res;

    //curl = curl_easy_init();
    //if (curl)
    //{
    //    username = _username;
    //    password = _password;
    //    struct MemoryStruct chunk;

    //    chunk.memory = (char*)malloc(1);
    //    chunk.size = 0;

    //    //initialisation
    //    curl_global_init(CURL_GLOBAL_ALL);
    //    curl = curl_easy_init();


    //    //login
    //    curl_easy_setopt(curl, CURLOPT_USERNAME, _username);
    //    curl_easy_setopt(curl, CURLOPT_PASSWORD, _password);

    //    popsAccount = "pop3s://pop.gmail.com:995/"; //Adding '1' retrieves attachments

    //    curl_easy_setopt(curl, CURLOPT_URL, popsAccount);
    //    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    //    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    //    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    //    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->WriteMemoryCallback);
    //    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    //    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    //    //some servers needs this validation
    //    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    //    /* Perform the request, res will get the return code */
    //    res = curl_easy_perform(curl);

    //    /* Check for errors */
    //    if (res != CURLE_OK)
    //    {
    //        fprintf(stderr, "curl_easy_perform() failed: %s\n",
    //                curl_easy_strerror(res));
    //    }
    //    else
    //    {
    //        /*
    //        here is where you can work with the data inside the chunk
    //        */
    //        printf("%s\n", chunk.memory); //here is the information
    //        printf("%lu bytes retrieved\n", (long)chunk.size);
    //    }

    //    /* always cleanup */
    //    curl_easy_cleanup(curl);

    //    if (chunk.memory)
    //        free(chunk.memory);
    //    /* always cleanup */

    //    curl_global_cleanup();
    //}
}

void MailAPIsManager::RetrieveAttachments(const juce::String& _jsonString)
{
    // Parse the JSON response
    juce::var _responseJson = juce::JSON::parse(_jsonString);

    // Get the list of emails
    if (juce::Array<juce::var>* _emails = _responseJson.getProperty("stringValue", juce::var()).getArray())
    {
        // Iterate through the emails
        for (auto& _email : *_emails)
        {
            // Get the ID of the email
            juce::String _id = _email.getProperty("id", juce::var()).toString();

            // Get the list of attachments for the email
            juce::String attachmentsUrl = MAILBOX_URL + _id + "/attachments";
        }
    }
}

bool MailAPIsManager::RefreshToken(int& statusCode)
{
    juce::StringPairArray responseHeaders;

    juce::URL _refreshURL = MAILBOX_URL; // + "/login";

    juce::DynamicObject* _obj = new juce::DynamicObject();

    //TODO?: - Read strings from authBox
    // _obj->setProperty("user", "{authBox.user_value}");
    // _obj->setProperty("password", "{authBox.password_value}");

    juce::var _json(_obj);
    juce::String _jsonString = juce::JSON::toString(_json);

    if (!_jsonString.isEmpty())
    {
        _refreshURL = _refreshURL.withPOSTData(_jsonString);
    }

    const juce::String _contentTypeHeaders =
        "Content-Type: application/json";
    const juce::String _idHeaders =
        "username: {authBox.user_value}";
    const juce::String _pwdHeaders =
        "password: {authBox.password_value}";

    if (auto _refreshTokenStream = _refreshURL
        //.withPOSTData("/login HTTP/1.1")
        //.withParameter("user", "{authBox.user_value}") ???
        //.withParameter("password", {authBox.password_value}) ???
        .createInputStream(
            juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            //.withHttpRequestCmd("POST")
            .withConnectionTimeoutMs(10000)
            .withResponseHeaders(&responseHeaders)
            .withStatusCode(&statusCode)
            //.withExtraHeaders("/login")
            .withExtraHeaders(_contentTypeHeaders)
            //.withExtraHeaders(_idHeaders) ???
            //.withExtraHeaders(_pwdHeaders) ???
        ))
    {
        //TODO: Set new token value
        return true;
    }
    return false;
}

juce::String MailAPIsManager::GetResultText(const juce::URL& _url)
{
    juce::StringPairArray responseHeaders;
    juce::String _contentTypeHeaders = "Authorization: Bearer " + ACCESS_TOKEN + ", Content-Type: application/json";

    int statusCode = 0;

    //	// Set the authorization header
    //	struct curl_slist* headers = NULL;
    //	headers = curl_slist_append(headers, "Authorization: Bearer [ACCESS_TOKEN]");

    if (auto stream = _url.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                                             .withConnectionTimeoutMs(10000)
                                             .withResponseHeaders(&responseHeaders)
                                             .withStatusCode(&statusCode)
                                             .withExtraHeaders(_contentTypeHeaders)))
    {
        if (statusCode == 401) // Failed to authenticate
        {
            if (RefreshToken(statusCode))
            {
                if (auto _retryStream = _url.createInputStream(
                    juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                    .withConnectionTimeoutMs(10000)
                    .withResponseHeaders(&responseHeaders)
                    .withStatusCode(&statusCode)
                    .withExtraHeaders(_contentTypeHeaders)))
                {
                    return _retryStream->readEntireStreamAsString();
                }
                else
                {
                    return "Failed to refresh token, status code = " + juce::String(statusCode);
                }
            }
        }

        return (statusCode != 0 ? "Status code: " + juce::String(statusCode) + juce::newLine : juce::String())
            + "Response headers: " + juce::newLine
            + responseHeaders.getDescription() + juce::newLine
            + "----------------------------------------------------" + juce::newLine
            + stream->readEntireStreamAsString();
    }
    else
    {
        if (statusCode != 0)
            return "Failed to connect, status code = " + juce::String(statusCode);

        return "Failed to connect!";
    }
}

void MailAPIsManager::Fetch()
{
    MailAPIsManager* _mailAPI = new MailAPIsManager();
    _mailAPI->OpenGmail("test", "test");
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

void MailAPIsManager::confirmButtonClicked()
{
    //TODO
}

void MailAPIsManager::paint(juce::Graphics& graphics)
{
}

void MailAPIsManager::resized()
{
    int _width = getWidth(), _height = getHeight();

    userBox.setBounds(0, 0, _width / 3, _height / 3);
    passwordBox.setBounds(_width / 3, 0, _width / 3, _height / 3);
    confirmButton.setBounds(_width - _width / 3, 0, _width / 3, _height / 3);

    urlBox.setBounds(0, _height / 3, _width, _height / 3);

    resultsBox.setBounds(0, _height - _height / 3, _width, _height);
}

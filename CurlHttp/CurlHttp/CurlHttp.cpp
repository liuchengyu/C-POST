// CurlHttp.cpp : 定义控制台应用程序的入口点。
//
//  C++  预处理定义 加入 BUILDING_LIBCURL HTTP_ONLY
#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl\curl.h"
#include <iostream>
#include <string>
using namespace std;
#pragma comment(lib,"libcurl.lib")
  
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

int main()
{
  CURL *curl;
  CURLcode res;
  struct MemoryStruct chunk;
  static const char *postthis="Field=1&Field=2&Field=3";
 
  chunk.memory = (char*)malloc(1);  /* will be grown as needed by realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if(curl) {
 
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.dnat.link/api/member/mfcLogin");
 
    /* send all data to this function  */ 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
 
    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */ 
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
 
    /* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
       itself */ 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    else {
      /*
       * Now, our chunk.memory points to a memory block that is chunk.size
       * bytes big and contains the remote file.
       *
       * Do something nice with it!
       */
	  string resdata;
	  resdata.assign(chunk.memory, chunk.size);
	  cout<<resdata<<endl;
    }
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
 
    free(chunk.memory);
 
    /* we're done with libcurl, so clean it up */ 
    curl_global_cleanup();
  }
  return 0;
}
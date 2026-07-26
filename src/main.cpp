#include "app/Application.h"
#include <curl/curl.h>


int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    Application app;

    app.run();

    curl_global_cleanup();   // non raggiunto nel loop infinito attuale, ma corretto per completezza

    return 0;
}

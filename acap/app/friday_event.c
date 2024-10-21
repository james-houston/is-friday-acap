#include <curl.h>
#include <stdio.h>
#include <syslog.h>

#include <axsdk/axevent.h>
#include <glib-object.h>
#include <glib.h>

#include <cJSON.h>

static GMainLoop* loop = NULL;
static char URL[] = "http://192.168.1.197:8080/isfriday";   // URL where the API is located
CURL* curl;                                                 // curl handler accessible to entire file


typedef struct {
    AXEventHandler* event_handler;
    guint event_id;
    gboolean is_friday_state;
} AppData;

static AppData* app_data = NULL;


static void quit_main_loop(__attribute__((unused)) int signal_num) {
    g_main_loop_quit(loop);
}


/**
 * Exit ACAP on SIGINT
 */
static void set_sigterm_and_sigint_handler(void (*handler)(int)) {
    struct sigaction sa = {0};

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}


/**
 * Send axevent updating is_friday_state.
 *
 * param send_data Application data containing e.g. the event declaration id.
 * return TRUE
 */
static void send_event(void) {
    AXEventKeyValueSet* key_value_set = NULL;
    AXEvent* event                    = NULL;
    GError* error                     = NULL;


    key_value_set = ax_event_key_value_set_new();

    // Add the variable elements of the event to the set
    if(!ax_event_key_value_set_add_key_value(key_value_set, "Status", NULL, &app_data->is_friday_state, AX_VALUE_TYPE_BOOL, &error)) {
        syslog(LOG_WARNING, "[send_event] Unable to add key to key/value set: %s", error->message);
    }

    // Create the event
    // Use ax_event_new2 since ax_event_new is deprecated from 3.2
    event = ax_event_new2(key_value_set, NULL);

    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);

    // Send the event
    if(!ax_event_handler_send_event(app_data->event_handler, app_data->event_id, event, &error)){
        syslog(LOG_WARNING, "[send_event] Unable send event: %s", error->message);
    }

    syslog(LOG_INFO, "[send_event] is_friday_state toggled! updated event value to `%d`", app_data->is_friday_state);

    ax_event_free(event);
}


/**
 * brief Callback function called when there is data returned by CURL
 *
 * param buffer − The pointer to the array of elements to be written.
 * param size   − The size in bytes of each element to be written.
 * param nmemb  − The number of elements, each one with a size of size bytes.
 * param stream − The pointer to a FILE object that specifies an output stream.
 */
static size_t parse_curl_response(void* buffer, size_t size, size_t nmemb, void* stream) {
    (void) stream;
    (void) size;

    // parse the JSON data 
    cJSON *json = cJSON_Parse(buffer); 
    cJSON *is_friday = cJSON_GetObjectItemCaseSensitive(json, "friday"); 
    // Verify API returns bool, then check if the value returned by the API matches app_data's state
    // If not, toggle app_data state and send event
    syslog(LOG_INFO, "[parse_curl_response] API returned %d", cJSON_IsTrue(is_friday));
    if(cJSON_IsBool(is_friday) && cJSON_IsTrue(is_friday) != app_data->is_friday_state) {
        app_data-> is_friday_state = !app_data->is_friday_state;
        send_event();
    }

    return nmemb;
}


/**
 * Perform CURL to API
 * 
 * param user_data is unused, format of the function signature required for running with g_timeout_add_seconds
 */
static gboolean check_friday(gpointer user_data) {
    (void)user_data;
    CURLcode rv;
    rv = curl_easy_perform(curl);
    if (rv != CURLE_OK)
        syslog(LOG_INFO, "[check_friday] CURL Failed: Unexpected result, error code: %d", rv);
    return G_SOURCE_CONTINUE;
}


/**
 * Initialize the stateful event representing the current status of friday
 * 
 * param event_handler the event handler to attach the event to
 */
static guint setup_declaration(AXEventHandler* event_handler) {
    AXEventKeyValueSet* key_value_set = NULL;
    guint declaration                 = 0;
    guint token                       = 0;
    GError* error                     = NULL;
    
    // init friday state to FALSE
    gboolean start_value = FALSE;
    app_data->is_friday_state = start_value;

    // Create keys, namespaces and nice names for the event
    key_value_set = ax_event_key_value_set_new();
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "topic0",
                                         "tns1",
                                         "Monitoring",
                                         AX_VALUE_TYPE_STRING,
                                         NULL);
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "topic1",
                                         "tns1",
                                         "FridayStatus",
                                         AX_VALUE_TYPE_STRING,
                                         NULL);
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "Friday",
                                         NULL,
                                         &token,
                                         AX_VALUE_TYPE_INT,
                                         NULL);
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "Status",
                                         NULL,
                                         &start_value,
                                         AX_VALUE_TYPE_BOOL,
                                         NULL);
    ax_event_key_value_set_mark_as_source(key_value_set, "Friday", NULL, NULL);
    ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                                "Friday",
                                                NULL,
                                                "wstype:tt:ReferenceToken",
                                                NULL);
    ax_event_key_value_set_mark_as_data(key_value_set, "Status", NULL, NULL);
    ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                                "Status",
                                                NULL,
                                                "wstype:xs:float",
                                                NULL);

    // Declare event
    if (!ax_event_handler_declare(event_handler,
                                  key_value_set,
                                  FALSE,  // Indicate a property state event
                                  &declaration,
                                  NULL,
                                  NULL,
                                  &error)) {
        syslog(LOG_WARNING, "[setup_declaration] Could not declare: %s", error->message);
        g_error_free(error);
    } else {
        syslog(LOG_INFO, "[setup_declaration] Event declaration complete for: %d", declaration);
        // Set up a timer to be called every 5th second to check the API
        g_timeout_add_seconds(5, check_friday, NULL);
    }

    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);
    return declaration;
}


int main(void) {
    // Start logging
    openlog(NULL, LOG_PID, LOG_USER);

    // Log the curl library version used in the code
    curl_version_info_data* ver = curl_version_info(CURLVERSION_NOW);
    syslog(LOG_INFO,
           "[main] ACAP application curl version: %u.%u.%u\n",
           (ver->version_num >> 16) & 0xff,
           (ver->version_num >> 8) & 0xff,
           ver->version_num & 0xff);

    // This function sets up the program environment that libcurl needs
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    if (curl) {
        syslog(LOG_INFO, "[main] curl easy init successful - handle has been created");

        // Set URL for API
        curl_easy_setopt(curl, CURLOPT_URL, URL);

        // Define callback to get called when there is data to be written to file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_curl_response);

        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);

        // Event handler
        app_data                = calloc(1, sizeof(AppData));
        app_data->event_handler = ax_event_handler_new();
        // Declare Friday event, if successful start polling the API
        app_data->event_id      = setup_declaration(app_data->event_handler);



        // Main loop
        loop = g_main_loop_new(NULL, FALSE);
        set_sigterm_and_sigint_handler(quit_main_loop);
        syslog(LOG_INFO, "[main] Begin g_main_loop ***");
        g_main_loop_run(loop);
       

        curl_easy_cleanup(curl);
    }
      
    ax_event_handler_undeclare(app_data->event_handler, app_data->event_id, NULL);
    ax_event_handler_free(app_data->event_handler);
    // Cleanup of curl global environment
    curl_global_cleanup();
    // Free g_main_loop
    g_main_loop_unref(loop);
    free(app_data);
    return 0;
}
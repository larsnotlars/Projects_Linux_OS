#include "quiz.h"
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <string.h>

extern int parse(char *json, quiz_t *quiz)
{
    struct json_object *parsed_json;
    struct json_object *text;
    struct json_object *number;

    parsed_json = json_tokener_parse(json);

    json_object_object_get_ex(parsed_json, "text", &text);
    json_object_object_get_ex(parsed_json, "number", &number);

    quiz->text = strdup(json_object_get_string(text));
    quiz->number = json_object_get_int(number);
    json_object_put(parsed_json);

    free(json);
    return 0;
}

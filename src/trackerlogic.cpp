#include "trackerlogic.h"

int reader_instance_idx [READER_COUNT] = { };
boolean alert_idx_status [ALERT_TOTAL_COUNT] = { };

boolean door_closed_n_mqtt_published = false;

int count_door_closed = 0;
int count_missing[READER_COUNT] = { };
int count_empty[READER_COUNT] = { };
int count_incorrect[READER_COUNT] = { };
int count_unknown[READER_COUNT] = { };
int count_normal = 0;

std::vector<int> alert_empty_tag_instance_array;
std::vector<int> alert_incorrect_instance_array;
std::vector<int> alert_missing_instance_array;
std::vector<int> alert_decommission_instance_array;
std::vector<int> alert_unknown_nfc_reader_array;

DynamicJsonDocument rack_json(10240);

String mqtt_json_string = "";

void updateAllReadersStatus() {
    alert_empty_tag_instance_array.clear();
    alert_incorrect_instance_array.clear();
    alert_missing_instance_array.clear();
    alert_decommission_instance_array.clear();
    alert_unknown_nfc_reader_array.clear();
    String print_string = "";
    // update reader status configuration //
    print_string += updateReaderStatus(nfc1, 1) + "\t";
    print_string += updateReaderStatus(nfc2, 2) + "\t";
    print_string += updateReaderStatus(nfc3, 3) + "\t";
    print_string += updateReaderStatus(nfc4, 4) + "\t";
    print_string += updateReaderStatus(nfc5, 5) + "\t";
    print_string += updateReaderStatus(nfc6, 6) + "\t";
    print_string += updateReaderStatus(nfc7, 7) + "\t";
    print_string += updateReaderStatus(nfc8, 8) + "\t";
    print_string += updateReaderStatus(nfc9, 9) + "\t";
    print_string += updateReaderStatus(nfc10, 10) + "\t";
    ////////////////////////////////////////
    if (print_string != "") {
        Serial.println(print_string);
    }
}

void alertsAccordingToReaderStatuses() {
    boolean isAlert1 = instanceAlertPublish(alert_empty_tag_instance_array, ALERT_ID_ADD, "Added new asset tag for devices");
    boolean isAlert2 = instanceAlertPublish(alert_incorrect_instance_array, ALERT_ID_INCORRECT, "Unmatched asset tag of devices");
    boolean isAlert3 = instanceAlertPublish(alert_missing_instance_array, ALERT_ID_MISSING, "Missing asset tag of devices");
    boolean isAlert4 = readerAlertPublish(alert_unknown_nfc_reader_array, ALERT_ID_UNKNOWN, "Unkown asset tag at readers of U");
    if (!isAlert1 && !isAlert2 && !isAlert3 && !isAlert4) {
        Serial.println("normal count: " + String(count_normal));
        if (count_normal > ALERT_THRESHOLD) {
            normalStatusPublish(ALERT_ID_NORMAL);
        } else {
            count_normal++;
        }
    } else {
        count_normal = 0;
        // if(mqtt_json_string!=""){
        //     mqtt_json_string += "}";
        //     mqtt_pub(mqtt_json_string);
        // }
    }
}

void readerInstanceIdxInit() {
    for (int i = 0; i < READER_COUNT; i++) {
        reader_instance_idx[i] = -1; // -1 represents no instance at that reader
    }
}

void falsifyAlertIsTriggered() {
    for (int i = 0; i < ALERT_TOTAL_COUNT; i++) {
        alert_idx_status[i] = false;
    }
}

boolean doorIsOpened() {
    for (int i = 0; i < READER_COUNT; ++i) {
        if (NFCs_content[i].indexOf(TAG_NOT_PRESENT) == -1) { // not exist
            return false;
        }
    }
    count_normal = 0;
    return true;
}

void fetchRackStatusJson() {
    String serial = http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, HTTP_USERNAME, HTTP_PASSWORD);
    // String serial = "{\"code\": \"1\",\"data\": [{\"description\": \"test\",\"mid\": \"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"upos\": 40.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"2950III\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21115,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 28436,\"slots\": null,\"vname\": \"Dell\",\"at\": \"at123456\",\"reserved\": false,\"name\": \"NFC server demo\",\"_id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"position\": \"40.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1},{\"description\": null,\"mid\": \"9d2b566a-d52d-11ea-bf0d-b32c7d6d75cf\",\"upos\": 36.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"R740xd (12D 3.5)\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21116,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operatjoinional\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 54715,\"slots\": null,\"vname\": \"Dell\",\"at\": null,\"reserved\": false,\"name\": \"NFC Server Demo -1\",\"_id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"position\": \"38.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1}],\"message\": \"Success\"}";
    deserializeJson(rack_json, serial);
}

void updateInstanceReaderMapping() {
    int instance_count = 0;
    for (int i = 0; i<100; i++){
        String str = rack_json["data"][i];
        if(str!="null"){
            instance_count++;
        }else{
            break;
        }
    }
    // Serial.println("Instance number: " + String(instance_count));
    for(int i = 0; i<instance_count; i++) {
        int u_position = rack_json["data"][i]["upos"]; // getting u position of an instance/server
        int reader_num = (u_position - STARTING_U) / U_INTERVAL;  // calculate no. of reader which should detect that instance
        int status_id = rack_json["data"][i]["status"];
        if ( status_id == OPERATIONAL || status_id == RESERVED_AVAILABLE || status_id == RESERVED_MOVE || status_id == PLAN_DEMMISSION ) {
            reader_instance_idx [reader_num] = i; // eg.[1, 2, -1] 1&2: index of instance array, -1: supposed no nfc/instance should be detected
        }
    }
    String print_string = "reader - instance_index : ";
    for(int i=0; i < READER_COUNT; i++) {
        print_string += reader_instance_idx[i];
        print_string += " ";
    }
    Serial.println(print_string);
}

void postServerAssetTag(String id, String model_id, String name, String description, String nfc_uid) {
    DynamicJsonDocument attribute_json(1024);
    attribute_json["id"] = "0aeddd86-ce3c-11dd-8da6-001d091dd9dd";
    attribute_json["name"] = "asset_tag";
    attribute_json["value"] = nfc_uid;
    DynamicJsonDocument post_json(1024);
    post_json["id"] = id;
    post_json["model_id"] = model_id;
    post_json["name"] = name;
    post_json["description"] = description;
    post_json["attributes"][0] = attribute_json;
    String post_serial = "";
    serializeJson(post_json, post_serial);
    Serial.println("Post content: " + post_serial);
    http_post("/devices", post_serial, true, HTTP_USERNAME, HTTP_PASSWORD);
}

String updateReaderStatus(NfcAdapter &nfc, int reader_num) {
    int reader_idx = reader_num - 1;
    int instance_idx = reader_instance_idx[reader_idx];
    String nfc_content = NFCs_content[reader_idx];
    String return_string;
    if (instance_idx != -1) { // supposed to have nfc/server being detected
        if (nfc_content == TAG_NOT_PRESENT) { // !nfc.tagPresent(...)
            if (count_missing[reader_idx] >= ALERT_THRESHOLD) {
                if (rack_json["data"][instance_idx]["status"] == PLAN_DEMMISSION) {
                    return_string =  String(reader_num) + ": decommission(4)";
                    alert_decommission_instance_array.push_back(instance_idx);
                } else {
                    return_string =  String(reader_num) + ": missing(3)";
                    alert_missing_instance_array.push_back(instance_idx);
                }
            } else {
                count_missing[reader_idx]++;
            }
        } else { // nfc sticker read
            count_missing[reader_idx] = 0;
            String instance_id = rack_json["data"][instance_idx]["id"];
            String model_id = rack_json["data"][instance_idx]["mid"];
            String name = rack_json["data"][instance_idx]["name"];
            String description = rack_json["data"][instance_idx]["description"];
            if (nfc_content == TAG_NULL_CONTENT) {
                count_incorrect[reader_idx] = 0;
                if (count_empty[reader_idx] >= ALERT_THRESHOLD) {
                    return_string =  String(reader_num) + ": added(1)";
                    alert_empty_tag_instance_array.push_back(instance_idx);
                    nfc_write(nfc, instance_id);
                    postServerAssetTag(instance_id, model_id, name, description, NFCs_uid[reader_idx]);
                } else {
                    count_empty[reader_idx]++;
                    return_string =  String(reader_num) + ": detecting...";
                }
            } else if (nfc_content.indexOf(instance_id) == -1) { // instance/server id does not exist(match) in nfc sticker
                count_empty[reader_idx] = 0;
                if (count_incorrect[reader_idx] >= ALERT_THRESHOLD) {
                    return_string =  String(reader_num) + ": unmatched(2)";
                    alert_incorrect_instance_array.push_back(instance_idx);          
                } else {
                    count_incorrect[reader_idx]++;
                    return_string =  String(reader_num) + ": reading(unmatched)...";
                }
            } else {
                count_empty[reader_idx] = 0;
                count_incorrect[reader_idx] = 0;
                return_string =  String(reader_num) + ": matched(5)";
            }
        }
    } else { // not supposed to have nfc/server being detected
        if (nfc_content != TAG_NOT_PRESENT) {
            if (count_unknown[reader_idx] >= ALERT_THRESHOLD) {
                return_string =  String(reader_num) + ": unknown(6)";
                alert_unknown_nfc_reader_array.push_back(reader_idx);
            } else {
                count_unknown[reader_idx]++;
                return_string =  String(reader_num) + ": reading(unknown)...";
            }
        } else {
            count_unknown[reader_idx] = 0;
            return_string =  String(reader_num) + ": no sticker(correct)";
        }
    }
    return return_string;
}

boolean instanceAlertPublish(std::vector<int> &array, int alert_id, String mqtt_content_note) {
    if (array.size() > 0) {
        int arr_size = array.size();
        int arr[arr_size];
        std::copy(array.begin(), array.end(), arr);
        DynamicJsonDocument alert_json(1024);
        // alert_json["id"] = alert_id;
        String content = mqtt_content_note + ": ";
        for (int i = 0; i < arr_size; i++) {
            String name = rack_json["data"][arr[i]]["name"];
            content += "[" + name + "]";
            if (i < arr_size - 1) { content += ", "; }
        }
        // alert_json["content"] = content;
        alert_json[String(alert_id)] = content;
        String alert_string = "";
        serializeJson(alert_json, alert_string);
        if (alert_idx_status[alert_id-1] == false) { // only count once after door closed
            // mqtt_pub(alert_string);
            if(mqtt_json_string == ""){
                mqtt_json_string += "{";
            } else {
                mqtt_json_string += ", ";
            }
            mqtt_json_string += alert_string.substring(1, alert_string.length() - 1);
            alert_idx_status[alert_id-1] = true;
        }
        return true;
    }
    return false;
}

boolean readerAlertPublish(std::vector<int> &array, int alert_id, String mqtt_content_note) {
    if (array.size() > 0) {
        int arr_size = array.size();
        DynamicJsonDocument alert_json(1024);
        // alert_json["id"] = alert_id;
        String content = mqtt_content_note + ": ";
        for (int i = 0; i < arr_size; i++) {
            int reader_id = array[i]+1;
            content += (array[i]*2 + STARTING_U);
            if (i < arr_size - 1) { content += ", "; }
        }
        // alert_json["content"] = content;
        alert_json[String(alert_id)] = content;
        String alert_string = "";
        serializeJson(alert_json, alert_string);
        if (alert_idx_status[alert_id-1] == false) {
            // mqtt_pub(alert_string);
            if(mqtt_json_string == ""){
                mqtt_json_string += "{";
            } else {
                mqtt_json_string += ", ";
            }
            mqtt_json_string += alert_string.substring(1, alert_string.length() - 1);
            alert_idx_status[alert_id-1] = true;
        }
        return true;
    }
    return false;
}

void normalStatusPublish(int alert_id) {
    DynamicJsonDocument alert_json(1024);
    // alert_json["id"] = alert_id;
    // alert_json["content"] = "";
    alert_json[String(alert_id)] = "Normal";
    String alert_string = "";
    serializeJson(alert_json, alert_string);
    if (alert_idx_status[alert_id-1] == false) {
        mqtt_pub(alert_string);
        alert_idx_status[alert_id-1] = true;
    }
}
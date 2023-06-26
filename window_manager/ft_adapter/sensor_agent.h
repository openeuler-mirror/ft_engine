/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SENSOR_AGENT_H
#define SENSOR_AGENT_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef SENSOR_USER_DATA_SIZE
#define SENSOR_USER_DATA_SIZE 104
#endif

#ifndef NAME_MAX_LEN
#define NAME_MAX_LEN 128
#endif

typedef enum SensorTypeId {
    SENSOR_TYPE_ID_GRAVITY = 257,
} SensorTypeId;

typedef enum SensorMode {
    SENSOR_ON_CHANGE = 2,
} SensorMode;

typedef struct SensorEvent {
    int32_t sensorTypeId;
    int32_t version;
    int64_t timestamp;
    uint32_t option;
    int32_t mode;
    uint8_t *data = nullptr;
    uint32_t dataLen;
} SensorEvent;

typedef struct GravityData {
    float x;
    float y;
    float z;
} GravityData;

typedef void (*RecordSensorCallback)(SensorEvent *event);

typedef struct UserData {
    char userData[SENSOR_USER_DATA_SIZE];
} UserData;

typedef struct SensorUser {
    char name[NAME_MAX_LEN];
    RecordSensorCallback callback;
    UserData *userData = nullptr;
} SensorUser;

static int32_t SubscribeSensor(int32_t sensorTypeId, const SensorUser *user)
{
    return 0;
}
static int32_t UnsubscribeSensor(int32_t sensorTypeId, const SensorUser *user)
{
    return 0;
}
static int32_t SetBatch(int32_t sensorTypeId, const SensorUser *user, int64_t samplingInterval, int64_t reportInterval)
{
    return 0;
}
static int32_t ActivateSensor(int32_t sensorTypeId, const SensorUser *user)
{
    return 0;
}
static int32_t DeactivateSensor(int32_t sensorTypeId, const SensorUser *user)
{
    return 0;
}
static int32_t SetMode(int32_t sensorTypeId, const SensorUser *user, int32_t mode)
{
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* SENSOR_AGENT_H */
/** @} */
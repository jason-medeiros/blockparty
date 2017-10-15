/*
 * BP-DBSensorSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBSENSOR_SCHEMA
#define __BP_DBSENSOR_SCHEMA


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Sensor Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#define BP_DB_SQLITE_CREATE_TABLE_SENSORS\
	"\n CREATE TABLE IF NOT EXISTS sensors "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    sensor_identifier_sha1           TEXT,     "\
	"\n    group_id                         INTEGER,  "\
	"\n    name                             TEXT,     "\
	"\n    description                      TEXT,     "\
	"\n    created_ts                       INTEGER,  "\
	"\n    last_updated_ts                  INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_CONFIG_REGISTRY\
	"\n CREATE TABLE IF NOT EXISTS sensor_config_registry "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    sensor_id                        INTEGER,  "\
	"\n    token_hash                       TEXT,     "\
	"\n    data_hash                        TEXT,     "\
	"\n    token                            TEXT,     "\
	"\n    data                             TEXT,     "\
	"\n    created_ts                       INTEGER,  "\
	"\n    update_ts                        INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS sensor_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER,  "\
	"\n    last_updated_ts                  INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_HEALTH_UPDATES\
	"\n CREATE TABLE IF NOT EXISTS sensor_health_updates "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    sensor_identifier                TEXT,     "\
	"\n    update_type                      TEXT,     "\
	"\n    update_sha1                      TEXT,     "\
	"\n    update_value                     TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_MESSAGES\
	"\n CREATE TABLE IF NOT EXISTS sensor_messages "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    sensor_id                        INTEGER,  "\
	"\n    request_group_id                 INTEGER,  "\
	"\n    type                             INTEGER,  "\
	"\n    code                             INTEGER,  "\
	"\n    message                          TEXT,     "\
	"\n    ts                               INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_REQUESTS\
	"\n CREATE TABLE IF NOT EXISTS sensor_requests "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    sensor_id                        INTEGER,  "\
	"\n    group_id                         INTEGER,  "\
	"\n    param_name                       TEXT,     "\
	"\n    param_value                      TEXT,     "\
	"\n    status                           INTEGER,  "\
	"\n    ts                               INTEGER   "\
	"\n );"\


#define BP_DB_SQLITE_CREATE_TABLE_SENSOR_SUBSYSTEM_CONFIG\
	"\n CREATE TABLE IF NOT EXISTS sensor_subsystem_config "\
	"\n ("\
	"\n    id                                  INTEGER,  "\
	"\n    sensor_id                           INTEGER,  "\
	"\n    subsystem_type                      INTEGER,  "\
	"\n    max_worker_threads                  INTEGER,  "\
	"\n    worker_thread_count_at_last_polling INTEGER,  "\
	"\n    created                             INTEGER,  "\
	"\n    last_polled                         INTEGER,  "\
	"\n    enabled                             INTEGER   "\
	"\n );"\


#endif /* __BP_DBSENSOR_SCHEMA */

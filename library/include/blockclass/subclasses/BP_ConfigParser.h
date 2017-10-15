/*
 * BP_ConfigParser.h
 *
 *  Created on: Feb 10, 2011
 *      Author: root
 */


#ifndef BP_CONFIGPARSER_H_
#define BP_CONFIGPARSER_H_

//! configuration item typedef
typedef struct _BP_CONFIG_ITEM {

	char *token_used;
	char *config_item_data;

} BP_CONFIG_ITEM, *P_BP_CONFIG_ITEM;


//! Parses out configurations based on a token input list.  If a token isn't found, it's array position carries a null ptr.
class BP_ConfigParser : public BP_BaseError
{

public:

	// connection object used for queries (for retrieving configs from db)
	Block * mysqli;

	// config items parsed from file
	P_BP_CONFIG_ITEM config_items;
	size_t config_items_count;

	// class constructor
	BP_ConfigParser();

	// class deconstructor
	~BP_ConfigParser();

	// tokens to use for parsing config (must be set before any tokenizing will occur)
	char ** tokens;
	size_t token_count;

	// Parses a configuration from a file
	char ** parsed_config_items;
	size_t parsed_config_item_count;

	// parses a configuration from a file
	bool ParseConfigFromFile(char *file);

	// display config items
	bool DisplayConfigs();

	// retrieves a config string by key
	char * GetValByKey(char *key );

};


#endif /* BP_CONFIGPARSER_H_ */

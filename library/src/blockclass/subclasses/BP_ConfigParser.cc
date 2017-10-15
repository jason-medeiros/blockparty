/*
 * BP_ConfigParser.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: root
 */

#include "../../../include/BP-Main.h"

//! Class constructor.
BP_ConfigParser::BP_ConfigParser()
{

	this->config_items = NULL;
	this->config_items_count = 0;
	this->tokens = NULL;
	this->token_count = 0;

}

BP_ConfigParser::~BP_ConfigParser()
{

	size_t n = 0;
	for(; n < this->config_items_count; n++)
	{

		// destroy data if necessary
		if(this->config_items[n].config_item_data)
		{
			bpfree(this->config_items[n].config_item_data);
			this->config_items[n].config_item_data = NULL;
		}

		// destroy data if necessary
		if(this->config_items[n].token_used)
		{
			bpfree(this->config_items[n].token_used);
			this->config_items[n].token_used = NULL;
		}

	}

	// destroy containing structure
	if(this->config_items)
	{
		bpfree(this->config_items);
		this->config_items = NULL;
	}

	// set all iters to null
	this->config_items = NULL;
	this->config_items_count = 0;
	this->tokens = NULL;
	this->token_count = 0;



}

//! parses a config from a file
bool BP_ConfigParser::ParseConfigFromFile(char *file)
{

	// create block and get contents
	Block * file_data = new Block();

	// check for file read data
	if(file_data->FileGetContents(file) != ERR_SUCCESS)
	{
		printf("\n [+] Cannot read config contents.");
		return false;
	}

	if(file_data->got_contents_size == 0)
		return false;

	// create string util object from data and explode along newline
	BP_StringUtils * str_utils = new BP_StringUtils(file_data->got_contents);
	str_utils->Explode("[\n]");


	// array of config items and the counter indicating the stack size
	P_BP_CONFIG_ITEM config_items = NULL;
	size_t config_item_counter    = 0;

	// now walk exploded strings and match tokens
	size_t i = 0;
	for(;
		i < str_utils->exploded_count;
		i++
		)
	{

		// now walk match tokens
		size_t x = 0;
		for(;
			x < this->token_count;
			x++
			)
		{

			// check to make sure the string is long enough for the token
			size_t token_len = bpstrlen(this->tokens[x]);
			size_t line_len  = bpstrlen(str_utils->exploded[i]);


			if(line_len < token_len)
				continue;

			// now check to match token in string; any line which is not a token is omitted.
			if(
					memcmp(str_utils->exploded[i], this->tokens[x], token_len) == 0
				)
			{

				// calculate the difference in the line
				size_t val_len = line_len - token_len;

				// cant have a value length of 0
				if(val_len == 0)
					continue;

				// check string length to make sure we have an item (len == 1 means its only a newline)
				if( strnlen(&str_utils->exploded[i][token_len-1], 10) == 1)
				{
					continue;
				}

				// add config item
				config_item_counter++;
				config_items = (P_BP_CONFIG_ITEM) bprealloc(config_items, sizeof(BP_CONFIG_ITEM) * config_item_counter);
				config_items[config_item_counter-1].config_item_data = bpstrdup((char *) &str_utils->exploded[i][token_len]);
				config_items[config_item_counter-1].token_used       = bpstrndup((char *) this->tokens[x], token_len);

			}

		}

	}

	// set config item information before return
	this->config_items       = config_items;
	this->config_items_count = config_item_counter;

	// delete the string utils
	if(str_utils)
		delete str_utils;

	// delete the file data
	if(file_data)
		delete file_data;

	// return indicating success
	return true;

}


// display config items
bool BP_ConfigParser::DisplayConfigs()
{

	size_t x = 0;
	for(; x < this->config_items_count; x++)
	{
		printf("\n [+] Config item: %s", this->config_items[x]);
		printf("\n\t key: %s", this->config_items[x].token_used);
		printf("\n\t val: %s", this->config_items[x].config_item_data);
		printf("\n");
	}

	// return indicating success
	return true;

}

// retrieves a value from the config array by key
char * BP_ConfigParser::GetValByKey(char *key)
{

	size_t i = 0;
	for(;
		i < this->config_items_count;
		i++
		)
	{

		// check for the key, and return the ata in the case we've found it
		if(memcmp(this->config_items[i].token_used, key, bpstrlen(key)) == 0)
			return this->config_items[i].config_item_data;

	}

	return NULL;
}



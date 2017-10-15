/*
 * BP_StringFormattingSession.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: root
 */


#include "../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Class Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


BP_StringFormattingSession::BP_StringFormattingSession(char *session_name)
{

	// create new sfs
	this->sfs = BP_CreateStringFormattingSession(session_name);

	// %%% Nullify Indexes %%%%%%%%%%%%%%%%%%%%%%%%%

	// index for the current row group
	this->row_group_idx = NULL;

	// index for the current row within the row group
	this->row_idx = NULL;

	// index for the column group within the row index
	this->column_group_idx = NULL;

	// index for the column within the column group
	this->column_idx = NULL;

	// string entry index swithin the column
	this->string_entry_idx = NULL;


	// %%% Nullify Entry Ptrs %%%%%%%%%%%%%%%%%%%%%

	// the row group associated with the row group index
	this->row_group = NULL;

	// the row associated with the row index
	this->row = NULL;

	// the column group associated with the column group index
	this->column_group = NULL;

	// the column associated with the column index
	this->column = NULL;

	// the string entry associated with the string entry index
	this->string_entry = NULL;


	// %%% Nullify Search Results %%%%%%%%%%%%%%%%

	// set ptr to null
	this->sfs_search_results = NULL;



	// %%% Initialize Display Settings %%%%%%%%%%%

	// set all display settings to true by default (displays
	// full sfs structure)
	BP_SFS_DISPLAY_ALL_TRUE_STACK(this->display_settings);


}


// class deconstructor
BP_StringFormattingSession::~BP_StringFormattingSession()
{

	// destroy string formatting session if one exists
	if(this->sfs)
		BP_DestroyStringFormattingSession(this->sfs);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Routines to Change Indexes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// change the row group and update all entries
BP_ERROR_T BP_StringFormattingSession::changeRowGroupIdx(char *row_group_idx, BP_BOOL add_if_doesnt_exist)
{

	// ensure we have a row group idx
	if(!row_group_idx)
	{
		this->AddErr("Attempted to change/add row group with null row group index.");
		return ERR_FAILURE;
	}

	if(!this->sfs)
	{
		this->AddErr("Attempting to change row group index, but no sfs is set.");
		return ERR_FAILURE;
	}

	// since we're changing practically everything, just go ahead and reset
	// right off the bat.
	this->reset();

	// set the row group idx here
	this->row_group_idx = (char *) tq_strdup(this->sfs->tq, row_group_idx, BPLN);


	// printf("\n Here?!: %s", this->row_group_idx);

	// this->displaySFSClassData();

	// run the update routine here
	if(this->update() != ERR_SUCCESS)
	{

		// if we have no search results, and we're set to add rows, go ahead
		// and add it now
		if(this->sfs_search_results)
		if(!this->sfs_search_results->result_n && (add_if_doesnt_exist == BP_TRUE))
		{

			// add the row group to the session if possible
			if(BP_AddRowGroupToSFSession(this->sfs, this->row_group_idx) != ERR_SUCCESS)
			{
				this->AddErr("In changeRowGroupIdx, addition of new row is attempted, but fails.");
				this->reset();
				return ERR_FAILURE;
			}
			else
			{

				// call update again after we added the row (freshens the pointers)
				if(this->update() == ERR_FAILURE)
				{
					this->AddErr("After adding new row group, even though the addition row considers the addition successful, we are still failing to update somehow.");
					this->reset();
					return ERR_FAILURE;
				}
				else
				{

					// return success if our update went as planned
					return ERR_SUCCESS;

				}

			}

		}

		this->reset();
		return ERR_FAILURE;
	}

	// printf("\n Here After?!: %s", this->row_group_idx);

	// return indicating success
	return ERR_SUCCESS;

}

// change the row index and update all entries
BP_ERROR_T BP_StringFormattingSession::changeRowIdx(char *row_idx, BP_BOOL add_if_doesnt_exist)
{

	// ensure we have a row group idx
	if(!row_idx)
		return ERR_FAILURE;

	// ensure we have a sfs to search through
	if(!this->sfs)
	{
		this->AddErr("Attempting to change row group index, but no sfs is set.");
		return ERR_FAILURE;
	}

	// ensure we have a row group
	if(!this->row_group || !this->row_group_idx)
	{
		this->AddErr("Attempting to change row, but no row group index has been set.");
		return ERR_FAILURE;
	}

	// reset danglers
	this->reset(BP_FALSE);

	// set the row group idx here
	this->row_idx = (char *) tq_strdup(this->sfs->tq, row_idx, BPLN);

	// run the update routine here
	if(this->update() != ERR_SUCCESS)
	{

		// if we have no search results, and we're set to add rows, go ahead
		// and add it now
		if(this->sfs_search_results)
		if(!this->sfs_search_results->result_n && (add_if_doesnt_exist == BP_TRUE))
		{

			// attempt to add the row to the row group session
			if(BP_AddRowToRowGroupSFSession(this->sfs, this->row_group_idx, this->row_idx) != ERR_SUCCESS)
			{
				this->AddErr("In changeRowIdx, addition of new row is attempted, but fails.");
				this->reset();
				return ERR_FAILURE;
			}
			else
			{

				// call update again after we added the row (freshens the pointers)
				if(this->update() == ERR_FAILURE)
				{
					this->AddErr("After adding new row group, even though the addition row considers the addition successful, we are still failing to update somehow.");
					this->reset();
					return ERR_FAILURE;
				}
				else
				{

					// return success if our update went as planned
					return ERR_SUCCESS;

				}

			}

		}

		this->reset();
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// update the column group index and update all entries
BP_ERROR_T BP_StringFormattingSession::changeColGroupIdx(char *col_group_idx, BP_BOOL add_if_doesnt_exist)
{

	// ensure we have a row group idx
	if(!col_group_idx)
		return ERR_FAILURE;

	// ensure we have a sfs to search through
	if(!this->sfs)
	{
		this->AddErr("Attempting to change row group index, but no sfs is set.");
		return ERR_FAILURE;
	}

	// ensure we have a row group
	if(!this->row_group || !this->row_group_idx || !this->row || !this->row_idx)
	{
		this->AddErr("Attempting to change column, but no row_group/row has been set.");
		return ERR_FAILURE;
	}

	// reset indexes/data, omitting to destroy row group and row
	this->reset(BP_FALSE, BP_FALSE);

	// set the row group idx here
	this->column_group_idx = (char *) tq_strdup(this->sfs->tq, col_group_idx, BPLN);




	// run the update routine here
	if(this->update() != ERR_SUCCESS)
	{

		// if we have no search results, and we're set to add rows, go ahead
		// and add it now
		if(this->sfs_search_results)
		if(!this->sfs_search_results->result_n && (add_if_doesnt_exist == BP_TRUE))
		{

			// attempt to add the row to the row group session
			if(BP_AddColumnGroupToRowSFSession(this->sfs, this->row_group_idx, this->row_idx, this->column_group_idx) != ERR_SUCCESS)
			{
				this->AddErr("In changeRowIdx, addition of new row is attempted, but fails.");
				this->reset();
				return ERR_FAILURE;
			}
			else
			{

				// call update again after we added the row (freshens the pointers)
				if(this->update() == ERR_FAILURE)
				{
					this->AddErr("After adding new row group, even though the addition row considers the addition successful, we are still failing to update somehow.");
					this->reset();
					return ERR_FAILURE;
				}
				else
				{

					// return success if our update went as planned
					return ERR_SUCCESS;

				}

			}

		}

		this->reset();
		return ERR_FAILURE;
	}

	return ERR_SUCCESS;
}

// update the column index and update all entries
BP_ERROR_T BP_StringFormattingSession::changeColIdx(char *col_idx, BP_BOOL add_if_doesnt_exist)
{
	// ensure we have a column index
	if(!col_idx)
		return ERR_FAILURE;

	// ensure we have a sfs to search through
	if(!this->sfs)
	{
		this->AddErr("Attempting to change column index, but no sfs is set.");
		return ERR_FAILURE;
	}

	// ensure we have a row group
	if(!this->row_group || !this->row_group_idx || !this->row || !this->row_idx || !this->column_group || !this->column_group_idx)
	{
		this->AddErr("Attempting to change column, but no row_group/row/collumn group has been set.");
		return ERR_FAILURE;
	}

	// reset indexes/data, omitting to destroy row group and row
	this->reset(BP_FALSE, BP_FALSE, BP_FALSE);

	// set the collumn index here
	this->column_idx = (char *) tq_strdup(this->sfs->tq, col_idx, BPLN);

	// run the update routine here
	if(this->update() != ERR_SUCCESS)
	{

		// if we have no search results, and we're set to add rows, go ahead
		// and add it now
		if(this->sfs_search_results)
		if(!this->sfs_search_results->result_n && (add_if_doesnt_exist == BP_TRUE))
		{

			// attempt to add the row to the row group session
			if(BP_AddColumnToColumnGroupSFSession(this->sfs, this->row_group_idx, this->row_idx, this->column_group_idx, this->column_idx) != ERR_SUCCESS)
			{
				this->AddErr("In changeColIdx, addition of new row is attempted, but fails.");
				this->reset();
				return ERR_FAILURE;
			}
			else
			{

				// call update again after we added the row (freshens the pointers)
				if(this->update() == ERR_FAILURE)
				{
					this->AddErr("After adding new row group, even though the addition row considers the addition successful, we are still failing to update somehow.");
					this->reset();
					return ERR_FAILURE;
				}
				else
				{

					// return success if our update went as planned
					return ERR_SUCCESS;

				}

			}

		}

		this->reset();
		return ERR_FAILURE;
	}


	return ERR_SUCCESS;
}

// update the string entry index and update all entries
BP_ERROR_T BP_StringFormattingSession::changeStringEntryIdx
(
	char *                     str_entry_idx,
	BP_BOOL                    add_if_doesnt_exist,
	char *                     string_data,
	size_t                     length,
	size_t                     display_width,
	P_BP_COLOR_CODE_SET_T      initial_colors,
	BP_BOOL                    is_newline_delimited,
	size_t                     pad_lines_below,
	size_t                     pad_lines_above,
	P_BP_STRING_ROW_BORDER_SET border_set
)
{


	// ensure we have a string entry index
	if(!str_entry_idx)
		return ERR_FAILURE;

	// ensure we have a sfs to search through
	if(!this->sfs)
	{
		this->AddErr("Attempting to change column index, but no sfs is set.");
		return ERR_FAILURE;
	}

	// insure all previous indexes are set before moving indexes
	if
	(
		!this->row_group     ||
		!this->row_group_idx ||
		!this->row           ||
		!this->row_idx       ||
		!this->column_group  ||
		!this->column_group_idx
	)
	{
		this->AddErr("Attempting to change string index, but no row/col indexes are set.");
		return ERR_FAILURE;
	}

	// reset indexes/data, omitting to destroy row group and row
	this->reset(BP_FALSE, BP_FALSE, BP_FALSE, BP_FALSE);

	// set the collumn index here
	this->string_entry_idx = (char *) tq_strdup(this->sfs->tq, str_entry_idx, BPLN);

	// run the update routine here
	if(this->update() != ERR_SUCCESS)
	{

		// if we have no search results, and we're set to add rows, go ahead
		// and add it now
		if(this->sfs_search_results)
		if(!this->sfs_search_results->result_n && (add_if_doesnt_exist == BP_TRUE))
		{

			// attempt to add the row to the row group session
			if(string_data && length)
			{

				// Since we've gotten to this point, we are sure that the entry doesn't already
				// exist.  So we can go ahead and create the new entry here.
				P_BP_SFS_STRING_ENTRY new_entry = BP_GenerateStringEntryFromCharString
				(
					this->sfs,
					str_entry_idx,
					string_data,
					length,
					display_width,
					initial_colors,
					pad_lines_below,
					pad_lines_above,
					border_set
				);

				// set newline delimited here
				if(is_newline_delimited == BP_TRUE)
					new_entry->auto_calculate_max_width_by_newlines = BP_TRUE;

				// ensure we have a new entry
				if(!new_entry)
					return ERR_FAILURE;

				// add the new string entry
				if(BP_AddStringEntryToColumnSFSession(this->sfs, this->row_group_idx, this->row_idx, this->column_group_idx, this->column_idx, new_entry) != ERR_SUCCESS)
				{
					this->AddErr("In changeStringEntryIdx, addition of new row is attempted, but fails.");
					this->reset();
					return ERR_FAILURE;
				}
				else
				{

					// call update again after we added the row (freshens the pointers)
					if(this->update() == ERR_FAILURE)
					{
						this->AddErr("After adding new row group, even though the addition row considers the addition successful, we are still failing to update somehow.");
						this->reset();
						return ERR_FAILURE;
					}
					else
					{

						// return success if our update went as planned
						return ERR_SUCCESS;

					}

				}

			}

		}

		this->reset();
		return ERR_FAILURE;

	}


	return ERR_SUCCESS;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add routines are just convinence / logical wrappers for the change routines.  They
// just simply enable the add_if_doesnt_exist parameters.  This is mostly done for ease
// of development/increased code readability.

// add a row group
BP_ERROR_T BP_StringFormattingSession::addRowGroup(char *row_group_idx)
{

	// set random index here
	char random_idx[40];
	BP_ZERO_STACK(random_idx);

	// ensure we have a row group index
	if(!row_group_idx)
	{
		BP_GenRandomUUID_StoreInBuff(random_idx);
		row_group_idx = random_idx;
	}

	// attempt to add the row group
	return this->changeRowGroupIdx(row_group_idx, BP_TRUE);

}

// add a row
BP_ERROR_T BP_StringFormattingSession::addRow(char *row_idx)
{

	// set random index here
	char random_idx[40];
	BP_ZERO_STACK(random_idx);

	// ensure we have a row index
	if(!row_idx)
	{
		BP_GenRandomUUID_StoreInBuff(random_idx);
		row_idx = random_idx;
	}

	// attempt to add the row
	return this->changeRowIdx(row_idx, BP_TRUE);

}

// add a column group
BP_ERROR_T BP_StringFormattingSession::addColumnGroup(char *col_group_idx)
{

	// set random index here
	char random_idx[40];
	BP_ZERO_STACK(random_idx);

	// ensure we have a column group index to work with
	if(!col_group_idx)
	{
		BP_GenRandomUUID_StoreInBuff(random_idx);
		col_group_idx = random_idx;
	}

	// attempt to change the group idx
	return this->changeColGroupIdx(col_group_idx, BP_TRUE);

}

// add a column
BP_ERROR_T BP_StringFormattingSession::addColumn(char *column_idx)
{

	// set random index here
	char random_idx[40];
	BP_ZERO_STACK(random_idx);


	// ensure we have a column index
	if(!column_idx)
	{
		BP_GenRandomUUID_StoreInBuff(random_idx);
		column_idx = random_idx;
	}

	// attempt to add the column
	return this->changeColIdx(column_idx, BP_TRUE);

}

// add a string entry
BP_ERROR_T BP_StringFormattingSession::addStringEntry
(
	char *string_entry_idx,
	char *string_data,
	size_t length,
	size_t display_width,
	P_BP_COLOR_CODE_SET_T initial_colors,
	BP_BOOL is_newline_delimited,
	size_t pad_lines_below,
	size_t pad_lines_above,
	P_BP_STRING_ROW_BORDER_SET border_set
)
{

	// ensure the parameters were passed in
	if(!string_entry_idx || !string_data || !length)
		return ERR_FAILURE;

	// attempt to add entry
	if
	(
		this->changeStringEntryIdx
		(
			string_entry_idx,
			BP_TRUE,
			string_data,
			length,
			display_width,
			initial_colors,
			is_newline_delimited,
			pad_lines_below,
			pad_lines_above,
			border_set
		) != ERR_SUCCESS
	)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Update Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// moves to a specified placement within the sfs session.
BP_ERROR_T BP_StringFormattingSession::move
(
	char * move_row_group_idx,
	char * move_row_idx,
	char * move_col_group_idx,
	char * move_col_idx,
	char * move_str_entry_idx
)
{

	// ensure we have a string formatting session
	if(!this->sfs)
		return ERR_FAILURE;

	// run cascading checks
	if(!move_row_group_idx)
		return ERR_FAILURE;
	if(move_str_entry_idx && !move_col_idx)
		return ERR_FAILURE;
	if(move_col_idx && !move_col_group_idx)
		return ERR_FAILURE;
	if(move_col_group_idx && !move_row_idx)
		return ERR_FAILURE;
	if(move_row_idx && !move_row_group_idx)
		return ERR_FAILURE;

	// preserve old pointers
	char * old_row_group_idx = tq_strdup(this->sfs->tq, this->row_group_idx, BPLN);
	char * old_row_idx       = tq_strdup(this->sfs->tq, this->row_idx, BPLN);
	char * old_col_group_idx = tq_strdup(this->sfs->tq, this->column_group_idx, BPLN);
	char * old_col_idx       = tq_strdup(this->sfs->tq, this->column_idx, BPLN);
	char * old_str_entry_idx = tq_strdup(this->sfs->tq, this->string_entry_idx, BPLN);

	// destroy old data before continuing
	this->reset();

	// set new pointers
	if(move_row_group_idx)
		this->row_group_idx = tq_strdup(this->sfs->tq, move_row_group_idx, BPLN);

	if(move_row_idx)
		this->row_idx = tq_strdup(this->sfs->tq, move_row_idx, BPLN);

	if(move_col_group_idx)
		this->column_group_idx  = tq_strdup(this->sfs->tq, move_col_group_idx, BPLN);

	if(move_col_idx)
		this->column_idx = tq_strdup(this->sfs->tq, move_col_idx, BPLN);

	if(move_str_entry_idx)
		this->string_entry_idx = tq_strdup(this->sfs->tq, move_str_entry_idx, BPLN);

	// attempt the move, if the move fails, destroy data and reset old pointers
	if(this->update() == ERR_FAILURE)
	{

		// reset (destroy old pointers)
		this->reset();

		// restore old pointers
		this->row_group_idx    = old_row_group_idx;
		this->row_idx          = old_row_idx;
		this->column_group_idx = old_col_group_idx;
		this->column_idx       = old_col_idx;
		this->string_entry_idx = old_str_entry_idx;

		// return indicating that we were unable to move
		return ERR_FAILURE;

	}

	// since the move was successfull (updated ok) we can destroy
	// the old indexes if necessary.
	if(old_row_group_idx)
		tq_free(this->sfs->tq, old_col_group_idx);
	if(old_row_idx)
		tq_free(this->sfs->tq, old_row_idx);
	if(old_col_group_idx)
		tq_free(this->sfs->tq, old_col_group_idx);
	if(old_col_idx)
		tq_free(this->sfs->tq, old_col_idx);
	if(old_str_entry_idx)
		tq_free(this->sfs->tq, old_str_entry_idx);

	// return indicating success
	return ERR_SUCCESS;

}


// updates all data based on the current indexes
BP_ERROR_T BP_StringFormattingSession::update()
{

	// ensure we have a sfs to work with
	if(!this->sfs)
	{
		this->AddErr("Attempt to this->update() but no formatting session (sfs) is set.");
		return ERR_FAILURE;
	}

	// a row index requires a row group
	if(this->row_idx && !this->row_group_idx)
	{
		this->AddErr("Attempt to this->update().  A row index is set, but no row group index is set to match it with.");
		return ERR_FAILURE;
	}

	// a column group requries a row index
	if(this->column_group_idx && !this->row_idx)
	{
		this->AddErr("Attempt to this->update().  A column group index is set, but no row index is set to match it with.");
		return ERR_FAILURE;
	}

	// a column index, requires a column group idx
	if(this->column_idx && !this->column_group_idx)
	{
		this->AddErr("Attempt to this->update().  A column index is set, but no column group index is set to match it with.");
		return ERR_FAILURE;
	}

	// a string entry index, requires a column index
	if(this->string_entry_idx && !this->column_idx)
	{
		this->AddErr("Attempt to this->update().  A string entry index is set, but no column index is set to match it with.");
		return ERR_FAILURE;
	}

	// Destroy search results if they exist
	if(this->sfs_search_results)
	{

		// destroy the search results
		BP_DestroySearchResultsSFSession(this->sfs, this->sfs_search_results);

		// nullify pointer
		this->sfs_search_results = NULL;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Now that all cascading checks have passed, we can search as desired.
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new search query
	BP_SFS_SEARCH_QUERY search_query;
	BP_ZERO_STACK(search_query);


	// set all parameters to match class members (search query only needs
	// reference pointers to work successfully)
	search_query.row_group_id    = this->row_group_idx;
	search_query.row_id          = this->row_idx;
	search_query.column_group_id = this->column_group_idx;
	search_query.column_id       = this->column_idx;
	search_query.string_entry_id = this->string_entry_idx;

	// attempt to search
	this->sfs_search_results = BP_SearchStringFormattingSession(this->sfs, &search_query);
	// printf("\n Search Result Afters?: %s - %p", this->row_idx, this->sfs_search_results);


	// atttempt to search results
	if(this->sfs_search_results->result_n != 1)
	{
		// printf("\n Search result N?: %u", this->sfs_search_results->result_n);
		// this->AddErr("Update failed to search.");
		return ERR_FAILURE;
	}

	// BP_DisplaySFSSearchResults(this->sfs_search_results);

	// if we had results returned, go ahead and set the internal class pointers
	// with the results of the search.
	if(this->sfs_search_results->result_n)
	{

		// set the result from the first index
		P_BP_SFS_SEARCH_RESULT result = &this->sfs_search_results->results[0];

		// set individual values
		this->row_group     = result->row_group;
		this->row           = result->row;
		this->column_group  = result->column_group;
		this->column        = result->column;
		this->string_entry  = result->string_entry;

	}
	else
	{

		// if we have no matching results, return failure
		return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}


// resets all data (frees) stored in indexes and nullifies entries (doesn't free)
BP_ERROR_T BP_StringFormattingSession::reset
(
	BP_BOOL destroy_row_group_idx,
	BP_BOOL destroy_row_idx,
	BP_BOOL destroy_col_group_idx,
	BP_BOOL destroy_col_idx,
	BP_BOOL destroy_str_entry_idx
)
{

	// ensure we have a sfs session to work with
	if(!this->sfs)
	{
		this->AddErr("Attempt to this->reset() but no formatting session (sfs) is set.");
		return ERR_FAILURE;
	}

	// free row group idx
	if(destroy_row_group_idx == BP_TRUE)
	{

		if(this->row_group_idx)
			tq_free(this->sfs->tq, this->row_group_idx);

		this->row_group_idx = NULL;

	}

	// free row idx
	if(destroy_row_idx == BP_TRUE)
	{

		if(this->row_idx)
			tq_free(this->sfs->tq, this->row_idx);

		this->row_idx = NULL;
	}

	// free column group idx
	if(destroy_col_group_idx == BP_TRUE)
	{

		if(this->column_group_idx)
			tq_free(this->sfs->tq, this->column_group_idx);

		this->column_group_idx  = NULL;

	}

	// free column idx
	if(destroy_col_idx == BP_TRUE)
	{

		if(this->column_idx)
			tq_free(this->sfs->tq, this->column_idx);

		this->column_idx = NULL;

	}

	// free string entry idx
	if(destroy_str_entry_idx == BP_TRUE)
	{

		if(this->string_entry_idx)
			tq_free(this->sfs->tq, this->string_entry_idx);

		this->string_entry_idx  = NULL;

	}

	// free search result if set
	if(this->sfs_search_results)
		BP_DestroySearchResultsSFSession(this->sfs, this->sfs_search_results);

	// Since entries are just references to the SFS, we can just nullify them here
	this->row_group    = NULL;
	this->row          = NULL;
	this->column_group = NULL;
	this->column       = NULL;
	this->string_entry = NULL;

	// set the search results to null as well
	this->sfs_search_results = NULL;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Prepare Final Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// call this to take the entrie structure of the document and prepare it, applying
// colors, borders and whatnot, and generating the final output data for each element.
BP_ERROR_T BP_StringFormattingSession::prepareFinalOutput()
{

	// ensure we have a sfs
	if(!this->sfs)
		return ERR_FAILURE;

	// call the generation routine on the sfs
	return BP_GenerateFinalOutputSFSession(this->sfs);

}

// Displays the final output of the string formatting session (it's just a generic
// columnization at this point).
BP_ERROR_T BP_StringFormattingSession::displayFinalOutput(BP_BOOL display_colorized)
{

	if(display_colorized == BP_TRUE)
		BP_DisplayGenericColumnization(sfs->final_columnization_colorized, BP_TRUE);
	else
		BP_DisplayGenericColumnization(sfs->final_columnization, BP_TRUE);

	// add trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine to display sfs data within the class
BP_ERROR_T BP_StringFormattingSession::displaySFSClassData()
{


	printf("\n [+] SFS Class Data for (%p)", this);
	printf("\n     sfs:                %p", this->sfs);
	printf("\n     sfs_search_results: %p", this->sfs_search_results);
	printf("\n     row_group_idx:      %s", this->row_group_idx);
	printf("\n     row_idx:            %s", this->row_idx);
	printf("\n     column_group_idx:   %s", this->column_group_idx);
	printf("\n     column_idx:         %s", this->column_idx);
	printf("\n     string_entry_idx:   %s", this->string_entry_idx);
	printf("\n     row_group:          %p", this->row_group);
	printf("\n     row:                %p", this->row);
	printf("\n     column_group:       %p", this->column_group);
	printf("\n     column:             %p", this->column);
	printf("\n     string_entry:       %p", this->string_entry);
	printf("\n");

	// display search results
	if(this->sfs_search_results)
	{
		BP_DisplaySFSSearchResults(this->sfs_search_results);
	}

	// always return indicating success
	return ERR_SUCCESS;

}

// wrapper to display the current sfs session
BP_ERROR_T BP_StringFormattingSession::displaySFSSession()
{

	// ensure we have a valid sfs session
	if(!this->sfs)
		return ERR_FAILURE;


	// display a string formatting session
	BP_DisplayStringFormattingSession
	(
			this->sfs,
			&this->display_settings
	);

	// return indicating success
	return ERR_SUCCESS;

}



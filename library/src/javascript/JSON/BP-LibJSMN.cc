// include blockparty main
#include "../../include/BP-Main.h"


/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t *jsmn_alloc_token
(
	P_BP_JSON    json,
	jsmn_parser *parser,
	jsmntok_t   *tokens,
	size_t       num_tokens
)
{

	jsmntok_t *tok;

	if (parser->toknext >= num_tokens)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "parser->toknext >= num_tokens.  JSON parsing attempt exhausted preallocated token space.  Likely cause is JSON being too large.");
		return NULL;
	}

	tok = &tokens[parser->toknext++];

	// note, this was modified by JM due to compilation errors from this = that = -1;
	tok->end    = -1;
	tok->start  = tok->end;
	tok->size   = 0;
	tok->parent = -1;
	return tok;

}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token
(
	P_BP_JSON    json,
	jsmntok_t *token,
	jsmntype_t type,
    int start,
	int end
)
{

	if(!token)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_fill_token was called with a NULL token.");
		return;
	}

	token->type = type;
	token->start = start;
	token->end = end;
	token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive
(
	P_BP_JSON    json,
	jsmn_parser *parser,
	const char *js,
	size_t len,
	jsmntok_t *tokens,
	size_t num_tokens
)
{

	jsmntok_t *token = NULL;
	int start = 0;

	start = parser->pos;

	for
	(
		;
		parser->pos < len && js[parser->pos] != '\0';
		parser->pos++
	)
	{

		// switch on current character
		switch (js[parser->pos])
		{

			case '\t':
			case '\r':
			case '\n':
			case ' ':
			case ',':
			case ']':
			case '}':
				goto found;

		}

		if
		(
			js[parser->pos] < 32 || js[parser->pos] >= 127
		)
		{

			// create a small temporary stack buffer
			char tmp_err_buff[512];
			BP_ZERO_STACK(tmp_err_buff);

			// convert values for output
			sprintf((char *) &tmp_err_buff, "jsmn_parse_primitive() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.",js[parser->pos], js[parser->pos], parser->pos);

			// add the error message
			BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff);

			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}

	}

	/* In strict mode primitive must be followed by a comma/object/array */
	parser->pos = start;

	// add the error message
	BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse_primitive() reports that the JSON provided was 'too short'.  This meaning that the json was parsing fine, except the buffer ended prematurely with regards to parse logic.");
	return JSMN_ERROR_PART;

// token was found to be acceptable
found:

	if (tokens == NULL)
	{
		parser->pos--;
		return 0;
	}

	// attempt to allocate token
	token = jsmn_alloc_token(json, parser, tokens, num_tokens);


	if (!token)
	{
		parser->pos = start;
		BP_JSONParseAddErrorMsg(json, NULL, "Parser indicates that we've run out of memory.");
		return JSMN_ERROR_NOMEM;
	}

	jsmn_fill_token(json, token, JSMN_PRIMITIVE, start, parser->pos);

	token->parent = parser->toksuper;
	parser->pos--;
	return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string
(
	P_BP_JSON    json,
	jsmn_parser *parser,
	const char * js,
	size_t       len,
	jsmntok_t *  tokens,
	size_t       num_tokens
)
{
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	// temporary error buffer
	char tmp_err_buff[512];
	BP_ZERO_STACK(tmp_err_buff);

	/* Skip starting quote */
	for
	(
		;
		parser->pos < len && js[parser->pos] != '\0';
		parser->pos++
	)
	{

		// set reference to current character
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"')
		{

			// if tokens is unset, exit immediately
			if (!tokens)
				return 0;


			token = jsmn_alloc_token(json, parser, tokens, num_tokens);

			if (token == NULL)
			{
				parser->pos = start;
				BP_JSONParseAddErrorMsg(json, NULL, "Parser indicates that jsmn_alloc_token() has failed (potentially out of memory).");
				return JSMN_ERROR_NOMEM;
			}

			jsmn_fill_token(json, token, JSMN_STRING, start+1, parser->pos);
			token->parent = parser->toksuper;
			return 0;

		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len)
		{

			int i;

			parser->pos++;

			switch (js[parser->pos])
			{

				/* Allowed escaped symbols */
				case '\"':
				case '/' :
				case '\\' :
				case 'b' :
				case 'f' :
				case 'r' :
				case 'n'  :
				case 't' :
					break;

				/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;

					// inner search loop for \uXXXX chars
					for
					(
						i = 0;
						i < 4 && parser->pos < len && js[parser->pos] != '\0';
						i++
					)
					{

						/* If it isn't a hex character we have an error */
						if
						(
							!(
								(js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
								(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
								(js[parser->pos] >= 97 && js[parser->pos] <= 102)   /* a-f */
							)
						)
						{
							parser->pos = start;

							// create a small temporary stack buffer
							BP_ZERO_STACK(tmp_err_buff);

							// convert values for output
							sprintf((char *) &tmp_err_buff, "jsmn_parse_string() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

							// add the error message
							BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff);

							return JSMN_ERROR_INVAL;
						}

						parser->pos++;

					}

					parser->pos--;
					break;


				/* Unexpected symbol */
				default:
					parser->pos = start;
					// create a small temporary stack buffer

					BP_ZERO_STACK(tmp_err_buff);

					// convert values for output
					sprintf((char *) &tmp_err_buff, "jsmn_parse_string() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

					// add the error message
					BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff);
					return JSMN_ERROR_INVAL;

			}

		}

	}

	parser->pos = start;
	BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse_string() indicates that we have recieved only partial data.  In short, we were parsing fine, but then we unexpectedly ran out of data to parse.");
	return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.  This routine has been modified in
 * order to provide error output information and increase readability.
 */
int jsmn_parse
(
	P_BP_JSON    json,
	jsmn_parser *parser,
	const char * js,
	size_t       len,
	jsmntok_t *  tokens,
	unsigned int num_tokens
)
{

	// ensure we have js
	if(!js)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called with a NULL json buffer.");
		return JSMN_ERROR_INVAL;
	}

	// ensure the buffer isn't zero length
	if(!bpstrlen(js))
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called with a zero length json buffer.");
		return JSMN_ERROR_INVAL;
	}

	// ensure the string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString((char*) js, bpstrlen(js), BP_TRUE, BP_TRUE))
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called with a len parameter set to zero.");
		return JSMN_ERROR_INVAL;
	}

	// check for length mismatch
	if(len != bpstrlen(js))
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called with a len parameter set to zero.");
		return JSMN_ERROR_INVAL;
	}

	// ensure the byte parameter matches up
	if(!len)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called with a len parameter set to zero.");
		return JSMN_ERROR_INVAL;
	}

	// ensure we have a parser
	if(!parser)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "jsmn_parse() was called without a parser.");
		return JSMN_ERROR_INVAL;
	}

	int r = 0;

	int i = 0;

	jsmntok_t *token = NULL;

	int count = parser->toknext;

	// create and zero a temporary buffer
	char tmp_err_buff[512];
	BP_ZERO_STACK(tmp_err_buff);


	for
	(
		;
		parser->pos < len && js[parser->pos] != '\0';
		parser->pos++
	)
	{
		char c;
		jsmntype_t type;

		c = js[parser->pos];

		switch (c)
		{

			case '{':
			case '[':

				// increase the count
				count++;

				// exit case if tokens is unset
				if (!tokens)
					break;

				// attempt to allocate new token
				token = jsmn_alloc_token(json, parser, tokens, num_tokens);
				if (token == NULL)
				{
					BP_JSONParseAddErrorMsg(json, NULL, "Parser indicates that we've run out of memory.", (int) parser->pos, c);
					return JSMN_ERROR_NOMEM;
				}

				if (parser->toksuper != -1)
				{
					tokens[parser->toksuper].size++;
					token->parent = parser->toksuper;
				}

				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;
				parser->toksuper = parser->toknext - 1;
				break;


			case '}':
			case ']':

				if(!tokens)
					break;

				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);

				if (parser->toknext < 1)
				{


					// convert values for output
					BP_ZERO_STACK(tmp_err_buff);
					sprintf((char *) &tmp_err_buff, "jsmn_parse() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

					// add the error message
					BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff, (int) parser->pos, c);
					return JSMN_ERROR_INVAL;
				}

				token = &tokens[parser->toknext - 1];

				for (;;)
				{

					if (token->start != -1 && token->end == -1)
					{

						if (token->type != type)
						{
							// convert values for output
							BP_ZERO_STACK(tmp_err_buff);
							sprintf((char *) &tmp_err_buff, "jsmn_parse() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

							// add the error message
							BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff, (int) parser->pos, c);
							return JSMN_ERROR_INVAL;
						}

						token->end = parser->pos + 1;
						parser->toksuper = token->parent;
						break;

					}

					if (token->parent == -1)
					{
						break;
					}

					token = &tokens[token->parent];

				}

				break;

			case '\"':
				r = jsmn_parse_string(json, parser, js, len, tokens, num_tokens);
				if (r < 0)
					return r;

				count++;
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;

				break;

			case '\t' : case '\r' : case '\n' : case ' ':
				break;

			case ':':
				parser->toksuper = parser->toknext - 1;
				break;

			case ',':
				if (tokens != NULL && parser->toksuper != -1 &&
						tokens[parser->toksuper].type != JSMN_ARRAY &&
						tokens[parser->toksuper].type != JSMN_OBJECT) {

					parser->toksuper = tokens[parser->toksuper].parent;

				}
				break;

			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :

				/* And they must not be keys of the object */
				if (tokens != NULL && parser->toksuper != -1)
				{

					jsmntok_t *t = &tokens[parser->toksuper];

					if
					(
							t->type == JSMN_OBJECT ||
							(t->type == JSMN_STRING && t->size != 0)
					)
					{
						// convert values for output
						BP_ZERO_STACK(tmp_err_buff);
						sprintf((char *) &tmp_err_buff, "jsmn_parse() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

						// add the error message
						BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff, (int) parser->pos, c);
						return JSMN_ERROR_INVAL;
					}

				}

				r = jsmn_parse_primitive(json, parser, js, len, tokens, num_tokens);

				if (r < 0)
					return r;

				count++;

				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;

				break;


			/* Unexpected char in strict mode */
			default:
				// convert values for output
				BP_ZERO_STACK(tmp_err_buff);
				sprintf((char *) &tmp_err_buff, "jsmn_parse() reports that an invalid character \\x%.02x (%c) was found within the json buffer at byte-position: %u.", js[parser->pos], js[parser->pos], parser->pos);

				// add the error message
				BP_JSONParseAddErrorMsg(json, NULL, (char *) &tmp_err_buff, (int) parser->pos, c);
				return JSMN_ERROR_INVAL;

		}

	}

	if (tokens != NULL)
	{

		for (i = parser->toknext - 1; i >= 0; i--)
		{

			/* Unmatched opened object or array */
			if (tokens[i].start != -1 && tokens[i].end == -1)
			{
				BP_JSONParseAddErrorMsg(json, NULL, "json_parse() was parsing OK but ran out of data to parse before state was balanced.");
				return JSMN_ERROR_PART;
			}

		}

	}

	return count;

}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmn_init
(
		P_BP_JSON    json,
		jsmn_parser *parser
)
{

	// basic null checks
	if(!json)
		return;
	if(!parser)
		return;

	parser->pos      = 0;
	parser->toknext  = 0;
	parser->toksuper = -1;
}

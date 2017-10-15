/*
 * BP-StringUtilities.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Jason Medeiros
 */

#ifndef BPSTRINGUTILITIES_H_
#define BPSTRINGUTILITIES_H_



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! NOTICE REGARDING COMPLEX STRING FORMATTING / GENERATION !!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// IMPORTANT:  This file contains a large number of utilities for working
//             with raw strings.  HOWEVER, if you're trying to generate
//             data suitable for terminal output, this is not where you
//             want to be looking.
//
//             Instead, look at the code within utilities/BP-StringFormattingSessions.(h/cc)
//             for a more useful set of routines.  A unit test for working
//             with these sessions is available in:
//
//              applications/examples/string_formatting_sessions_ut.cc
//
//             The unit test code contains examples for working with C and C++
//             both.
//
//
//             Session Features:
//             ------------------------------------------------------
//                   - terminal colors (per character)
//                   - rows/row groups
//                   - columns/column groups
//                   - easy memory allocation/cleanup (tailqueue allocation)
//                   - word wrapping
//                   - definitive column wrapping
//                   - terminal width auto-detection for wrapping
//                   - element searching (rows/columns/groups/strings)
//                   - and more
//
//             Additionally, a C++ class is available for working
//             with string formatting sessions directly.  This class
//             is available in:
//
//                   include/blockclass/subclasses/BP-StringFormattingSession.(cc/h)
//
//
//
//


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic PCRE Expressions (useful) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// matches a url in a body of text
#define GENERIC_MATCH_URL_IN_TEXT_PCRE "[a-zA-Z0-9\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.ninja|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\.\\:\\-\\?\\&\\(\\)\\=\\*\\%%\\$\\#\\@\\!\\,]{0,}"


// matches a url from the beginning of a string
#define GENERIC_MATCH_URL_FROM_START_PCRE "^[a-zA-Z0-9\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.ninja|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\.\\:\\-\\?\\&\\(\\)\\=\\*\\%%\\$\\#\\@\\!\\,]{0,}"

// matches a url without requiring a tld
#define GENERIC_MATCH_URL_FROM_START_NON_STRICT_TLD "^[a-zA-Z0-9\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\.\\:\\-\\?\\&\\(\\)\\=\\*\\%%\\$\\#\\@\\!\\,]{0,}"


// matches a base, and requires a valid TLD
#define GENERIC_MATCH_BASE_FROM_START_REQUIRE_TLD      "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}\\/"

// matches a base and doesn't require a valid TLD
#define GENERIC_MATCH_BASE_FROM_START_TLD_NOT_REQUIRED "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})\\/"


// matches a base and doesn't require a valid TLD
#define GENERIC_MATCH_PATH_FROM_START_REQUIRE_TLD "[a-zA-Z0-9\\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,100}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac)(?:[\\:\\/0-9\\/]{1,})(?:[0-9a-zA-Z\\/\\-\\~\_]{1,}[\\/])"

// matches a base and doesn't require a valid TLD
#define GENERIC_MATCH_PATH_FROM_START_TLD_NOT_REQUIRED "[a-zA-Z0-9\\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,100}(?:(\\/[\\!\\:\\@\\.\\_a-zA-Z0-9\\-\\!\\$]{0,})\\/)"


// matches a host (out of a base, not a url) and requires a dot to be found
#define GENERIC_MATCH_HOST_FROM_BASE_REQUIRE_DOT     "(?:[\\w-]+\\.)(?:\\.?[\\w\\-]{1,})+"

// matches a host (out of a base, not a url) and requires a dot to be found
#define GENERIC_MATCH_HOST_FROM_BASE_NO_DOT_REQUIRED "(?:[\\w-]+)(?:\\.?[\\w\\-]{1,})+"

// matches a script, requires TLD
#define GENERIC_MATCH_SCRIPT_FROM_URL_REQUIRE_TLD "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\_\\.\\-#]{0,}"

// #define GENERIC_MATCH_SCRIPT_FROM_URL_TLD_NOT_REQUIRED "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\.\\-\\_#]{0,}"
#define GENERIC_MATCH_SCRIPT_FROM_URL_TLD_NOT_REQUIRED "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:[\\:\\/0-9]{0,})\\/(?:([\\.\\_\\/\\~0-9a-zA-Z]){0,})"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Perl Compatible Regular Expression Structures %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// match type for perl compatible regular expressions
typedef struct __pregmatch_t {

	// offset in bytes to the match start of
	// the regexp.
	size_t match_index;

	// length of the match
	size_t match_len;

	// optional string saved (will be allocated on heap and will need to be free'd)
	char *matched_string;

}pregmatch_t, *p_pregmatch_t;



// match information structure returned from regex search
typedef struct _BP_REGEX_MATCH_INFO {

	// regular expression match info
	regmatch_t * matches;

	// match count
	size_t num_matched;

	// ===========================================
	// perl compatible regular expressions

	// flag indicating whether or not pcre was ienabled
	size_t pcre_enabled;

	// if this is set, each pregmatch_t item has the string
	// which was matched saved (alloc on heap) in the structure (and must be freed)
	size_t pcre_string_matches_saved;

	// array of matches (structure stack) if matches were found
	p_pregmatch_t preg_matches;

	// match count if this was a pcre regex match set
	size_t pcre_num_matched;

} BP_REGEX_MATCH_INFO, *P_BP_REGEX_MATCH_INFO;


// Attempt to match regular expression against haystack of haystack_size bytes using
// POSIX regular expression in regular_expression.  The regexp_flags variable can be set
// to alter the behavior of the evaluation.  Possible flags shown below.

//	 These are the bit flags that you can pass to the function below:
//		 REG_EXTENDED
//   		 Treat the pattern as an extended regular expression, rather than as a basic regular expression.
//		 REG_ICASE
//    		Ignore case when matching letters.
//		 REG_NOSUB
//    		Don't bother storing the contents of the matches-ptr array.
// 		 REG_NEWLINE
//    		Treat a newline in string as dividing string into multiple lines, so that $ can match before the newline and ^ can match after. Also, don't permit . to match a newline, and don't permit [^...] to match a newline.


BP_ERROR_T BP_MatchRegularExpression
(
		char *  haystack,
		size_t  haystack_size,
		char *  regular_expression,
		int     regexp_flags,
		size_t  max_match_count,
		P_BP_REGEX_MATCH_INFO match_info
);

// memory duplication routine (simple)
void *memdup(const void * src, size_t n);

// displays match info for string
BP_ERROR_T BP_DisplayRegexMatchInfo(P_BP_REGEX_MATCH_INFO match_info);

// String find and replace all occurances of replace_this in in_buff.  Returns new pointer with replacements made.
// This function does not modify in_buff directly.  Replacing strings is binary safe, finding strings is not.
char * BP_RegFindReplace(char *in_buff, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements, size_t regexp_flags);

// Standard find and replace.  Returns a newly allocated string which
// must be freed.  If you're trying to remove something from a string, use BP_FindRemove instead.
char * BP_FindReplace(char *in_buff, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements);

// This will attempt to find and remove patterns in a string.  Binary safe so long as
// provided lengths are given.  This is primarily to allow FindReplace the ability to
// remove strings and replace them with null strings.
char * BP_FindRemoveBin
(
		char * in_buff,
		size_t in_buff_size,
		char * remove_this,
		size_t remove_this_size,
		size_t max_removals
);


// Attempt to match regular expression against haystack of haystack_size bytes using
// POSIX regular expression in regular_expression.  The regexp_flags variable can be set
// to alter the behavior of the evaluation.  Possible flags shown below.

//	 These are the bit flags that you can pass to the function below:
//		 REG_EXTENDED
//   		 Treat the pattern as an extended regular expression, rather than as a basic regular expression.
//		 REG_ICASE
//    		Ignore case when matching letters.
//		 REG_NOSUB
//    		Don't bother storing the contents of the matches-ptr array.
// 		 REG_NEWLINE
//    		Treat a newline in string as dividing string into multiple lines, so that $ can match before the newline and ^ can match after. Also, don't permit . to match a newline, and don't permit [^...] to match a newline.


BP_ERROR_T BP_MatchRegularExpression
(
		char *  haystack,
		size_t  haystack_size,
		char *  regular_expression,
		int     regexp_flags,
		size_t  max_match_count,
		P_BP_REGEX_MATCH_INFO match_info
);

// displays match info for string
BP_ERROR_T BP_DisplayRegexMatchInfo(P_BP_REGEX_MATCH_INFO match_info);

// String find and replace all occurances of replace_this in in_buff.  Returns new pointer with replacements made.
// This function does not modify in_buff directly.  Replacing strings is binary safe, finding strings is not.
char * BP_RegFindReplace(char *in_buff, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements, size_t regexp_flags);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Very Basic/Fast String Conformity Validator %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//
// NOTICE: This content validator does not use expressions, it is used for
//         quickly (loop) determining basic content of a string to provide
//         a mechanism to run literal data validation without much hassle. This
//         capability depends on the BUFFER ANALYSIS subsystem built directly
//         into blockparty.
//

// Enumerator providing easily switchable types for determining string
// contents.
typedef enum BP_STRING_CONFORMITY {
	BP_STRING_CONFORMITY_BADVAL,
	BP_STRING_CONFORMS_AS_EMPTY_STRING,
	BP_STRING_CONFORMS_AS_NUMERIC_ONLY,
	BP_STRING_CONFORMS_AS_HEXIDECIMAL, /* note: should never mix-case hexidecimal */
	BP_STRING_CONFORMS_AS_HEXIDECIMAL_LOWER_ONLY,
	BP_STRING_CONFORMS_AS_HEXIDECIMAL_UPPER_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHA,
	BP_STRING_CONFORMS_AS_ASCII_ALPHA_LOWER_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHA_UPPER_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHA_MIXED_CASE,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_LOWER_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_UPPER_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_MIXED_CASE
};

// create new typedef for conformity flags
typedef BP_FLAGS_T BP_STRING_CONFORMITY_FLAGS_T;

// takes a string and runs a sequence of checks to determine whether the
// the string conforms to a basic sequence mapping.
BP_STRING_CONFORMITY_FLAGS_T BP_StringConformsAs
(
		char *string,
		size_t len,
		char *exception_chars = NULL,
		size_t exception_chars_n = 0
);

// displays string conformity flags
BP_ERROR_T BP_DisplayStringConformityFlags(BP_STRING_CONFORMITY_FLAGS_T c_flags);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Explosion / Splitting Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// explodes a buffer provided and returns an allocated array of allocated pointers (must free entire array
// or call BP_DestroyPointerArray on the array itself).  This routine is not binary safe.
char ** BP_ExplodeBuffer(char *buffer, char *delim_string);

// Explodes a buffer provided and returns an allocated array of allocated pointers (must free entire array
// or call BP_DestroyPointerArray on the array itself).  The maximum elements parameter determines exactly
// how many extractions will be done.  Setting this parameter to 0 allows for infinite extractions.
// This routine is binary safe.
char ** BP_ExplodeBufferBin(char *buffer, size_t buffer_len, char *delim_string, size_t delim_len, size_t max_elements);

// Splits a string along the first instance of the provided token, returns two allocated
// strings, within a char ** or NULL if there was no split performed (aka character
// does not exist).  Both the char ** and the individual char * within the array must
// be freed.  You can also call BP_DestroyPointerArray to destroy all allocated memory
// quickly and efficiently without having to write your own logic.   This routine is not binary safe.
char ** BP_SplitBuffer(char * buffer, char *delim_string);

// Splits a string along the first instance of the provided token, returns two allocated
// strings, within a char ** or NULL if there was no split performed (aka character
// does not exist).  Both the char ** and the individual char * within the array must
// be freed.  You can also call BP_DestroyPointerArray to destroy all allocated memory
// quickly and efficiently without having to write your own logic.  This routine is binary
// safe.
char ** BP_SplitBufferBin(char *buffer, size_t buffer_len, char *delim_string, size_t delim_len);


// Counts the number of instances a given token is found within a buffer.  This function
// is binary safe.
size_t BP_CountTokensInBuffer(char *buffer, size_t buffer_len, char *token, size_t token_len);

// Counts the number of pointers in a null pointer terminated pointer array.  Set
// the max_count parameter to zero to allow for uncapped matches.
size_t BP_CountNumberOfPointersInPointerArray(void ** ptrs, size_t max_count);

// returns a heap allocated set of size_t entries corresponding to each individual string in
// the ptrs array.  This is primarily used for linking in existing string arrays to a tailqueue
// allocator.
size_t * BP_CountStringLengthsInStringPointerArrayAndReturnValues(char ** ptrs, size_t max_count, size_t max_line_len);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Column-ization (splits strings into arrays of columns) %%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// This structure is returned by the split column routines below.  It's required
// due to the necessity of passing columinzation normalization data back to the
// caller.
typedef struct __BP_GENERIC_STRING_COLUMNIZATION {



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Tailqueue Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this was allocated via a tailqueue, this is BP_TRUE
	BP_BOOL allocated_via_tq;

	// tailqueue
	P_BP_LINKL tq;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Column Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the acutal columns created
	char ** columns;

	// the column count
	size_t column_count;

	// the number of spaces added to each line, to create
	// equidistant entries.
	size_t * space_normalization_data;

	// the difference between uncolorized data and colorized
	// data.
	size_t * color_normalization_data;

	// width of the longest line
	size_t longest_width;
	size_t longest_width_colorized;


} BP_GENERIC_STRING_COLUMNIZATION, *P_BP_GENERIC_STRING_COLUMNIZATION;


// Returns P_BP_GENERIC_STRING_COLUMNIZATION split along a column_width boundary.  The array is NULL pointer
// terminated.  This can be freed with the BP_DestroyPointerArray routine.  If the generation
// of string data fails for whatever reason, this routine returns NULL.
P_BP_GENERIC_STRING_COLUMNIZATION BP_SplitStringIntoEqualWidthColumns
(
		char * str_to_split,     /* the string you wish to split into columns */
		size_t str_length,       /* the length of the string provided */
		size_t column_width,     /* the total width you'd like the columns to be*/
		BP_BOOL hyphenate,       /* if you want words to be hyphenated along truncated lines, set to BP_TRUE, else BP_FALSE*/
		size_t empty_pad_lines,  /* you can specify if you want to pad the buffer with empty column_width lines here.  If not, set to zero. */
		P_BP_COLOR_CODE_SET_T * colors = NULL,
		P_BP_LINKL tq = NULL           /* if you want to use a tailqueue allocator, you can set one to use here */
);

// Returns a P_BP_GENERIC_STRING_COLUMNIZATION containing equal width columns split along a character boundary.  Aka
// this will take (for example) an ASCII art file, newline delimited, and measure
// each line.  It will then create a set of columns, with the longest line being
// the column width, and copy every line into the newly created columns.

P_BP_GENERIC_STRING_COLUMNIZATION BP_SplitStringIntoEqualWidthColumnsByDeliniation
(
	char * str_to_split,
	size_t str_len,
	unsigned char deliniation_char,
	size_t min_length_of_columns,
	P_BP_COLOR_CODE_SET_T * colors = NULL,
	P_BP_LINKL tq = NULL           /* if you want to use a tailqueue allocator, you can set one to use here */
);

// Display a generic columnization.
BP_ERROR_T BP_DisplayGenericColumnization(P_BP_GENERIC_STRING_COLUMNIZATION columnization, BP_BOOL display_only_column_data = BP_FALSE);

// Destroys a generic columnization.  Set the second parameter to BP_TRUE if you want to also
// destroy the data which was passed in (aka the structure itself).  You'd not want to enable
// this if you were using a local stack structure (not heap).
BP_ERROR_T BP_DestroyGenericColumnization(P_BP_GENERIC_STRING_COLUMNIZATION columnization, BP_BOOL destroy_struct, P_BP_LINKL tq = NULL);

// Merge a stack of pointers to columnizations into one columnization structure.  This
// routine will merge them vertically (one on top of the other).
P_BP_GENERIC_STRING_COLUMNIZATION BP_MergeGenericColumnizationsVertically(P_BP_GENERIC_STRING_COLUMNIZATION * columnizations, size_t columnization_count);

// Merge a stack of pointers to columnizations into one columnization structure.  This
// routine will merge them vertically (one on top of the other).
P_BP_GENERIC_STRING_COLUMNIZATION BP_MergeGenericColumnizationsHorizontally(P_BP_GENERIC_STRING_COLUMNIZATION * columnizations, size_t columnization_count);


// Since columnizations typically don't work within the greater mindest of tailqueue allocators, this
// routine allows you to duplicate a columnization and store it in a tailqueue.
P_BP_GENERIC_STRING_COLUMNIZATION BP_StoreColumnizationInTailQ(P_BP_GENERIC_STRING_COLUMNIZATION columnization, P_BP_LINKL tq, BP_BOOL destroy_copied_columnization = BP_FALSE);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Buffer Borderization %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Clones a border set on the heap.  If a tq is set, it will be cloned within that allocator.
P_BP_STRING_ROW_BORDER_SET BP_CloneBorderSet(P_BP_STRING_ROW_BORDER_SET border_set_to_clone, P_BP_LINKL tq = NULL);

// Add a border to a buffer.  Does not modify the incomming strings, instead it
// returns a new set of strings.
P_BP_GENERIC_STRING_COLUMNIZATION BP_BorderizeStringColumnization(P_BP_GENERIC_STRING_COLUMNIZATION string_columnization, P_BP_STRING_ROW_BORDER_SET border_set);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Line Oriented Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// returns the number of lines in a buffer deliniated by the supplied delinator.
size_t BP_GetNumberOfLinesInBuffer(char *buff, size_t buff_len, unsigned char deliniator);

// returns the longest line in a buffer deliniated by the supplied deliniator
size_t BP_GetLongestLineInBuffer(char *buff, size_t buff_len, unsigned char deliniator);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Character Position Finder %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// the lookup direction used when searching for characters below
typedef enum BP_STRING_OFFSET_LOOKUP_DIRECTION {
	BP_STRING_OFFSET_LOOKUP_DIRECTION_UNSET,
	BP_STRING_OFFSET_LOOKUP_DIRECTION_FORWARD,
	BP_STRING_OFFSET_LOOKUP_DIRECTION_REVERSE
};

// Returns the index within the provided buffer of the first instance of
// character.  This is used for finding offsets into buffers.  Returns -1
// if an offset cannot be found (or error)
int BP_LookupFirstCharacterOffsetInBuffer
(
	char * buffer,
	size_t buffer_len,
	char   lookup_char,
	BP_STRING_OFFSET_LOOKUP_DIRECTION lookup_direction
);

// Same as above but requires a reasonable string as input.
int BP_LookupFirstCharacterOffsetInReasonableString
(
	char * reasonable_string,
	char   lookup_char,
	BP_STRING_OFFSET_LOOKUP_DIRECTION lookup_direction
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Quote Testing / Removal %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple typedef return set for determining string
// quote semantics.  Used by the below routine
typedef enum BP_STRING_QUOTE_CHECK_RESULTS {
	BP_STRING_IS_UNDEFINED_OR_EMPTY,
	BP_STRING_IS_NOT_QUOTED,
	BP_STRING_IS_DOUBLE_QUOTED,      /* Aka: "string" */
	BP_STRING_IS_SINGLE_QUOTED,      /* Aka: 'string' */
	BP_STRING_IS_MISMATCHED_QUOTED,  /* Aka: "string or 'string or some variation therein */
	BP_STRING_IS_MIXED_QUOTED_ERROR, /* Aka: 'string" or "string' or */
	BP_STRING_RESULT_IS_UNDEFINED    /* Undefined */
};

// Tests to see if the provided string is a quoted string.
BP_STRING_QUOTE_CHECK_RESULTS BP_CheckIfBufferIsQuoted(char * buffer, size_t buffer_len);

// Strips quotes from a string.  This relies on BP_CheckIfBufferIsQuoted to determine
// quote types and strips from there.  The return value is NULL in the case that the
// string has quotes which were unable to be removed, or a newly allocated string
// containing the unquoted string.  The return value must be freed.  It is recommended
// that you call the check routine before utilizing this routine for thoroughness reasons.
char * BP_UnquoteQuotedString(char *buffer, size_t buffer_len);

// The BP_CheckIfBufferIsQuoted() routine returns a check result.  This result can be
// translated into a string representation by supplying that check result to this routine.
// The value returned from this routine is allocated on the heap and must be freed manually.
char * BP_QuoteCheckResultToStringValue(BP_STRING_QUOTE_CHECK_RESULTS quote_check_results);


// --- Quote escaping tests -----------------------------

// Note: Escaped quotes are escaped by the '\' character.

// Returns BP_TRUE if the buffer contains an unescaped single quote.  Set char_pos_inout to a size_t
// to retrieve the position in the buffer where the unquoted character was found.
BP_BOOL BP_BufferContainsUnescapedSingleQuote(char *buffer, size_t buff_size, size_t *char_pos_inout);

// Returns BP_TRUE if the buffer contains an unescaped double quote.  Set char_pos_inout to a size_t
// to retrieve the position in the buffer where the unquoted character was found.
BP_BOOL BP_BufferContainsUnescapedDoubleQuote(char *buffer, size_t buff_size, size_t *char_pos_inout);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Display Routines for Arbitrary Types %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an array of strings.  This is primarily a utility routine to assist in development
// related tasks, such as verifying exploded data content.  The array must be NULL pointer
// terminated.
BP_ERROR_T BP_DisplayStringArray(char * prefix, size_t new_lines_between_item, size_t tabs_before_item, char **array);




// ###################################################################
// ## PCRE Routines ##################################################
// ###################################################################

// perl compatible regular expression match routine.
// BP_MatchRegularExpressionPCRE

BP_ERROR_T BP_MatchRegularExpressionPCRE
(
		_vin char *  haystack,
		_vin size_t  haystack_size,
		_vin char *  regular_expression,
		_vin int     regexp_flags,
		_vin size_t  max_match_count,
		_vinout P_BP_REGEX_MATCH_INFO match_info,
		_vin size_t  save_matches
);

// perl compatible regular expression match info display routine.
//BP_DisplayRegexMatchInfoPCRE
BP_ERROR_T BP_DisplayRegexMatchInfoPCRE(P_BP_REGEX_MATCH_INFO match_info);

// perl compatible regular expression find and replace routine (return value is a allocated string or null on failure)
char * BP_RegFindReplacePCRE(char *in_buff, size_t buff_len, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements, size_t regexp_flags);

// destroys all member data within the match structure
BP_ERROR_T BP_DestroyRegexpMatchInfo( _vdestroy P_BP_REGEX_MATCH_INFO match_info);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% URL Decoding %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// used in the url decoder below
char BP_FromHex(char ch);

// decodes a url from %nn characters.  This was taken from an example on
// the internet, and is not of my own design.  This was due to a rush on time,
// and no other technical reason.  I have to re-write this at a later date.  This
// note is just so that no one who reads this thinks I'm a dirty code thief.
char *BP_URLDecode(char *str);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blacklist Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This function takes a blacklist, an item, and an item count and checks
// to see that the item exists as a regexp in the blacklist.  If so it returns
// the position (first, second, third) of the index which matched. If there
// is no match, it returns 0.  Note: The 0 index, is the first position.
int BP_StringMatchesBlacklistREGEXP(char ** blacklist, size_t blacklist_cnt, char *item);


// Destroys an array of pointers, similar to that which is
// used in the fmtTable used by the test generator.  Useful for
// destroying arrays of string which were created dynamically.
size_t BP_DestroyPointerArray(void ** ptrs, size_t ptrCount);

// Destroys an array of pointers but relies on a trailing
// null terminating null pointer to determine when it should
// stop destroying.
BP_ERROR_T BP_DestroyPointerArrayNullTerminated(void ** ptrs);

// Destroys an array of pointers, TAILQ version..
size_t BP_DestroyPointerArrayTAILQ(P_BP_LINKL tq, void ** ptrs, size_t ptrCount);


// simple enumerator to define different string configurations for random ascii string generator
typedef enum {
	BP_GEN_RANDOM_ASCII_STRING_TYPE_UNSET,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_CAPS_OR_LOWER,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_CAPS_ONLY,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_NUMERIC_CAPS_OR_LOWER,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_NUMERIC_CAPS_ONLY,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_NUMERIC_ONLY,
	BP_GEN_RANDOM_ASCII_STRING_TYPE_SYMBOLS_ONLY
} BP_GEN_RANDOM_ASCII_STRING_TYPE;

// creates a string containing pseudorandom (not cryptographically secure) ASCII data
char * BP_GenRandomASCIICharString(size_t len, BP_GEN_RANDOM_ASCII_STRING_TYPE type, P_BP_LINKL tq = NULL);

// creates a string containing pseudorandom unsigned char data.
unsigned char * BP_GenRandomUnsignedCharString(size_t len);

// randomizes a pointer array from an existing array
char ** BP_RandomizePointerArray(char **ptrs, size_t num_ptrs);

// randomizes a pointer array from an existing array
char ** BP_RandomizePointerArrayTQ(char **ptrs, size_t num_ptrs, P_BP_LINKL m);


// *NON-INLINE VERSION* Function will take a C string and convert it to ASM
char * BP_ASMStringEncode(char *str, int len);

// *INLINE VERSION* Function will take a C string and convert it to a ASM
char * BP_InlineASMStringEncode(char *str, int len);

// *changes it to a php suitable injection string
char * BP_PHPInjectionStringEncode(char *str, int len);

// *C injection string encoder
char * BP_CInjectionStringEncode(char *str, int len);

// Encodes as UTF8
char * BP_UTF8StringEncode(char *str, int len, P_BP_LINKL tq = NULL);

// Encodes for stringToCharCode XSS
char * BP_XSSInjectionStringEncode(char *str, int len);

// MySQL styled CHAR() encoding
char * BP_MYSQLCharFuncInjectionStringEncode(char *str, int len);

// MSQL styled CHAR() encoding
char * BP_MSQLCharFuncInjectionStringEncode(char *str, int len);

// convert size_t to buffer
char * BP_SizeTToMySQLBuffer(size_t val, P_BP_LINKL tq = NULL);



// convert buffer to hex string. eg \x00 = 00.  String
// needs to be freed' after allocation.  Null on return.
char * BP_BufferToHexString(char *str, size_t len);

// convert buffer to hex string. eg \x00 = 00.  String
// needs to be freed' after allocation.  Null on return.
char * BP_BufferToMySQLHexString(char *str, size_t len, P_BP_LINKL tq = NULL);

// convert size_t to buffer
char * BP_SizeTToBuffer(size_t val);

// convert size_t to 0xhex value
char * BP_SizeTToHexBuffer(size_t val);

// Generates a hexidecimal string representation of a buffer provided.  Uses
// the supplied tail queue allocator if one is passed in.  If the passed tail
// queue allocator is null, this function uses standard allocation routines to generate
// the buffer, which implies they must be freed directly after use.
char * BP_GenHexStringFromBuffer(unsigned char * buff, size_t size, P_BP_LINKL tq);

// same as the non-escaped version, except this version escapes the string as if it were
// \xNN encoded (aka \x00\x01\x02)
char * BP_GenEscapedHexStringFromBuffer(unsigned char * buff, size_t size, P_BP_LINKL tq);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Case Converstion %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// converts a string to lower case (returns bp allocated memory, must be bpfreed)
char * BP_StringToLowerCase(char * buff, size_t buff_len);

// converts a string to upper case (returns bp allocated memory, must be bpfreed)
char * BP_StringToUpperCase(char * buff, size_t buff_len);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Standard Type to Ascii Conversions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// these conversion types must be specified below
typedef enum BP_NUMERIC_STRING_CONVERSIONS
{
	BP_NUMERIC_STRING_CONVERSIONS_STANDARD,
	BP_NUMERIC_STRING_CONVERSIONS_HEX,
	BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x,
	BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION
};

// unsigned value converters
char * BP_UInt64ToASCII(uint64_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_UInt32ToASCII(uint32_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_UInt16ToASCII(uint16_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_UInt8ToASCII(uint8_t  val,  BP_NUMERIC_STRING_CONVERSIONS conversion);

// signed value converters
char * BP_Int64ToASCII(int64_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_Int32ToASCII(int32_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_Int16ToASCII(int16_t val, BP_NUMERIC_STRING_CONVERSIONS conversion);
char * BP_Int8ToASCII(int8_t   val, BP_NUMERIC_STRING_CONVERSIONS conversion);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Print Bits in Bytes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to print bits in a byte
BP_ERROR_T BP_PrintBitsInByte(unsigned char in_byte);

// attept to print bits in a series of bytes (space delimited)
BP_ERROR_T BP_PrintBitsInBytes(unsigned char *in_bytes, size_t byte_count);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple String to Number Conversion Utilties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Check if string contains only numeric ascii digits (no other chars allowed)
BP_BOOL BP_StringContainsOnlyNumericASCIIDigits
(
	char  *str,
	size_t len
);


// Converts a string to a signed long (base10).  Stores result in store_value.
BP_ERROR_T BP_StringToSignedLongBase10
(
	char * str,
	size_t len,
	long * store_value
);

// Converts a string to a unsigned long (base10).  Stores result in store_value.
BP_ERROR_T BP_StringToUnsignedLongBase10
(
	char * str,
	size_t len,
	unsigned long * store_value
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Convert String To Binary Blob %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




// This will convert a string such as 0xCAFEBABE in ascii, to a binary string.  The length
// of the new bpcalloced buffer (returned pointer).  String must be hex-ascii.  A 0x prefix
// can be used (will simply be skipped over).
char * BP_StringHexToBinaryBlob(char *ascii_hex_string_to_convert, size_t *out_blob_length);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Quoting Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// quotes a string (result must be freed)
char * BP_SingleQuoteString(char *string);
char * BP_DoubleQuoteString(char *string);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% XOR Encryption / Decryption %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// XOR "Encrypts" a provided value.  Value can contain binary data.
unsigned char * BP_XOREncrypt(unsigned char *val, size_t val_length, unsigned char * key, size_t key_length);

// XOR Decryption routine (it's just a logical placeholder for
// XOREncrypt)
unsigned char * BP_XORDecrypt(unsigned char *val, size_t val_length, unsigned char * key, size_t key_length);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Base 64 Encoding / Decoding %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// structure which holds decoded data (since size varies with
// base64 decodes).
typedef struct __BP_BASE64_DECODED_DATA {

	// this contains the decoded data
	unsigned char * buff;

	// decoded length length
	size_t buff_length;

} BP_BASE64_DECODED_DATA, *P_BP_BASE64_DECODED_DATA;


// encodes a buffer with base64 (must be freed)
char * BP_Base64Encode(unsigned char *src, size_t len);

// decodes a buffer which has been encoded with base64
P_BP_BASE64_DECODED_DATA BP_Base64Decode(char *src, size_t len);

// destroys the data including structure (call this every time you need
// to destroy the result from the above decode function)
BP_ERROR_T BP_Base64DestroyDecodedData(P_BP_BASE64_DECODED_DATA data);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Array Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Pushes an array onto a heap allocated array of char*s.
BP_ERROR_T BP_ArrayPushDupString(char * value, char *** stack, size_t *stack_n);

// TAIL QUEUE VERSION
// Pushes an array onto a heap allocated array of char*s.
// TAIL QUEUE VERSION
BP_ERROR_T BP_ArrayPushDupStringTQ(P_BP_LINKL tq, char * value, char *** stack, size_t *stack_n);

// HASH REGISTRY VERSION
// Pushes an array onto a heap allocated array of char*s.
// HASH REGISTRY VERSION
BP_ERROR_T BP_ArrayPushDupStringHREG(P_BP_HASH_TABLE_REGISTRY hreg, char * value, char *** stack, size_t *stack_n);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String/Mem Joining Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE: These routines are very simple and just use a tail queue allocator
//       to join up strings/memory by allocating them in the tq, and then flattening
//       the tq.  The final string must be bpfree()d or tq_free'd if you
//       link the returned result into your own tailqueue.

// simply joins two strings and returns a heap buffer for you to use.  If a tq_in parameter is supplied
// the returned pointer is automatically linked into the tailqueue.
char * BP_JoinStrings(char *string_front, char *string_tail, P_BP_LINKL tq_in = NULL);

// joins two pieces of memory in a newly allocated buffer. If a tq_in parameter is supplied
// the returned pointer is automatically linked into the tailqueue.
void * BP_JoinMem(void *mem_front, size_t front_len, void *mem_tail, size_t tail_len, P_BP_LINKL tq_in = NULL);

// joins all elements in string_array by calling BP_JoinStrings on each element consecutively.
char * BP_JoinStringArray(char ** string_array, size_t entry_count, P_BP_LINKL tq_in = NULL);

// This routine joins an array of buffers (simple structure containing a pointer and a size), and stores the resulting
// joined buffer in the out_buff parameter.  It's done this way so we don't have to allocate the out_buff
// for the user, and they can just supply a stack pointer etc.  If this routine returns ERR_SUCCESS, it means
// that the data and len members of out_buff have been set to the joined data, and can then be used.  The
// data member is allocated on the heap directly if no tq_in is provided.  If one is provided, it is allocated
// via that tailqueue.
BP_ERROR_T BP_JoinMemArray(P_BP_BUFFER_T mem_array, size_t entry_count, P_BP_BUFFER_T out_buff, P_BP_LINKL tq_in = NULL);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String/Mem Combining Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to join a string array
char ** BP_CombineStringArray(char ** array_head, char **array_tail);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Regular Expression URL Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These url parsing routines are more percise than their non
// expressive counterparts in the webexploitation tree.  Use
// these routines whereever possible instead.

// Parses a string to a url.  If the require_valid_tld_match option is set, the PCRE
// used is substituted for one that matches along lines of any valid tld.
char * BP_ParseStringToURLPCRE(char *long_form_url, BP_BOOL require_valid_tld_match);

// parses a url to a host (no http etc)
char * BP_ParseURLToHostPCRE(char *long_form_url, BP_BOOL require_dot_in_path_for_match = BP_FALSE);

// parses a url to a base using pcre
char * BP_ParseURLToBasePCRE(char *long_form_url, BP_BOOL require_valid_tld = BP_FALSE);

// parses a url to a path using pcre
char * BP_ParseURLToPathPCRE(char *long_form_url, BP_BOOL require_valid_tld = BP_FALSE);

// parses a url to a script using pcre
char * BP_ParseURLToScriptPCRE(char *long_form_url, BP_BOOL require_valid_tld = BP_FALSE);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specific Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to parse out a csv file from a buffer.  This is primarily
// used with the sqlmap subsystem which generates a csv file directly.
char * BP_ParseBufferToUNIXFilesystemPathCSVPCRE(char *buffer, size_t buffer_size);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Regular Expression String Type Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// string conforms to a basic URL (uses preg)
BP_ERROR_T BP_StringIsURL(char *str_value, BP_BOOL require_valid_tld_match);

// string conforms to a host semantic (uses preg)
BP_ERROR_T BP_StringIsHostURL(char *str_value);

// string conforms to a base semantic (uses preg)
BP_ERROR_T BP_StringIsBaseURL(char *str_value);

// string conforms to a script semantic (uses preg)
BP_ERROR_T BP_StringIsScriptURL(char * str_value);

// string conforms to a path semantic (uses preg)
BP_ERROR_T BP_StringIsPathURL(char * str_value);

// checks if a string is an IPv4 address (doesn't use preg)
BP_ERROR_T BP_StringIsIPv4Address(char * str_value);

// uses regular expressions to determine if a string is a valid email address.
BP_ERROR_T BP_StringIsValidEmailAddress(char *str_value);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Stack Searching %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Checks a stack of strings to see if the string is already
// in the stack.
BP_ERROR_T BP_StringExistsInPointerArray(char ** strings_to_search, size_t stack_size, char *string);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Unique ID Generation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Generates a random uuid.  This is purely random, not based on the GUID/UUID standard
// which relies on various pieces of system data.  The reason for this is that the UUID
// standard can leak information in UUIDs, so this aims to avoid that data leak.
char * BP_GenRandomUUID();

// Buffer must be at least 40 bytes long
BP_ERROR_T BP_GenRandomUUID_StoreInBuff(char *buff);

// Simple check to ensure a string matches a UUID pattern.
BP_ERROR_T BP_StringIsUUID(char * str);

// Generates a random SHA1() hash in ascii (20bytes random as ascii)
char * BP_GenRandomSHA1();

// Generates a random MD5() hash in ascii (16bytes random as ascii)
char * BP_GenRandomMD5();



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SHA1 Generation from Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Generates a SHA1 has provided given input.  Returned string must be freed.
char * BP_GenSHA1(char *in, size_t in_size);

// Generates a SHA1 provided given input of a null ptr terminated string array.  Returned string must be freed.  This function
// is obviously not length safe, and should be used with caution.  Max string length for this
// function is 2048.
char * BP_GenSHA1FromStringArray(char **in);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SHA256 Generation From Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// !!!! BEGIN SHA256 ALGORIGHTM DEPENDENCIES !!!!

// NOTE: Used from http://bradconte.com/sha256_c (the author specifies no license or terms
//       of restriction for this code.

// DBL_INT_ADD treats two unsigned ints a and b as one 64-bit integer and adds c to it
#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

typedef struct {
   unsigned char data[64];
   unsigned int datalen;
   unsigned int bitlen[2];
   unsigned int state[8];
} BP_SHA256_CTX;


// !!!! END SHA256 ALGORIGHTM DEPENDENCIES !!!!

// USE THIS, DO NOT USE THE OTHER FUNCTIONS ABOVE
// Attempts to generate a sha256 hash from a provided buffer
char * BP_GenSHA256(char *in, size_t size);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% MD5 Generation From Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simple routine to generate a MD5 Hash
char * BP_GenMD5(char *in, size_t in_size);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% PHP Gadget String Encoders %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// PHP string encoders encode binary buffers as strings of php code.  The reason for this
// is obfuscating or otherwise modifying a string of bytes to meet some encoder criteria.
//

// Typedef used for determining how individual characters should be encoded.  This is used
// internally in the functions below for determining how data should be generated.
typedef size_t BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR;
typedef enum BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR_ENUM {
	BP_PHP_GADGET_BUFFER_ENCODE_UNSET,
	BP_PHP_GADGET_BUFFER_ENCODE_ASCII_ONLY,
	BP_PHP_GADGET_BUFFER_ENCODE_CHR_ONLY,
	BP_PHP_GADGET_BUFFER_ENCODE_HEX_ONLY,
	BP_PHP_GADGET_BUFFER_ENCODE_DECIMAL_ONLY,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_CHR,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_HEX_DECIMAL,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_DECIMAL,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_DECIMAL,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_HEX,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_DECIMAL_ORD,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_ORD,
	BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_DECIMAL_ORD,
};

// === PHP UChar Encoders =============================

// Encodes a single character to a char string.  Non printables are hex encoded as escaped hex.
char * BP_GenUCharAsPHPEscapedASCII(unsigned char val, P_BP_LINKL tq);

// Encodes a single character as ord('n')
char * BP_GenUCharAsPHPOrd(unsigned char val, P_BP_LINKL tq);

// Encodes a single character as 0xNN
char * BP_GenUCharAsPHPHex(unsigned char val, P_BP_LINKL tq);

// Encodes a single character as a decimal number
char * BP_GenUCharAsPHPDecimal(unsigned char val, P_BP_LINKL tq);

// Encodes a single character as chr(dec)
char * BP_GenUCharAsPHPChr(unsigned char val, P_BP_LINKL tq);


// decimal next



// === PHP UChar Variable Encoder =====================

// Encodes a single character to specified selection
char * BP_GenPHPGadgetEncodeUChar(unsigned char val, BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR selector, P_BP_LINKL tq);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Buffer->Code Encoders (translate strings to code) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// generates a set of pack('c*', 0xval, dec) values
char * BP_GenPHPGadgetEncodeBufferAsPack(unsigned char * buff, size_t buff_size, BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR encode_selector, P_BP_LINKL tq);

// generates a set of implode(array(chr(dec),chr(dec2)) values
char * BP_GenPHPGadgetEncodeBufferAsImplodingArray(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);

// generates a imploding scanf with hex encoding
char * BP_GenPHPGadgetEncodeBufferAsImplodingSScanf(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);

// generates a sprintf statement with hex encoding
char * BP_GenPHPGadgetEncodeBufferAsSprintf(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);

// generates a randomly selected buff
char * BP_GenPHPGadgetEncodeBufferAsDotJoinedChars(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);

// generates a base64_decode("base64_string") statement
char * BP_GenPHPGadgetEncodeBufferAsBase64Decode(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);

// -- random encoding --

// Selects a random buffer encoder from all of the available php encoders, and encodes
// the string using that.
char * BP_GenPHPGadgetEncodeBufferAsRandomEncode(unsigned char * buff, size_t buff_size, P_BP_LINKL tq);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Format Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// returns an array full of alternating pointers to the color code sets found in codes.
P_BP_COLOR_CODE_SET_T * BP_GenerateAlternatingColorStack(P_BP_COLOR_CODE_SET_T codes, size_t code_count, size_t num_to_generate_as_alterating, P_BP_LINKL tq = NULL);

// Generates a heap buffer storing the color prefix data for a color set.  Result
// must be freed using bpfree (or tq_free if linked into a linkl)
char * BP_GenerateTerminalColorPrefixFromColorSet(P_BP_COLOR_CODE_SET_T colors);

// Colorizes a string based around the colors passed in.  Each character must have a
// matching colors pointer entry.  This means that the string length must equal the number
// of pointers available.
char * BP_ColorizeString(char * string, size_t string_len, P_BP_COLOR_CODE_SET_T * colors, BP_BOOL terminate_with_color_reset = BP_TRUE);

// Simple wrapper for printf
BP_ERROR_T BP_printf(const char * fmt, ...);

// Wrapper for printf using VT colors
BP_ERROR_T BP_printfVTColor(const char * fmt,  BP_FOREGROUND_COLOR_CODE_T fgc, BP_BACKGROUND_COLOR_CODE_T bgc, ...);

// If blockparty is in a debug configuration, this will print out debug info,
// if the library is not in a debug configuration, this will print nothing.
BP_ERROR_T BP_dbg_printf(const char * fmt, ...);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Trimming %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Enumeration for trim direction.
typedef enum
{
	BP_STRING_TRIM_DIRECTION_UNSET,
	BP_STRING_TRIM_DIRECTION_RIGHT,
	BP_STRING_TRIM_DIRECTION_LEFT,
	BP_STRING_TRIM_DIRECTION_BOTH

} BP_STRING_TRIM_DIRECTION;

// Attempt to trim whitepace off the ends of a string (uses BP_StringTrim
// under the hood)
char * BP_StringTrimWhitespace
(
		char * str_to_trim,
		BP_STRING_TRIM_DIRECTION trim_direction,
		size_t  max_trim
);

// This routine will attempt to trim a REASONABLE
// string based around parameters.  Max trim is the
// maximum trim depth this will allow.  The returned
// string is allocated with bpcalloc and must be free'd
// with bpfree().
char * BP_StringTrim
(
		char * str_to_trim,
		BP_STRING_TRIM_DIRECTION trim_direction,
		BP_BOOL trim_newline,
		BP_BOOL trim_tab,
		BP_BOOL trim_space,
		BP_BOOL trim_carriage_return,
		char    trim_arbitrary_char,
		size_t  max_trim
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Safer String Length Drop in (non macro) Replacements %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to calculate string length (generic drop in replacement for strlen() but stops on any binary characters)
size_t bpstrlen_real(const char *str, BPLN_PARMS);

// attempt to calculate string length (generic drop in replacement for strlen() but stops on any binary characters)
size_t bpstrnlen_real(const char *str, size_t max_len, BPLN_PARMS);

// Use these macros to automatically pass through the bpln paramaters
#define bpstrlen(str)\
	bpstrlen_real( str,  __FILE__, (size_t) __LINE__, (char *) __FUNCTION__)

#define bpstrnlen(str, len)\
	bpstrnlen_real( str, len, (char *) __FILE__, (size_t) __LINE__, (char *) __FUNCTION__)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quick Analysis Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Reasonable, is a term I defined.  This means that the string contains only printable characters, and optionally, newlines.  I use this
// routine to do quick checks on user input strings, so that I can make determinations on whether or not the strings should be usable.  This
// relies on the buffer analysis capability of the library to work.  Again, this is a quick use routine that I personally use, and isn't
// something forced upon you.  You can use the buffer analysis capability directly if you prefer.
BP_BOOL BP_StringIsReasonablePrintableString
(
		char * string_to_analyze,
		size_t len,
		BP_BOOL allow_newlines =		BP_FALSE,
		BP_BOOL allow_carriage_returns = BP_FALSE
);

// This attempts to determine if a string is reasonable with regard to a string which may become a unix filesystem path.  This routine
// will ensure that the string is a reasonable/printable string (using BP_StringIsReasonablePrintableString) but will run secondary
// character checks to make sure a user isn't sending in malformed path data.  The attempt_to_stat_file parameter will attempt to actually
// touch the file on the filesystem if it's enabled (disabled by default)
BP_BOOL BP_StringIsReasonableForUnixEXT4Directory
(
		char * directory_path_to_analyze,
		size_t len,
		BP_BOOL attempt_to_stat_directory = BP_FALSE
);


// This attempts to determine if a string is reasonable with regard to a string which may become a unix file (as in regular file) path.  This routine
// will ensure that the string is a reasonable/printable string (using BP_StringIsReasonablePrintableString) but will run secondary
// character checks to make sure a user isn't sending in malformed path data.  The attempt_to_stat_file parameter will attempt to actually
// touch the file on the filesystem if it's enabled (disabled by default)
BP_BOOL BP_StringIsReasonableForUnixEXT4Filename
(
		char * file_path_to_analyze,
		size_t len,
		BP_BOOL attempt_to_stat_file = BP_FALSE
);

// checks to ensure that a string is reasonable for a json property name
BP_BOOL BP_StringIsReasonableForJSONPropertyNameString
(
		char * json_propname_string_to_analyze,
		size_t json_propname_string_len
);

// This attempts to determine if a string is reasonable with regard to JSON string output.
BP_BOOL BP_StringIsReasonableForJSONOutputString
(
		char * json_output_string_to_analyze,
		size_t json_output_string_len
);

// This attempts to determine if a string is reasonable for a signed integer value.
BP_BOOL BP_StringIsReasonableForSignedIntegerASCII
(
		char * string_to_analyze,
		size_t string_len
);

// This attempts to determine if a string is reasonable for an unsigned integer value.
BP_BOOL BP_StringIsReasonableForUnsignedIntegerASCII
(
		char * string_to_analyze,
		size_t string_len
);

// This attempts to see if a string only contains hexidecimal characters.
BP_BOOL BP_StringIsReasonableForHexUpperASCII
(
		char * string_to_analyze,
		size_t string_len
);

// This attempts to see if a string only contains hexidecimal characters.
BP_BOOL BP_StringIsReasonableForHexLowerASCII
(
		char * string_to_analyze,
		size_t string_len
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tab Buffer Generation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to create a tab buffer at tab_depth length.  If store_in_linkl is used, the buffer
// will be created via the linkl.  If not, it will be created using bpcalloc, and will thus need
// to be free'd.
char * BP_CreateTabBuffer(size_t tab_depth, P_BP_LINKL store_in_linkl = NULL);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Null Terminated String Compares/Matching %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Checks if two strings are the same.  Strings must be reasonable/printable or this routine will return false.  This
// is a slightly safer version of strcmp.  Strings which are not null terminated can still result in access violations.
BP_BOOL BP_StringsMatchAsReasonableStrings(char *str1, char *str2);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Password Reading / Masking from Input %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to read a password or other sensitive information using a mask char.  To
// disable mask characters getting output at all, you can supply a null byte.  Returned
// value is bpcalloced buffer and must be bpfreed().
char * BP_GetPasswordFromTerminal
(
	FILE * read_from,
	size_t max_read_len,
	char char_to_mask_output_with
);

// clear a linux terminal using xterm sequence.
BP_ERROR_T BP_ClearLinuxTerminalOutput();




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Array Copy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// copy a string array.  Result must be destroyed using BP_DestroyPointerArrayNullTerminated(**)
char ** BP_StringArrayCreateCopyOnHeap(char ** null_terminated_array_of_reasonable_strings);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Password Policy Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*

 example json policy:

        // fill out password policy here
        char *pass_policy = ""\
        "       \"password_policy\":                                                    "\
        "       {                                                                       "\
        "               \"min_length\":                \"10\",                          "\
        "               \"max_length\":                \"256\",                         "\
        "               \"special_chars_set\":         \"!@#$%^&*()_+=-:;][}{/?,><.\",  "\
        "               \"disallowed_chars_set\":      \"\\\"'\",                       "\
        "               \"min_special_chars\":         \"3\",                           "\
        "               \"max_special_chars\":         \"255\",                         "\
        "               \"min_char_diversity\":        \"5\",                           "\
        "               \"check_known_bad_passwords\": \"yes\",                         "\
        "               \"known_bad_password_lists\":                                   "\
        "               {                                                               "\
        "                       \"0\":  \"./bad_passes0.list\",                         "\
        "                       \"1\": \"./bad_passes1.list\"                           "\
        "               }                                                               "\
        "       }";

*/

// Returns BP_TRUE if the provided password passes the provided policy checks.  Password must be
// a reasonable-printable or check will not run.  Policy check information is returned in the
// provided json_in_out.
BP_BOOL BP_PasswordPassesPolicyCheck(P_BP_JSON policy_in_out, char *password);

// Attempts to open the pass file and read it line by line, checking password.
BP_BOOL BP_PasswordExistsInFile(char *pass_file, char *password, size_t * line_number_in_out);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Check Password Against Cleartext %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns true if the salted password matches the plaintext provided
BP_BOOL BP_PasswordMatchesSaltedPlaintext
(
	char *plaintext_password,
	char *hash_algo,
	char *hash,
	char *salt
);

#endif /* BPSTRINGUTILITIES_H_ */

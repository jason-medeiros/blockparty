/*
 * BP-GeneralPurposeTemplateSubstitution.h
 *
 *  Created on: Dec 18, 2011
 *      Author: root
 */

#ifndef BPGENERALPURPOSETEMPLATESUBSTITUTION_H_
#define BPGENERALPURPOSETEMPLATESUBSTITUTION_H_


// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be freed).  This does not use any
// form of regular expression and performs a literal one to one match.
char * BP_GenerateSubstitutedTemplate(char *template_data, char **find_strs, char **replace_strs);

// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be bpfreed).  This function uses
// POSIX regular expressions.
char * BP_GenerateSubstitutedTemplateREGEXP(char *template_data, char **find_strs, char **replace_strs);

// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be bpfreed).  This function uses
// PCRE / Perl Compatible Regular Expressions.
char * BP_GenerateSubstitutedTemplatePCRE(char *template_data, char **find_strs, char **replace_strs);


#endif /* BPGENERALPURPOSETEMPLATESUBSTITUTION_H_ */

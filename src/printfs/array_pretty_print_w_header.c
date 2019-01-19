#include "libs_support.h"
#include "util.h"
#include <string.h>


/*-----------------------------------------------------------------------------------------------

	Print 'src' into 'out' as HexASCII, 16 bytes per line, enclosed in "[]". 'prefix' in front.

	If the printout will inside 'maxLine' then print one one line; else drop after 'prefix'
	and print 16 bytes per line.

	Multi-line prints can be indented by appending the indent to 'prefix'. E.g
	'A_Prefix:\r\n    " prints:

		"A_Prefix:
		     [0x00 01 02 03 04....
			    10 11 12 13 14...
				20 21 22 23 24...     ]"

	If not all of 'src' will fit in 'out' then print as much as will fit.
*/
PUBLIC C8 const * ArrayPrettyPrint(C8 *out, U16 outBufLen, C8 const *prefix, U8 const *src, size_t len, U16 maxLine)
{
	C8 const empty[] = "";
	if(prefix == NULL) { prefix = empty; }						// No 'prefix' supplied? => make it "".

	/* If 'prefix' contains '\r\n' then '\r\n.... ' is our indent. Else it's '\r\n' if whole print
	   is too long for one line; else just spaces.
	*/
	C8 const *indent;
	if( (indent = strstr(prefix, "\r\n")) == NULL) {			// No '\r\n' in 'prefix'?
		indent = (strlen(prefix) + 5 + (3 * len)) > maxLine		// then, Too long to print one one line?
						? "\r\n"								// then 16 HexACSII bytes per line.
						: "   "; }								// else prefix and HexASCII on one line.

	// Print something if 'src' and 'out' exist and there's room for at least 'prefix'.
	if(src != NULL && out != NULL && outBufLen >= strlen(prefix)+10) {
		strcpy(out, prefix);
		strcat(out, "[0x");										// First byte is printed '0xnn' to remind us it's Hex.
		C8 *p = out + strlen(out);

		U16 i; for(i = 0; i < len; i++, src++) {
			if(outBufLen < p-out+10)							// Look ahead. is there room to add another HexASCII digit?
                { break; }										// No! then add no more
																// if yes, then add digit, preceded by newline if necessary

            if(i > 0 && (i % 0x10 == 0))                        // Start next 16 bytes?
                { p += sprintf(p, "%s  %02x ", indent, *src); }	// then print indent then number
            else                                                // else print just number.
                { p += sprintf(p, "%02x ", *src); }}			// else print just number.
		strcat(p, "]\r\n"); }									// Closing ']'
	return out;
}

// ===================================== eof ==========================================================

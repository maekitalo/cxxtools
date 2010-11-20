/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <cxxtools/xml/entityresolver.h>
#include <stdexcept>

namespace cxxtools
{

namespace xml
{

EntityResolver::EntityResolver()
{
    clear();
}


EntityResolver::~EntityResolver()
{
}


void EntityResolver::clear()
{
    _entityMap.clear();

    _entityMap.insert(EntityMap::value_type(L"Acirc", String(1, Char(0x00C2))));
    _entityMap.insert(EntityMap::value_type(L"acirc", String(1, Char(0x00E2))));
    _entityMap.insert(EntityMap::value_type(L"acute", String(1, Char(0x00B4))));
    _entityMap.insert(EntityMap::value_type(L"AElig", String(1, Char(0x00C6))));
    _entityMap.insert(EntityMap::value_type(L"aelig", String(1, Char(0x00E6))));
    _entityMap.insert(EntityMap::value_type(L"Agrave", String(1, Char(0x00C0))));
    _entityMap.insert(EntityMap::value_type(L"agrave", String(1, Char(0x00E0))));
    _entityMap.insert(EntityMap::value_type(L"alefsym", String(1, Char(0x2135))));
    _entityMap.insert(EntityMap::value_type(L"Alpha", String(1, Char(0x0391))));
    _entityMap.insert(EntityMap::value_type(L"alpha", String(1, Char(0x03B1))));
    _entityMap.insert(EntityMap::value_type(L"amp", String(1, Char(0x0026))));
    _entityMap.insert(EntityMap::value_type(L"and", String(1, Char(0x2227))));
    _entityMap.insert(EntityMap::value_type(L"ang", String(1, Char(0x2220))));
    _entityMap.insert(EntityMap::value_type(L"apos", String(1, Char(0x0027))));
    _entityMap.insert(EntityMap::value_type(L"Aring", String(1, Char(0x00C5))));
    _entityMap.insert(EntityMap::value_type(L"aring", String(1, Char(0x00E5))));
    _entityMap.insert(EntityMap::value_type(L"asymp", String(1, Char(0x2248))));
    _entityMap.insert(EntityMap::value_type(L"Atilde", String(1, Char(0x00C3))));
    _entityMap.insert(EntityMap::value_type(L"atilde", String(1, Char(0x00E3))));
    _entityMap.insert(EntityMap::value_type(L"Auml", String(1, Char(0x00C4))));
    _entityMap.insert(EntityMap::value_type(L"auml", String(1, Char(0x00E4))));
    _entityMap.insert(EntityMap::value_type(L"bdquo", String(1, Char(0x201E))));
    _entityMap.insert(EntityMap::value_type(L"Beta", String(1, Char(0x0392))));
    _entityMap.insert(EntityMap::value_type(L"beta", String(1, Char(0x03B2))));
    _entityMap.insert(EntityMap::value_type(L"brvbar", String(1, Char(0x00A6))));
    _entityMap.insert(EntityMap::value_type(L"bull", String(1, Char(0x2022))));
    _entityMap.insert(EntityMap::value_type(L"cap", String(1, Char(0x2229))));
    _entityMap.insert(EntityMap::value_type(L"Ccedil", String(1, Char(0x00C7))));
    _entityMap.insert(EntityMap::value_type(L"ccedil", String(1, Char(0x00E7))));
    _entityMap.insert(EntityMap::value_type(L"cedil", String(1, Char(0x00B8))));
    _entityMap.insert(EntityMap::value_type(L"cent", String(1, Char(0x00A2))));
    _entityMap.insert(EntityMap::value_type(L"Chi", String(1, Char(0x03A7))));
    _entityMap.insert(EntityMap::value_type(L"chi", String(1, Char(0x03C7))));
    _entityMap.insert(EntityMap::value_type(L"circ", String(1, Char(0x02C6))));
    _entityMap.insert(EntityMap::value_type(L"clubs", String(1, Char(0x2663))));
    _entityMap.insert(EntityMap::value_type(L"cong", String(1, Char(0x2245))));
    _entityMap.insert(EntityMap::value_type(L"copy", String(1, Char(0x00A9))));
    _entityMap.insert(EntityMap::value_type(L"crarr", String(1, Char(0x21B5))));
    _entityMap.insert(EntityMap::value_type(L"cup", String(1, Char(0x222A))));
    _entityMap.insert(EntityMap::value_type(L"curren", String(1, Char(0x00A4))));
    _entityMap.insert(EntityMap::value_type(L"Dagger", String(1, Char(0x2021))));
    _entityMap.insert(EntityMap::value_type(L"dagger", String(1, Char(0x2020))));
    _entityMap.insert(EntityMap::value_type(L"dArr", String(1, Char(0x21D3))));
    _entityMap.insert(EntityMap::value_type(L"darr", String(1, Char(0x2193))));
    _entityMap.insert(EntityMap::value_type(L"deg", String(1, Char(0x00B0))));
    _entityMap.insert(EntityMap::value_type(L"Delta", String(1, Char(0x0394))));
    _entityMap.insert(EntityMap::value_type(L"delta", String(1, Char(0x03B4))));
    _entityMap.insert(EntityMap::value_type(L"diams", String(1, Char(0x2666))));
    _entityMap.insert(EntityMap::value_type(L"divide", String(1, Char(0x00F7))));
    _entityMap.insert(EntityMap::value_type(L"Eacute", String(1, Char(0x00C9))));
    _entityMap.insert(EntityMap::value_type(L"eacute", String(1, Char(0x00E9))));
    _entityMap.insert(EntityMap::value_type(L"Ecirc", String(1, Char(0x00CA))));
    _entityMap.insert(EntityMap::value_type(L"ecirc", String(1, Char(0x00EA))));
    _entityMap.insert(EntityMap::value_type(L"Egrave", String(1, Char(0x00C8))));
    _entityMap.insert(EntityMap::value_type(L"egrave", String(1, Char(0x00E8))));
    _entityMap.insert(EntityMap::value_type(L"empty", String(1, Char(0x2205))));
    _entityMap.insert(EntityMap::value_type(L"emsp", String(1, Char(0x2003))));
    _entityMap.insert(EntityMap::value_type(L"ensp", String(1, Char(0x2002))));
    _entityMap.insert(EntityMap::value_type(L"Epsilon", String(1, Char(0x0395))));
    _entityMap.insert(EntityMap::value_type(L"epsilon", String(1, Char(0x03B5))));
    _entityMap.insert(EntityMap::value_type(L"equiv", String(1, Char(0x2261))));
    _entityMap.insert(EntityMap::value_type(L"Eta", String(1, Char(0x0397))));
    _entityMap.insert(EntityMap::value_type(L"eta", String(1, Char(0x03B7))));
    _entityMap.insert(EntityMap::value_type(L"ETH", String(1, Char(0x00D0))));
    _entityMap.insert(EntityMap::value_type(L"eth", String(1, Char(0x00F0))));
    _entityMap.insert(EntityMap::value_type(L"Euml", String(1, Char(0x00CB))));
    _entityMap.insert(EntityMap::value_type(L"euml", String(1, Char(0x00EB))));
    _entityMap.insert(EntityMap::value_type(L"euro", String(1, Char(0x20AC))));
    _entityMap.insert(EntityMap::value_type(L"exist", String(1, Char(0x2203))));
    _entityMap.insert(EntityMap::value_type(L"fnof", String(1, Char(0x0192))));
    _entityMap.insert(EntityMap::value_type(L"forall", String(1, Char(0x2200))));
    _entityMap.insert(EntityMap::value_type(L"frac12", String(1, Char(0x00BD))));
    _entityMap.insert(EntityMap::value_type(L"frac14", String(1, Char(0x00BC))));
    _entityMap.insert(EntityMap::value_type(L"frac34", String(1, Char(0x00BE))));
    _entityMap.insert(EntityMap::value_type(L"frasl", String(1, Char(0x2044))));
    _entityMap.insert(EntityMap::value_type(L"Gamma", String(1, Char(0x0393))));
    _entityMap.insert(EntityMap::value_type(L"gamma", String(1, Char(0x03B3))));
    _entityMap.insert(EntityMap::value_type(L"ge", String(1, Char(0x2265))));
    _entityMap.insert(EntityMap::value_type(L"gt", String(1, Char(0x003E))));
    _entityMap.insert(EntityMap::value_type(L"hArr", String(1, Char(0x21D4))));
    _entityMap.insert(EntityMap::value_type(L"harr", String(1, Char(0x2194))));
    _entityMap.insert(EntityMap::value_type(L"hearts", String(1, Char(0x2665))));
    _entityMap.insert(EntityMap::value_type(L"hellip", String(1, Char(0x2026))));
    _entityMap.insert(EntityMap::value_type(L"Iacute", String(1, Char(0x00CD))));
    _entityMap.insert(EntityMap::value_type(L"iacute", String(1, Char(0x00ED))));
    _entityMap.insert(EntityMap::value_type(L"Icirc", String(1, Char(0x00CE))));
    _entityMap.insert(EntityMap::value_type(L"icirc", String(1, Char(0x00EE))));
    _entityMap.insert(EntityMap::value_type(L"iexcl", String(1, Char(0x00A1))));
    _entityMap.insert(EntityMap::value_type(L"Igrave", String(1, Char(0x00CC))));
    _entityMap.insert(EntityMap::value_type(L"igrave", String(1, Char(0x00EC))));
    _entityMap.insert(EntityMap::value_type(L"image", String(1, Char(0x2111))));
    _entityMap.insert(EntityMap::value_type(L"infin", String(1, Char(0x221E))));
    _entityMap.insert(EntityMap::value_type(L"int", String(1, Char(0x222B))));
    _entityMap.insert(EntityMap::value_type(L"Iota", String(1, Char(0x0399))));
    _entityMap.insert(EntityMap::value_type(L"iota", String(1, Char(0x03B9))));
    _entityMap.insert(EntityMap::value_type(L"iquest", String(1, Char(0x00BF))));
    _entityMap.insert(EntityMap::value_type(L"isin", String(1, Char(0x2208))));
    _entityMap.insert(EntityMap::value_type(L"Iuml", String(1, Char(0x00CF))));
    _entityMap.insert(EntityMap::value_type(L"iuml", String(1, Char(0x00EF))));
    _entityMap.insert(EntityMap::value_type(L"Kappa", String(1, Char(0x039A))));
    _entityMap.insert(EntityMap::value_type(L"kappa", String(1, Char(0x03BA))));
    _entityMap.insert(EntityMap::value_type(L"Lambda", String(1, Char(0x039B))));
    _entityMap.insert(EntityMap::value_type(L"lambda", String(1, Char(0x03BB))));
    _entityMap.insert(EntityMap::value_type(L"lang", String(1, Char(0x2329))));
    _entityMap.insert(EntityMap::value_type(L"laquo", String(1, Char(0x00AB))));
    _entityMap.insert(EntityMap::value_type(L"lArr", String(1, Char(0x21D0))));
    _entityMap.insert(EntityMap::value_type(L"larr", String(1, Char(0x2190))));
    _entityMap.insert(EntityMap::value_type(L"lceil", String(1, Char(0x2308))));
    _entityMap.insert(EntityMap::value_type(L"ldquo", String(1, Char(0x201C))));
    _entityMap.insert(EntityMap::value_type(L"le", String(1, Char(0x2264))));
    _entityMap.insert(EntityMap::value_type(L"lfloor", String(1, Char(0x230A))));
    _entityMap.insert(EntityMap::value_type(L"lowast", String(1, Char(0x2217))));
    _entityMap.insert(EntityMap::value_type(L"loz", String(1, Char(0x25CA))));
    _entityMap.insert(EntityMap::value_type(L"lrm", String(1, Char(0x200E))));
    _entityMap.insert(EntityMap::value_type(L"lsaquo", String(1, Char(0x2039))));
    _entityMap.insert(EntityMap::value_type(L"lsquo", String(1, Char(0x2018))));
    _entityMap.insert(EntityMap::value_type(L"lt", String(1, Char(0x003C))));
    _entityMap.insert(EntityMap::value_type(L"macr", String(1, Char(0x00AF))));
    _entityMap.insert(EntityMap::value_type(L"mdash", String(1, Char(0x2014))));
    _entityMap.insert(EntityMap::value_type(L"micro", String(1, Char(0x00B5))));
    _entityMap.insert(EntityMap::value_type(L"middot", String(1, Char(0x00B7))));
    _entityMap.insert(EntityMap::value_type(L"minus", String(1, Char(0x2212))));
    _entityMap.insert(EntityMap::value_type(L"Mu", String(1, Char(0x039C))));
    _entityMap.insert(EntityMap::value_type(L"mu", String(1, Char(0x03BC))));
    _entityMap.insert(EntityMap::value_type(L"nabla", String(1, Char(0x2207))));
    _entityMap.insert(EntityMap::value_type(L"nbsp", String(1, Char(0x00A0))));
    _entityMap.insert(EntityMap::value_type(L"ndash", String(1, Char(0x2013))));
    _entityMap.insert(EntityMap::value_type(L"ne", String(1, Char(0x2260))));
    _entityMap.insert(EntityMap::value_type(L"ni", String(1, Char(0x220B))));
    _entityMap.insert(EntityMap::value_type(L"not", String(1, Char(0x00AC))));
    _entityMap.insert(EntityMap::value_type(L"notin", String(1, Char(0x2209))));
    _entityMap.insert(EntityMap::value_type(L"nsub", String(1, Char(0x2284))));
    _entityMap.insert(EntityMap::value_type(L"Ntilde", String(1, Char(0x00D1))));
    _entityMap.insert(EntityMap::value_type(L"ntilde", String(1, Char(0x00F1))));
    _entityMap.insert(EntityMap::value_type(L"Nu", String(1, Char(0x039D))));
    _entityMap.insert(EntityMap::value_type(L"nu", String(1, Char(0x03BD))));
    _entityMap.insert(EntityMap::value_type(L"Oacute", String(1, Char(0x00D3))));
    _entityMap.insert(EntityMap::value_type(L"oacute", String(1, Char(0x00F3))));
    _entityMap.insert(EntityMap::value_type(L"Ocirc", String(1, Char(0x00D4))));
    _entityMap.insert(EntityMap::value_type(L"ocirc", String(1, Char(0x00F4))));
    _entityMap.insert(EntityMap::value_type(L"OElig", String(1, Char(0x0152))));
    _entityMap.insert(EntityMap::value_type(L"oelig", String(1, Char(0x0153))));
    _entityMap.insert(EntityMap::value_type(L"Ograve", String(1, Char(0x00D2))));
    _entityMap.insert(EntityMap::value_type(L"ograve", String(1, Char(0x00F2))));
    _entityMap.insert(EntityMap::value_type(L"oline", String(1, Char(0x203E))));
    _entityMap.insert(EntityMap::value_type(L"Omega", String(1, Char(0x03A9))));
    _entityMap.insert(EntityMap::value_type(L"omega", String(1, Char(0x03C9))));
    _entityMap.insert(EntityMap::value_type(L"Omicron", String(1, Char(0x039F))));
    _entityMap.insert(EntityMap::value_type(L"omicron", String(1, Char(0x03BF))));
    _entityMap.insert(EntityMap::value_type(L"oplus", String(1, Char(0x2295))));
    _entityMap.insert(EntityMap::value_type(L"or", String(1, Char(0x2228))));
    _entityMap.insert(EntityMap::value_type(L"ordf", String(1, Char(0x00AA))));
    _entityMap.insert(EntityMap::value_type(L"ordm", String(1, Char(0x00BA))));
    _entityMap.insert(EntityMap::value_type(L"Oslash", String(1, Char(0x00D8))));
    _entityMap.insert(EntityMap::value_type(L"oslash", String(1, Char(0x00F8))));
    _entityMap.insert(EntityMap::value_type(L"Otilde", String(1, Char(0x00D5))));
    _entityMap.insert(EntityMap::value_type(L"otilde", String(1, Char(0x00F5))));
    _entityMap.insert(EntityMap::value_type(L"otimes", String(1, Char(0x2297))));
    _entityMap.insert(EntityMap::value_type(L"Ouml", String(1, Char(0x00D6))));
    _entityMap.insert(EntityMap::value_type(L"ouml", String(1, Char(0x00F6))));
    _entityMap.insert(EntityMap::value_type(L"para", String(1, Char(0x00B6))));
    _entityMap.insert(EntityMap::value_type(L"part", String(1, Char(0x2202))));
    _entityMap.insert(EntityMap::value_type(L"permil", String(1, Char(0x2030))));
    _entityMap.insert(EntityMap::value_type(L"perp", String(1, Char(0x22A5))));
    _entityMap.insert(EntityMap::value_type(L"Phi", String(1, Char(0x03A6))));
    _entityMap.insert(EntityMap::value_type(L"phi", String(1, Char(0x03C6))));
    _entityMap.insert(EntityMap::value_type(L"Pi", String(1, Char(0x03A0))));
    _entityMap.insert(EntityMap::value_type(L"pi", String(1, Char(0x03C0))));
    _entityMap.insert(EntityMap::value_type(L"piv", String(1, Char(0x03D6))));
    _entityMap.insert(EntityMap::value_type(L"plusmn", String(1, Char(0x00B1))));
    _entityMap.insert(EntityMap::value_type(L"pound", String(1, Char(0x00A3))));
    _entityMap.insert(EntityMap::value_type(L"Prime", String(1, Char(0x2033))));
    _entityMap.insert(EntityMap::value_type(L"prime", String(1, Char(0x2032))));
    _entityMap.insert(EntityMap::value_type(L"prod", String(1, Char(0x220F))));
    _entityMap.insert(EntityMap::value_type(L"prop", String(1, Char(0x221D))));
    _entityMap.insert(EntityMap::value_type(L"Psi", String(1, Char(0x03A8))));
    _entityMap.insert(EntityMap::value_type(L"psi", String(1, Char(0x03C8))));
    _entityMap.insert(EntityMap::value_type(L"quot", String(1, Char(0x0022))));
    _entityMap.insert(EntityMap::value_type(L"radic", String(1, Char(0x221A))));
    _entityMap.insert(EntityMap::value_type(L"rang", String(1, Char(0x232A))));
    _entityMap.insert(EntityMap::value_type(L"raquo", String(1, Char(0x00BB))));
    _entityMap.insert(EntityMap::value_type(L"rArr", String(1, Char(0x21D2))));
    _entityMap.insert(EntityMap::value_type(L"rarr", String(1, Char(0x2192))));
    _entityMap.insert(EntityMap::value_type(L"rceil", String(1, Char(0x2309))));
    _entityMap.insert(EntityMap::value_type(L"rdquo", String(1, Char(0x201D))));
    _entityMap.insert(EntityMap::value_type(L"real", String(1, Char(0x211C))));
    _entityMap.insert(EntityMap::value_type(L"reg", String(1, Char(0x00AE))));
    _entityMap.insert(EntityMap::value_type(L"rfloor", String(1, Char(0x230B))));
    _entityMap.insert(EntityMap::value_type(L"Rho", String(1, Char(0x03A1))));
    _entityMap.insert(EntityMap::value_type(L"rho", String(1, Char(0x03C1))));
    _entityMap.insert(EntityMap::value_type(L"rlm", String(1, Char(0x200F))));
    _entityMap.insert(EntityMap::value_type(L"rsaquo", String(1, Char(0x203A))));
    _entityMap.insert(EntityMap::value_type(L"rsquo", String(1, Char(0x2019))));
    _entityMap.insert(EntityMap::value_type(L"sbquo", String(1, Char(0x201A))));
    _entityMap.insert(EntityMap::value_type(L"Scaron", String(1, Char(0x0160))));
    _entityMap.insert(EntityMap::value_type(L"scaron", String(1, Char(0x0161))));
    _entityMap.insert(EntityMap::value_type(L"sdot", String(1, Char(0x22C5))));
    _entityMap.insert(EntityMap::value_type(L"sect", String(1, Char(0x00A7))));
    _entityMap.insert(EntityMap::value_type(L"shy", String(1, Char(0x00AD))));
    _entityMap.insert(EntityMap::value_type(L"Sigma", String(1, Char(0x03A3))));
    _entityMap.insert(EntityMap::value_type(L"sigma", String(1, Char(0x03C3))));
    _entityMap.insert(EntityMap::value_type(L"sigmaf", String(1, Char(0x03C2))));
    _entityMap.insert(EntityMap::value_type(L"sim", String(1, Char(0x223C))));
    _entityMap.insert(EntityMap::value_type(L"spades", String(1, Char(0x2660))));
    _entityMap.insert(EntityMap::value_type(L"sub", String(1, Char(0x2282))));
    _entityMap.insert(EntityMap::value_type(L"sube", String(1, Char(0x2286))));
    _entityMap.insert(EntityMap::value_type(L"sum", String(1, Char(0x2211))));
    _entityMap.insert(EntityMap::value_type(L"sup", String(1, Char(0x2283))));
    _entityMap.insert(EntityMap::value_type(L"sup1", String(1, Char(0x00B9))));
    _entityMap.insert(EntityMap::value_type(L"sup2", String(1, Char(0x00B2))));
    _entityMap.insert(EntityMap::value_type(L"sup3", String(1, Char(0x00B3))));
    _entityMap.insert(EntityMap::value_type(L"supe", String(1, Char(0x2287))));
    _entityMap.insert(EntityMap::value_type(L"szlig", String(1, Char(0x00DF))));
    _entityMap.insert(EntityMap::value_type(L"Tau", String(1, Char(0x03A4))));
    _entityMap.insert(EntityMap::value_type(L"tau", String(1, Char(0x03C4))));
    _entityMap.insert(EntityMap::value_type(L"there4", String(1, Char(0x2234))));
    _entityMap.insert(EntityMap::value_type(L"Theta", String(1, Char(0x0398))));
    _entityMap.insert(EntityMap::value_type(L"theta", String(1, Char(0x03B8))));
    _entityMap.insert(EntityMap::value_type(L"thetasym", String(1, Char(0x03D1))));
    _entityMap.insert(EntityMap::value_type(L"thinsp", String(1, Char(0x2009))));
    _entityMap.insert(EntityMap::value_type(L"THORN", String(1, Char(0x00DE))));
    _entityMap.insert(EntityMap::value_type(L"thorn", String(1, Char(0x00FE))));
    _entityMap.insert(EntityMap::value_type(L"tilde", String(1, Char(0x02DC))));
    _entityMap.insert(EntityMap::value_type(L"times", String(1, Char(0x00D7))));
    _entityMap.insert(EntityMap::value_type(L"trade", String(1, Char(0x2122))));
    _entityMap.insert(EntityMap::value_type(L"Uacute", String(1, Char(0x00DA))));
    _entityMap.insert(EntityMap::value_type(L"uacute", String(1, Char(0x00FA))));
    _entityMap.insert(EntityMap::value_type(L"uArr", String(1, Char(0x21D1))));
    _entityMap.insert(EntityMap::value_type(L"uarr", String(1, Char(0x2191))));
    _entityMap.insert(EntityMap::value_type(L"Ucirc", String(1, Char(0x00DB))));
    _entityMap.insert(EntityMap::value_type(L"ucirc", String(1, Char(0x00FB))));
    _entityMap.insert(EntityMap::value_type(L"Ugrave", String(1, Char(0x00D9))));
    _entityMap.insert(EntityMap::value_type(L"ugrave", String(1, Char(0x00F9))));
    _entityMap.insert(EntityMap::value_type(L"uml", String(1, Char(0x00A8))));
    _entityMap.insert(EntityMap::value_type(L"upsih", String(1, Char(0x03D2))));
    _entityMap.insert(EntityMap::value_type(L"Upsilon", String(1, Char(0x03A5))));
    _entityMap.insert(EntityMap::value_type(L"upsilon", String(1, Char(0x03C5))));
    _entityMap.insert(EntityMap::value_type(L"Uuml", String(1, Char(0x00DC))));
    _entityMap.insert(EntityMap::value_type(L"uuml", String(1, Char(0x00FC))));
    _entityMap.insert(EntityMap::value_type(L"weierp", String(1, Char(0x2118))));
    _entityMap.insert(EntityMap::value_type(L"Xi", String(1, Char(0x039E))));
    _entityMap.insert(EntityMap::value_type(L"xi", String(1, Char(0x03BE))));
    _entityMap.insert(EntityMap::value_type(L"Yacute", String(1, Char(0x00DD))));
    _entityMap.insert(EntityMap::value_type(L"yacute", String(1, Char(0x00FD))));
    _entityMap.insert(EntityMap::value_type(L"yen", String(1, Char(0x00A5))));
    _entityMap.insert(EntityMap::value_type(L"Yuml", String(1, Char(0x0178))));
    _entityMap.insert(EntityMap::value_type(L"yuml", String(1, Char(0x00FF))));
    _entityMap.insert(EntityMap::value_type(L"Zeta", String(1, Char(0x0396))));
    _entityMap.insert(EntityMap::value_type(L"zeta", String(1, Char(0x03B6))));
    _entityMap.insert(EntityMap::value_type(L"zwj", String(1, Char(0x200D))));
    _entityMap.insert(EntityMap::value_type(L"zwnj", String(1, Char(0x200C))));
}


void EntityResolver::addEntity(const String& entity, const String& token)
{
  _entityMap.insert( std::pair<String, String>(entity, token) );
}


String EntityResolver::resolveEntity(const String& entity)
{
    if (!entity.empty() && entity[0] == L'#')
    {
        int code = 0;
        if (entity.size() > 2 && entity[1] == L'x')
        {
            // hex notation: &#xxxx;
            for (String::const_iterator it = entity.begin() + 2; it != entity.end(); ++it)
            {
                if (*it >= L'0' && *it <= L'9')
                    code = code * 16 + (it->value() - L'0');
                else if (*it >= L'A' && *it <= L'F')
                    code = code * 16 + (it->value() - L'A' + 10);
                else if (*it >= L'a' && *it <= L'f')
                    code = code * 16 + (it->value() - L'a' + 10);
                else
                    throw std::runtime_error(std::string("invalid entity ") + entity.narrow());
            }
        }
        else
        {
            // dec notation: &9999;
            for (String::const_iterator it = entity.begin() + 1; it != entity.end(); ++it)
            {
                if (*it >= L'0' && *it <= L'9')
                    code = code * 10 + (it->value() - '0');
                else
                    throw std::runtime_error(std::string("invalid entity ") + entity.narrow());
            }
        }

        return String( 1, Char(code) );
    }

    std::map<String, String>::const_iterator it = _entityMap.find(entity);
    if( it == _entityMap.end() )
    {
        throw std::runtime_error("invalid entity " + entity.narrow());
    }

    return it->second;
}

} // namespace xml

} // namespace cxxtools

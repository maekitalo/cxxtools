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
#include <cxxtools/stringstream.h>
#include <stdexcept>

namespace cxxtools
{

namespace xml
{

namespace
{
  struct Ent
  {
    const wchar_t* entity;
    cxxtools::uint32_t charValue;
  };

  static const Ent ent[] = {
    { L"AElig", 0x00C6 },
    { L"Acirc", 0x00C2 },
    { L"Agrave", 0x00C0 },
    { L"Alpha", 0x0391 },
    { L"Aring", 0x00C5 },
    { L"Atilde", 0x00C3 },
    { L"Auml", 0x00C4 },
    { L"Beta", 0x0392 },
    { L"Ccedil", 0x00C7 },
    { L"Chi", 0x03A7 },
    { L"Dagger", 0x2021 },
    { L"Delta", 0x0394 },
    { L"ETH", 0x00D0 },
    { L"Eacute", 0x00C9 },
    { L"Ecirc", 0x00CA },
    { L"Egrave", 0x00C8 },
    { L"Epsilon", 0x0395 },
    { L"Eta", 0x0397 },
    { L"Euml", 0x00CB },
    { L"Gamma", 0x0393 },
    { L"Iacute", 0x00CD },
    { L"Icirc", 0x00CE },
    { L"Igrave", 0x00CC },
    { L"Iota", 0x0399 },
    { L"Iuml", 0x00CF },
    { L"Kappa", 0x039A },
    { L"Lambda", 0x039B },
    { L"Mu", 0x039C },
    { L"Ntilde", 0x00D1 },
    { L"Nu", 0x039D },
    { L"OElig", 0x0152 },
    { L"Oacute", 0x00D3 },
    { L"Ocirc", 0x00D4 },
    { L"Ograve", 0x00D2 },
    { L"Omega", 0x03A9 },
    { L"Omicron", 0x039F },
    { L"Oslash", 0x00D8 },
    { L"Otilde", 0x00D5 },
    { L"Ouml", 0x00D6 },
    { L"Phi", 0x03A6 },
    { L"Pi", 0x03A0 },
    { L"Prime", 0x2033 },
    { L"Psi", 0x03A8 },
    { L"Rho", 0x03A1 },
    { L"Scaron", 0x0160 },
    { L"Sigma", 0x03A3 },
    { L"THORN", 0x00DE },
    { L"Tau", 0x03A4 },
    { L"Theta", 0x0398 },
    { L"Uacute", 0x00DA },
    { L"Ucirc", 0x00DB },
    { L"Ugrave", 0x00D9 },
    { L"Upsilon", 0x03A5 },
    { L"Uuml", 0x00DC },
    { L"Xi", 0x039E },
    { L"Yacute", 0x00DD },
    { L"Yuml", 0x0178 },
    { L"Zeta", 0x0396 },
    { L"acirc", 0x00E2 },
    { L"acute", 0x00B4 },
    { L"aelig", 0x00E6 },
    { L"agrave", 0x00E0 },
    { L"alefsym", 0x2135 },
    { L"alpha", 0x03B1 },
    { L"amp", 0x0026 },
    { L"and", 0x2227 },
    { L"ang", 0x2220 },
    { L"apos", 0x0027 },
    { L"aring", 0x00E5 },
    { L"asymp", 0x2248 },
    { L"atilde", 0x00E3 },
    { L"auml", 0x00E4 },
    { L"bdquo", 0x201E },
    { L"beta", 0x03B2 },
    { L"brvbar", 0x00A6 },
    { L"bull", 0x2022 },
    { L"cap", 0x2229 },
    { L"ccedil", 0x00E7 },
    { L"cedil", 0x00B8 },
    { L"cent", 0x00A2 },
    { L"chi", 0x03C7 },
    { L"circ", 0x02C6 },
    { L"clubs", 0x2663 },
    { L"cong", 0x2245 },
    { L"copy", 0x00A9 },
    { L"crarr", 0x21B5 },
    { L"cup", 0x222A },
    { L"curren", 0x00A4 },
    { L"dArr", 0x21D3 },
    { L"dagger", 0x2020 },
    { L"darr", 0x2193 },
    { L"deg", 0x00B0 },
    { L"delta", 0x03B4 },
    { L"diams", 0x2666 },
    { L"divide", 0x00F7 },
    { L"eacute", 0x00E9 },
    { L"ecirc", 0x00EA },
    { L"egrave", 0x00E8 },
    { L"empty", 0x2205 },
    { L"emsp", 0x2003 },
    { L"ensp", 0x2002 },
    { L"epsilon", 0x03B5 },
    { L"equiv", 0x2261 },
    { L"eta", 0x03B7 },
    { L"eth", 0x00F0 },
    { L"euml", 0x00EB },
    { L"euro", 0x20AC },
    { L"exist", 0x2203 },
    { L"fnof", 0x0192 },
    { L"forall", 0x2200 },
    { L"frac12", 0x00BD },
    { L"frac14", 0x00BC },
    { L"frac34", 0x00BE },
    { L"frasl", 0x2044 },
    { L"gamma", 0x03B3 },
    { L"ge", 0x2265 },
    { L"gt", 0x003E },
    { L"hArr", 0x21D4 },
    { L"harr", 0x2194 },
    { L"hearts", 0x2665 },
    { L"hellip", 0x2026 },
    { L"iacute", 0x00ED },
    { L"icirc", 0x00EE },
    { L"iexcl", 0x00A1 },
    { L"igrave", 0x00EC },
    { L"image", 0x2111 },
    { L"infin", 0x221E },
    { L"int", 0x222B },
    { L"iota", 0x03B9 },
    { L"iquest", 0x00BF },
    { L"isin", 0x2208 },
    { L"iuml", 0x00EF },
    { L"kappa", 0x03BA },
    { L"lArr", 0x21D0 },
    { L"lambda", 0x03BB },
    { L"lang", 0x2329 },
    { L"laquo", 0x00AB },
    { L"larr", 0x2190 },
    { L"lceil", 0x2308 },
    { L"ldquo", 0x201C },
    { L"le", 0x2264 },
    { L"lfloor", 0x230A },
    { L"lowast", 0x2217 },
    { L"loz", 0x25CA },
    { L"lrm", 0x200E },
    { L"lsaquo", 0x2039 },
    { L"lsquo", 0x2018 },
    { L"lt", 0x003C },
    { L"macr", 0x00AF },
    { L"mdash", 0x2014 },
    { L"micro", 0x00B5 },
    { L"middot", 0x00B7 },
    { L"minus", 0x2212 },
    { L"mu", 0x03BC },
    { L"nabla", 0x2207 },
    { L"nbsp", 0x00A0 },
    { L"ndash", 0x2013 },
    { L"ne", 0x2260 },
    { L"ni", 0x220B },
    { L"not", 0x00AC },
    { L"notin", 0x2209 },
    { L"nsub", 0x2284 },
    { L"ntilde", 0x00F1 },
    { L"nu", 0x03BD },
    { L"oacute", 0x00F3 },
    { L"ocirc", 0x00F4 },
    { L"oelig", 0x0153 },
    { L"ograve", 0x00F2 },
    { L"oline", 0x203E },
    { L"omega", 0x03C9 },
    { L"omicron", 0x03BF },
    { L"oplus", 0x2295 },
    { L"or", 0x2228 },
    { L"ordf", 0x00AA },
    { L"ordm", 0x00BA },
    { L"oslash", 0x00F8 },
    { L"otilde", 0x00F5 },
    { L"otimes", 0x2297 },
    { L"ouml", 0x00F6 },
    { L"para", 0x00B6 },
    { L"part", 0x2202 },
    { L"permil", 0x2030 },
    { L"perp", 0x22A5 },
    { L"phi", 0x03C6 },
    { L"pi", 0x03C0 },
    { L"piv", 0x03D6 },
    { L"plusmn", 0x00B1 },
    { L"pound", 0x00A3 },
    { L"prime", 0x2032 },
    { L"prod", 0x220F },
    { L"prop", 0x221D },
    { L"psi", 0x03C8 },
    { L"quot", 0x0022 },
    { L"rArr", 0x21D2 },
    { L"radic", 0x221A },
    { L"rang", 0x232A },
    { L"raquo", 0x00BB },
    { L"rarr", 0x2192 },
    { L"rceil", 0x2309 },
    { L"rdquo", 0x201D },
    { L"real", 0x211C },
    { L"reg", 0x00AE },
    { L"rfloor", 0x230B },
    { L"rho", 0x03C1 },
    { L"rlm", 0x200F },
    { L"rsaquo", 0x203A },
    { L"rsquo", 0x2019 },
    { L"sbquo", 0x201A },
    { L"scaron", 0x0161 },
    { L"sdot", 0x22C5 },
    { L"sect", 0x00A7 },
    { L"shy", 0x00AD },
    { L"sigma", 0x03C3 },
    { L"sigmaf", 0x03C2 },
    { L"sim", 0x223C },
    { L"spades", 0x2660 },
    { L"sub", 0x2282 },
    { L"sube", 0x2286 },
    { L"sum", 0x2211 },
    { L"sup", 0x2283 },
    { L"sup1", 0x00B9 },
    { L"sup2", 0x00B2 },
    { L"sup3", 0x00B3 },
    { L"supe", 0x2287 },
    { L"szlig", 0x00DF },
    { L"tau", 0x03C4 },
    { L"there4", 0x2234 },
    { L"theta", 0x03B8 },
    { L"thetasym", 0x03D1 },
    { L"thinsp", 0x2009 },
    { L"thorn", 0x00FE },
    { L"tilde", 0x02DC },
    { L"times", 0x00D7 },
    { L"trade", 0x2122 },
    { L"uArr", 0x21D1 },
    { L"uacute", 0x00FA },
    { L"uarr", 0x2191 },
    { L"ucirc", 0x00FB },
    { L"ugrave", 0x00F9 },
    { L"uml", 0x00A8 },
    { L"upsih", 0x03D2 },
    { L"upsilon", 0x03C5 },
    { L"uuml", 0x00FC },
    { L"weierp", 0x2118 },
    { L"xi", 0x03BE },
    { L"yacute", 0x00FD },
    { L"yen", 0x00A5 },
    { L"yuml", 0x00FF },
    { L"zeta", 0x03B6 },
    { L"zwj", 0x200D },
    { L"zwnj", 0x200C }
  };

  static const Ent rent[] = {
    { L"quot", 0x0022 },
    { L"amp", 0x0026 },
    { L"apos", 0x0027 },
    { L"lt", 0x003C },
    { L"gt", 0x003E },
    { L"nbsp", 0x00A0 },
    { L"iexcl", 0x00A1 },
    { L"cent", 0x00A2 },
    { L"pound", 0x00A3 },
    { L"curren", 0x00A4 },
    { L"yen", 0x00A5 },
    { L"brvbar", 0x00A6 },
    { L"sect", 0x00A7 },
    { L"uml", 0x00A8 },
    { L"copy", 0x00A9 },
    { L"ordf", 0x00AA },
    { L"laquo", 0x00AB },
    { L"not", 0x00AC },
    { L"shy", 0x00AD },
    { L"reg", 0x00AE },
    { L"macr", 0x00AF },
    { L"deg", 0x00B0 },
    { L"plusmn", 0x00B1 },
    { L"sup2", 0x00B2 },
    { L"sup3", 0x00B3 },
    { L"acute", 0x00B4 },
    { L"micro", 0x00B5 },
    { L"para", 0x00B6 },
    { L"middot", 0x00B7 },
    { L"cedil", 0x00B8 },
    { L"sup1", 0x00B9 },
    { L"ordm", 0x00BA },
    { L"raquo", 0x00BB },
    { L"frac14", 0x00BC },
    { L"frac12", 0x00BD },
    { L"frac34", 0x00BE },
    { L"iquest", 0x00BF },
    { L"Agrave", 0x00C0 },
    { L"Acirc", 0x00C2 },
    { L"Atilde", 0x00C3 },
    { L"Auml", 0x00C4 },
    { L"Aring", 0x00C5 },
    { L"AElig", 0x00C6 },
    { L"Ccedil", 0x00C7 },
    { L"Egrave", 0x00C8 },
    { L"Eacute", 0x00C9 },
    { L"Ecirc", 0x00CA },
    { L"Euml", 0x00CB },
    { L"Igrave", 0x00CC },
    { L"Iacute", 0x00CD },
    { L"Icirc", 0x00CE },
    { L"Iuml", 0x00CF },
    { L"ETH", 0x00D0 },
    { L"Ntilde", 0x00D1 },
    { L"Ograve", 0x00D2 },
    { L"Oacute", 0x00D3 },
    { L"Ocirc", 0x00D4 },
    { L"Otilde", 0x00D5 },
    { L"Ouml", 0x00D6 },
    { L"times", 0x00D7 },
    { L"Oslash", 0x00D8 },
    { L"Ugrave", 0x00D9 },
    { L"Uacute", 0x00DA },
    { L"Ucirc", 0x00DB },
    { L"Uuml", 0x00DC },
    { L"Yacute", 0x00DD },
    { L"THORN", 0x00DE },
    { L"szlig", 0x00DF },
    { L"agrave", 0x00E0 },
    { L"acirc", 0x00E2 },
    { L"atilde", 0x00E3 },
    { L"auml", 0x00E4 },
    { L"aring", 0x00E5 },
    { L"aelig", 0x00E6 },
    { L"ccedil", 0x00E7 },
    { L"egrave", 0x00E8 },
    { L"eacute", 0x00E9 },
    { L"ecirc", 0x00EA },
    { L"euml", 0x00EB },
    { L"igrave", 0x00EC },
    { L"iacute", 0x00ED },
    { L"icirc", 0x00EE },
    { L"iuml", 0x00EF },
    { L"eth", 0x00F0 },
    { L"ntilde", 0x00F1 },
    { L"ograve", 0x00F2 },
    { L"oacute", 0x00F3 },
    { L"ocirc", 0x00F4 },
    { L"otilde", 0x00F5 },
    { L"ouml", 0x00F6 },
    { L"divide", 0x00F7 },
    { L"oslash", 0x00F8 },
    { L"ugrave", 0x00F9 },
    { L"uacute", 0x00FA },
    { L"ucirc", 0x00FB },
    { L"uuml", 0x00FC },
    { L"yacute", 0x00FD },
    { L"thorn", 0x00FE },
    { L"yuml", 0x00FF },
    { L"OElig", 0x0152 },
    { L"oelig", 0x0153 },
    { L"Scaron", 0x0160 },
    { L"scaron", 0x0161 },
    { L"Yuml", 0x0178 },
    { L"fnof", 0x0192 },
    { L"circ", 0x02C6 },
    { L"tilde", 0x02DC },
    { L"Alpha", 0x0391 },
    { L"Beta", 0x0392 },
    { L"Gamma", 0x0393 },
    { L"Delta", 0x0394 },
    { L"Epsilon", 0x0395 },
    { L"Zeta", 0x0396 },
    { L"Eta", 0x0397 },
    { L"Theta", 0x0398 },
    { L"Iota", 0x0399 },
    { L"Kappa", 0x039A },
    { L"Lambda", 0x039B },
    { L"Mu", 0x039C },
    { L"Nu", 0x039D },
    { L"Xi", 0x039E },
    { L"Omicron", 0x039F },
    { L"Pi", 0x03A0 },
    { L"Rho", 0x03A1 },
    { L"Sigma", 0x03A3 },
    { L"Tau", 0x03A4 },
    { L"Upsilon", 0x03A5 },
    { L"Phi", 0x03A6 },
    { L"Chi", 0x03A7 },
    { L"Psi", 0x03A8 },
    { L"Omega", 0x03A9 },
    { L"alpha", 0x03B1 },
    { L"beta", 0x03B2 },
    { L"gamma", 0x03B3 },
    { L"delta", 0x03B4 },
    { L"epsilon", 0x03B5 },
    { L"zeta", 0x03B6 },
    { L"eta", 0x03B7 },
    { L"theta", 0x03B8 },
    { L"iota", 0x03B9 },
    { L"kappa", 0x03BA },
    { L"lambda", 0x03BB },
    { L"mu", 0x03BC },
    { L"nu", 0x03BD },
    { L"xi", 0x03BE },
    { L"omicron", 0x03BF },
    { L"pi", 0x03C0 },
    { L"rho", 0x03C1 },
    { L"sigmaf", 0x03C2 },
    { L"sigma", 0x03C3 },
    { L"tau", 0x03C4 },
    { L"upsilon", 0x03C5 },
    { L"phi", 0x03C6 },
    { L"chi", 0x03C7 },
    { L"psi", 0x03C8 },
    { L"omega", 0x03C9 },
    { L"thetasym", 0x03D1 },
    { L"upsih", 0x03D2 },
    { L"piv", 0x03D6 },
    { L"ensp", 0x2002 },
    { L"emsp", 0x2003 },
    { L"thinsp", 0x2009 },
    { L"zwnj", 0x200C },
    { L"zwj", 0x200D },
    { L"lrm", 0x200E },
    { L"rlm", 0x200F },
    { L"ndash", 0x2013 },
    { L"mdash", 0x2014 },
    { L"lsquo", 0x2018 },
    { L"rsquo", 0x2019 },
    { L"sbquo", 0x201A },
    { L"ldquo", 0x201C },
    { L"rdquo", 0x201D },
    { L"bdquo", 0x201E },
    { L"dagger", 0x2020 },
    { L"Dagger", 0x2021 },
    { L"bull", 0x2022 },
    { L"hellip", 0x2026 },
    { L"permil", 0x2030 },
    { L"prime", 0x2032 },
    { L"Prime", 0x2033 },
    { L"lsaquo", 0x2039 },
    { L"rsaquo", 0x203A },
    { L"oline", 0x203E },
    { L"frasl", 0x2044 },
    { L"euro", 0x20AC },
    { L"image", 0x2111 },
    { L"weierp", 0x2118 },
    { L"real", 0x211C },
    { L"trade", 0x2122 },
    { L"alefsym", 0x2135 },
    { L"larr", 0x2190 },
    { L"uarr", 0x2191 },
    { L"rarr", 0x2192 },
    { L"darr", 0x2193 },
    { L"harr", 0x2194 },
    { L"crarr", 0x21B5 },
    { L"lArr", 0x21D0 },
    { L"uArr", 0x21D1 },
    { L"rArr", 0x21D2 },
    { L"dArr", 0x21D3 },
    { L"hArr", 0x21D4 },
    { L"forall", 0x2200 },
    { L"part", 0x2202 },
    { L"exist", 0x2203 },
    { L"empty", 0x2205 },
    { L"nabla", 0x2207 },
    { L"isin", 0x2208 },
    { L"notin", 0x2209 },
    { L"ni", 0x220B },
    { L"prod", 0x220F },
    { L"sum", 0x2211 },
    { L"minus", 0x2212 },
    { L"lowast", 0x2217 },
    { L"radic", 0x221A },
    { L"prop", 0x221D },
    { L"infin", 0x221E },
    { L"ang", 0x2220 },
    { L"and", 0x2227 },
    { L"or", 0x2228 },
    { L"cap", 0x2229 },
    { L"cup", 0x222A },
    { L"int", 0x222B },
    { L"there4", 0x2234 },
    { L"sim", 0x223C },
    { L"cong", 0x2245 },
    { L"asymp", 0x2248 },
    { L"ne", 0x2260 },
    { L"equiv", 0x2261 },
    { L"le", 0x2264 },
    { L"ge", 0x2265 },
    { L"sub", 0x2282 },
    { L"sup", 0x2283 },
    { L"nsub", 0x2284 },
    { L"sube", 0x2286 },
    { L"supe", 0x2287 },
    { L"oplus", 0x2295 },
    { L"otimes", 0x2297 },
    { L"perp", 0x22A5 },
    { L"sdot", 0x22C5 },
    { L"lceil", 0x2308 },
    { L"rceil", 0x2309 },
    { L"lfloor", 0x230A },
    { L"rfloor", 0x230B },
    { L"lang", 0x2329 },
    { L"rang", 0x232A },
    { L"loz", 0x25CA },
    { L"spades", 0x2660 },
    { L"clubs", 0x2663 },
    { L"hearts", 0x2665 },
    { L"diams", 0x2666 }
  };

  void printEntity(std::basic_ostream<Char>& os, const wchar_t* p)
  {
      os << Char('&');
      while (*p)
          os << Char(*p++);
      os << Char(';');
  }
}

EntityResolver::EntityResolver()
{
}


void EntityResolver::clear()
{
    _entityMap.clear();
}


void EntityResolver::addEntity(const String& entity, const String& token)
{
    _entityMap.insert( std::pair<String, String>(entity, token) );
}


String EntityResolver::resolveEntity(const String& entity) const
{
    if (!entity.empty() && entity[0] == L'#')
    {
        int code = 0;
        if (entity.size() > 2 && entity[1] == L'x')
        {
            // hex notation: &#xABCD;
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
            // dec notation: &#9999;
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

    unsigned u = 0;
    unsigned o = sizeof(ent)/sizeof(Ent) - 1;
    while (o - u > 1)
    {
        unsigned m = (o + u) / 2;
        int c = entity.compare(ent[m].entity);
        if (c == 0)
            return String(1, Char(ent[m].charValue));
        else if (c < 0)
            o = m;
        else
            u = m;
    }

    if (entity.compare(ent[u].entity) == 0)
        return String(1, Char(ent[u].charValue));

    if (entity.compare(ent[o].entity) == 0)
        return String(1, Char(ent[o].charValue));

    EntityMap::const_iterator it = _entityMap.find(entity);
    if( it == _entityMap.end() )
    {
        it = _entityMap.find(entity);
        if( it == _entityMap.end() )
            throw std::runtime_error("invalid entity " + entity.narrow());
    }

    return it->second;
}


String EntityResolver::getEntity(Char ch) const
{
    OStringStream s;
    getEntity(s, ch);
    return s.str();
}


void EntityResolver::getEntity(std::basic_ostream<Char>& os, Char ch) const
{
    unsigned u = 0;
    unsigned o = sizeof(rent)/sizeof(Ent) - 1;
    while (o - u > 1)
    {
        unsigned m = (o + u) / 2;
        if (rent[m].charValue == ch.value())
        {
            printEntity(os, rent[m].entity);
            return;
        }

        if (ch.value() < rent[m].charValue)
            o = m;
        else
            u = m;
    }

    if (rent[u].charValue == ch.value())
        printEntity(os, rent[u].entity);
    else if (rent[o].charValue == ch.value())
        printEntity(os, rent[o].entity);
    else if (ch.value() >= ' ' && ch.value() <= 0x7F)
        os << ch;
    else
        os << Char('&') << Char('#') << ch.value() << Char(';');
}


} // namespace xml

} // namespace cxxtools

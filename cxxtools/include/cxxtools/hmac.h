/*
 * Copyright (C) 2011 by Julian Wiesener
 * 
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

/* HMAC algorithm as described by RFC 2104
 * http://tools.ietf.org/html/rfc2104
 */

#ifndef CXXTOOLS_HMAC_H
#define CXXTOOLS_HMAC_H

#include <sstream>
#include <algorithm>

namespace cxxtools
{

template <typename Algo>
void hmac_pad_key(Algo& key_hash, const std::string& key, std::string& o_key_pad, std::string& i_key_pad)
{ 
	std::string _key(key);

	if(key.length() > Algo::blockSize)
		_key = key_hash.getDigest();
	_key.resize(Algo::blockSize);

	std::string::const_iterator key_it = _key.begin();
	for(std::string::iterator pad_it = o_key_pad.begin();
			pad_it != o_key_pad.end(); pad_it++)
		*pad_it = *pad_it ^ *key_it++;

	key_it = _key.begin();
	for(std::string::iterator pad_it = i_key_pad.begin();
			pad_it != i_key_pad.end(); pad_it++)
		*pad_it = *pad_it ^ *key_it++;
}

template <typename Algo, typename data_type>
std::string hmac(const std::string& key, const data_type& msg)
{ 
	Algo key_hash(key);
	std::string o_key_pad(Algo::blockSize, 0x5c);
	std::string i_key_pad(Algo::blockSize, 0x36);

	cxxtools::hmac_pad_key(key_hash, key, o_key_pad, i_key_pad);

	Algo inner_hash(i_key_pad + msg);
	Algo outer_hash(o_key_pad + inner_hash.getDigest());
	return outer_hash.getHexDigest();
}

template <typename Algo, typename iterator_type>
std::string hmac(const std::string& key, 
		iterator_type from, iterator_type to)
{ 
	Algo key_hash(key);
	std::string o_key_pad(Algo::blockSize, 0x5c);
	std::string i_key_pad(Algo::blockSize, 0x36);
	std::stringstream is;

	cxxtools::hmac_pad_key(key_hash, key, o_key_pad, i_key_pad);

	std::copy(from, to, std::ostreambuf_iterator<char>(is));

	Algo inner_hash(i_key_pad + is.str());
	Algo outer_hash(o_key_pad + inner_hash.getDigest());
	return outer_hash.getHexDigest();
}

}

#endif // CXXTOOLS_HMAC_H

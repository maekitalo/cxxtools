/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <cxxtools/bin/parser.h>
#include <cxxtools/bin/serializer.h>
#include <cxxtools/bin/deserializer.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>

#include <streambuf>
#include <sstream>
#include <math.h>

log_define("cxxtools.bin.parser")

namespace cxxtools
{
namespace bin
{
namespace
{
    const char* typeName(unsigned char typeCode)
    {
        switch (typeCode)
        {
            case Serializer::TypeEmpty:
            case Serializer::TypePlainEmpty: return "";
            case Serializer::TypeBool:
            case Serializer::TypePlainBool: return "bool";
            case Serializer::TypeChar:
            case Serializer::TypePlainChar: return "char";
            case Serializer::TypeString:
            case Serializer::TypePlainString: return "string";
            case Serializer::TypeInt:
            case Serializer::TypePlainInt:
            case Serializer::TypeInt8:
            case Serializer::TypePlainInt8:
            case Serializer::TypeInt16:
            case Serializer::TypePlainInt16:
            case Serializer::TypeInt32:
            case Serializer::TypePlainInt32:
            case Serializer::TypeInt64:
            case Serializer::TypePlainInt64:
            case Serializer::TypeUInt8:
            case Serializer::TypePlainUInt8:
            case Serializer::TypeUInt16:
            case Serializer::TypePlainUInt16:
            case Serializer::TypeUInt32:
            case Serializer::TypePlainUInt32:
            case Serializer::TypeUInt64:
            case Serializer::TypePlainUInt64: return "int";
            case Serializer::TypeBinary2:
            case Serializer::TypePlainBinary2:
            case Serializer::TypeBinary4:
            case Serializer::TypePlainBinary4: return "binary";
            case Serializer::TypeShortFloat:
            case Serializer::TypePlainShortFloat:
            case Serializer::TypeMediumFloat:
            case Serializer::TypePlainMediumFloat:
            case Serializer::TypeLongFloat:
            case Serializer::TypePlainLongFloat:
            case Serializer::TypeBcdFloat:
            case Serializer::TypePlainBcdFloat: return "double";
            case Serializer::TypePair:
            case Serializer::TypePlainPair: return "pair";
            case Serializer::TypeArray:
            case Serializer::TypePlainArray: return "array";
            case Serializer::TypeList:
            case Serializer::TypePlainList: return "list";
            case Serializer::TypeDeque:
            case Serializer::TypePlainDeque: return "deque";
            case Serializer::TypeSet:
            case Serializer::TypePlainSet: return "set";
            case Serializer::TypeMultiset:
            case Serializer::TypePlainMultiset: return "multiset";
            case Serializer::TypeMap:
            case Serializer::TypePlainMap: return "map";
            case Serializer::TypeMultimap:
            case Serializer::TypePlainMultimap: return "multimap";
            default:
            {
                std::ostringstream msg;
                msg << "unknown serialization type code <" << std::hex << static_cast<unsigned>(static_cast<unsigned char>(typeCode)) << '>';
                SerializationError::doThrow(msg.str());
            }
        }
        return 0;  // never reached
    }

    static const char bcdDigits[16] = "0123456789+-.: ";
}

void Parser::begin(Deserializer& handler, bool resetDictionary)
{
    _deserializer = &handler;
    _state = state_type;
    _nextstate = state_type;
    _int = 0;
    _exp = 0;
    _token.clear();
    delete _next;
    _next = 0;

    if (resetDictionary)
        _mydictionary.clear();
}

void Parser::finish()
{
    _deserializer = 0;
    _token.clear();
    delete _next;
    _next = 0;
}

void Parser::skip()
{
    _state = state_type;
    _deserializer = 0;
    _int = 0;
    _exp = 0;
    _token.clear();
}


bool Parser::advance(std::streambuf& in, bool atLeastOne)
{
    while (in.in_avail() || atLeastOne)
    {
        char ch = std::streambuf::traits_type::to_char_type(in.sgetc());

        log_finest(static_cast<void*>(this) << " advance " << std::hex << static_cast<unsigned>(static_cast<unsigned char>(ch)) << std::dec << " state " << _state << " nextstate " << _nextstate);
        switch (_state)
        {
            case state_type:
                {
                    Serializer::TypeCode tc = static_cast<Serializer::TypeCode>(static_cast<unsigned char>(ch));
                    if (tc == Serializer::CategoryObject)
                    {
                        _nextstate = state_object_type;
                        _state = state_name;
                        if (_deserializer)
                            _deserializer->setCategory(SerializationInfo::Object);
                    }
                    else if (tc == Serializer::CategoryArray)
                    {
                        _nextstate = state_array_type;
                        _state = state_name;
                        if (_deserializer)
                            _deserializer->setCategory(SerializationInfo::Array);
                    }
                    else if (tc == Serializer::TypeOther)
                    {
                        log_debug("type other");
                        _nextstate = state_name;
                        _state = state_value_type_other;
                    }
                    else if (tc == Serializer::TypePlainOther)
                    {
                        log_debug("type plain other");
                        _nextstate = state_value_value;
                        _state = state_value_type_other;
                    }
                    else if (tc == Serializer::TypeBcd)
                    {
                        log_debug("type bcd");
                        _nextstate = state_name;
                        _state = state_value_type_bcd;
                    }
                    else if (tc == Serializer::TypePlainBcd)
                    {
                        log_debug("type plain bcd");
                        _nextstate = state_value_bcd0;
                        _state = state_value_type_bcd;
                    }
                    else
                    {
                        log_debug("type code " << std::hex << tc << " => type " << typeName(ch));
                        if (_deserializer)
                        {
                            _deserializer->setTypeName(typeName(ch));
                            _deserializer->setCategory(SerializationInfo::Value);
                        }

                        switch (tc)
                        {
                            case Serializer::TypeEmpty:
                                if (_deserializer)
                                    _deserializer->setNull();
                                _nextstate = state_end;
                                _state = state_name;
                                break;

                            case Serializer::TypeChar:
                                _nextstate = state_value_char;
                                _state = state_name;
                                break;

                            case Serializer::TypeString:
                            case Serializer::TypeInt:
                                _nextstate = state_value_value;
                                _state = state_name;
                                break;

                            case Serializer::TypeBool:
                                _nextstate = state_value_bool;
                                _state = state_name;
                                break;

                            case Serializer::TypeBinary2:
                                _count = 2;
                                _nextstate = state_value_binary_length;
                                _state = state_name;
                                break;

                            case Serializer::TypeBinary4:
                                _count = 4;
                                _nextstate = state_value_binary_length;
                                _state = state_name;
                                break;

                            case Serializer::TypeInt8:
                                _count = 1;
                                _nextstate = state_value_intsign;
                                _state = state_name;
                                break;

                            case Serializer::TypeInt16:
                                _count = 2;
                                _nextstate = state_value_intsign;
                                _state = state_name;
                                break;

                            case Serializer::TypeInt32:
                                _count = 4;
                                _nextstate = state_value_intsign;
                                _state = state_name;
                                break;

                            case Serializer::TypeInt64:
                                _count = 8;
                                _nextstate = state_value_intsign;
                                _state = state_name;
                                break;

                            case Serializer::TypeUInt8:
                                _count = 1;
                                _nextstate = state_value_uint;
                                _state = state_name;
                                break;

                            case Serializer::TypeUInt16:
                                _count = 2;
                                _nextstate = state_value_uint;
                                _state = state_name;
                                break;

                            case Serializer::TypeUInt32:
                                _count = 4;
                                _nextstate = state_value_uint;
                                _state = state_name;
                                break;

                            case Serializer::TypeUInt64:
                                _count = 8;
                                _nextstate = state_value_uint;
                                _state = state_name;
                                break;

                            case Serializer::TypeShortFloat:
                                _nextstate = state_sfloat_exp;
                                _count = 1;
                                _state = state_name;
                                break;

                            case Serializer::TypeMediumFloat:
                                _nextstate = state_mfloat_exp;
                                _count = 1;
                                _state = state_name;
                                break;

                            case Serializer::TypeLongFloat:
                                _nextstate = state_lfloat_exp;
                                _count = 2;
                                _state = state_name;
                                break;

                            case Serializer::TypeBcdFloat:
                                _nextstate = state_value_bcd0;
                                _state = state_name;
                                break;

                            case Serializer::TypeArray:
                            case Serializer::TypeVector:
                            case Serializer::TypeList:
                            case Serializer::TypeDeque:
                            case Serializer::TypeSet:
                            case Serializer::TypeMultiset:
                                _state = state_name;
                                _nextstate = state_array_member;
                                if (_deserializer)
                                    _deserializer->setCategory(SerializationInfo::Array);
                                break;

                            case Serializer::TypePair:
                            case Serializer::TypeMap:
                            case Serializer::TypeMultimap:
                                _nextstate = state_object_member;
                                _state = state_name;
                                if (_deserializer)
                                    _deserializer->setCategory(SerializationInfo::Object);
                                break;

                            case Serializer::TypePlainEmpty:
                                if (_deserializer)
                                    _deserializer->setNull();
                                _state = state_end;
                                break;

                            case Serializer::TypePlainChar:
                                _state = state_value_char;
                                break;

                            case Serializer::TypePlainString:
                            case Serializer::TypePlainInt:
                                _state = state_value_value;
                                break;

                            case Serializer::TypePlainBool:
                                _state = state_value_bool;
                                break;

                            case Serializer::TypePlainBcdFloat:
                                _state = state_value_bcd0;
                                break;

                            case Serializer::TypePlainBinary2:
                                _count = 2;
                                _state = state_value_binary_length;
                                break;

                            case Serializer::TypePlainBinary4:
                                _count = 4;
                                _state = state_value_binary_length;
                                break;

                            case Serializer::TypePlainInt8:
                                _count = 1;
                                _state = state_value_intsign;
                                break;

                            case Serializer::TypePlainInt16:
                                _count = 2;
                                _state = state_value_intsign;
                                break;

                            case Serializer::TypePlainInt32:
                                _count = 4;
                                _state = state_value_intsign;
                                break;

                            case Serializer::TypePlainInt64:
                                _count = 8;
                                _state = state_value_intsign;
                                break;

                            case Serializer::TypePlainUInt8:
                                _count = 1;
                                _state = state_value_uint;
                                break;

                            case Serializer::TypePlainUInt16:
                                _count = 2;
                                _state = state_value_uint;
                                break;

                            case Serializer::TypePlainUInt32:
                                _count = 4;
                                _state = state_value_uint;
                                break;

                            case Serializer::TypePlainUInt64:
                                _count = 8;
                                _state = state_value_uint;
                                break;

                            case Serializer::TypePlainShortFloat:
                                _state = state_sfloat_exp;
                                _count = 1;
                                break;

                            case Serializer::TypePlainMediumFloat:
                                _state = state_mfloat_exp;
                                _count = 1;
                                break;

                            case Serializer::TypePlainLongFloat:
                                _state = state_lfloat_exp;
                                _count = 2;
                                break;

                            case Serializer::TypePlainArray:
                            case Serializer::TypePlainVector:
                            case Serializer::TypePlainList:
                            case Serializer::TypePlainDeque:
                            case Serializer::TypePlainSet:
                            case Serializer::TypePlainMultiset:
                                _state = state_array_type;
                                if (_deserializer)
                                    _deserializer->setCategory(SerializationInfo::Array);
                                break;

                            case Serializer::TypePlainPair:
                            case Serializer::TypePlainMap:
                            case Serializer::TypePlainMultimap:
                                _state = state_object_type;
                                if (_deserializer)
                                    _deserializer->setCategory(SerializationInfo::Object);
                                break;

                            default:
                                {
                                    std::ostringstream msg;
                                    msg << "invalid type code <0x" << std::hex << tc << '>';
                                    SerializationError::doThrow(msg.str());
                                }
                        }
                    }

                    in.sbumpc();
                }
                break;

            case state_name:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    if (ch == '\0')
                    {
                        log_debug("name=" << _token);
                        if (_deserializer)
                            _deserializer->setName(_token);

                        dict(_token);
                        _token.clear();
                        _state = _nextstate;
                        break;
                    }
                    else if (_token.empty() && ch == '\1')
                    {
                        _state = state_name_idx0;
                        break;
                    }
                    else
                        _token += ch;
                }
                break;

            case state_name_idx0:
                _dictidx = static_cast<unsigned>(ch) << 8;
                _state = state_name_idx1;
                in.sbumpc();
                break;

            case state_name_idx1:
                _dictidx |= static_cast<unsigned>(ch);
                if (_dictidx >= _dictionary->size())
                {
                    log_error("invalid dictionary index " << _dictidx);
                    SerializationError::doThrow("invalid dictionary index");
                }

                log_debug("dictidx=" << _dictidx << " name=" << (*_dictionary)[_dictidx]);
                if (_deserializer)
                    _deserializer->setName((*_dictionary)[_dictidx]);
                _state = _nextstate;
                in.sbumpc();
                break;

            case state_value_type_other:
            case state_value_type_bcd:
                if (ch == '\0')
                {
                    log_debug("typename=" << _token);
                    if (_deserializer)
                        _deserializer->setTypeName(_token);

                    dict(_token);
                    _token.clear();
                    State nextstate = (_state == state_value_type_bcd ? state_value_bcd : state_value_value);
                    _state = _nextstate;
                    _nextstate = nextstate;
                }
                else if (_token.empty() && ch == '\1')
                {
                    _state = (_state == state_value_type_bcd ? state_value_type_bcd_idx0 : state_value_type_other_idx0);
                }
                else
                    _token += ch;
                in.sbumpc();
                break;

            case state_value_type_other_idx0:
            case state_value_type_bcd_idx0:
                _dictidx = static_cast<unsigned>(ch) << 8;
                _state = (_state == state_value_type_bcd_idx0 ? state_value_type_bcd_idx1 : state_value_type_other_idx1);
                in.sbumpc();
                break;

            case state_value_type_other_idx1:
            case state_value_type_bcd_idx1:
                _dictidx |= static_cast<unsigned>(ch);
                if (_dictidx >= _dictionary->size())
                {
                    log_error("invalid dictionary index " << _dictidx);
                    SerializationError::doThrow("invalid dictionary index");
                }

                log_debug("dictidx=" << _dictidx << " typename=" << (*_dictionary)[_dictidx]);
                if (_deserializer)
                    _deserializer->setTypeName((*_dictionary)[_dictidx]);

                {
                State nextstate = (_state == state_value_type_bcd_idx1 ? state_value_bcd : state_value_value);
                _state = _nextstate;
                _nextstate = nextstate;
                }
                in.sbumpc();
                break;

            case state_value_intsign:
                if (static_cast<signed char>(ch) < 0)
                    _int = static_cast<uint64_t>(-1);  // set all bits
                else
                    _int = 0;

                _state = state_value_int;
                // no break

            case state_value_int:
            case state_value_uint:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    _int = (_int << 8) | static_cast<unsigned char>(ch);
                    if (--_count == 0)
                    {
                        if (_deserializer)
                        {
                            if (_state == state_value_int)
                            {
                                Deserializer::int_type value = Deserializer::int_type(_int);
                                _deserializer->setValue(value);
                            }
                            else
                            {
                                Deserializer::unsigned_type value = Deserializer::unsigned_type(_int);
                                _deserializer->setValue(value);
                            }
                        }

                        _int = 0;
                        return true;
                    }
                }
                break;

            case state_value_bool:
                if (_deserializer)
                    _deserializer->setValue(ch != '\0');

                in.sbumpc();
                return true;

            case state_value_bcd0:
                if (ch == '\xf0')
                {
                    if (_deserializer)
                        _deserializer->setValue("nan");
                    _state = state_end;
                    in.sbumpc();
                    break;
                }
                else if (ch == '\xf1')
                {
                    if (_deserializer)
                        _deserializer->setValue("inf");
                    _state = state_end;
                    in.sbumpc();
                    break;
                }
                else if (ch == '\xf2')
                {
                    if (_deserializer)
                        _deserializer->setValue("-inf");
                    _state = state_end;
                    in.sbumpc();
                    break;
                }

                _state = state_value_bcd;

                // no break

            case state_value_bcd:
                if (ch == '\xff')
                {
                    if (_deserializer)
                        _deserializer->setValue(_token);
                    _token.clear();
                    in.sbumpc();
                    return true;
                }
                else
                {
                    _token += bcdDigits[static_cast<uint8_t>(ch) >> 4];
                    if ((ch & '\xf') == '\xf')
                    {
                        if (_deserializer)
                            _deserializer->setValue(_token);
                        _token.clear();
                        _state = state_end;
                    }
                    else
                    {
                        _token += bcdDigits[static_cast<uint8_t>(ch) & '\xf'];
                    }

                    in.sbumpc();
                }

                break;

            case state_value_binary_length:
                _int = (_int << 8) | static_cast<unsigned char>(ch);
                if (--_count == 0)
                {
                    _count = static_cast<unsigned>(_int);
                    _int = 0;
                    if (_count == 0)
                    {
                        if (_deserializer)
                            _deserializer->setValue(std::string());
                        _state = state_end;
                    }
                    else
                    {
                        _state = state_value_binary;
                    }
                }
                in.sbumpc();
                break;

            case state_value_binary:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());

                    if (_deserializer)
                        _token += ch;

                    if (--_count == 0)
                    {
                        if (_deserializer)
                            _deserializer->setValue(_token);
                        return true;
                    }
                }

                break;

            case state_value_value:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    if (ch == '\0')
                    {
                        if (_deserializer)
                            _deserializer->setValue(Utf8Codec::decode(_token));
                        _token.clear();
                        _state = state_end;
                        break;
                    }
                    else
                        _token += ch;
                }

                break;

            case state_value_char:
                ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                if (_deserializer)
                    _deserializer->setValue(ch);
                return true;

            case state_sfloat_exp:
                _isNeg = (ch & '\x80') != 0;
                _exp = ch & '\x7f';
                _state = state_sfloat_base;
                _count = 2;
                in.sbumpc();
                break;

            case state_mfloat_exp:
                _isNeg = (ch & '\x80') != 0;
                _exp = ch & '\x7f';
                _state = state_mfloat_base;
                _count = 4;
                in.sbumpc();
                break;

            case state_lfloat_exp:
                if (--_count == 1)
                {
                    _isNeg = (ch & '\x80') != 0;
                    _exp = ch & '\x7f';
                }
                else
                {
                    _exp = (_exp << 8) | static_cast<unsigned char>(ch);
                    _count = 8;
                    _state = state_lfloat_base;
                }
                in.sbumpc();
                break;

            case state_sfloat_base:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    if (processFloatBase(ch, 48, 63))
                        return true;
                }
                break;

            case state_mfloat_base:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    if (processFloatBase(ch, 32, 63))
                        return true;
                }
                break;

            case state_lfloat_base:
                while (in.in_avail() || atLeastOne)
                {
                    atLeastOne = false;
                    ch = std::streambuf::traits_type::to_char_type(in.sbumpc());
                    if (processFloatBase(ch, 0, 16383))
                        return true;
                }
                break;

            case state_object_type:
                if (static_cast<Serializer::TypeCode>(ch) == Serializer::TypePlainOther
                    || static_cast<Serializer::TypeCode>(ch) == Serializer::TypeOther)
                    _state = state_object_type_other;
                else
                {
                    if (_deserializer)
                        _deserializer->setTypeName(typeName(ch));
                    _state = state_object_member;
                }
                in.sbumpc();
                break;

            case state_object_type_other:
                if (ch == '\0')
                {
                    if (_deserializer)
                        _deserializer->setTypeName(_token);
                    dict(_token);
                    _token.clear();
                    _state = state_object_member;
                }
                else if (_token.empty() && ch == '\1')
                    _state = state_object_type_other_idx0;
                else
                    _token += ch;
                in.sbumpc();
                break;

            case state_object_type_other_idx0:
                _dictidx = static_cast<unsigned>(ch) << 8;
                _state = state_object_type_other_idx1;
                in.sbumpc();
                break;

            case state_object_type_other_idx1:
                _dictidx |= static_cast<unsigned>(ch);
                if (_dictidx >= _dictionary->size())
                {
                    log_error("invalid dictionary index " << _dictidx);
                    SerializationError::doThrow("invalid dictionary index");
                }

                log_debug("dictidx=" << _dictidx << " typename=" << (*_dictionary)[_dictidx]);
                if (_deserializer)
                    _deserializer->setTypeName((*_dictionary)[_dictidx]);

                _token.clear();
                _state = state_object_member;
                in.sbumpc();
                break;

            case state_object_member:
                if (ch == '\xff')
                {
                    in.sbumpc();
                    return true;
                }

                if (_next == 0)
                    _next = new Parser(_dictionary);

                if (_deserializer)
                {
                    _deserializer->beginMember(_token, "", SerializationInfo::Void);
                    _next->begin(*_deserializer);
                }
                else
                    _next->skip();

                _state = state_object_member_value;
                break;

            case state_object_member_value:
                if (_next->advance(in, atLeastOne))
                {
                    if (_deserializer)
                        _deserializer->leaveMember();
                    _state = state_object_member;
                }
                break;

            case state_array_type:
                {
                    Serializer::TypeCode tc = static_cast<Serializer::TypeCode>(static_cast<unsigned char>(ch));
                    if (tc == Serializer::TypeOther)
                    {
                        _state = state_name;
                        _nextstate = state_array_type_other;
                    }
                    else if (tc == Serializer::TypePlainOther)
                    {
                        _state = state_array_type_other;
                    }
                    else
                    {
                        if (_deserializer)
                            _deserializer->setTypeName(typeName(ch));
                        _state = state_array_member;
                    }

                    in.sbumpc();
                }
                break;

            case state_array_type_other:
                if (ch == '\0')
                {
                    if (_deserializer)
                        _deserializer->setTypeName(_token);
                    dict(_token);
                    _token.clear();
                    _state = state_array_member;
                }
                else if (_token.empty() && ch == '\1')
                    _state = state_array_type_other_idx0;
                else
                    _token += ch;

                in.sbumpc();
                break;

            case state_array_type_other_idx0:
                _dictidx = static_cast<unsigned>(ch) << 8;
                _state = state_array_type_other_idx1;
                in.sbumpc();
                break;

            case state_array_type_other_idx1:
                _dictidx |= static_cast<unsigned>(ch);
                if (_dictidx >= _dictionary->size())
                {
                    log_error("invalid dictionary index " << _dictidx);
                    SerializationError::doThrow("invalid dictionary index");
                }

                log_debug("dictidx=" << _dictidx << " typename=" << (*_dictionary)[_dictidx]);
                if (_deserializer)
                    _deserializer->setTypeName((*_dictionary)[_dictidx]);

                _state = state_array_member;
                in.sbumpc();
                break;

            case state_array_member:
                if (ch == '\xff')
                {
                    in.sbumpc();
                    return true;
                }

                if (_next == 0)
                    _next = new Parser(_dictionary);

                if (_deserializer)
                {
                    _deserializer->beginMember("", "", SerializationInfo::Void);
                    _next->begin(*_deserializer);
                }
                else
                {
                    _next->skip();
                }

                _state = state_array_member_value;
                // no break

            case state_array_member_value:
                if (_next->advance(in, atLeastOne))
                {
                    if (_deserializer)
                        _deserializer->leaveMember();
                    _state = state_array_member_value_next;
                }
                break;

            case state_array_member_value_next:
                if (ch == '\xff')
                {
                    in.sbumpc();
                    return true;
                }
                else
                {
                    if (_deserializer)
                    {
                        _deserializer->beginMember("", "", SerializationInfo::Void);
                        _next->begin(*_deserializer);
                    }
                    else
                    {
                        _next->skip();
                    }

                    _state = state_array_member_value;
                }
                break;

            case state_end:
                if (ch != '\xff')
                {
                    std::ostringstream msg;
                    msg << "end of value marker expected; got <0x" << std::hex << static_cast<unsigned>(static_cast<unsigned char>(ch)) << '>';
                    SerializationError::doThrow(msg.str());
                }
                in.sbumpc();
                log_debug("end of value");
                return true;
        }

        atLeastOne = false;
    }

    return false;
}

bool Parser::processFloatBase(char ch, unsigned shift, unsigned expOffset)
{
    _int = (_int << 8) | static_cast<unsigned char>(ch);
    if (--_count == 0)
    {
        _int <<= shift;
        long double v;

        if (expOffset == 63 && _exp == 0x7f)
        {
            log_debug("float: value is special");
            v = (_int == 0 ? _isNeg ? -std::numeric_limits<long double>::infinity()
                                    : std::numeric_limits<long double>::infinity()
               : std::numeric_limits<long double>::quiet_NaN());
        }
        else if (_exp == 0 && _int == 0)
        {
            log_debug("float: value is zero");
            v = 0.0;
        }
        else
        {
            long double ss = static_cast<long double>(_int)
                           / (static_cast<long double>(std::numeric_limits<uint64_t>::max()) + 1.0l)
                              / 2.0l + .5l;

            v = ldexp(ss, _exp - expOffset);
            if (_isNeg)
                v = -v;

            log_debug("float: s=" << ss << " man=" << std::hex << _int << std::dec << " exp=" << _exp << " isNeg=" << _isNeg << " value=" << v);
        }

        if (_deserializer)
            _deserializer->setValue(v);

        _int = 0;
        return true;
    }

    return false;
}

void Parser::dict(const std::string& value)
{
    if (value.empty())
        return;

    for (unsigned idx = 0; idx < _dictionary->size(); ++idx)
    {
        if ((*_dictionary)[idx] == value)
            return;
    }

    log_debug("add dictionary value \"" << value << "\" idx=" << _dictionary->size());
    _dictionary->push_back(value);
}

std::ostream& operator << (std::ostream& out, const Parser::StringBuffer& s)
{
    if (s._size > 0)
        out.write(s._buffer, s._size);
    return out;
}

void Parser::StringBuffer::extend()
{
    // allocate new
    auto newcapacity = _capacity * 2;
    if (newcapacity < _size)
        throw std::runtime_error("failed to extend buffer above " + std::to_string(_capacity) + " bytes");

    char* newbuffer = new char[newcapacity];
    std::memcpy(newbuffer, _buffer, _size);
    delete[] _dynBuffer;
    _dynBuffer = newbuffer;
    _buffer = _dynBuffer;
    _capacity = newcapacity;
}

}
}

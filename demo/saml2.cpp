/*
 * Copyright (C) 2017 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/xml/xmlwriter.h>
#include <cxxtools/datetime.h>

int main(int argc, char* argv[])
{
    try
    {
        cxxtools::xml::XmlWriter w(std::cout);
        cxxtools::xml::XmlWriter::Element assertion(w, L"saml:Assertion",
            {
                { L"xmlns:saml", L"urn:oasis:names:tc:SAML:2.0:assertion" },
                { L"xmlns:xs", L"http://www.w3.org/2001/XMLSchema" },
                { L"xmlns:xsi", L"http://www.w3.org/2001/XMLSchema-instance" },
                { L"ID", L"b07b804c-7c29-ea16-7300-4f3d6f7928ac" },
                { L"Version", L"2.0" },
                { L"IssueInstant", cxxtools::DateTime(2004, 12, 5, 9, 22, 5) }
            });

        {
            cxxtools::xml::XmlWriter::Element issuer(w, L"saml:Issuer");
            issuer.writeContent(L"https://idp.example.org/SAML2");
        }

        {
            cxxtools::xml::XmlWriter::Element issuer(w, L"ds:Signature",
                {
                    { L"xmlns:ds", L"http://www.w3.org/2000/09/xmldsig#" }
                });
            issuer.writeContent(L"...");
        }

        {
            cxxtools::xml::XmlWriter::Element subject(w, L"saml:Subject");
            {
                cxxtools::xml::XmlWriter::Element nameID(w, L"saml:NameID",
                    { { L"Format", L"urn:oasis:names:tc:SAML:2.0:nameid-format:transient" } });
                nameID.writeContent(L"3f7b3dcf-1674-4ecd-92c8-1544f346baf8");
            }

            cxxtools::xml::XmlWriter::Element sc(w, L"saml:SubjectConfirmation",
                { { L"Method", L"urn:oasis:names:tc:SAML:2.0:cm:bearer" } });
            cxxtools::xml::XmlWriter::Element cd(w, L"saml:SubjectConfirmationData",
                {
                    { L"InResponseTo", L"aaf23196-1773-2113-474a-fe114412ab72" },
                    { L"Recipient", L"https://sp.example.com/SAML2/SSO/POST" },
                    { L"NotOnOrAfter", cxxtools::DateTime(2004, 12, 5, 9, 27, 5) }
                });
        }

        {
            cxxtools::xml::XmlWriter::Element conditions(w, L"saml:Conditions",
                {
                    { L"NotBefore", cxxtools::DateTime(2004, 12, 5, 9, 17, 5) },
                    { L"NotOnOrAfter", cxxtools::DateTime(2004, 12, 5, 9, 27, 5) }
                });

            {
                cxxtools::xml::XmlWriter::Element ar(w, L"saml:AudienceRestriction");
                cxxtools::xml::XmlWriter::Element au(w, L"saml:Audience");
                au.writeContent(L"https://sp.example.com/SAML2");
            }
        }

        {
            cxxtools::xml::XmlWriter::Element as(w, L"saml:AuthnStatement",
                {
                    { L"AuthnInstant", cxxtools::DateTime(2004, 12, 5, 9, 22, 0) },
                    { L"SessionIndex", L"b07b804c-7c29-ea16-7300-4f3d6f7928ac" }
                });
            cxxtools::xml::XmlWriter::Element ac(w, L"saml:AuthnContext");
            cxxtools::xml::XmlWriter::Element cf(w, L"saml:AuthnContextClassRef");
            cf.writeContent(L"urn:oasis:names:tc:SAML:2.0:ac:classes:PasswordProtectedTransport");
        }
        {
            cxxtools::xml::XmlWriter::Element as(w, L"saml:AttributeStatement");
            cxxtools::xml::XmlWriter::Element at(w, L"saml:Attribute",
                {
                    { L"xmlns:x500", L"urn:oasis:names:tc:SAML:2.0:profiles:attribute:X500" },
                    { L"x500:Encoding", L"LDAP" },
                    { L"NameFormat", L"urn:oasis:names:tc:SAML:2.0:attrname-format:uri" },
                    { L"Name", L"urn:oid:1.3.6.1.4.1.5923.1.1.1.1" },
                    { L"FriendlyName", L"eduPersonAffiliation" }
                });
            {
                cxxtools::xml::XmlWriter::Element av(w, L"saml:AttributeValue",
                    {
                        { L"xsi:type", L"xs:string" }
                    });
                av.writeContent(L"member");
            }
            {
                cxxtools::xml::XmlWriter::Element av(w, L"saml:AttributeValue",
                    {
                        { L"xsi:type", L"xs:string" }
                    });
                av.writeContent(L"staff");
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


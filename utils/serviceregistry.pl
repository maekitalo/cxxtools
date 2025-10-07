#!/usr/bin/perl -w

=head1 NAME

serviceregistry.pl

=head1 SYNOPSIS

serviceregistry.pl -n 8

=head1 DESCRIPTION

Generates template class ServiceRegistry with variable number of arguments.

=head1 OPTIONS

=over
=item
=item
=back

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use Getopt::Std;

my %opt;

getopts('n:', \%opt);

my $N = $opt{n} || 10;

########################################################################
## proloque
##
print <<EOF;
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

#ifndef CXXTOOLS_SERVICEREGISTRY_H
#define CXXTOOLS_SERVICEREGISTRY_H

#include <cxxtools/serviceprocedure.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace cxxtools
{
    class ServiceRegistry
    {
        public:
            ServiceRegistry() = default;

            template <typename R>
            void registerFunction(const std::string& name, R (*fn)())
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(fn));
            }

            template <typename F, typename R>
            void registerFunction(const std::string& name, const F<R()>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(fn));
            }

EOF

########################################################################
## registerFunction
##
for (my $i = 1; $i <= $N; ++$i)
{

    my $tparams = join ', ', map { "A$_" } (1..$i);
    my $ttparams = join ', ', map { "typename A$_" } (1..$i);

print <<EOF;
            template <typename R, $ttparams>
            void registerFunction(const std::string& name, R (*fn)($tparams))
            {
                this->registerProcedure(name, BasicServiceProcedure<R, $tparams>(fn));
            }

            template <typename F, typename R, $ttparams>
            void registerFunction(const std::string& name, const F<R($tparams)>& fn)
            {
                this->registerProcedure(name, BasicServiceProcedure<R, $tparams>(fn));
            }

EOF
}

########################################################################
## registerMethod
##
print <<EOF;
            template <typename R, class C>
            void registerMethod(const std::string& name, C& obj, R (C::*method)() )
            {
                this->registerProcedure(name, BasicServiceProcedure<R>(std::bind(method, &obj)));
            }

EOF
for (my $i = 1; $i <= $N; ++$i)
{

    my $tparams = join ', ', map { "A$_" } (1..$i);
    my $ttparams = join ', ', map { "typename A$_" } (1..$i);
    my $placeholders = join ', ', map { "std::placeholders::_$_" } (1..$i);

print <<EOF;
            template <typename R, class C, $ttparams>
            void registerMethod(const std::string& name, C& obj, R (C::*method)($tparams) )
            {
                this->registerProcedure(name, BasicServiceProcedure<R, $tparams>(std::bind(method, &obj, $placeholders)));
            }

EOF
}

########################################################################
## finalize
##
print <<EOF;

           std::unique_ptr<ServiceProcedure> getProcedure(const std::string& name) const;

            std::vector<std::string> getProcedureNames() const;

            void registerProcedure(const std::string& name, const ServiceProcedure& proc);
            void registerDefaultProcedure(const ServiceProcedure& proc)
                { _defaultProcedure = std::unique_ptr<ServiceProcedure>(proc.clone()); }

        private:
            typedef std::map<std::string, std::unique_ptr<ServiceProcedure>> ProcedureMap;
            std::unique_ptr<ServiceProcedure> _defaultProcedure;
            ProcedureMap _procedures;
    };

}

#endif // CXXTOOLS_SERVICEREGISTRY_H

EOF

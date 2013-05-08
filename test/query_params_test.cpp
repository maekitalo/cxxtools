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
////////////////////////////////////////////////////////////////////////
// query_params_test.cpp
//

#include <cxxtools/query_params.h>
#include <assert.h>
#include <iostream>

void func(query_params& q);
void test_qq(query_params& p);

int main(int argc, char* argv)
{
  query_params p;

  p.add("p1", "wert1");

  query_params q(&p);
  q.add("p2", "wert2");

  std::cout << "p=" << &p << " - " << p.getUrl() << std::endl;
  std::cout << "q=" << &q << " - " << q.getUrl() << std::endl;

  std::cout << "test q" << std::endl;
  assert(q.has("p1"));
  assert(q.has("p2"));
  assert(!q.has("p3"));

  func(q);

  // jetzt hat p die Werte p1 und p3, q hat p1, p2 und p3
  std::cout << "test p" << std::endl;
  assert(p.has("p1"));
  assert(!p.has("p2"));
  assert(p.has("p3"));

  std::cout << "test p" << std::endl;
  assert(q.has("p1"));
  assert(q.has("p2"));
  assert(q.has("p3"));

  // use_parent_values testen
  test_qq(p);
}

void func(query_params& q)
{
  // q hat die Werte p1 und p2
  std::cout << "test q" << std::endl;
  assert(q.has("p1"));
  assert(q.has("p2"));
  assert(!q.has("p3"));

  q.ret("p3", "wert3");
}

void test_qq(query_params& p)
{
  query_params qq(&p, false);
  std::cout << "test qq\n"
            << " url=" << qq << '\n'
            << " dump: " << qq.dump() << std::endl;
  assert(!qq.has("p1"));
  assert(!qq.has("p2"));
  assert(!qq.has("p3"));
}

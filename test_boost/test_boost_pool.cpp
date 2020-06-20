//
//  test_boost_pool.cpp
//  test_boost
//
//  Created by AsteriskZuo on 2020/6/13.
//  Copyright © 2020 AsteriskZuo. All rights reserved.
//

#include "test_boost_pool.hpp"

static int test(int argc, char * argv[]);
static void test2();
static void test3();
static void test4();
static void test5();
void test_boost_pool()
{
//    int argc = 1;
//    char * argv[1] = {"haha"};
//    test(argc, argv);
    
    test3();
}

// Copyright (C) 2000, 2001 Stephen Cleary
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/object_pool.hpp>

#include <iostream>
#include <vector>
#include <list>
#include <set>

#include <ctime>
#include <cerrno>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

// Symbols: malloc_allocator, new_delete_allocator

#include <cstddef>
#include <cstdlib>
#include <boost/limits.hpp>
#include <new>

template <typename T>
struct malloc_allocator
{
  typedef T * pointer;
  typedef const T * const_pointer;
  typedef T & reference;
  typedef const T & const_reference;
  typedef T value_type;

  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  template <typename U>
  struct rebind
  {
    typedef malloc_allocator<U> other;
  };

  static pointer address(reference r) { return &r; }
  static const_pointer address(const_reference r) { return &r; }
  static pointer allocate(const size_type n, const void* = 0)
  {
    const pointer ret = (pointer) std::malloc(n * sizeof(T));
    if (ret == 0)
      throw std::bad_alloc();
    return ret;
  }
  static void deallocate(const pointer p, const size_type)
  { std::free(p); }
  static size_type max_size() { return (std::numeric_limits<size_type>::max)(); }

  bool operator==(const malloc_allocator &) const { return true; }
  bool operator!=(const malloc_allocator &) const { return false; }

  malloc_allocator() { }
  template <typename U>
  malloc_allocator(const malloc_allocator<U> &) { }

  static void construct(const pointer p, const_reference t)
  { new ((void *) p) T(t); }
  static void destroy(const pointer p)
  { p->~T(); }
};

template <typename T>
struct new_delete_allocator
{
  typedef T * pointer;
  typedef const T * const_pointer;
  typedef T & reference;
  typedef const T & const_reference;
  typedef T value_type;

  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  template <typename U>
  struct rebind
  {
    typedef new_delete_allocator<U> other;
  };

  static pointer address(reference r) { return &r; }
  static const_pointer address(const_reference r) { return &r; }
  static pointer allocate(const size_type n, const void* = 0)
  { return (pointer) new char[n * sizeof(T)]; }
  static void deallocate(const pointer p, const size_type)
  { delete [] (char*)p; }
  static size_type max_size() { return (std::numeric_limits<size_type>::max)(); }

  bool operator==(const new_delete_allocator &) const { return true; }
  bool operator!=(const new_delete_allocator &) const { return false; }

  new_delete_allocator() { }
  template <typename U>
  new_delete_allocator(const new_delete_allocator<U> &) { }

  static void construct(const pointer p, const_reference t)
  { new ((void *) p) T(t); }
  static void destroy(const pointer p)
  { p->~T(); }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

unsigned long num_ints;
unsigned long num_loops = 10;
unsigned l;

template <unsigned N>
struct larger_structure
{
  char data[N];
};

unsigned test_number;

template <unsigned N>
static void timing_test_alloc_larger()
{
  typedef boost::fast_pool_allocator<larger_structure<N>,
      boost::default_user_allocator_new_delete,
      boost::details::pool::null_mutex> alloc;
  typedef boost::fast_pool_allocator<larger_structure<N> > alloc_sync;

  double end[1][6];
  std::clock_t start;

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::allocator<larger_structure<N> > a;
    for (unsigned long i = 0; i < num_ints; ++i)
      a.deallocate(a.allocate(1), 1);
  }
  end[0][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      std::free(std::malloc(sizeof(larger_structure<N>)));
  }
  end[0][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      delete new (std::nothrow) larger_structure<N>;
  }
  end[0][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc::deallocate(alloc::allocate());
  }
  end[0][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc_sync::deallocate(alloc_sync::allocate());
  }
  end[0][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    boost::pool<> p(sizeof(larger_structure<N>));
    for (unsigned long i = 0; i < num_ints; ++i)
    {
      void * const t = p.malloc();
      if (t != 0)
        p.free(t);
    }
  }
  end[0][5] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  std::cout << "Test " << test_number++ << ": Alloc & Dealloc " << num_ints << " structures of size " << sizeof(larger_structure<N>) << ":" << std::endl;
  std::cout << "  std::allocator: " << end[0][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[0][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[0][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[0][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[0][4] << " seconds" << std::endl;
  std::cout << "  Pool:           " << end[0][5] << " seconds" << std::endl;
}

static void timing_test_alloc()
{
  typedef boost::fast_pool_allocator<int,
      boost::default_user_allocator_new_delete,
      boost::details::pool::null_mutex> alloc;
  typedef boost::fast_pool_allocator<int> alloc_sync;

  double end[2][6];
  std::clock_t start;

  int ** p = new int*[num_ints];

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::allocator<int> a;
    for (unsigned long i = 0; i < num_ints; ++i)
      a.deallocate(a.allocate(1), 1);
  }
  end[0][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      std::free(std::malloc(sizeof(int)));
  }
  end[0][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      delete new (std::nothrow) int;
  }
  end[0][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc::deallocate(alloc::allocate());
  }
  end[0][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc_sync::deallocate(alloc_sync::allocate());
  }
  end[0][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    boost::pool<> p2(sizeof(int));
    for (unsigned long i = 0; i < num_ints; ++i)
    {
      void * const t = p2.malloc();
      if (t != 0)
        p2.free(t);
    }
  }
  end[0][5] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);


  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::allocator<int> a;
    for (unsigned long i = 0; i < num_ints; ++i)
      p[i] = a.allocate(1);
    for (unsigned long i = 0; i < num_ints; ++i)
      a.deallocate(p[i], 1);
  }
  end[1][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      p[i] = (int *) std::malloc(sizeof(int));
    for (unsigned long i = 0; i < num_ints; ++i)
      std::free(p[i]);
  }
  end[1][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      p[i] = new (std::nothrow) int;
    for (unsigned long i = 0; i < num_ints; ++i)
      delete p[i];
  }
  end[1][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      p[i] = alloc::allocate();
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc::deallocate(p[i]);
  }
  end[1][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    for (unsigned long i = 0; i < num_ints; ++i)
      p[i] = alloc_sync::allocate();
    for (unsigned long i = 0; i < num_ints; ++i)
      alloc_sync::deallocate(p[i]);
  }
  end[1][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);
    
    std::cout << __func__ << ":" << __LINE__ << ":" << sizeof(int) << std::endl;
    std::cout << __func__ << ":" << __LINE__ << ":" << sizeof(int*) << std::endl;
    int*** ppp = new int **[num_ints];

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    boost::pool<> pl(sizeof(int));
      for (unsigned long i = 0; i < num_ints; ++i) {
          p[i] = reinterpret_cast<int *>(pl.malloc());
          ppp[i] = &p[i];
      }
      for (unsigned long i = 0; i < num_ints; ++i) {
          if (p[i] != 0)
              pl.free(p[i]);
      }
  }
  end[1][5] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  delete [] p;

  std::cout << "Test 3: Alloc & Dealloc " << num_ints << " ints:" << std::endl;
  std::cout << "  std::allocator: " << end[0][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[0][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[0][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[0][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[0][4] << " seconds" << std::endl;
  std::cout << "  Pool:           " << end[0][5] << " seconds" << std::endl;

  std::cout << "Test 4: Alloc " << num_ints << " ints & Dealloc " << num_ints << " ints:" << std::endl;
  std::cout << "  std::allocator: " << end[1][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[1][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[1][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[1][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[1][4] << " seconds" << std::endl;
  std::cout << "  Pool:           " << end[1][5] << " seconds" << std::endl;
}

static void timing_test_containers()
{
  typedef boost::pool_allocator<int,
      boost::default_user_allocator_new_delete,
      boost::details::pool::null_mutex> alloc;
  typedef boost::pool_allocator<int> alloc_sync;
  typedef boost::fast_pool_allocator<int,
      boost::default_user_allocator_new_delete,
      boost::details::pool::null_mutex> fast_alloc;
  typedef boost::fast_pool_allocator<int> fast_alloc_sync;

  double end[3][5];
  std::clock_t start;

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::vector<int, std::allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[0][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::vector<int, malloc_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[0][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::vector<int, new_delete_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[0][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::vector<int, alloc> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[0][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::vector<int, alloc_sync> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[0][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);


  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::set<int, std::less<int>, std::allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.insert(0);
  }
  end[1][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::set<int, std::less<int>, malloc_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.insert(0);
  }
  end[1][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::set<int, std::less<int>, new_delete_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.insert(0);
  }
  end[1][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::set<int, std::less<int>, fast_alloc> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.insert(0);
  }
  end[1][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::set<int, std::less<int>, fast_alloc_sync> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.insert(0);
  }
  end[1][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);


  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::list<int, std::allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[2][0] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::list<int, malloc_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[2][1] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::list<int, new_delete_allocator<int> > x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[2][2] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::list<int, fast_alloc> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[2][3] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  start = std::clock();
  for(l = 0; l < num_loops; ++l)
  {
    std::list<int, fast_alloc_sync> x;
    for (unsigned long i = 0; i < num_ints; ++i)
      x.push_back(0);
  }
  end[2][4] = (std::clock() - start) / ((double) CLOCKS_PER_SEC);

  std::cout << "Test 0: Insertion & deletion of " << num_ints << " ints in a vector:" << std::endl;
  std::cout << "  std::allocator: " << end[0][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[0][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[0][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[0][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[0][4] << " seconds" << std::endl;
  std::cout << "  Pool:           not possible" << std::endl;
  std::cout << "Test 1: Insertion & deletion of " << num_ints << " ints in a set:" << std::endl;
  std::cout << "  std::allocator: " << end[1][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[1][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[1][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[1][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[1][4] << " seconds" << std::endl;
  std::cout << "  Pool:           not possible" << std::endl;
  std::cout << "Test 2: Insertion & deletion of " << num_ints << " ints in a list:" << std::endl;
  std::cout << "  std::allocator: " << end[2][0] << " seconds" << std::endl;
  std::cout << "  malloc/free:    " << end[2][1] << " seconds" << std::endl;
  std::cout << "  new/delete:     " << end[2][2] << " seconds" << std::endl;
  std::cout << "  Pool Alloc:     " << end[2][3] << " seconds" << std::endl;
  std::cout << "  Pool /w Sync:   " << end[2][4] << " seconds" << std::endl;
  std::cout << "  Pool:           not possible" << std::endl;
}

static int test(int argc, char * argv[])
{
  if (argc != 1 && argc != 2)
  {
    std::cerr << "Usage: " << argv[0]
        << " [number_of_ints_to_use_each_try]" << std::endl;
    return 1;
  }

  errno = 0;

  if (argc == 2)
  {
    num_ints = std::strtoul(argv[1], 0, 10);
    if (errno != 0)
    {
      std::cerr << "Cannot convert number \"" << argv[1] << '"' << std::endl;
      return 2;
    }
  }
  else
    num_ints = 700000;

#ifndef _NDEBUG
  num_ints /= 100;
#endif

  try
  {
    timing_test_containers();
    timing_test_alloc();
    test_number = 5;
    timing_test_alloc_larger<64>();
    test_number = 6;
    timing_test_alloc_larger<256>();
    test_number = 7;
    timing_test_alloc_larger<4096>();
  }
  catch (const std::bad_alloc &)
  {
    std::cerr << "Timing tests ran out of memory; try again with a lower value for number of ints"
        << " (current value is " << num_ints << ")" << std::endl;
    return 3;
  }
  catch (const std::exception & e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 4;
  }
  catch (...)
  {
    std::cerr << "Unknown error" << std::endl;
    return 5;
  }

  return 0;
}

/*

Output:

MSVC 10.0  using mutli-threaded DLL

 time_pool_alloc.cpp
     Creating library J:\Cpp\pool\pool\Release\alloc_example.lib and object J:\Cpp\pool\pool\Release\alloc_example.exp
  Generating code
  Finished generating code
  alloc_example.vcxproj -> J:\Cpp\pool\pool\Release\alloc_example.exe
  Test 0: Insertion & deletion of 700000 ints in a vector:
    std::allocator: 0.062 seconds
    malloc/free:    0.078 seconds
    new/delete:     0.078 seconds
    Pool Alloc:     0.328 seconds
    Pool /w Sync:   0.343 seconds
    Pool:           not possible
  Test 1: Insertion & deletion of 700000 ints in a set:
    std::allocator: 0.561 seconds
    malloc/free:    0.546 seconds
    new/delete:     0.562 seconds
    Pool Alloc:     0.109 seconds
    Pool /w Sync:   0.094 seconds
    Pool:           not possible
  Test 2: Insertion & deletion of 700000 ints in a list:
    std::allocator: 0.671 seconds
    malloc/free:    0.67 seconds
    new/delete:     0.671 seconds
    Pool Alloc:     0.094 seconds
    Pool /w Sync:   0.093 seconds
    Pool:           not possible
  Test 3: Alloc & Dealloc 700000 ints:
    std::allocator: 0.5 seconds
    malloc/free:    0.468 seconds
    new/delete:     0.592 seconds
    Pool Alloc:     0.032 seconds
    Pool /w Sync:   0.015 seconds
    Pool:           0.016 seconds
  Test 4: Alloc 700000 ints & Dealloc 700000 ints:
    std::allocator: 0.593 seconds
    malloc/free:    0.577 seconds
    new/delete:     0.717 seconds
    Pool Alloc:     0.032 seconds
    Pool /w Sync:   0.031 seconds
    Pool:           0.047 seconds
  Test 5: Alloc & Dealloc 700000 structures of size 64:
    std::allocator: 0.499 seconds
    malloc/free:    0.483 seconds
    new/delete:     0.624 seconds
    Pool Alloc:     0.016 seconds
    Pool /w Sync:   0.031 seconds
    Pool:           0.016 seconds
  Test 6: Alloc & Dealloc 700000 structures of size 256:
    std::allocator: 0.499 seconds
    malloc/free:    0.484 seconds
    new/delete:     0.639 seconds
    Pool Alloc:     0.016 seconds
    Pool /w Sync:   0.015 seconds
    Pool:           0.016 seconds
  Test 7: Alloc & Dealloc 700000 structures of size 4096:
    std::allocator: 0.515 seconds
    malloc/free:    0.515 seconds
    new/delete:     0.639 seconds
    Pool Alloc:     0.031 seconds
    Pool /w Sync:   0.016 seconds
    Pool:           0.016 seconds

*/

static void test2()
{
    // 64位系统，申请内存单元小于8字节，则按照8字节申请，100是数量，最终大小816字节
    boost::pool<> p1(sizeof(char), 100);
    p1.ordered_malloc(100);
    boost::pool<> p2(sizeof(std::uint16_t), 100);
    p2.ordered_malloc(100);
    boost::pool<> p3(sizeof(std::uint32_t), 100);
    p3.ordered_malloc(100);
    boost::pool<> p4(sizeof(char), 100);
    char* ret1 = (char*)p4.ordered_malloc(101);
    char* ret2 = (char*)p4.ordered_malloc(101);
    const char* cstr = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    std::memcpy(ret1, cstr, 101);
    std::memcpy(ret2, cstr, 101);
    
    char* test1 = (char*)std::malloc(100);
    std::memcpy(test1, cstr, 103);
    int a = 0;
}

static void test3()
{
    // 放在list中，但是不会再更新first
    boost::pool<> p1(sizeof(char), 100);
    char* ret = (char*)p1.ordered_malloc(900);
    char* ret2 = (char*)p1.ordered_malloc(900);
    int a = 0;
}

    class test_pool_object {
    public:
        char varchar;
        std::uint16_t var16;
        std::uint32_t var32;
        std::uint64_t var64;
    };
    
    static void test4() {
        // 可以给对象申请内存，但是没有初始化，如果需要初始化请使用object_pool
        boost::pool<> p1(sizeof(test_pool_object));
        test_pool_object* obj1 = (test_pool_object*)p1.ordered_malloc(1);
        int a  =0;
    }
    
    static void test5() {
        // 第二次add_block会出现loc不为空的情况
        boost::pool<> p1(sizeof(char), 110);
        char* ret = (char*)p1.ordered_malloc(800);
        char* ret2 = (char*)p1.ordered_malloc(100);
        int a = 0;
    }


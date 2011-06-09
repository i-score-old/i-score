/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2004
 *
 *  Last modified:
 *     $Date: 2009-09-08 21:10:29 +0200 (Tue, 08 Sep 2009) $ by $Author: schulte $
 *     $Revision: 9692 $
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode { namespace Iter { namespace Ranges {

  /**
   * \defgroup FuncIterRangesOp Operations on range iterators
   *
   * \ingroup FuncIterRanges
   */

  //@{
  /// Size of all ranges of range iterator \a i
  template<class I>
  unsigned int size(I& i);

  /// Check whether range iterators \a i and \a j are equal
  template<class I, class J>
  bool equal(I& i, J& j);

  /// Check whether range iterator \a i is subset of range iterator \a j
  template<class I, class J>
  bool subset(I& i, J& j);

  /// Check whether range iterators \a i and \a j are disjoint
  template<class I, class J>
  bool disjoint(I& i, J& j);

  /// Is one iterator subsumed by another, or is the intersection empty?
  enum SubsumptionStatus {
    SS_SUBSUMED, ///< Subsumed (subset)
    SS_EMPTY,    ///< Intersection is empty
    SS_NONE      ///< Neither of the above
  };

  /// Check whether range iterator \a i subsumes \a j, or whether intersection is empty
  template<class I, class J>
  SubsumptionStatus subsumes(I& i, J& j);
  //@}


  template<class I>
  inline unsigned int
  size(I& i) {
    IsRangeIter<I>();
    unsigned int s = 0;
    while (i()) {
      s += i.width(); ++i;
    }
    return s;
  }

  template<class I, class J>
  forceinline bool
  equal(I& i, J& j) {
    IsRangeIter<I>();
    IsRangeIter<J>();
    // Are i and j equal?
    while (i() && j())
      if ((i.min() == j.min()) && (i.max() == j.max())) {
        ++i; ++j;
      } else {
        return false;
      }
    return !i() && !j();
  }

  template<class I, class J>
  forceinline bool
  subset(I& i, J& j) {
    IsRangeIter<I>();
    IsRangeIter<J>();
    // Is i subset of j?
    while (i() && j())
      if (j.max() < i.min()) {
        ++j;
      } else if ((i.min() >= j.min()) && (i.max() <= j.max())) {
        ++i;
      } else {
        return false;
      }
    return !i();
  }

  template<class I, class J>
  forceinline bool
  disjoint(I& i, J& j) {
    IsRangeIter<I>();
    IsRangeIter<J>();
    // Are i and j disjoint?
    while (i() && j())
      if (j.max() < i.min()) {
        ++j;
      } else if (i.max() < j.min()) {
        ++i;
      } else {
        return false;
      }
    return true;
  }

  template<class I, class J>
  inline SubsumptionStatus
  subsumes(I& i, J& j) {
    IsRangeIter<I>();
    IsRangeIter<J>();
    bool subsumed = true;
    bool empty    = true;
    while (i() && j()) {
      if (i.max() < j.min()) {
        ++i;
      } else if (j.max() < i.min()) {
        ++j; subsumed = false;
      } else if ((j.min() >= i.min()) && (j.max() <= i.max())) {
        ++j; empty = false;
      } else if (j.max() <= i.max()) {
        ++j; empty = false; subsumed = false;
      } else if (i.max() <= j.max()) {
        ++i; empty = false; subsumed = false;
      }
    }
    if (j())
      subsumed = false;
    if (subsumed)
      return SS_SUBSUMED;
    return empty ? SS_EMPTY : SS_NONE;
  }

}}}

// STATISTICS: iter-any


// Copyright (c) 2003,2004  INRIA Sophia-Antipolis (France) and
// Notre Dame University (U.S.A.).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>



#ifndef SEGMENT_VORONOI_DIAGRAM_SIMPLE_SITE_H
#define SEGMENT_VORONOI_DIAGRAM_SIMPLE_SITE_H

#include <iostream>
#include <CGAL/assertions.h>

CGAL_BEGIN_NAMESPACE

  /** A Site is either a point or a segment or a point defined as the
      intersection of two non-parallel segments (if defined)
   */

template <class R_>
class Segment_Voronoi_diagram_simple_site_2 
{
public:
  typedef R_ R;
  typedef R  Rep;
  typedef typename R::Point_2   Point_2;
  typedef typename R::Segment_2 Segment_2;

protected:
  typedef typename R::FT        FT;
  typedef typename R::RT        RT;
  typedef Segment_Voronoi_diagram_simple_site_2<Rep>  Self;

public:
  Segment_Voronoi_diagram_simple_site_2() : type_(0) {}

  // constructs point site using input point
  Segment_Voronoi_diagram_simple_site_2(const Point_2 &p) {
    initialize_site(p);
  }

  // constructs segment site using input segment
  Segment_Voronoi_diagram_simple_site_2(const Segment_2 &s) {
    initialize_site(s);
  }

  // the compiler complains that it cannot find this constructor;
  // solution: make the insert_intersecting_segment a template
  // method...
  template<class A1, class A2>
  Segment_Voronoi_diagram_simple_site_2(const A1&, const A2&) {
    //    THIS_CONSTRUCTOR_SHOULD_HAVE_NEVER_BEEN_CALLED;
    CGAL_assertion( false );
  }

  template<class A1, class A2, class A3>
  Segment_Voronoi_diagram_simple_site_2(const A1&, const A2&,
					const A3&) {
    //    THIS_CONSTRUCTOR_SHOULD_HAVE_NEVER_BEEN_CALLED;
    CGAL_assertion( false );
  }

  Segment_Voronoi_diagram_simple_site_2(const Object &o) {
    if ( assign(p_, o) ) {
      initialize_site(p_);
      return;
    }

    Segment_2 s;
    if ( assign(s, o) ) {
      initialize_site(s);
      return;
    }

    type_ = 0;
  }

  bool is_defined() const { return type_; }
  bool is_point() const { return type_ == 1; }
  bool is_segment() const { return type_ == 2; }
  bool is_exact() const { return true; }
  bool is_exact(unsigned int i) const { return true; }

  Point_2 point() const { 
    CGAL_precondition ( is_point() );
    return p_;
  }

  Segment_2 segment() const {
    CGAL_precondition ( is_segment() ); 
    return Segment_2( p_, p2_ );
  }

  Point_2 source() const {
    CGAL_precondition ( is_segment() ); 
    return p_;
  }

  Point_2 target() const {
    CGAL_precondition ( is_segment() ); 
    return p2_;
  }

  Self source_site() const {
    CGAL_precondition( is_segment() );
    return Self(p_);
  }

  Self target_site() const {
    CGAL_precondition( is_segment() );
    return Self(p2_);
  }

  Self opposite_site() const {
    CGAL_precondition( is_segment() );
    return Self( segment().opposite() );
  }

  Segment_2 supporting_segment() const {
    CGAL_precondition( is_segment() );
    return segment();
  }

  Segment_2 supporting_segment(unsigned int i) const {
    CGAL_precondition( is_point() && i < 2 );
    return Segment_2(p_, p_);
  }

  Segment_2 crossing_segment(unsigned int i) const {
    CGAL_precondition( is_segment() && i < 2 );
    return Segment_2(p_, p2_);
  }

  void set_point(const Point_2& p) {
    initialize_site(p);
  }

  void set_segment(const Segment_2& s) {
    initialize_site(s);
  }

  // the compiler complains that it cannot find this constructor;
  // solution: make the insert_intersecting_segment a template
  // method...
  template<class A1, class A2>
  void set_point(const A1&, const A2&)
  {
    CGAL_assertion(false);
  }

  template<class A1, class A2, class A3>
  void set_segment(const A1&, const A2&, const A3&)
  {
    CGAL_assertion(false);
  }

  std::ostream& write(std::ostream& os)
  {
    return os << (*this);
  }

protected:
  void initialize_site(const Point_2& p)
  {
    type_ = 1;
    p_ = p;
  }

  void initialize_site(const Segment_2& s)
  {
    type_ = 2;
    p_ = s.source();
    p2_ = s.target();
  }

protected:
  Point_2 p_;
  Point_2 p2_;
  char type_;
};

//-------------------------------------------------------------------------

template <class R>
std::ostream&
operator<<(std::ostream& os, 
	   const Segment_Voronoi_diagram_simple_site_2<R>& s)
{
  if (!s.is_defined())
    return os << "u";
  if (s.is_point())
    return os << "p " << s.point ();
  return os << "s " << s.segment ();
}

template <class R>
std::istream &
operator>>(std::istream &is,
	   Segment_Voronoi_diagram_simple_site_2<R>& t)
{
  typedef Segment_Voronoi_diagram_simple_site_2<R>   Site_2;
  typedef typename Site_2::Point_2                   Point_2;
  typedef typename Site_2::Segment_2                 Segment_2;

  char type;
  if (is >> type) {
    if (type == 'p') {
      Point_2 p;
      is >> p;
      t.set_point(p);
    } else if (type == 's') {
      Segment_2 s;
      is >> s;
      t.set_segment(s);
    }
  }
  return is;
}

template < class R, class Stream >
Stream&
operator<<(Stream& str, Segment_Voronoi_diagram_simple_site_2<R>& t)
{
  if ( t.is_defined() ) {
    if ( t.is_point() ) {
      str << "p " << t.point();
    } else {
      str << "s " << t.segment().source() << "  "
	  << t.segment().target();
    }
  }

  return str;
}


CGAL_END_NAMESPACE

#endif // SEGMENT_VORONOI_DIAGRAM_SIMPLE_SITE_H

#ifndef CGAL_INTERNAL_TETRAHEDRON_INTERSECTION_HELPERS_H
#define CGAL_INTERNAL_TETRAHEDRON_INTERSECTION_HELPERS_H

#include <CGAL/kernel_basic.h>
#include <list>
#include <vector>

namespace CGAL {

namespace Intersections {

namespace internal {

template<typename Segment>
void filter_segments(const std::vector<Segment>& input,
                     std::vector<Segment>& output)
{
  std::list<Segment> tmp(input.begin(), input.end());

  do
  {
  Segment s = tmp.back();
  tmp.pop_back();
  auto s_it = tmp.begin();
  for(; s_it != tmp.end();)
  {
    if(s == *s_it || s == s_it->opposite())
    {
      s_it = tmp.erase(s_it);
    }
    else {
      ++s_it;
    }
  }
  output.push_back(s);
  }while (!tmp.empty());
}

template <typename Segment,
          typename Point,
          typename Triangle>
void fill_segments_infos(std::vector<Segment>& segments,
                         std::list<Point>& points, const Triangle& input_tr)
{
  struct Wrapped_segment
  {
    Segment segment;
    bool s_dangling;
    bool t_dangling;
    std::size_t s_neighbor;
    std::size_t t_neighbor;
    Wrapped_segment(const Segment& s)
      :segment(s), s_dangling(true),
        t_dangling(true){}
  };

  std::vector<Wrapped_segment> wrapped_segments;

  for(auto s:segments)
    wrapped_segments.push_back(Wrapped_segment(s));

  std::vector<Segment> bis = segments;
  for(int plouf = 0; plouf < bis.size()-1; ++plouf)
  {
    Segment s = bis.back();
    bis.pop_back();
    Wrapped_segment& super_s = wrapped_segments.back();
    if(!super_s.s_dangling && ! super_s.t_dangling)
      continue;
    for(std::size_t i = 0; i< bis.size(); ++i)
    {
      const Segment& s2 = bis[i];
      if(s2.target() == s.source())
      {
        super_s.s_dangling = false;
        super_s.s_neighbor = i;
        //same i because we empty from the bottom
        wrapped_segments[i].t_dangling = false;
        wrapped_segments[i].t_neighbor = bis.size();
      }
      else if(s2.target() == s.target())
      {
        super_s.t_dangling = false;
        super_s.t_neighbor = i;
        wrapped_segments[i].s_dangling = false;
        wrapped_segments[i].s_neighbor = bis.size();
        wrapped_segments[i].segment = wrapped_segments[i].segment.opposite(); //also orient the structure

      }
      else if(s2.source() == s.source())
      {
        super_s.s_dangling = false;
        super_s.s_neighbor = i;
        wrapped_segments[i].t_dangling = false;
        wrapped_segments[i].t_neighbor = bis.size();
        wrapped_segments[i].segment = wrapped_segments[i].segment.opposite();
      }
      else if(s2.source() == s.target())
      {
        super_s.t_dangling = false;
        super_s.t_neighbor = i;
        wrapped_segments[i].s_dangling = false;
        wrapped_segments[i].s_neighbor = bis.size();
      }
    }

    //fill dangling extremities using triangle edges
    if(super_s.s_dangling)
    {
      for(std::size_t e_id = 0; e_id < 3; ++e_id)
      {
        Segment edge(input_tr.vertex(e_id), input_tr.vertex(e_id+1));
        if(!edge.has_on(s.source()))
        {
          continue;
        }
        for(std::size_t i = 0; i< bis.size(); ++i)
        {
          if(edge.has_on(bis[i].source()))
          {
            super_s.s_dangling = false;
            super_s.s_neighbor = i;
            //same i because we empty from the bottom
            wrapped_segments[i].t_dangling = false;
            wrapped_segments[i].t_neighbor = bis.size();
            wrapped_segments[i].segment = wrapped_segments[i].segment.opposite();
          }
          else if(edge.has_on(bis[i].target()))
          {
            super_s.s_dangling = false;
            super_s.s_neighbor = i;
            //same i because we empty from the bottom
            wrapped_segments[i].t_dangling = false;
            wrapped_segments[i].t_neighbor = bis.size();
          }
        }
      }
    }
    if(super_s.t_dangling)
    {
      for(std::size_t e_id = 0; e_id < 3; ++e_id)
      {
        Segment edge(input_tr.vertex(e_id), input_tr.vertex(e_id+1));
        if(!edge.has_on(s.target()))
        {
          continue;
        }
        for(std::size_t i = 0; i< bis.size(); ++i)
        {
          if(edge.has_on(bis[i].source()))
          {
            super_s.t_dangling = false;
            super_s.t_neighbor = i;
            //same i because we empty from the bottom
            wrapped_segments[i].s_dangling = false;
            wrapped_segments[i].s_neighbor = bis.size();
          }
          else if(edge.has_on(bis[i].target()))
          {
            super_s.t_dangling = false;
            super_s.t_neighbor = i;
            //same i because we empty from the bottom
            wrapped_segments[i].s_dangling = false;
            wrapped_segments[i].s_neighbor = bis.size();
            wrapped_segments[i].segment = wrapped_segments[i].segment.opposite();
          }
        }
      }
    }

    if(super_s.s_dangling || super_s.t_dangling)
    {
      std::cerr<<"Error. Kernel must have exact constructions to compute this intersection."<<std::endl;
      return;
    }
  }

  //finally fill points
  std::size_t voyager = 0;
  do
  {
    Wrapped_segment& ws = wrapped_segments[voyager];
    points.push_back(ws.segment.source());
    if(wrapped_segments[ws.t_neighbor].segment.source() != ws.segment.target())
      points.push_back(ws.segment.target());
    voyager = ws.t_neighbor;
  }while(voyager!=0);
}


template <typename Segment,
          typename Point>
void fill_points_list(std::list<Segment>& segments, std::list<Point>& points)
{
  assert(segments.size() > 1);
  //init : take seg.front = seg.
  Segment seg = segments.front();
  segments.pop_front();
  //put source and target in points.
  points.push_back(seg.source());
  points.push_back(seg.target());
  //find first seg with a point in common with seg.front = s2.
  do{
    auto seg_it = segments.begin();
    bool found = false;
    for(;seg_it != segments.end(); ++seg_it)
    {
      if(seg_it->source() == points.front())
      {
        points.push_front(seg_it->target());
        found = true;
      }
      else if(seg_it->source() == points.back())
      {
        points.push_back(seg_it->target());
        found = true;
      }
      else if(seg_it->target() == points.front())
      {
        points.push_front(seg_it->source());
        found = true;
      }
      else if(seg_it->target() == points.back())
      {
        points.push_back(seg_it->source());
        found = true;
      }
      if(found)
      {
        break;
      }
    }
    if(!found)
    {
      std::cerr<<"Error. Kernel must have exact constructions to compute this intersection."<<std::endl;
      return;
    }
    segments.erase(seg_it);
    //if loop, pop first point to avoid double
    if(points.front() == points.back())
      points.pop_front();
  }while(!segments.empty());
}

}}}//end namespaces
#endif // CGAL_INTERNAL_TETRAHEDRON_INTERSECTION_HELPERS_H

#include <options.hpp>
#if NAO_ENABLED
#ifndef NAO_V4L2_HPP_
#define NAO_V4L2_HPP_

#include <linux/videodev2.h>

namespace nao {



class V4L2 {
public:
  V4L2(V4L2 const&) = delete;
  V4L2(V4L2&&) = delete;
  V4L2& operator=(V4L2 const&) = delete;
  read();
protected:
  asdf
}



} // namespace nao

#endif // NAO_V4L2_HPP_

#endif // NAO_ENABLED

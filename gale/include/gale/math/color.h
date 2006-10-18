#ifndef COLOR_H
#define COLOR_H

/**
 * \file Color representation class implementations
 */

#include "tuple.h"

namespace gale {

namespace math {

/**
 * As partial specialization on class template member functions is not supported
 * by C++, this helper class only has a single template parameter. This way, we
 * can fully specialize only those class template member functions that should
 * not be implictly obtained from the base template.
 */
template<typename T>
class ColorModel {
  public:
    static T getMinIntensity() {
        return std::numeric_limits<T>::min();
    }

    static T getMaxIntensity() {
        return std::numeric_limits<T>::max();
    }

    //void RGB2HSV(T r,T g,T b,T& h,T& s,T& v) {}
};

template<>
inline float ColorModel<float>::getMinIntensity() {
    return 0;
}

template<>
inline float ColorModel<float>::getMaxIntensity() {
    return 1;
}

template<>
inline double ColorModel<double>::getMinIntensity() {
    return 0;
}

template<>
inline double ColorModel<double>::getMaxIntensity() {
    return 1;
}

/**
 * Color class implementation based on an RGB-tuple, featuring predefined
 * constants for commonly used colors, HSV conversion and useful operators.
 *
 * Example usage:
 * \code
 * Col4ub colors[8];
 * glColorPointer(4,GL_UNSIGNED_BYTE,0,colors);
 * \endcode
 */
template<unsigned int N,typename T>
class Color:public TupleBase<N,T,Color<N,T> >,public ColorModel<T> {
    /// This type definition simplifies base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Color<N,T> > Base;

  public:
    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here (see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14).
     */
    //@{
    /// Returns a color which represents black.
    static Color const& BLACK() {
        static Color const c(0x0);
        return c;
    }

    /// Returns a color which represents blue.
    static Color const& BLUE() {
        static Color const c(0x4);
        return c;
    }

    /// Returns a color which represents green.
    static Color const& GREEN() {
        static Color const c(0x2);
        return c;
    }

    /// Returns a color which represents cyan.
    static Color const& CYAN() {
        static Color const c(0x6);
        return c;
    }

    /// Returns a color which represents red.
    static Color const& RED() {
        static Color const c(0x1);
        return c;
    }

    /// Returns a color which represents magenta.
    static Color const& MAGENTA() {
        static Color const c(0x5);
        return c;
    }

    /// Returns a color which represents yellow.
    static Color const& YELLOW() {
        static Color const c(0x3);
        return c;
    }

    /// Returns a color which represents white.
    static Color const& WHITE() {
        static Color const c(0x7);
        return c;
    }
    //@}

    /**
     * \name Constructors
     */
    //@{
    /// For performance reasons, do not initialize any data by default.
    Color() {
    }

    /// Sets each color channel to either the maximum or minimum intensity
    /// depending on the bits set in \a mask. Bit 0 maps to the first channel,
    /// bit 1 to second one and so on. This constructor is required to
    /// initialize the static class constants.
    explicit Color(unsigned int mask) {
        meta::LoopFwd<N,meta::OpAssign>::
          iterateIndexMask(Base::getData(),mask,getMaxIntensity(),getMinIntensity());
    }

    /// Allows to initialize 3-channel colors directly.
    Color(T const& x,T const& y,T const& z):Base(x,y,z) {
    }

    /// Allows to initialize 4-channel colors directly.
    Color(T const& x,T const& y,T const& z,T const& w):Base(x,y,z,w) {
    }

    /// Conversion constructor to be able to use colors of different types
    /// together.
    template<typename U>
    Color(const Color<N,U>& v) {
        meta::LoopFwd<N,meta::OpAssign>::iterate(Base::getData(),v.getData());
    }
    //@}
};

/**
 * \name Type definitions for use as OpenGL colors
 */
//@{
typedef Color<3,double> Col3d;
typedef Color<3,float> Col3f;
typedef Color<3,int> Col3i;
typedef Color<3,unsigned int> Col3ui;
typedef Color<3,short> Col3s;
typedef Color<3,unsigned short> Col3us;
typedef Color<3,signed char> Col3b;
typedef Color<3,unsigned char> Col3ub;

typedef Color<4,double> Col4d;
typedef Color<4,float> Col4f;
typedef Color<4,int> Col4i;
typedef Color<4,unsigned int> Col4ui;
typedef Color<4,short> Col4s;
typedef Color<4,unsigned short> Col4us;
typedef Color<4,signed char> Col4b;
typedef Color<4,unsigned char> Col4ub;
//@}

} // namespace math

} // namespace gale

#endif // COLOR_H

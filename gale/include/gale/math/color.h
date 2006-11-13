#ifndef COLOR_H
#define COLOR_H

/**
 * \file
 * Color representation class implementations
 */

#include "tuple.h"

namespace gale {

namespace math {

/**
 * Helper class with only a single template parameter; needed as partial
 * specialization of member functions of template classes with multiple template
 * parameters is not supported by C++. Using this class we can fully specialize
 * only those class template member functions that should not be implictly
 * obtained from the base template.
 */
template<typename T>
class ColorModel {
  public:
    /// Returns the minimum intensity value for a color channel.
    static T getMinIntensity() {
        return std::numeric_limits<T>::min();
    }

    /// Returns the maximum intensity value for a color channel.
    static T getMaxIntensity() {
        return std::numeric_limits<T>::max();
    }

    /// Converts a color representation from the RGB to the HSV model. All
    /// channel values are interpreted to range from the value returned by
    /// getMinIntensity() to the value returned by getMaxIntensity().
    void RGB2HSV(T const r,T const g,T const b,T& h,T& s,T& v) {
        const T min=getMinIntensity(),range=getMaxIntensity()-min;

        // Always convert RGB values to range [0,1].
        double R=clamp(double(r-min)/range,0.0,1.0);
        double G=clamp(double(g-min)/range,0.0,1.0);
        double B=clamp(double(b-min)/range,0.0,1.0);

        double m=min(R,G,B);
        double V=max(R,G,B);

        double delta=V-m;
        double S=(v>0)?(delta/v):0;

        double H;

        if (S>0) {
            double rd=(V-R)/delta;
            double gd=(V-G)/delta;
            double bd=(V-B)/delta;

            if (OpCmpEqualEps::evaluate(V,R))
                H=OpCmpEqualEps::evaluate(m,G)?(5+bd):(1-gd);
            else if (OpCmpEqualEps::evaluate(v,G))
                H=OpCmpEqualEps::evaluate(m,B)?(1+rd):(3-bd);
            else
                H=OpCmpEqualEps::evaluate(m,R)?(3+gd):(5-rd);

            H=(H<6)?(H*60):0;
        } else {
            // Achromatic case, actually hue is undefined!
            H=0;
        }

        // At this point hue ranges from 0 to 360, saturation and value from 0
        // to 1. Convert these ranges back to the initial RGB channel range.
        h=T(H*range/360+min);
        s=T(S*range+min);
        v=T(V*range+min);
    }

    /// Converts a color representation from the HSV to the RGB model. All
    /// channel values are interpreted to range from the value returned by
    /// getMinIntensity() to the value returned by getMaxIntensity().
    void HSV2RGB(T const h,T const s,T const v,T& r,T& g,T& b) {
        const T min=getMinIntensity(),range=getMaxIntensity()-min;

        // Always convert HSV values to range [0,1].
        double H=clamp(double(h-min)/range,0.0,1.0);
        double S=clamp(double(s-min)/range,0.0,1.0);
        double V=clamp(double(v-min)/range,0.0,1.0);

        double R,G,B;

        if (S>0) {
            H*=6;
            long long i=roundToZero(H);
            double f=H-i;

            double p=V*(1-S);
            double q=V*(1-S*f);
            double t=V*(1-S*(1-f));

            switch (i) {
                case 6:
                case 0: {
                    R=V; G=t; B=p;
                    break;
                }
                case 1: {
                    R=q; G=V; B=p;
                    break;
                }
                case 2: {
                    R=p; G=V; B=t;
                    break;
                }
                case 3: {
                    R=p; G=q; B=V;
                    break;
                }
                case 4: {
                    R=t; G=p; B=V;
                    break;
                }
                case 5: {
                    R=V; G=p; B=q;
                    break;
                }
            }
        } else {
            // Achromatic case, actually hue is undefined!
            R=G=B=V;
        }

        // At this point red, green and blue range from 0 to 1. Convert this
        // range back to the initial HSV channel range.
        r=T(R*range+min);
        g=T(G*range+min);
        b=T(B*range+min);
    }
};

/// \cond SPECIALIZATION
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
/// \endcond

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
    /// \copydoc Base
    typedef ColorModel<T> Model;

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
          iterateIndexMask(Base::getData(),mask,Model::getMaxIntensity(),Model::getMinIntensity());
    }

    /// Allows to initialize 3-channel colors directly.
    Color(T const& x,T const& y,T const& z):Base(x,y,z) {
    }

    /// Allows to initialize 4-channel colors directly.
    Color(T const& x,T const& y,T const& z,T const& w):Base(x,y,z,w) {
    }

    /// Converts a color of different type but with the same amount of channels
    /// to this type.
    template<typename U>
    Color(Color<N,U> const& v) {
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

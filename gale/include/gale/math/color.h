/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at http://developer.berlios.de/projects/gale/
 *
 * Copyright (C) 2005-2006  Sebastian Schuberth <sschuberth@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

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
 * Color class implementation based on an RGB(A)-tuple, featuring predefined
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

    /// Initialize to black by default.
    Color() {
        *this=BLACK();
    }

    /// Sets each color channel to either the maximum or minimum intensity
    /// depending on the bits set in \a mask. Bit 0 maps to the first channel,
    /// bit 1 to second one and so on. This constructor is required to
    /// initialize the static class constants.
    explicit Color(unsigned int mask):m_hsv_outdated(true),m_rgb_outdated(false) {
        meta::LoopFwd<N,meta::OpAssign>::
          iterateIndexMask(Base::getData(),mask,Model::getMaxIntensity(),Model::getMinIntensity());
    }

    /// Allows to initialize 3-channel colors directly.
    Color(T const& r,T const& g,T const& b):
      Base(r,g,b),m_hsv_outdated(true),m_rgb_outdated(false)
    {
    }

    /// Allows to initialize 4-channel colors directly.
    Color(T const& r,T const& g,T const& b,T const& a):
      Base(r,g,b,a),m_hsv_outdated(true),m_rgb_outdated(false)
    {
    }

    /// Converts a color of different type but with the same amount of channels
    /// to this type.
    template<typename U>
    Color(Color<N,U> const& v):m_hsv_outdated(true),m_rgb_outdated(false) {
        meta::LoopFwd<N,meta::OpAssign>::iterate(Base::getData(),v.getData());
    }

    //@}

    /**
     * \name RGB(A) color model channel access methods
     */
    //@{

    /// Returns a reference to the red channel.
    T& getR() {
        G_ASSERT(N>=1)
        updateRGB();
        return Base::m_data[0];
    }

    /// Assigns a new value to the red channel.
    void setR(T const& r) {
        G_ASSERT(N>=1)
        m_hsv_outdated=true;
        Base::m_data[0]=r;
    }

    /// Returns a reference to the green channel.
    T& getG() {
        G_ASSERT(N>=2)
        updateRGB();
        return Base::m_data[1];
    }

    /// Assigns a new value to the green channel.
    void setG(T const& g) {
        G_ASSERT(N>=2)
        m_hsv_outdated=true;
        Base::m_data[1]=g;
    }

    /// Returns a reference to the blue channel.
    T& getB() {
        G_ASSERT(N>=3)
        updateRGB();
        return Base::m_data[2];
    }

    /// Assigns a new value to the blue channel.
    void setB(T const& b) {
        G_ASSERT(N>=3)
        m_hsv_outdated=true;
        Base::m_data[2]=b;
    }

    /// Returns a reference to the alpha channel.
    T& getA() {
        G_ASSERT(N>=4)
        return Base::m_data[3];
    }

    /// Returns a \c constant reference to the alpha channel.
    T const& getA() const {
        G_ASSERT(N>=4)
        return Base::m_data[3];
    }

    /// Assigns a new value to the alpha channel.
    void setA(T const& a) {
        G_ASSERT(N>=4)
        Base::m_data[3]=a;
    }

    //@}

    /**
     * \name HSV color model channel access methods
     */
    //@{

    /// Returns a reference to the hue channel.
    T& getH() {
        G_ASSERT(N>=1)
        updateHSV();
        return m_h;
    }

    /// Assigns a new value to the hue channel.
    void setH(T const& h) {
        G_ASSERT(N>=1)
        m_rgb_outdated=true;
        m_h=h;
    }

    /// Returns a reference to the saturation channel.
    T& getS() {
        G_ASSERT(N>=2)
        updateHSV();
        return m_s;
    }

    /// Assigns a new value to the saturation channel.
    void setS(T const& s) {
        G_ASSERT(N>=2)
        m_rgb_outdated=true;
        m_s=s;
    }

    /// Returns a reference to the value channel.
    T& getV() {
        G_ASSERT(N>=3)
        updateHSV();
        return m_v;
    }

    /// Assigns a new value to the value channel.
    void setV(T const& v) {
        G_ASSERT(N>=3)
        m_rgb_outdated=true;
        m_v=v;
    }

    //@}

    /**
     * \name Color inversion methods
     */
    //@{

    /// Inverts this color (if present, the alpha channel is omitted).
    Color& invert() {
        Color tmp=WHITE()-(*this);
        if (N==4)
            tmp.setA(getA());
        return *this=tmp;
    }

    /// Returns an inverted copy of color \a c.
    friend Color operator!(Color const& c) {
        return Color(c).invert();
    }
    //@}

  private:

    /// Updates the HSV channels to match the RGB channels if required.
    void updateHSV() {
        if (!m_hsv_outdated)
            return;
        RGB2HSV(m_data[0],m_data[1],m_data[2],m_h,m_s,m_v);
        m_hsv_outdated=false;
    }

    /// Updates the RGB channels to match the HSV channels if required.
    void updateRGB() {
        if (!m_rgb_outdated)
            return;
        HSV2RGB(m_h,m_s,m_v,m_data[0],m_data[1],m_data[2]);
        m_rgb_outdated=false;
    }

    bool m_hsv_outdated; ///< Flag to indicate the validity of the HSV channel variables.
    bool m_rgb_outdated; ///< Flag to indicate the validity of the RGB channel variables.

    T m_h; ///< Cache variable for the HSV channel values.
    T m_s; ///< \copydoc m_h
    T m_v; ///< \copydoc m_h
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

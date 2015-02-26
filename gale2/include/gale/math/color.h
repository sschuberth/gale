/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at https://github.com/sschuberth/gale/.
 *
 * Copyright (C) 2005-2011  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#pragma once

/**
 * \file
 * RGB color routines
 */

#include "tuple.h"

namespace gale {

namespace math {

class ColorModelHSV;

/**
 * Helper class with only a single template parameter. This is needed because
 * partial specialization of class templates by only specifying the differing
 * member functions (not the whole class) is not supported by C++ for multiple
 * template arguments. Using this class we can specialize only those class
 * template member functions that should not be implicitly obtained from the
 * base template.
 */
template<typename T>
struct ColorChannel
{
    /// Returns the minimum value allowed for a color channel.
    static T MIN_VALUE() {
        return Numerics<T>::MIN();
    }

    /// Returns the maximum value allowed for a color channel.
    static T MAX_VALUE() {
        return Numerics<T>::MAX();
    }

    /// Returns the value range allowed for a color channel.
    static double RANGE() {
        return static_cast<double>(MAX_VALUE())-MIN_VALUE();
    }
};

/// \cond DOXYGEN_IGNORE
template<>
inline float ColorChannel<float>::MIN_VALUE()
{
    return 0;
}

template<>
inline float ColorChannel<float>::MAX_VALUE()
{
    return 1;
}

template<>
inline double ColorChannel<double>::MIN_VALUE()
{
    return 0;
}

template<>
inline double ColorChannel<double>::MAX_VALUE()
{
    return 1;
}
/// \endcond

/**
 * Color representation based on an RGB(A)-tuple. It features predefined
 * constants for commonly used colors and useful operators.
 *
 * OpenGL example usage:
 * \code
 * glColor4ubv(Col4ub::RED());
 * \endcode
 *
 * RGB is an additive color model with primary colors red, green and blue,
 * secondary colors cyan, magenta and yellow, and neutral color white (for fully
 * saturated colors).
 */
template<unsigned int N,typename T>
class Color:public TupleBase<N,T,Color<N,T> >,public ColorChannel<T>
{
    /// This type definition simplifies base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Color<N,T> > Base;

  public:

    /// The number of channels of this template instance.
    static int const Channels=N;

    /// The data type of this template instance.
    typedef T Type;

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14.
     */
    //@{

    /// Returns a color which represents black.
    static Color const& BLACK() {
        static Color const c(0x0|0x8);
        return c;
    }

    /// Returns a color which represents blue.
    static Color const& BLUE() {
        static Color const c(0x4|0x8);
        return c;
    }

    /// Returns a color which represents green.
    static Color const& GREEN() {
        static Color const c(0x2|0x8);
        return c;
    }

    /// Returns a color which represents cyan.
    static Color const& CYAN() {
        static Color const c(0x6|0x8);
        return c;
    }

    /// Returns a color which represents red.
    static Color const& RED() {
        static Color const c(0x1|0x8);
        return c;
    }

    /// Returns a color which represents magenta.
    static Color const& MAGENTA() {
        static Color const c(0x5|0x8);
        return c;
    }

    /// Returns a color which represents yellow.
    static Color const& YELLOW() {
        static Color const c(0x3|0x8);
        return c;
    }

    /// Returns a color which represents white.
    static Color const& WHITE() {
        static Color const c(0x7|0x8);
        return c;
    }

    //@}

    /// Returns a random color generated by \a r.
    template<typename RT,class RI>
    static Color random(RandomBase<RT,RI>& r) {
        Color<N,double> c;
        for (int i=0;i<N;++i) {
            c[i]=r.random01();
        }
        return Color(c);
    }

    /**
     * \name Constructors
     */
    //@{

  protected:

    /// Sets each color channel to either the maximum or minimum value
    /// depending on the bits set in \a mask. Bit 0 maps to the first channel,
    /// bit 1 to second one and so on. This constructor is required to
    /// initialize the static class constants.
    explicit Color(unsigned int const mask)
    {
        // Do not allow colors with less than 3 elements.
        G_ASSERT(N>=3)

        meta::LoopFwd<N,meta::OpAssign>
            ::iterateIndexMask(Base::data(),mask,MAX_VALUE(),MIN_VALUE());
    }

  public:

    /// Initialize to black by default.
    Color() {
        *this=BLACK();
    }

    /// Allows to initialize 3-channel colors directly.
    Color(T const r,T const g,T const b)
    :   Base(r,g,b)
    {}

    /// Allows to initialize 4-channel colors directly.
    Color(T const r,T const g,T const b,T const a)
    :   Base(r,g,b,a)
    {}

    /// Converts a color of different type but with the same amount of channels
    /// to this type with correct range mapping applied.
    template<typename U>
    Color(Color<N,U> const& v)
    {
        // Do not allow colors with less than 3 elements.
        G_ASSERT(N>=3)

        // Cast the color to double for high-precision calculations.
        Color<N,double> tmp;
        meta::LoopFwd<N,meta::OpAssign>::iterate(tmp.data(),v.data());

        // Map to range [0,1].
        meta::LoopFwd<N,meta::OpArithSub>::iterate(tmp.data(),static_cast<double>(v.MIN_VALUE()));
        meta::LoopFwd<N,meta::OpArithDiv>::iterate(tmp.data(),v.RANGE());

        // Map to this color's range.
        meta::LoopFwd<N,meta::OpArithMul>::iterate(tmp.data(),RANGE());
        meta::LoopFwd<N,meta::OpArithAdd>::iterate(tmp.data(),static_cast<double>(MIN_VALUE()));

        // Convert to this color's data type (the const-cast is required to call
        // the correct iterate() method).
        meta::LoopFwd<N,meta::OpAssign>::iterate(data(),const_cast<double const*>(tmp.data()));
    }

    //@}

    /**
     * \name RGB(A) color model channel access methods
     */
    //@{

    /// Returns the red channel.
    T getR() {
        return Base::data()[0];
    }

    /// Returns a constant reference to the red channel.
    T const& getR() const {
        return Base::data()[0];
    }

    /// Assigns a new value to the red channel.
    void setR(T const r) {
        Base::data()[0]=r;
    }

    /// Returns the green channel.
    T getG() {
        return Base::data()[1];
    }

    /// Returns a constant reference to the green channel.
    T const& getG() const {
        return Base::data()[1];
    }

    /// Assigns a new value to the green channel.
    void setG(T const g) {
        Base::data()[1]=g;
    }

    /// Returns the blue channel.
    T getB() {
        return Base::data()[2];
    }

    /// Returns a constant reference to the blue channel.
    T const& getB() const {
        return Base::data()[2];
    }

    /// Assigns a new value to the blue channel.
    void setB(T const b) {
        Base::data()[2]=b;
    }

    /// Returns the alpha channel.
    T getA() {
        G_ASSERT(N>=4)
        return Base::data()[3];
    }

    /// Returns a constant reference to the alpha channel.
    T const& getA() const {
        G_ASSERT(N>=4)
        return Base::data()[3];
    }

    /// Assigns a new value to the alpha channel.
    void setA(T const a) {
        G_ASSERT(N>=4)
        Base::data()[3]=a;
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns the complementary color of color \a c.
    friend Color operator!(Color const& c) {
        return c.complement();
    }

    /// Returns the additive mix between colors \a a and \a b.
    friend Color operator&(Color const& a,Color const& b) {
        return a.mixAdd(b);
    }

    /// Returns the subtractive mix between colors \a a and \a b.
    friend Color operator|(Color const& a,Color const& b) {
        return a.mixSub(b);
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns a blend of 5 colors that match this color to form a \a palette.
    void blend(ColorModelHSV(&palette)[5]) {
        ColorModelHSV hsv(*this);

        double h,s,v;

        if (hsv.getV()>70) {
            v=hsv.getV()-30;
        }
        else {
            v=hsv.getV()+30;
        }

        palette[0].setH(hsv.getH());
        palette[0].setS(hsv.getS());
        palette[0].setV(v);

        palette[2].setV(v);

        if (hsv.getH()>=0 && hsv.getH()<30) {
            palette[1].setH(hsv.getH()+30);
            palette[1].setS(hsv.getS());
            palette[1].setV(hsv.getV());

            palette[2].setH(hsv.getH()+30);
            palette[2].setS(hsv.getS());
        }
        else if (hsv.getH()>=30 && hsv.getH()<60) {
            palette[1].setH(hsv.getH()+150);
            palette[1].setS(clamp(hsv.getS()-30,0.0,100.0));
            palette[1].setV(clamp(hsv.getV()-20,0.0,100.0));

            palette[2].setH(hsv.getH()+150);
            palette[2].setS(clamp(hsv.getS()-50,0.0,100.0));

            // Overwrite the previously set value.
            palette[2].setV(clamp(hsv.getV()+20,0.0,100.0));
        }
        else if (hsv.getH()>=60 && hsv.getH()<180) {
            h=hsv.getH()-40;

            palette[1].setH(h);
            palette[1].setS(hsv.getS());
            palette[1].setV(hsv.getV());

            palette[2].setH(h);
            palette[2].setS(hsv.getS());
        }
        else if (hsv.getH()>=180 && hsv.getH()<220) {
            palette[1].setH(hsv.getH()-160);
            palette[1].setS(hsv.getS());
            palette[1].setV(hsv.getV());

            palette[2].setH(hsv.getH()-170);
            palette[2].setS(hsv.getS());
        }
        else if (hsv.getH()>=220 && hsv.getH()<300) {
            s=clamp(hsv.getS()-40,0.0,100.0);

            palette[1].setH(hsv.getH());
            palette[1].setS(s);
            palette[1].setV(hsv.getV());

            palette[2].setH(hsv.getH());
            palette[2].setS(s);
        }
        else {
            h=wrap(hsv.getH()+20,0.0,360.0);

            if (hsv.getS()>50) {
                s=hsv.getS()-40;
            }
            else {
                s=hsv.getS()+40;
            }

            palette[1].setH(h);
            palette[1].setS(s);
            palette[1].setV(hsv.getV());

            palette[2].setH(h);
            palette[2].setS(s);
        }

        palette[3].setH(0);
        palette[3].setS(0);
        palette[3].setV(100-hsv.getV());

        palette[4].setH(0);
        palette[4].setS(0);
        palette[4].setV(hsv.getV());
    }

    /// Returns the color's complementary color. As not only hue, but also
    /// saturation and value are taken into account, different tints or shades
    /// of the same pure color return different complementary colors. An alpha
    /// channel is ignored, if present. See
    /// http://en.wikipedia.org/wiki/Complementary_color.
    Color complement() const {
        Color tmp=WHITE()-*this+BLACK();

// Warning C4127: Conditional expression is constant.
#pragma warning(disable:4127)

        if (N==4) {
            tmp.setA(getA());
        }

#pragma warning(default:4127)

        return tmp;
    }

    //@}

    /**
     * \name Mixing methods, see http://en.wikipedia.org/wiki/Color_mixing.
     */
    //@{

    /// Returns the additive mix between this color and color \a c in ratio \a s.
    Color mixAdd(Color const& c,double s=0.5) const {
        Color<N,double> mix=(Color<N,double>(*this)*(1-s)+Color<N,double>(c)*s)*2;

// Warning C4127: Conditional expression is constant.
#pragma warning(disable:4127)

        if (N==4) {
            // Undo the multiplication by 2 from above for the alpha channel.
            mix.setA(mix.getA()*0.5);
        }

#pragma warning(default:4127)

        // Normalize the result to the unit color cube.
        double max=mix.maxElement();
        if (max>1.0) {
            mix/=max;
        }

        return Color(mix);
    }

    /// Returns the subtractive mix between this color and color \a c in ratio \a s.
    Color mixSub(Color const& c,double s=0.5) const {
        // The complements of the subtractive primary colors are the additive
        // primary colors and vice versa.
        Color tCMY=complement(),cCMY=c.complement();
        return tCMY.mixAdd(cCMY,s).complement();
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

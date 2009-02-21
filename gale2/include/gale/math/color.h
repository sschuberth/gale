/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2009  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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
 * An implementation to represent colors
 */

#include "tuple.h"

namespace gale {

namespace math {

/**
 * Color helper class with only a single template parameter. This is needed
 * because partial specialization of member functions of template classes with
 * multiple template parameters is not supported by C++. Using this class we can
 * fully specialize only those class template member functions that should not
 * be implicitly obtained from the base template.
 */
template<typename T>
struct ColorModel
{
    /// Returns the minimum intensity value allowed for a color channel.
    static T MIN_INTENSITY() {
        return Numerics<T>::MIN();
    }

    /// Returns the maximum intensity value allowed for a color channel.
    static T MAX_INTENSITY() {
        return Numerics<T>::MAX();
    }

    /// Converts a color representation from the RGB to the HSV model. The RGB
    /// input channel values have to be in the range from MIN_INTENSITY() to
    /// MAX_INTENSITY(). The HSV output channel values will be in range
    /// [0,360[ for hue, and in range [0,100] for saturation and value.
    static void RGB2HSV(T const r,T const g,T const b,float& h,float& s,float& v) {
        // Convert RGB values to range [0,1].
        T const range=MAX_INTENSITY()-MIN_INTENSITY();

        float R=clamp(float(r-MIN_INTENSITY())/range,0.0f,1.0f);
        float G=clamp(float(g-MIN_INTENSITY())/range,0.0f,1.0f);
        float B=clamp(float(b-MIN_INTENSITY())/range,0.0f,1.0f);

        float m=min(R,G,B);
        float V=max(R,G,B);

        float delta=V-m;
        float S=(V>0)?(delta/V):0;

        float H;

        if (S>0) {
            float rd=(V-R)/delta;
            float gd=(V-G)/delta;
            float bd=(V-B)/delta;

            if (R==V) {
                // Red channel is the maximum, determine the minimum.
                H=(G==m)?(5+bd):(1-gd);
            }
            else if (G==V) {
                // Green channel is the maximum, determine the minimum.
                H=(B==m)?(1+rd):(3-bd);
            }
            else {
                // Blue channel is the maximum, determine the minimum.
                H=(R==m)?(3+gd):(5-rd);
            }

            H=wrap(H,0.0f,6.0f);
        }
        else {
            // Achromatic case, actually hue is undefined!
            H=0;
        }

        // At this point hue is in range [0,6[, saturation and value in range
        // [0,1]. Convert the first to degrees and the latter two to percent.
        h=H*60;
        s=S*100;
        v=V*100;
    }

    /// Converts a color representation from the HSV to the RGB model. The HSV
    /// input channel values have to be in range [0,360[ for hue, and in range
    /// [0,100] for saturation and value. The RGB output channel values will be
    /// in the range from MIN_INTENSITY() to MAX_INTENSITY().
    static void HSV2RGB(float const h,float const s,float const v,T& r,T& g,T& b) {
        // Convert H value to range [0,6[ and SV values to range [0,1].
        float H=wrap(h/60,0.0f,6.0f);
        float S=clamp(s/100,0.0f,1.0f);
        float V=clamp(v/100,0.0f,1.0f);

        float R,G,B;

        if (S>0) {
            long long i=roundToZero(H);
            float f=H-i;

            float p=V*(1-S);
            float q=V*(1-S*f);
            float t=V*(1-S*(1-f));

            switch (i) {
                default: {
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
        }
        else {
            // Achromatic case, actually hue is undefined!
            R=G=B=V;
        }

        // At this point red, green and blue are in range [0,1]. Convert them
        // to the intensity range.
        T const range=MAX_INTENSITY()-MIN_INTENSITY();

        r=T(R*range+MIN_INTENSITY());
        g=T(G*range+MIN_INTENSITY());
        b=T(B*range+MIN_INTENSITY());
    }
};

/// \cond DOXYGEN_IGNORE
template<>
inline float ColorModel<float>::MIN_INTENSITY()
{
    return 0;
}

template<>
inline float ColorModel<float>::MAX_INTENSITY()
{
    return 1;
}

template<>
inline double ColorModel<double>::MIN_INTENSITY()
{
    return 0;
}

template<>
inline double ColorModel<double>::MAX_INTENSITY()
{
    return 1;
}
/// \endcond

/**
 * Color representation based on an RGB(A)-tuple. It features predefined
 * constants for commonly used colors, HSV conversion and useful operators.
 *
 * OpenGL example usage:
 * \code
 * Col4ub colors[8];
 * glColorPointer(4,GL_UNSIGNED_BYTE,0,colors);
 * \endcode
 */
template<unsigned int N,typename T>
class Color:public TupleBase<N,T,Color<N,T> >,public ColorModel<T>
{
    //@{

    /// These type definitions simplify base class access to identifiers that
    /// are not visible until instantiation time because they do not dependent
    /// on template arguments.
    typedef TupleBase<N,T,Color<N,T> > Base;
    typedef ColorModel<T> Model;

    //@}

  public:

    /**
     * \name Predefined constants
     * In order to avoid the so called "static initialization order fiasco",
     * static methods instead of static variables are used here, see
     * <http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14>.
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

  protected:

    /// Sets each color channel to either the maximum or minimum intensity
    /// depending on the bits set in \a mask. Bit 0 maps to the first channel,
    /// bit 1 to second one and so on. This constructor is required to
    /// initialize the static class constants.
    explicit Color(unsigned int mask)
    :   m_hsv_outdated(true)
    {
        meta::LoopFwd<N,meta::OpAssign>
            ::iterateIndexMask(Base::data(),mask,Model::MAX_INTENSITY(),Model::MIN_INTENSITY());
    }

  public:

    /// Initialize to black by default.
    Color() {
        *this=BLACK();
    }

    /// Allows to initialize 3-channel colors directly.
    Color(T const r,T const g,T const b)
    :   Base(r,g,b),m_hsv_outdated(true) {}

    /// Allows to initialize 4-channel colors directly.
    Color(T const r,T const g,T const b,T const a)
    :   Base(r,g,b,a),m_hsv_outdated(true) {}

    /// Converts a color of different type but with the same amount of channels
    /// to this type.
    template<typename U>
    Color(Color<N,U> const& v)
    :   m_hsv_outdated(true)
    {
        meta::LoopFwd<N,meta::OpAssign>::iterate(Base::data(),v.data());
    }

    //@}

    /**
     * \name RGB(A) color model channel access methods
     */
    //@{

    /// Assigns new values to the red, green and blue channels.
    void setRGB(T const r,T const g,T const b) {
        G_ASSERT(N>=3)
        Base::data()[0]=r;
        Base::data()[1]=g;
        Base::data()[2]=b;
        m_hsv_outdated=true;
    }

    /// Assigns new values to the red, green, blue and alpha channels.
    void setRGBA(T const r,T const g,T const b,T const a) {
        G_ASSERT(N>=4)
        Base::data()[0]=r;
        Base::data()[1]=g;
        Base::data()[2]=b;
        Base::data()[3]=a;
        m_hsv_outdated=true;
    }

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
        m_hsv_outdated=true;
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
        m_hsv_outdated=true;
    }

    /// Returns the blue channel.
    T getB() {
        G_ASSERT(N>=3)
        return Base::data()[2];
    }

    /// Returns a constant reference to the blue channel.
    T const& getB() const {
        G_ASSERT(N>=3)
        return Base::data()[2];
    }

    /// Assigns a new value to the blue channel.
    void setB(T const b) {
        G_ASSERT(N>=3)
        Base::data()[2]=b;
        m_hsv_outdated=true;
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
     * \name HSV color model channel access methods
     */
    //@{

    /// Assigns new values to the hue, saturation and value channels.
    void setHSV(float const h,float const s,float const v) {
        G_ASSERT(N>=3)
        HSV2RGB(m_h=h,m_s=s,m_v=v,Base::data()[0],Base::data()[1],Base::data()[2]);
    }

    /// Assigns new values to the hue, saturation, value and alpha channels.
    void setHSVA(float const h,float const s,float const v,T const a) {
        G_ASSERT(N>=4)
        HSV2RGB(m_h=h,m_s=s,m_v=v,Base::data()[0],Base::data()[1],Base::data()[2]);
        Base::data()[3]=a;
    }

    /// Returns the hue channel.
    float getH() {
        updateHSV();
        return m_h;
    }

    /// Assigns a new value to the hue channel.
    void setH(float const h) {
        updateHSV();
        HSV2RGB(m_h=h,m_s,m_v,Base::data()[0],Base::data()[1],Base::data()[2]);
    }

    /// Returns the saturation channel.
    float getS() {
        updateHSV();
        return m_s;
    }

    /// Assigns a new value to the saturation channel.
    void setS(float const s) {
        updateHSV();
        HSV2RGB(m_h,m_s=s,m_v,Base::data()[0],Base::data()[1],Base::data()[2]);
    }

    /// Returns the value channel.
    float getV() {
        G_ASSERT(N>=3)
        updateHSV();
        return m_v;
    }

    /// Assigns a new value to the value channel.
    void setV(float const v) {
        G_ASSERT(N>=3)
        updateHSV();
        HSV2RGB(m_h,m_s,m_v=v,Base::data()[0],Base::data()[1],Base::data()[2]);
    }

    //@}

    /**
     * \name Convenience operators for named methods
     */
    //@{

    /// Returns an inverted copy of color \a c.
    friend Color operator!(Color const& c) {
        return Color(c).inverse();
    }

    /// Returns the additive mix between colors \a a and \a b.
    friend Color operator&(Color const& a,Color const& b) {
        return Color(a).mixAdd(b);
    }

    /// Returns the subtractive mix between colors \a a and \a b.
    friend Color operator|(Color const& a,Color const& b) {
        return Color(a).mixSub(b);
    }

    //@}

    /**
     * \name Miscellaneous methods
     */
    //@{

    /// Returns a blend of 5 colors that match this color to form a \a palette.
    void blend(Color (&palette)[5]) {
        G_ASSERT(N>=3)

        updateHSV();

        float h,s,v;

        if (m_v>70) {
            v=m_v-30;
        }
        else {
            v=m_v+30;
        }

        palette[0].setH(m_h);
        palette[0].setS(m_s);
        palette[0].setV(v);

        palette[2].setV(v);

        if (m_h>=0 && m_h<30) {
            palette[1].setH(m_h+30);
            palette[1].setS(m_s);
            palette[1].setV(m_v);

            palette[2].setH(m_h+30);
            palette[2].setS(m_s);
        }
        else if (m_h>=30 && m_h<60) {
            palette[1].setH(m_h+150);
            palette[1].setS(clamp(m_s-30,0.0f,100.0f));
            palette[1].setV(clamp(m_v-20,0.0f,100.0f));

            palette[2].setH(m_h+150);
            palette[2].setS(clamp(m_s-50,0.0f,100.0f));
            palette[2].setV(clamp(m_v+20,0.0f,100.0f));
        }
        else if (m_h>=60 && m_h<180) {
            h=m_h-40;

            palette[1].setH(h);
            palette[1].setS(m_s);
            palette[1].setV(m_v);

            palette[2].setH(h);
            palette[2].setS(m_s);
        }
        else if (m_h>=180 && m_h<220) {
            palette[1].setH(m_h-160);
            palette[1].setS(m_s);
            palette[1].setV(m_v);

            palette[2].setH(m_h-170);
            palette[2].setS(m_s);
        }
        else if (m_h>=220 && m_h<300) {
            s=clamp(m_s-40,0.0f,100.0f);

            palette[1].setH(m_h);
            palette[1].setS(s);
            palette[1].setV(m_v);

            palette[2].setH(m_h);
            palette[2].setS(s);
        }
        else {
            h=wrap(m_h+20,0.0f,360.0f);

            if (m_s>50) {
                s=m_s-40;
            }
            else {
                s=m_s+40;
            }

            palette[1].setH(h);
            palette[1].setS(s);
            palette[1].setV(m_v);

            palette[2].setH(h);
            palette[2].setS(s);
        }

        palette[3].setHSV(0,0,100-m_v);
        palette[4].setHSV(0,0,m_v);
    }

    /// Returns the complementary color of this color.
    Color complement() {
        Color c(*this);
        c.setH(wrap(getH()+180,T(0),T(360)));
        return c;
    }

    /// Returns the color's inverse (the alpha channel is omitted, if present).
    Color inverse() const {
        // Normalize the color to range [0,1].
        Color<N,double> tD(*this);
        tD-=BLACK();

        T const range=MAX_INTENSITY()-MIN_INTENSITY();
        tD/=range;

        // This can be seen as converting from RGB to CMY, meaning that "iD"
        // represents the same color in the CMY model as "tD" in the RGB model.
        Color<N,double> iD=Color<N,double>::WHITE()-tD;

// Warning C4127: Conditional expression is constant.
#pragma warning(disable:4127)

        if (N==4) {
            iD.setA(tD.getA());
        }

#pragma warning(default:4127)

        // Map the result back to the original range.
        iD*=range;
        iD+=BLACK();

        return iD;
    }

    //@}

    /**
     * \name Mixing methods
     */
    //@{

    /// Returns the additive mix between this color and color \a c.
    Color mixAdd(Color const& c) {
        // Normalize the colors to range [0,1] and mix them.
        Color<N,double> tD(*this);
        tD-=BLACK();

        Color<N,double> cD(c);
        cD-=BLACK();

        T const range=MAX_INTENSITY()-MIN_INTENSITY();
        Color<N,double> mD=(tD+cD)/range;

        // Normalize the result to the unit color cube.
        double me=mD.maxElement();
        if (me>1.0) {
            mD/=me;
        }

        // Map the result back to the original range.
        mD*=range;
        mD+=BLACK();

        return mD;
    }

    /// Returns the subtractive mix between this color and color \a c.
    Color mixSub(Color const& c) {
        Color tCMY=inverse(),cCMY=c.inverse();
        return tCMY.mixAdd(cCMY).inverse();
    }

    //@}

  private:

    /// Updates the HSV channels to match the RGB channels if required.
    void updateHSV() {
        if (!m_hsv_outdated) {
            return;
        }
        RGB2HSV(Base::data()[0],Base::data()[1],Base::data()[2],m_h,m_s,m_v);
        m_hsv_outdated=false;
    }

    /// Dirty-flag for the HSV channel variables. The RGB channel variables are
    /// always kept up to date.
    bool m_hsv_outdated;

    float m_h; ///< Variable for the "hue" channel value.
    float m_s; ///< Variable for the "saturation" channel value.
    float m_v; ///< Variable for the "value" channel value.
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

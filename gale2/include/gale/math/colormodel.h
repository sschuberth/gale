/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
 *
 * Copyright (C) 2005-2010  Sebastian Schuberth <sschuberth_AT_gmail_DOT_com>
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

#ifndef COLORMODEL_H
#define COLORMODEL_H

/**
 * \file
 * Color model representations
 */

#include "color.h"
#include "interpolator.h"

namespace gale {

namespace math {

/**
 * Base class for color model representations different than RGB.
 */
class G_NO_VTABLE ColorModel
{
  public:

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    virtual void fromRGB(double const r,double const g,double const b)=0;

    /// Gets the \a r, \a g, \a b values from the color model, where all values
    /// are in range [0,1].
    virtual void toRGB(double& r,double& g,double& b) const=0;

    /// Sets the color model to the specified RGB \a color.
    template<unsigned int N,typename T>
    void setRGB(Color<N,T> const& color) {
        // Map RGB to range [0,1].
        Color<N,double> rgb(color);

        // Convert to the color model.
        fromRGB(rgb.getR(),rgb.getG(),rgb.getB());
    }

    /// Gets the RGB representation of the color model for color class \c C.
    template<class C>
    C getRGB() const {
        // Convert from the color model.
        Color<C::Channels,double> rgb;
        toRGB(rgb[0],rgb[1],rgb[2]);

        // Map RGB to the color's range.
        return C(rgb);
    }
};

/**
 * An HSV (also known as HSB) color model representation.
 */
class ColorModelHSV:public ColorModel
{
  public:

    /// Initializes the color model to black.
    ColorModelHSV()
    :   m_h(0),m_s(0),m_v(0)
    {}

    /// Initializes the color model to the specified RGB \a color.
    template<unsigned int N,typename T>
    ColorModelHSV(Color<N,T> const& color) {
        setRGB(color);
    }

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    void fromRGB(double const r,double const g,double const b) {
        double m=min(r,g,b);
        double V=max(r,g,b);

        double delta=V-m;

        double H,S;

        if (delta) {
            // If delta>0, it is V>0, too.
            S=delta/V;

            if (r==V) {
                H=(g-b)/delta;
            }
            else if (g==V) {
                H=(b-r)/delta+2;
            }
            else {
                H=(r-g)/delta+4;
            }
        }
        else {
            // Achromatic case (gray), actually hue is undefined!
            H=S=0;
        }

        // Map H to range [0,360[ and SV to range [0,100].
        m_h=wrap(H*60.0,360.0);
        m_s=S*100.0;
        m_v=V*100.0;
    }

    /// Gets the \a r, \a g, \a b values from the color model, where all values
    /// are in range [0,1].
    void toRGB(double& r,double& g,double& b) const {
        // Map H to range [0,6[ and SV to range [0,1].
        double H=wrap(m_h/60.0,6.0);
        double S=clamp(m_s/100.0,0.0,1.0);
        double V=clamp(m_v/100.0,0.0,1.0);

        // Convert to RGB.
        if (S) {
            int i=static_cast<int>(roundToZero(H));
            double f=H-i;

            double p=V*(1-S);
            double q=V*(1-S*f);
            double t=V*(1-S*(1-f));

            switch (i) {
                default: {
                    r=V; g=t; b=p;
                    break;
                }
                case 1: {
                    r=q; g=V; b=p;
                    break;
                }
                case 2: {
                    r=p; g=V; b=t;
                    break;
                }
                case 3: {
                    r=p; g=q; b=V;
                    break;
                }
                case 4: {
                    r=t; g=p; b=V;
                    break;
                }
                case 5: {
                    r=V; g=p; b=q;
                    break;
                }
            }
        }
        else {
            // Achromatic case (gray), actually hue is undefined!
            r=g=b=V;
        }
    }

    /**
     * \name Color model access methods
     */
    //@{

    /// Returns the hue.
    double getH() {
        return m_h;
    }

    /// Returns a constant reference to the hue.
    double const& getH() const {
        return m_h;
    }

    /// Assigns a new hue.
    void setH(double const h) {
        m_h=wrap(h,360.0);
    }

    /// Returns the saturation.
    double getS() {
        return m_s;
    }

    /// Returns a constant reference to the saturation.
    double const& getS() const {
        return m_s;
    }

    /// Assigns a new saturation.
    void setS(double const s) {
        m_s=clamp(s,0.0,100.0);
    }

    /// Returns the value.
    double getV() {
        return m_v;
    }

    /// Returns a constant reference to the value.
    double const& getV() const {
        return m_v;
    }

    /// Assigns a new value.
    void setV(double const v) {
        m_v=clamp(v,0.0,100.0);
    }

    //@}

  private:

    double m_h; ///< The color's hue.
    double m_s; ///< The color's saturation.
    double m_v; ///< The color's value.
};

/**
 * An RYB color model representation, see
 * <http://threekings.tk/mirror/ryb_TR.pdf>.
 */
class ColorModelRYB:public ColorModel
{
  public:

    /// Initializes the color model to black.
    ColorModelRYB()
    :   m_r(1),m_y(1),m_b(1)
    {}

    /// Initializes the color model to the specified RGB \a color.
    template<unsigned int N,typename T>
    ColorModelRYB(Color<N,T> const& color) {
        setRGB(color);
    }

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    void fromRGB(double const r,double const g,double const b) {
    }

    /// Gets the \a r, \a g, \a b values from the color model, where all values
    /// are in range [0,1].
    void toRGB(double& r,double& g,double& b) const {
        // Pair-wise interpolate RGB corner values along the B-axis of the RYB
        // interpolation cube.
        Tup3d b00=cubic(WHITE()  , BLUE()   , m_b);
        Tup3d b10=cubic(YELLOW() , GREEN()  , m_b);
        Tup3d b01=cubic(RED()    , PURPLE() , m_b);
        Tup3d b11=cubic(ORANGE() , BLACK()  , m_b);

        // Pair-wise interpolate RGB edge values along the Y-axis of the RYB
        // interpolation cube.
        Tup3d y0=cubic(b00,b10,m_y);
        Tup3d y1=cubic(b01,b11,m_y);

        // Pair-wise interpolate RGB face values along the R-axis of the RYB
        // interpolation cube.
        Tup3d rgb=cubic(y0,y1,m_r);

        r=rgb[0];
        g=rgb[1];
        b=rgb[2];
    }

    /**
     * \name Color model access methods
     */
    //@{

    /// Returns the red percentage.
    double getR() {
        return m_r;
    }

    /// Returns a constant reference to the red percentage.
    double const& getR() const {
        return m_r;
    }

    /// Assigns a new red percentage.
    void setR(double const r) {
        m_r=clamp(r,0.0,1.0);
    }

    /// Returns the yellow percentage.
    double getY() {
        return m_y;
    }

    /// Returns a constant reference to the yellow percentage.
    double const& getY() const {
        return m_y;
    }

    /// Assigns a new yellow percentage.
    void setY(double const y) {
        m_y=clamp(y,0.0,1.0);
    }

    /// Returns the blue percentage.
    double getB() {
        return m_b;
    }

    /// Returns a constant reference to the blue percentage.
    double const& getB() const {
        return m_b;
    }

    /// Assigns a new blue percentage.
    void setB(double const b) {
        m_b=clamp(b,0.0,1.0);
    }

    //@}

  private:

    /// Returns the RGB coordinates for the white corner in the RYB
    /// interpolation cube.
    static Tup3d const& WHITE() {
        static Tup3d t(1.0,1.0,1.0);
        return t;
    }

    /// Returns the RGB coordinates for the red corner in the RYB
    /// interpolation cube.
    static Tup3d const& RED() {
        static Tup3d t(1.0,0.0,0.0);
        return t;
    }

    /// Returns the RGB coordinates for the yellow corner in the RYB
    /// interpolation cube.
    static Tup3d const& YELLOW() {
        static Tup3d t(1.0,1.0,0.0);
        return t;
    }

    /// Returns the RGB coordinates for the blue corner in the RYB
    /// interpolation cube.
    static Tup3d const& BLUE() {
        static Tup3d t(0.163,0.373,0.6);
        return t;
    }

    /// Returns the RGB coordinates for the black corner in the RYB
    /// interpolation cube.
    static Tup3d const& BLACK() {
        static Tup3d t(0.2,0.094,0.0);
        return t;
    }

    /// Returns the RGB coordinates for the green corner in the RYB
    /// interpolation cube.
    static Tup3d const& GREEN() {
        static Tup3d t(0.0,0.66,0.2);
        return t;
    }

    /// Returns the RGB coordinates for the purple corner in the RYB
    /// interpolation cube.
    static Tup3d const& PURPLE() {
        // NOTE: There is a typo in the original paper which erroneously defines
        // purple as (0.5,0.5,0.0), this has been fixed in the technical report!
        static Tup3d t(0.5,0.0,0.5);
        return t;
    }

    /// Returns the RGB coordinates for the orange corner in the RYB
    /// interpolation cube.
    static Tup3d const& ORANGE() {
        static Tup3d t(1.0,0.5,0.0);
        return t;
    }

    double m_r; ///< The color's red percentage.
    double m_y; ///< The color's yellow percentage.
    double m_b; ///< The color's blue percentage.
};

} // namespace math

} // namespace gale

#endif // COLORMODEL_H

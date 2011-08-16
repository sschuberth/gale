/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <http://developer.berlios.de/projects/gale/>.
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
 * Color space representations, see <http://en.wikipedia.org/wiki/Color_space>.
 */

#include "color.h"

namespace gale {

namespace math {

/**
 * Base class for color spaces based on the RGB color model.
 */
class G_NO_VTABLE ColorSpace
{
  public:

    /// Sets the color space to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    virtual void fromRGB(double const r,double const g,double const b)=0;

    /// Gets the \a r, \a g, \a b values from the color space, where all values
    /// are in range [0,1].
    virtual void toRGB(double& r,double& g,double& b) const=0;

    /// Sets the color space to the specified RGB \a color.
    template<unsigned int N,typename T>
    void setRGB(Color<N,T> const& color) {
        // Map RGB to range [0,1].
        Color<N,double> rgb(color);

        // Convert to the color space.
        fromRGB(rgb.getR(),rgb.getG(),rgb.getB());
    }

    /// Gets the RGB representation of the color space for color class \c C.
    template<class C>
    C getRGB() const {
        // Convert from the color space.
        Color<C::Channels,double> rgb;
        toRGB(rgb[0],rgb[1],rgb[2]);

        // Map RGB to the color's range.
        return C(rgb);
    }
};

/**
 * An HSV (also known as HSB) color space representation, see
 * <http://en.wikipedia.org/wiki/List_of_color_spaces_and_their_uses#HSV>.
 */
class ColorSpaceHSV:public ColorSpace
{
  public:

    /// Initializes the color space to the given \a hue, \a saturation and
    /// \a value, or to black by default.
    ColorSpaceHSV(double const hue=0,double const saturation=0,double const value=0)
    :   m_h(hue),m_s(saturation),m_v(value)
    {}

    /// Initializes the color space to the specified RGB \a color.
    template<unsigned int N,typename T>
    ColorSpaceHSV(Color<N,T> const& color) {
        setRGB(color);
    }

    /// Sets the color space to match the given \a r, \a g, \a b values, where
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

    /// Gets the \a r, \a g, \a b values from the color space, where all values
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
     * \name Color space access methods
     */
    //@{

    /// Returns a constant reference to the hue.
    double const& getH() const {
        return m_h;
    }

    /// Assigns a new \a hue.
    void setH(double const hue) {
        m_h=wrap(hue,360.0);
    }

    /// Returns a constant reference to the saturation.
    double const& getS() const {
        return m_s;
    }

    /// Assigns a new \a saturation.
    void setS(double const saturation) {
        m_s=clamp(saturation,0.0,100.0);
    }

    /// Returns a constant reference to the value.
    double const& getV() const {
        return m_v;
    }

    /// Assigns a new \a value.
    void setV(double const value) {
        m_v=clamp(value,0.0,100.0);
    }

    //@}

  private:

    double m_h; ///< The color's hue in range [0,360[.
    double m_s; ///< The color's saturation in range [0,100].
    double m_v; ///< The color's value in range [0,100].
};

} // namespace math

} // namespace gale

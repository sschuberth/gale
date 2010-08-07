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

namespace gale {

namespace math {

/**
 * Base class for color model representations different than RGB.
 */
class ColorModel
{
  public:

    /// Initializes the color model with an RGB \a color.
    template<unsigned int N,typename T>
    void init(Color<N,T> const& color) {
        // Map RGB to range [0,1].
        float r=clamp((color.getR()-color.MIN_VALUE())/color.RANGE(),0.0f,1.0f);
        float g=clamp((color.getG()-color.MIN_VALUE())/color.RANGE(),0.0f,1.0f);
        float b=clamp((color.getB()-color.MIN_VALUE())/color.RANGE(),0.0f,1.0f);

        // Convert to the color model.
        fromRGB(r,g,b);
    }

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    virtual void fromRGB(float const r,float const g,float const b)=0;

    /// Gets the \a r, \a g, \a b values from the color model, where all values
    /// are in range [0,1].
    virtual void toRGB(float& r,float& g,float& b)=0;

    /// Converts the color model to RGB color class \c C with proper range mapping.
    template<class C>
    C rgb() {
        // Convert from the color model.
        float r,g,b;
        toRGB(r,g,b);

        // Map RGB to the color's range.
        C c;
        c.setR(static_cast<C::Type>(C::MIN_VALUE()+r*C::RANGE()));
        c.setG(static_cast<C::Type>(C::MIN_VALUE()+g*C::RANGE()));
        c.setB(static_cast<C::Type>(C::MIN_VALUE()+b*C::RANGE()));

        return c;
    }
};

/**
 * An HSV color model representation.
 */
class ColorModelHSV:public ColorModel
{
  public:

    /// Initializes the color model to black.
    ColorModelHSV()
    :   m_h(0),m_s(0),m_v(0)
    {}

    /// Initializes the color model with an RGB \a color.
    template<unsigned int N,typename T>
    ColorModelHSV(Color<N,T> const& color) {
        init(color);
    }

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    void fromRGB(float const r,float const g,float const b) {
        float m=min(r,g,b);
        float V=max(r,g,b);

        float delta=V-m;

        float H,S;

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
        m_h=wrap(H*60.0f,360.0f);
        m_s=S*100.0f;
        m_v=V*100.0f;
    }

    /// Gets the \a r, \a g, \a b values from the color model, where all values
    /// are in range [0,1].
    void toRGB(float& r,float& g,float& b) {
        // Map H to range [0,6[ and SV to range [0,1].
        float H=wrap(m_h/60.0f,6.0f);
        float S=clamp(m_s/100.0f,0.0f,1.0f);
        float V=clamp(m_v/100.0f,0.0f,1.0f);

        // Convert to RGB.
        if (S) {
            int i=roundToZero(H);
            float f=H-i;

            float p=V*(1-S);
            float q=V*(1-S*f);
            float t=V*(1-S*(1-f));

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
    float getH() {
        return m_h;
    }

    /// Returns a constant reference to the hue.
    float const& getH() const {
        return m_h;
    }

    /// Assigns a new hue.
    void setH(float const h) {
        m_h=wrap(h,360.0f);
    }

    /// Returns the saturation.
    float getS() {
        return m_s;
    }

    /// Returns a constant reference to the saturation.
    float const& getS() const {
        return m_s;
    }

    /// Assigns a new saturation.
    void setS(float const s) {
        m_s=clamp(s,0.0f,100.0f);
    }

    /// Returns the value.
    float getV() {
        return m_v;
    }

    /// Returns a constant reference to the value.
    float const& getV() const {
        return m_v;
    }

    /// Assigns a new value.
    void setV(float const v) {
        m_v=clamp(v,0.0f,100.0f);
    }

    //@}

  private:

    float m_h; ///< The color's hue.
    float m_s; ///< The color's saturation.
    float m_v; ///< The color's value.
};

} // namespace math

} // namespace gale

#endif // COLORMODEL_H

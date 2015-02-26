/*                                     __
 *                      .-----..---.-.|  |.-----.
 *                      |  _  ||  _  ||  ||  -__|
 *                      |___  ||___._||__||_____|
 * This file is part of |_____| the Graphics Abstraction Layer & Engine,
 * see the project page at <https://sourceforge.net/projects/gale-opengl/>.
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
 * Color model representations, see <http://en.wikipedia.org/wiki/Color_model>.
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
 * An HSV (also known as HSB) color model representation, see
 * <http://en.wikipedia.org/wiki/HSL_and_HSV>.
 */
class ColorModelHSV:public ColorModel
{
  public:

    /// Initializes the color model to the given \a hue, \a saturation and
    /// \a value, or to black by default.
    ColorModelHSV(double const hue=0,double const saturation=0,double const value=0)
    :   m_h(hue),m_s(saturation),m_v(value)
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

/**
 * An RYB color model representation, see
 * <http://threekings.tk/mirror/ryb_TR.pdf>.
 */
class ColorModelRYB:public ColorModel
{
  public:

    /**
     * \name Hue conversion methods, see
     * <http://www.daleroose.com/web_design/color_chart/>.
     */
    //@{

    /// Returns the hue \a h_rgb in the RGB color model for the color given by
    /// the hue \a h_ryb in the RYB color model.
    static void RYBHueToRGBHue(double const h_ryb,double& h_rgb) {
        ColorModelHSV hsv(h_ryb,100,100);

        // Abuse the HSV to RGB conversion: The amount of green for a given hue
        // in RGB matches the amount of yellow for the same hue in RYB.
        double r,y,b;
        hsv.toRGB(r,y,b);

        // Convert RYB to RGB.
        ColorModelRYB ryb(r,y,b);
        double g;
        ryb.toRGB(r,g,b);

        // Get the hue in RGB.
        hsv.fromRGB(r,g,b);
        h_rgb=hsv.getH();
    }

    /// Returns the hue \a h_ryb in the RYB color model for the color given by
    /// the hue \a h_rgb in the RGB color model. Arguments \a a and \a b are
    /// only used internally for the approximation using nested intervals.
    static void RGBHueToRYBHue(double const h_rgb,double& h_ryb,double const a=0,double const b=360) {
        // Guess an RYB hue ...
        h_ryb=(a+b)*0.5;

        // ... and convert it to an RGB hue ...
        double h_rgb_guessed;
        RYBHueToRGBHue(h_ryb,h_rgb_guessed);

        // ... to see how closely we match the given RGB hue ...
        double h_rgb_diff=h_rgb_guessed-h_rgb;
        if (abs(h_rgb_diff)<=Numd::ZERO_TOLERANCE()) {
            return;
        }
        // ... and adjust our guess accordingly, if required.
        else if (h_rgb_diff<0) {
            RGBHueToRYBHue(h_rgb,h_ryb,h_ryb,b);
        }
        else {
            RGBHueToRYBHue(h_rgb,h_ryb,a,h_ryb);
        }
    }

    //@}

    /// Initializes the color model to the given \a red, \a yellow and \a blue
    /// percentages, or to black by default.
    ColorModelRYB(double const red=1,double const yellow=1,double const blue=1)
    :   m_r(red),m_y(yellow),m_b(blue)
    {}

    /// Initializes the color model to the specified RGB \a color.
    template<unsigned int N,typename T>
    ColorModelRYB(Color<N,T> const& color) {
        setRGB(color);
    }

    /// Sets the color model to match the given \a r, \a g, \a b values, where
    /// all values need to be in range [0,1].
    void fromRGB(double const r,double const g,double const b) {
        // Convert RGB to HSV.
        ColorModelHSV hsv_rgb;
        hsv_rgb.fromRGB(r,g,b);

        // Get the RYB hue for the RGB hue.
        double h_ryb;
        RGBHueToRYBHue(hsv_rgb.getH(),h_ryb);

        // Abuse the HSV to RGB conversion: The amount of green for a given hue
        // in RGB matches the amount of yellow for the same hue in RYB.
        ColorModelHSV hsv_ryb(h_ryb,100,100);
        Tup3d ryb_hue_only;
        hsv_ryb.toRGB(ryb_hue_only[0],ryb_hue_only[1],ryb_hue_only[2]);

        Tup3d white(0,0,0);
        Tup3d black(1,1,1);
        Tup3d ryb_with_saturation=lerp(white,ryb_hue_only,hsv_rgb.getS()/100.0);
        Tup3d ryb=lerp(black,ryb_with_saturation,hsv_rgb.getV()/100.0);

        m_r=ryb[0];
        m_y=ryb[1];
        m_b=ryb[2];
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

    /// Returns a constant reference to the red percentage.
    double const& getR() const {
        return m_r;
    }

    /// Assigns a new \a red percentage.
    void setR(double const red) {
        m_r=clamp(red,0.0,1.0);
    }

    /// Returns a constant reference to the yellow percentage.
    double const& getY() const {
        return m_y;
    }

    /// Assigns a new \a yellow percentage.
    void setY(double const yellow) {
        m_y=clamp(yellow,0.0,1.0);
    }

    /// Returns a constant reference to the blue percentage.
    double const& getB() const {
        return m_b;
    }

    /// Assigns a new \a blue percentage.
    void setB(double const blue) {
        m_b=clamp(blue,0.0,1.0);
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

    double m_r; ///< The color's red percentage in range [0,1].
    double m_y; ///< The color's yellow percentage in range [0,1].
    double m_b; ///< The color's blue percentage in range [0,1].
};

} // namespace math

} // namespace gale

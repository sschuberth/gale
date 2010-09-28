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

#include "colorspace.h"
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
        ColorSpaceHSV hsv(h_ryb,100,100);

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

#endif // COLORMODEL_H

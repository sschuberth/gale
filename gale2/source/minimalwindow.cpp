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

#include "gale/wrapgl/minimalwindow.h"

using namespace gale::global;

namespace gale {

namespace wrapgl {

// TODO: Add Linux implementation.
#ifdef G_OS_WINDOWS

MinimalWindow::MinimalWindow(
    LPCTSTR title
,   int width
,   int height
,   global::AttributeListi const* const context_attr
,   global::AttributeListi const* const pixel_attr
,   int const samples
)
:   RenderSurface(context_attr,pixel_attr,samples)
,   m_close_requested(false)
,   m_timeout(0)
{
    // Manually post a WM_SIZE message as the one sent by SetWindowPos()
    // below does not reach the derived window's onResize() method because
    // virtual functions are not yet virtual during a class' construction, see
    // http://www.artima.com/cppsource/nevercall.html.
    PostMessage(windowHandle(),WM_SIZE,SIZE_RESTORED,MAKELPARAM(width,height));

    SetWindowText(windowHandle(),title);

    // Calculate the window size from the desired client area size.
    RECT rect={0,0,width,height};
    G_ASSERT_CALL(AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE))

    width=rect.right-rect.left;
    height=rect.bottom-rect.top;

    int x=(GetSystemMetrics(SM_CXSCREEN)-width)/2;
    int y=(GetSystemMetrics(SM_CYSCREEN)-height)/2;

    // Adjust the render surface's dummy window style, position and dimensions.
    SetWindowLongPtr(windowHandle(),GWL_STYLE,WS_OVERLAPPEDWINDOW);
    SetWindowPos(windowHandle(),HWND_TOP,x,y,width,height,SWP_FRAMECHANGED);
}

void MinimalWindow::processEvents()
{
    MSG msg;

    while (!m_close_requested) {
        // We need to use the non-blocking PeekMessage() here which causes high
        // CPU usage instead of the blocking GetMessage() because we want to be
        // able to do something during idle time.
        if (PeekMessage(&msg,m_window,0,0,PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // Idle if there are no messages to process.
            if (onIdle()) {
                repaint();
            }
            else {
                // Relinquish the rest of the time slice.
                Sleep(0);
            }
        }

        // If there is a timeout set ...
        if (m_timeout>0) {
            double elapsed;
            m_timer.elapsed(elapsed);
            // ... and the timeout elapsed, call the event handler.
            if (elapsed>=m_timeout) {
                onTimeout();
                m_timer.reset();
            }
        }
    }
}

LRESULT MinimalWindow::handleMessage(UINT const uMsg,WPARAM const wParam,LPARAM const lParam)
{
    switch (uMsg) {
        // This is sent to a window after its size has changed.
        case WM_SIZE: {
            onResize(LOWORD(lParam),HIWORD(lParam));
            break;
        }

        // This is sent to a window when a portion should be painted.
        case WM_PAINT: {
            onPaint();
            G_ASSERT_OPENGL

            // Make use of double-buffering.
            SwapBuffers(m_context.device);

            // Notify Windows that we have finished painting.
            ValidateRect(m_window,NULL);

            break;
        }

        // This is sent to a window when it should terminate.
        case WM_CLOSE: {
            onClose();

            // Delay the call to wglDeleteContext() until class destruction time
            // so other OpenGL wrapper classes can free their resources in their
            // destructors before the rendering context gets deleted.
            m_close_requested=true;

            break;
        }

        default: {
            // Handle more elementary messages in the base class.
            return RenderSurface::handleMessage(uMsg,wParam,lParam);
        }
    }

    return 0;
}

#endif // G_OS_WINDOWS

} // namespace wrapgl

} // namespace gale

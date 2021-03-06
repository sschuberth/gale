[![Build status](https://ci.appveyor.com/api/projects/status/jxoll6s40w0gind4/branch/master?svg=true)](https://ci.appveyor.com/project/sschuberth/gale/branch/master)

# Introduction

The "Graphics Abstraction Layer & Engine" (GALE) is an object oriented wrapper around OpenGL and its extensions. It is designed to be modern, lightweight and self-consistent (there is no CRT dependency). Typical targets are small graphical applications. You can probably think of it as something similar to (but far less extensive than) the great <a href="http://www.geometrictools.com/">Wild Magic</a> and <a href="http://g3d.sourceforge.net/">G3D Innovation Engine</a> libraries, with a bit of <a href="http://glew.sourceforge.net/">GLEW</a> and <a href="http://glm.g-truc.net/">GLM</a>, however with a focus on very compact programs.

The current version of the C++ library is <a href="https://github.com/sschuberth/gale/tree/master/gale2">GALE 2</a>, a complete rewrite of the abandoned GALE 1 which was previously hosted in SVN at SourceForge. GALE 2 has all new OpenGL wrappers and <a href="http://en.wikipedia.org/wiki/Polygon_mesh#Vertex-vertex_meshes">vertex-vertex mesh</a> classes with <a href="http://en.wikipedia.org/wiki/Subdivision_surface">subdivision</a> support. It makes heavy use of (meta-)templates for more generic code, better performance, as well as smaller code size, and also comes with minimal CRT replacement routines.
Not all of the GALE 1 functionality is available in GALE 2 yet (e.g. images and billboards), but on the other hand GALE 2 already provides a lot of functionality not available in GALE 1, like <a href="http://threekings.tk/gale/classgale_1_1wrapgl_1_1_frame_buffer_object.html">Frame Buffer Object wrappers</a> and <a href="http://threekings.tk/gale/classgale_1_1math_1_1_interpolator.html">interpolator classes</a>.

For building GALE 2, please refer to the <a href="https://github.com/sschuberth/gale/blob/master/gale2/prerequisites.txt">prerequisites</a>.

# Screenshots

## [demo_camera](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_camera.exe):

![demo_camera](gale2/demo/camera/screenshot.jpg)

## [demo_color](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_color.exe):

![demo_color](gale2/demo/color/screenshot.jpg)

## [demo_fbo](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_fbo.exe):

![demo_fbo](gale2/demo/fbo/screenshot.jpg)

## [demo_heartbeat](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_heartbeat.exe):

![demo_heartbeat](gale2/demo/heartbeat/screenshot.jpg)

## [demo_interpolator](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_interpolator.exe):

![demo_interpolator](gale2/demo/interpolator/screenshot.jpg)

## [demo_mesh](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_mesh.exe):

![demo_mesh](gale2/demo/mesh/screenshot.jpg)

## [demo_quaternion](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_quaternion.exe):

![demo_quaternion](gale2/demo/quaternion/screenshot.jpg)

## [demo_shader](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_shader.exe):

![demo_shader](gale2/demo/shader/screenshot.jpg)

## [demo_texture](https://ci.appveyor.com/api/projects/sschuberth/gale/artifacts/gale2/bin/Debug/demo_texture.exe):

![demo_texture](gale2/demo/texture/screenshot.jpg)

# TODO

* Implement demo_distfield.
* Leverage [Read The Docs](https://readthedocs.org/) (via [Breathe](https://breathe.readthedocs.org/)).

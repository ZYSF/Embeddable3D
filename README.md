# Embeddable3D
A simple software renderer for C.

## Usage

Basic rendering logic is provided in the header and a simple demo program is provided (it just prints a simple scene in ASCII and accepts rotation parameters).

To do things like display textures or render in a window you'll need to add some extra code.

## Features

* Does most of the work required for a useful software renderer
* Very simple (only a couple of hundred lines of code)
* Should compile just about anywhere (only depends on libc/libm)
* Doesn't assume any particular memory management environment (only uses stack allocation internally, other structures are always allocated by the caller)

## Limitations

Some of these might be addressed better in the future:

* Only 3x3 transformation matrix is provided (although it would be pretty easy to swap out with a more complex one)
* No textures (each triangle just has a "pixel" value, although textures should be easy to add given the position in the triangle is already calculated)
* No pixel blending is provided (although it would be easy to add at the point where the pixel is set)
* No lookup for which triangle is at a certain position for mouse clicks (although that would be easy to add as a special case of the renderer)
* Not very well-optimised
* No high-level/convenience API (e.g. you have to initialise a matrix yourself)

## Example

The demo program just renders a scene made of triangles (with ASCII "pixels"), accepting one or two rotation parameters.

    ############################################################
    ############################################################
    ############################################################
    #################################c##########################
    ###############################ccb##########################
    ############################ccccbbb#########################
    ###########################ccccbbbb#########################
    ##########################ccccbbbbb#########################
    ##########################cccbbbbbbb########################
    #########################cccbbbbbbbb########################
    #########################cccbbbbbbbb########################
    #########################ccbbbbbbbbbb#######################
    ########################ccbbbbbbbbbbb#######################
    ########################cbbbbbbbbbbbb#######################
    #######################cbbbbbbbbbbbbb#######################
    #######################bbbbbbbbbbbbbbb######################
    ################################bbbbbb######################
    ############################################################
    ############################################################
    ############################################################

## See Also

* I mostly used this tutorial as a reference: http://blog.rogach.org/2015/08/how-to-create-your-own-simple-3d-render.html
* For a more robust (but still fairly simple) 3D engine, try: https://github.com/LukasBanana/PicoRenderer

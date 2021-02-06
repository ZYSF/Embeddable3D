# Embeddable3D
A simple software renderer for C.

## Usage

Basic rendering logic is provided in the header and a simple demo program is provided (it just prints a simple scene in ASCII and accepts rotation parameters).

To do things like display textures or render in a window you'll need to add some extra code.

## Features

* Very simple
* Should compile anywhere

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

## Limitations

* Only 3x3 transformation matrix is provided (although it would be pretty easy to swap out with a more complex one)
* No textures (each triangle just has a "pixel" value, although textures should be easy to add in theory)
* No pixel blending is provided (although it would be easy to add at the point where the pixel is set)
* No lookup for which triangle is at a certain position for mouse clicks (although that would be easy to add as a special case of the renderer)

## See Also

* Mostly based on the tutorial here: http://blog.rogach.org/2015/08/how-to-create-your-own-simple-3d-render.html

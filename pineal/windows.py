import pyglet
import pyglet.gl as gl
from pineal.core import layer


def new_renderer(draw, size):
    w, h = size
    window = pyglet.window.Window(caption='(pineal renderer)',
                                  width=w,
                                  height=h,
                                  visible=False)

    gl.glEnable(gl.GL_BLEND)
    gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

    gl.glEnable(gl.GL_LINE_SMOOTH)
    gl.glEnable(gl.GL_POLYGON_SMOOTH)
    gl.glHint(gl.GL_LINE_SMOOTH_HINT, gl.GL_NICEST)
    gl.glHint(gl.GL_POLYGON_SMOOTH_HINT, gl.GL_NICEST)

    window.texture = None

    @window.event
    def on_draw():
        pyglet.clock.tick()
        window.clear()

        gl.glLoadIdentity()
        draw()

        buf = pyglet.image.get_buffer_manager().get_color_buffer()
        rawimage = buf.get_image_data()
        window.texture = rawimage.get_texture()

    return window


def new_output_window(name, show_fps=False):
    window = pyglet.window.Window(resizable=True)

    if show_fps:
        fps = pyglet.clock.ClockDisplay()

    @window.event
    def on_draw():
        w, h = window.width, window.height
        side = max(w, h)

        window.clear()

        gl.glPushMatrix()
        gl.glTranslatef(w/2, h/2, 0)
        gl.glScalef(side/2.0, -side/2.0, 1)
        layer(name).draw()
        gl.glPopMatrix()

        if show_fps:
            fps.draw()

    @window.event
    def on_key_press(symbol, modifiers):
        if symbol == pyglet.window.key.ESCAPE:
            return pyglet.event.EVENT_HANDLED

    return window

#ifndef WINDOW_H
#define WINDOW_H

extern bool windowWillClose;
void init_window(int width, int height);
void update_window();
void swap_window_buffers();
void ask_window_to_close();
void set_window_name(const char *windowName);
void set_window_vsync(bool sync);
void set_window_clear_color(float r, float g, float b, float a);
void resize_window(int width, int height);

#endif /* end of include guard: WINDOW_H */

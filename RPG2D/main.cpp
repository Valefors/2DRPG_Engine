#include <iostream>

#include "user_input.h"
#include "graphics.h"
#include "window.h"

int main(int argc, const char **argv)
{
	init_window(800, 600);
	set_window_vsync(true);
	init_graphics();

	std::cout << "coucou\n" << std::flush;

	while (!windowWillClose)
	{
		update_window();

		if (is_input_key_pressed(INPUT_KEY_ESCAPE))
		{
			ask_window_to_close();
			break;
		} else if (is_input_key_pressed(INPUT_KEY_ENTER)) {
			std::cout << "lol\n" << std::flush;
		}

		draw_triangle();

		swap_window_buffers();
	}

	return EXIT_SUCCESS;
}
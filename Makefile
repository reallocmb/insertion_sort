build:
	cc -g `pkg-config --cflags gtk4` main.c -o selection_sort_animation `pkg-config --libs gtk4`
	./selection_sort_animation

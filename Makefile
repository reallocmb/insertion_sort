build:
	cc -g `pkg-config --cflags gtk4` insertion_sort.c -o insertion_sort `pkg-config --libs gtk4`
	./insertion_sort

# openthumbnailer

A little program to write video thumbnails based on OpenCV

# Requirements

* <sys/time.h> with setitimer
* SIGVTALARM
* boost::program_options
* opencv2

# Usage

<code>for n in /path/to/your/video/collection/*; do ./openthumbnailer -i "$n" -o "$n".%010d.jpg --frame-advance 10000 ; done</code>

# Issues

Possible code injection via ouput format string as it uses snprintf and I'm too lazy to figure the out the safe C++ way to do this.

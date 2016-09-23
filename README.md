# openthumbnailer

A little program to write video thumbnails based on OpenCV

# Requirements

* <sys/time.h> with setitimer
* SIGVTALARM
* boost::program_options
* opencv2

# Usage

<code>for n in /path/to/your/video/collection/*; do ./openthumbnailer -i "$n" -o "$n".%010d.jpg --frame-advance 10000 ; done</code>

<pre>
  -h [ --help ]                         Output help text and exit successfully
  --frame-offset arg (=0)               With wich frame to start writing thumbs
  --frame-advance arg (=100)            How many frames to advance between 
                                        writing thumbs
  --max-frame arg (=4294967295)         Frame at which to stop processing
  -i [ --input-file ] arg (=video.mp4)  The input video file name
  -o [ --output-file ] arg (=output_%05d.jpg)
                                        The basename for the output thumbnails.
                                        Note that this is a format string for 
                                        snprintf()
  --watchdog-timeout arg (=5)           How long to wait for seeking calls, 
                                        etc, to finish. If this time (seconds) 
                                        is exceeded abort with failure.
</pre>

# Issues

Possible code injection via ouput format string as it uses snprintf and I'm too lazy to figure the out the safe C++ way to do this.




/*
    Copyright (C) 2016 Florian Paul Schmidt <mista.tapas@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <string>
#include <climits>

#include <sys/time.h>

/**
    Returns cv::Mat() if some benign error condition has occured. 
    Might throw if serious error occured
*/
cv::Mat get_frame(cv::VideoCapture &video_capture, unsigned frame)
{
    bool success = true;

    success = video_capture.set(CV_CAP_PROP_POS_FRAMES, (double)frame);
    
    if (false == success)
    {
        std::cout << "Failed to seek to frame: " << frame << ". Exiting..." << std::endl;
        return cv::Mat();
    }
    
    double current_video_capture_frame = video_capture.get(CV_CAP_PROP_POS_FRAMES);
    if ((double)frame != current_video_capture_frame)
    {
        std::cout << "Failed to seek to frame: " << frame << ". Exiting..." << std::endl;
        return cv::Mat();
    }

    std::cout << "Extracting frame..." << std::endl;
    
    cv::Mat video_frame;
    success = video_capture.read(video_frame);
    
    if (false == success)
    {
        std::cout << "Failed to read frame. Exiting..." << std::endl;
        return cv::Mat();
    }
    
    if (video_frame.empty())
    {
        std::cout << "Failed to read frame - got empty frame. Exiting..." << std::endl;
        return cv::Mat();
    }

    return video_frame;
}

void set_watchdog_timeout(unsigned watchdog_timeout_seconds)
{
    if (0 != watchdog_timeout_seconds)
    {
        itimerval timer;
        itimerval old_timer;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        timer.it_value.tv_sec = watchdog_timeout_seconds;
        timer.it_value.tv_usec = 0;
        int timer_success = setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
        
        if (0 != timer_success)
        {
            throw std::runtime_error("Failed to set watchdog timeout");
        }
    }
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    
    unsigned frame_offset = 0;
    unsigned frame_advance = 100;
    unsigned max_frame = UINT_MAX;
    
    std::vector<unsigned> frames;
    
    std::string input_file = "video.mp4";
    std::string output_file = "output_%010d.jpg";
    
    unsigned watchdog_timeout_seconds = 15u;
    
    const unsigned MAX_OUTPUT_FILENAME_LENGTH = 100000;
    char output_filename_buffer[MAX_OUTPUT_FILENAME_LENGTH + 1];

    const char *license = 
        #include "LICENSE.txt"
    ;
    
    try
    {
        po::options_description options_description;
        options_description.add_options()
            ("help,h", "Output help text and exit successfully")
            ("frame-offset", po::value<unsigned>(&frame_offset)->default_value(frame_offset), "With wich frame to start writing thumbs")
            ("frame-advance", po::value<unsigned>(&frame_advance)->default_value(frame_advance), "How many frames to advance between writing thumbs")
            ("frame,f", po::value<std::vector<unsigned>>(&frames), "Explicit frames. Once --frame is specified at least once, then the frame-offset, max-frame and frame-advance options are ignored and only the explicit frames are processed.")
            ("max-frame", po::value<unsigned>(&max_frame)->default_value(max_frame), "Frame at which to stop processing")
            ("input-file,i", po::value<std::string>(&input_file)->default_value(input_file), "The input video file name")
            ("output-file,o", po::value<std::string>(&output_file)->default_value(output_file), "The basename for the output thumbnails. Note that this is a format string for snprintf()")
            ("watchdog-timeout", po::value<unsigned>(&watchdog_timeout_seconds)->default_value(watchdog_timeout_seconds), "How long to wait for processing a frame (including seeking, etc) to finish. If this time (seconds) is exceeded abort with failure. Use 0 to disable the watchdog.")
            ("license", "Output license information")
        ;
        
        po::variables_map variables_map;
        po::store(po::parse_command_line(argc, argv, options_description), variables_map);
        po::notify(variables_map);
        
        if (variables_map.count("help"))
        {
            std::cout << options_description << std::endl;
            return EXIT_SUCCESS;
        }
        
        if (variables_map.count("license"))
        {
            std::cout << license << std::endl;
            return EXIT_SUCCESS;
        }

        
        std::cout
            << "openthumbnailer Copyright (C) 2016 Florian Paul Schmidt" << std::endl
            << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl
            << "This is free software, and you are welcome to redistribute it" << std::endl
            << "under certain conditions; Use --license for details" << std::endl
            ;
        
        std::cout << "Processing input file: " << input_file << std::endl;
        
        cv::VideoCapture video_capture(input_file);
        
        if (false == video_capture.isOpened())
        {
            throw std::runtime_error("Failed to open video: " + input_file);
        }
        
        auto frames_iterator = frames.begin();
        
        unsigned current_frame = 0;
        
        if (false == frames.empty())
        {
            current_frame = *frames_iterator;
        }
        else
        {
            current_frame = frame_offset;
        }
        
        while(true)
        {
            set_watchdog_timeout(watchdog_timeout_seconds);
            
            if (true == frames.empty())
            {
                if (current_frame >= max_frame)
                {
                    std::cout << "Exceeded max-frame: " << max_frame << ". Exiting..." << std::endl;
                    break;
                }
            }
            
            std::cout << "Processing frame: " << current_frame << std::endl;

            cv::Mat frame = get_frame(video_capture, current_frame);
            
            if (frame.empty())
            {
                std::cout << "Seems we're done here..." << std::endl;
                break;
            }
            
            snprintf(output_filename_buffer, MAX_OUTPUT_FILENAME_LENGTH, output_file.c_str(), current_frame);
            
            std::cout << "Writing thumbnail: " << output_filename_buffer << std::endl;
            
            bool success = cv::imwrite(output_filename_buffer, frame);
            
            if (false == success)
            {
                throw std::runtime_error("Failed to write image");
            }
            
            if (false == frames.empty())
            {
                ++frames_iterator;
                if (frames_iterator == frames.end())
                {
                    break;
                }
                current_frame = *frames_iterator;
            }
            else
            {
                current_frame += frame_advance;
            }   
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Fail: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
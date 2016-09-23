#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <string>
#include <climits>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    
    unsigned frame_offset = 0;
    unsigned frame_advance = 100;
    unsigned max_frame = UINT_MAX;
    
    std::string input_file = "";
    std::string output_file = "";
    
    const unsigned MAX_OUTPUT_FILENAME_LENGTH = 100000;
    
    char output_filename_buffer[MAX_OUTPUT_FILENAME_LENGTH + 1];
    
    try
    {
        po::options_description options_description;
        options_description.add_options()
            ("help,h", "Output help text and exit successfully")
            ("frame-offset", po::value<unsigned>(&frame_offset)->default_value(0u), "With wich frame to start writing thumbs")
            ("frame-advance", po::value<unsigned>(&frame_advance)->default_value(100u), "How many frames to advance between writing thumbs")
            ("max-frame", po::value<unsigned>(&max_frame)->default_value(UINT_MAX), "Frame at which to stop processing")
            ("input-file,i", po::value<std::string>(&input_file)->default_value("video.mp4"), "The input video file name")
            ("output-file,o", po::value<std::string>(&output_file)->default_value("output_%05d.jpg"), "The basename for the output thumbnails. Note that this is a format string for snprintf()")
        ;
        
        po::variables_map variables_map;
        po::store(po::parse_command_line(argc, argv, options_description), variables_map);
        po::notify(variables_map);
        
        if (variables_map.count("help"))
        {
            std::cout << options_description << std::endl;
            return EXIT_SUCCESS;
        }
        
        
        cv::VideoCapture video_capture(input_file);
        
        if (false == video_capture.isOpened())
        {
            throw std::runtime_error("Failed to open video: " + input_file);
        }
        
        unsigned current_frame = frame_offset;
        
        while(true)
        {
            if (current_frame >= max_frame)
            {
                std::cout << "Exceeded max-frame: " << max_frame << ". Exiting..." << std::endl;
                break;
            }
            
            std::cout << "Processing frame: " << current_frame << std::endl;
            bool success = true;
        
            success = video_capture.set(CV_CAP_PROP_POS_FRAMES, (double)current_frame);
            
            if (false == success)
            {
                std::cout << "Failed to seek to frame: " << current_frame << ". Exiting..." << std::endl;
                break;
            }
            
            cv::Mat frame;
            success = video_capture.read(frame);
            
            if (false == success)
            {
                std::cout << "Failed to read frame. Exiting..." << std::endl;
                break;
            }
            
            snprintf(output_filename_buffer, MAX_OUTPUT_FILENAME_LENGTH, output_file.c_str(), current_frame);
            
            std::cout << "Writing thumbnail: " << output_filename_buffer << std::endl;
            
            success = cv::imwrite(output_filename_buffer, frame);
            
            if (false == success)
            {
                throw std::runtime_error("Failed to write image");
            }
            
            current_frame += frame_advance;
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Fail: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
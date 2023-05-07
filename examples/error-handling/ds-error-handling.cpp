#include "libdepthsense/ds.hpp"
#include "libdepthsense/dsutils.h"
#include "libdepthsense/dsutils.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <iostream>
#include <opencv2/opencv.hpp>


#define TOTAL_CAMERA_CNT        (1)

/* 
 * This is an error handling example demo, you can change it according to your requirement.
 * This demo will polling until `TOTAL_CAMERA_CNT` cameras were plugged into the system and 
 * try to get depth frame from each of them. 
 * If any of the cameras disconneted, the `ds::camera_disconneted_error` will be raised and this
 * example shows how to handle such error. After correctly handling disconnect error, the system
 * will be ready to continue capturing depth frames if the cameras are plugged back agin.
 *
 */

int main(int argc, char * argv[]) try
{
    // Set minimal log level as DS_LOG_WARN
    // - DS_LOG_ERROR /*!< Critical errors, software module can not recover on its own */
    // - DS_LOG_WARN  /*!< Error conditions from which recovery measures have been taken */
    // - DS_LOG_INFO  /*!< Information messages which describe normal flow of events */
    // - DS_LOG_DEBUG /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ds_set_log_level(DS_LOG_WARN);


    // TODO: expected cameras to be connected, change it according to your application case
    const int total_camera_count = TOTAL_CAMERA_CNT;

    while (1) {
        ds::context ctx;

        // get camera count
        int camera_count = 0;

        // waiting for all cameras are connected!
        while (1) {
            camera_count = ctx.get_camera_count();
            if (camera_count < total_camera_count) {
                std::cout << "Waiting for " << total_camera_count << " cameras connected! Current camera count " << camera_count << std::endl;
                // sleep for 500ms
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                // NOTE: call refresh again to see whether the camera is plugged back
                ctx.refresh_devices();
            }
            else {
                break;
            }
        }

        std::cout << "camera_count is " << camera_count << std::endl;

        for(int i=0; i<camera_count; ++i) {
            ds::camera cam = ctx.get_camera(i);
           
            // print camera name
            fprintf(stdout, "camera name: %s.\n", cam.get_name());
            
            // enable depth stream: w 640, h 400, 16-bit, 30fps
            cam.enable_stream(DS_STREAM_DEPTH, 640, 400, DS_FORMAT_Z16, 30);
        
            // start capture with enabled streams
            cam.start_capture();
        }

        while (1) try {
            for(int i = 0; i < camera_count; ++i)
            {
                ds::camera cam = ctx.get_camera(i);

                // wait for all enabled streams
                cam.wait_all_streams();
        
                // get depth stream data pointer
                ds::Mat depth = cam.get_image_mat(DS_STREAM_DEPTH);
                
                std::cout<<"depth.cols. "<<depth.cols<<" depth.rows. "<<depth.rows<<std::endl;
                cv::Mat depth_opencv(depth.rows, depth.cols, 0, depth.data);
                
                for(int i=0; i<depth_opencv.rows; i++) {
                    for(int j=0; j<depth_opencv.cols; j++) {
                        std::cout << (int)depth_opencv.at<uchar>(i,j) << " ";
                    }
                    std::cout << std::endl;
                }

                // Display image
                cv::imshow("Depth Image", depth_opencv);
                cv::waitKey(0);
                //std::this_thread::sleep_for(std::chrono::milliseconds(35));

            }
        }
        catch (const ds::camera_disconnected_error &e) {
            std::cerr << "Camera disconnected!" << std::endl;
            // break the current while loop
            break;
        }
        catch (const ds::recoverable_error &e) {
            std::cerr << "Recoverable error, please try again!" << std::endl;
        }
        catch (const ds::error &e) {
            // other errors, raise to the upper level
            throw e;
        }

    } // end of while

    return 0;
}
catch (const ds::error &e)
{
    std::cerr << e.what() << std::endl;
}

#include "libdepthsense/ds.hpp"
#include "libdepthsense/dsutils.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void usage()
{
    fprintf(stderr, "ds-force-reset-camera \n");
}


int main(int argc, char * argv[]) try
{
    // Set minimal log level as DS_LOG_WARN
    // - DS_LOG_ERROR /*!< Critical errors, software module can not recover on its own */
    // - DS_LOG_WARN  /*!< Error conditions from which recovery measures have been taken */
    // - DS_LOG_INFO  /*!< Information messages which describe normal flow of events */
    // - DS_LOG_DEBUG /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ds_set_log_level(DS_LOG_WARN);

    ds::context ctx;
    
    // get camera count
    int camera_count = ctx.get_camera_count();

    if (camera_count < 1) {
        throw std::runtime_error("No camera detected. Is it plugged in?");
    }

    std::cout << "camera_count is " << camera_count << std::endl;

    // open camera 0
    ds::camera cam = ctx.get_camera(0);

    // print camera name
    fprintf(stdout, "camera name: %s.\n", cam.get_name());

    // enable depth stream: w 640, h 400, 16-bit, 30fps
    cam.enable_stream(DS_STREAM_DEPTH, 640, 400, DS_FORMAT_Z16, 30);


    // start capture with enabled streams
    cam.start_capture();

    // let the camera run a few seconds...
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // NOTE: reset camera
    cam.set_option(DS_OPTION_FORCE_RESET, NULL, 0);

    std::cout << "Reset camera successfully!" << std::endl;

    return 0;

}
catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

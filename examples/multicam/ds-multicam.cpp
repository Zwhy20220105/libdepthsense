#include "libdepthsense/ds.hpp"
#include "libdepthsense/dsutils.h"
#include "libdepthsense/dsutils.hpp"

#include <iostream>

int main(int argc, char *argv[])
try
{
    // Set minimal log level as DS_LOG_WARN
    // - DS_LOG_ERROR /*!< Critical errors, software module can not recover on its own */
    // - DS_LOG_WARN  /*!< Error conditions from which recovery measures have been taken */
    // - DS_LOG_INFO  /*!< Information messages which describe normal flow of events */
    // - DS_LOG_DEBUG /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ds_set_log_level(DS_LOG_WARN);

    ds::context ctx;
    int w, h;

    // get camera count
    int camera_count = ctx.get_camera_count();

    if (camera_count < 1)
    {
        throw std::runtime_error("No camera detected. Is it plugged in?");
    }

    std::cout << "camera_count is " << camera_count << std::endl;

    for (int i = 0; i < camera_count; ++i)
    {
        ds::camera cam = ctx.get_camera(i);

        // print camera name
        fprintf(stdout, "camera name: %s.\n", cam.get_name());

        // enable depth stream: w 640, h 400, 16-bit, 30fps
        cam.enable_stream(DS_STREAM_DEPTH, 640, 400, DS_FORMAT_Z16, 30);

        // start capture with enabled streams
        cam.start_capture();

        // set depth quality level as optimized.
        uint8_t depth_level = DS_DEPTH_LEVEL_OPTIMIZED;
        cam.set_option(DS_OPTION_DEPTH_CONTROL_PRESET, &depth_level, sizeof(depth_level));

        // enable auto exposure
        uint8_t ae_enable = 1;
        cam.set_option(DS_OPTION_LR_AUTO_EXPOSURE_ENABLED, &ae_enable, sizeof(ae_enable));

        // enable spot filter
        uint8_t spot_filt_en = 1;
        cam.set_option(DS_OPTION_SPOT_FILT_ENABLED, &spot_filt_en, sizeof(spot_filt_en));

        // set minimum depth distance
        uint32_t min_depth_mm = 260; // 260   mm
        cam.set_option(DS_OPTION_DEPTH_CLAMP_MIN, &min_depth_mm, sizeof(min_depth_mm));

        // set maximum depth distance
        uint32_t max_depth_mm = 10000; // 10000 mm
        cam.set_option(DS_OPTION_DEPTH_CLAMP_MAX, &max_depth_mm, sizeof(max_depth_mm));
    }

    ds::camera cam = ctx.get_camera(0);
    // get depth image width and height
    w = cam.get_stream_width(DS_STREAM_DEPTH);
    h = cam.get_stream_height(DS_STREAM_DEPTH);
    fprintf(stdout, "depth stream: w is %d, h is %d\n", w, h);

    while (1)
    {
        for (int i = 0; i < camera_count; ++i)
        {
            ds::camera cam = ctx.get_camera(i);

            // get intrinsic matrix
            ds::intrinsics intr = cam.get_stream_intrinsics(DS_STREAM_DEPTH);
            // get extrinsic matrix
            // ds::extrinsics extr = cam.get_stream_extrinsics(DS_STREAM_INFRARED, DS_STREAM_INFRARED_2);

            // print out cx, cy, fx, fy and T
            // std::cout << "intr: cx " << intr.principal_point[0] << ", cy " << intr.principal_point[1] << std::endl;
            // std::cout << "intr: fx " << intr.focal_length[0] << ", fy " << intr.focal_length[1] << std::endl;
            // std::cout << "T is " << extr.translation[0] << std::endl;

            // wait for all enabled streams
            cam.wait_all_streams();

            // get depth stream data pointer
            ds::Mat depth = cam.get_image_mat(DS_STREAM_DEPTH);

            // transfer depth to point cloud
            ds::Mat img_3d;
            // if skip invalid flag is true, skip putting invalid 3d point into img_3d
            bool skip_invalid = true;
            // subsample every interval+1 row and every interval+1 column.
            // if interval == 0, no subsample.
            unsigned int interval = 0;
            ds_repoject_image_to_3d(img_3d, depth, intr, skip_invalid, interval);

            // TODO: read data from img_3d.data: 32-bit float x3 (X, Y, Z)
            //  float *point_cloud = (float *)img_3d.data;
            // For each point i:
            //   point_cloud[i*3+0] is X
            //   point_cloud[i*3+1] is Y
            //   point_cloud[i*3+2] is Z
            int valid_3d_point_count = img_3d.total() / img_3d.element_size();
            // NOTE: each element of valid_3d_point_count is [X, Y, Z] (Three 32-bit floats)
            fprintf(stdout, "cam %d: Valid image 3D point[X, Y, Z]: %d, data pointer addr: 0x%lx, frame num %ld, timestamp %ld ms\n",
                    i,
                    valid_3d_point_count,
                    img_3d.data,
                    img_3d.frame_number(),
                    img_3d.timestamp_ms());
        }
    }

    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

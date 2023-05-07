#include "libdepthsense/ds.hpp"
#include "libdepthsense/dsutils.h"
#include "libdepthsense/dsutils.hpp"
#include <iostream>

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char * argv[]) try
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

    if (camera_count < 1) {
        throw std::runtime_error("No camera detected. Is it plugged in?");
    }

    std::cout << "camera_count is " << camera_count << std::endl;

    // open camera 0
    ds::camera cam = ctx.get_camera(0);

    // print camera name
    fprintf(stdout, "camera name: %s.\n", cam.get_name());

    // device path [bus:x]-[ports:x.x]-[dev:x]
    fprintf(stdout, "device path: %s.\n", cam.get_device_path());

    // print serial number
    fprintf(stdout, "SN: %s.\n", cam.get_sn());

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
    uint32_t min_depth_mm = 260;        // 260   mm
    cam.set_option(DS_OPTION_DEPTH_CLAMP_MIN, &min_depth_mm, sizeof(min_depth_mm));

    // set maximum depth distance
    uint32_t max_depth_mm = 10000;      // 10000 mm
    cam.set_option(DS_OPTION_DEPTH_CLAMP_MAX, &max_depth_mm, sizeof(max_depth_mm));

    // get depth image width and height
    w = cam.get_stream_width(DS_STREAM_DEPTH);
    h = cam.get_stream_height(DS_STREAM_DEPTH);

    // get intrinsic matrix
    ds::intrinsics intr = cam.get_stream_intrinsics(DS_STREAM_DEPTH);
    // get extrinsic matrix
    ds::extrinsics extr = cam.get_stream_extrinsics(DS_STREAM_INFRARED, DS_STREAM_INFRARED_2);

    float hfov, vfov;
    hfov = ds_compute_fov(intr.image_size[0], intr.focal_length[0], intr.principal_point[0]);
    vfov = ds_compute_fov(intr.image_size[1], intr.focal_length[1], intr.principal_point[1]);

    // print out cx, cy, fx, fy and T
    std::cout << "intr: cx " << intr.principal_point[0] << ", cy " << intr.principal_point[1] << std::endl;
    std::cout << "intr: fx " << intr.focal_length[0] << ", fy " << intr.focal_length[1] << std::endl;
    std::cout << "T is " << extr.translation[0] << std::endl;

    std::cout << "HFOV: " << hfov << ", VFOV: " << vfov << std::endl;

    fprintf(stdout, "depth stream: w is %d, h is %d\n", w, h);

    while (1) {
        // 等待所有已启用的流都准备就绪
        cam.wait_all_streams();

        // 获取深度流数据指针
        ds::Mat depth = cam.get_image_mat(DS_STREAM_DEPTH);
        //
        // TODO: 从 depth.data 中读取深度数据，w(depth.cols)，h(depth.rows)，16位 uint16 类型
        //

        // 将深度数据转换为点云
        ds::Mat img_3d;
        // 如果跳过无效标志为 true，则不会将无效的 3D 点放入 img_3d 中
        bool skip_invalid = true;

        // 每隔 interval+1 行和 interval+1 列进行一次采样。
        // 如果 interval == 0，则不进行采样。
        unsigned int interval = 0;
        ds_repoject_image_to_3d(img_3d, depth, intr, skip_invalid, interval);


        ds::Mat color_depth;
        ds_colorize_depth(color_depth, depth);

        cv::Mat cv_color_depth(color_depth.rows, color_depth.cols, CV_8UC3, color_depth.data);      
        std::cout<<"cv_color_depth.cols. "<<cv_color_depth.cols<<" cv_color_depth.rows. "<<cv_color_depth.rows<<std::endl;

        // Display image
        cv::imshow("Depth Image", cv_color_depth);
        cv::waitKey(0);

        //
        // TODO: 从 img_3d.data 中读取数据：32 位浮点数 x3（X、Y、Z）
        //  float *point_cloud = (float *)img_3d.data;
        // 对于每个点 i：
        //   point_cloud[i*3+0] 是 X
        //   point_cloud[i*3+1] 是 Y
        //   point_cloud[i*3+2] 是 Z
        int valid_3d_point_count = img_3d.total() / img_3d.element_size();
        // 注意：valid_3d_point_count 的每个元素都是 [X、Y、Z]（三个 32 位浮点数）
        fprintf(stdout, "有效的 3D 点[X、Y、Z]：数量 %08d，地址 0x%08lx，帧数 %ld，时间戳 %ld 毫秒\n",
                valid_3d_point_count,
                img_3d.data,
                img_3d.frame_number(),
                img_3d.timestamp_ms()
                );
    }


    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

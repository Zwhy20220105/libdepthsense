#include <opencv2/opencv.hpp>
#include "libdepthsense/ds.hpp"
#include "libdepthsense/dsutils.hpp"
#include <iostream>

void usage()
{
    fprintf(stderr, "ds-opencv-example -iw [ir width] -ih [ir height] -cw [color width] -ch [color height]\n");
    fprintf(stderr, "   Supported: -iw 640  -ih 480\n");
    fprintf(stderr, "            : -iw 640  -ih 400\n");
    fprintf(stderr, "            : -iw 320  -ih 240\n");
    fprintf(stderr, "            : -iw 320  -ih 200\n");
    fprintf(stderr, "            : -cw 1280 -ch 720\n");
    fprintf(stderr, "            : -cw 640  -ch 480\n");
    fprintf(stderr, "            : -cw 640  -ch 400\n");
}


int main(int argc, char * argv[]) try
{

    int color_w = 0;
    int color_h = 0;
    int ir_w = 640;
    int ir_h = 400;
    bool is_valid_param = false;

    if (argc == 1) {
        usage();
        fprintf(stdout, "Use default param: w %d, h %d...\n", ir_w, ir_h);
        is_valid_param = true;
    }

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-iw", 3) == 0) {
            i++;
            if (i < argc) {
                ir_w = (int)strtol(argv[i], NULL, 0);
                is_valid_param = true;
            }
            else {
                is_valid_param = false;
            }
        }
        else if (strncmp(argv[i], "-ih", 3) == 0) {
            i++;
            if (i < argc) {
                ir_h = (int)strtol(argv[i], NULL, 0);
                is_valid_param = true;
            }
            else {
                is_valid_param = false;
            }
        }
        else if (strncmp(argv[i], "-cw", 3) == 0) {
            i++;
            if (i < argc) {
                color_w = (int)strtol(argv[i], NULL, 0);
                is_valid_param = true;
            }
            else {
                is_valid_param = false;
            }
        }
        else if (strncmp(argv[i], "-ch", 3) == 0) {
            i++;
            if (i < argc) {
                color_h = (int)strtol(argv[i], NULL, 0);
                is_valid_param = true;
            }
            else {
                is_valid_param = false;
            }
        }
        else {
            is_valid_param = false;
        }
    }

    if (!is_valid_param) {
        usage();
        throw std::runtime_error("Wrong params!");
    }


    while (1) {
        ds::context ctx;
        int camera_count;
        do {
            camera_count = ctx.get_camera_count();
            if (camera_count < 1) {
                cv::waitKey(1000);
                ctx.refresh_devices();
                fprintf(stdout, "No camera plugged? Waiting...\n");
            }
        } while (camera_count < 1);


        std::cout << "camera_count is " << camera_count << std::endl;

        ds::camera cam = ctx.get_camera(0);

        cam.enable_stream(DS_STREAM_DEPTH,      ir_w,  ir_h, DS_FORMAT_Z16, 30);
        cam.enable_stream(DS_STREAM_INFRARED,   ir_w,  ir_h, DS_FORMAT_Y8,  30);
        cam.enable_stream(DS_STREAM_INFRARED_2, ir_w,  ir_h, DS_FORMAT_Y8,  30);
        if (color_w != 0 && color_h != 0) {
            cam.enable_stream(DS_STREAM_COLOR, color_w, color_h, DS_FORMAT_RGB8, 30);
        }

        int w, h;

        cam.start_capture();

        w = cam.get_stream_width(DS_STREAM_DEPTH);
        h = cam.get_stream_height(DS_STREAM_DEPTH);

        uint8_t depth_level = DS_DEPTH_LEVEL_MEDIUM;
        //uint8_t depth_level = DS_DEPTH_LEVEL_OPTIMIZED;
        //uint8_t depth_level = DS_DEPTH_LEVEL_HIGH;
        cam.set_option(DS_OPTION_DEPTH_CONTROL_PRESET, &depth_level, sizeof(depth_level));
        
        uint8_t spot_filt_en = 0;
        cam.set_option(DS_OPTION_SPOT_FILT_ENABLED, &spot_filt_en, sizeof(spot_filt_en));
        
        uint8_t pyr_depth_en = 0;
        cam.set_option(DS_OPTION_PYR_DEPTH_ENABLED, &pyr_depth_en, sizeof(pyr_depth_en));

        uint8_t emitter_en = 1;
        cam.set_option(DS_OPTION_EMITTER_ENABLED, &emitter_en, sizeof(emitter_en));
        
        ds::intrinsics intr = cam.get_stream_intrinsics(DS_STREAM_DEPTH);
        std::cout << "intr: cx " << intr.principal_point[0] << ", cy " << intr.principal_point[1] << std::endl;
        std::cout << "intr: fx " << intr.focal_length[0] << ", fy " << intr.focal_length[1] << std::endl;

        ds::extrinsics extr = cam.get_stream_extrinsics(DS_STREAM_INFRARED, DS_STREAM_INFRARED_2);
        std::cout << "T is " << extr.translation[0] << std::endl;


        fprintf(stdout, "depth stream: w is %d, h is %d\n", w, h);

        uint32_t pre_time_ms = 0;

        while (1) try {
            cam.wait_all_streams();
            ds::Mat depth = cam.get_image_mat(DS_STREAM_DEPTH);
            
            uint32_t delta_ms = depth.timestamp_ms() - pre_time_ms;
            fprintf(stdout, "Depth timestamp_ms is %ld ms,  get frame cost %d ms, frame rate %.2f\n", 
                    depth.timestamp_ms(), delta_ms, 1000./delta_ms);
            pre_time_ms = depth.timestamp_ms();
            
            ds::Mat rgb_depth;
            ds_colorize_depth(rgb_depth, depth);

            cv::Mat cv_rgb_depth = cv::Mat(rgb_depth.rows, rgb_depth.cols, CV_8UC3, rgb_depth.data);

            const void *ir0_ptr = cam.get_image_pixels(DS_STREAM_INFRARED);
            cv::Mat ir0 = cv::Mat(h, w, CV_8UC1, (uint8_t*)ir0_ptr);
            
            const void *ir1_ptr = cam.get_image_pixels(DS_STREAM_INFRARED_2);
            cv::Mat ir1 = cv::Mat(h, w, CV_8UC1, (uint8_t*)ir1_ptr);

            cv::imshow("test", cv_rgb_depth);
            cv::imshow("ir0", ir0);
            cv::imshow("ir1", ir1);

            if (color_w != 0 && color_h != 0) {
                ds::Mat color = cam.get_image_mat(DS_STREAM_COLOR);
                cv::Mat cv_rgb_color = cv::Mat(color.rows, color.cols, CV_8UC3, color.data);
                cv::Mat cv_bgr_color;
                cv::cvtColor(cv_rgb_color, cv_bgr_color, cv::COLOR_RGB2BGR);
                cv::imshow("color", cv_bgr_color);
            }

            char key = cv::waitKey(1);
            if (key == 'q') {
                goto out;
            }
        }
        catch (const ds::camera_disconnected_error &e) {
            fprintf(stderr, "===================== > camera_disconnected_error =====================\n");
            std::cerr << e.what() << std::endl;
            //ctx.refresh_devices();
            break;
        }
    }

out:    
    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

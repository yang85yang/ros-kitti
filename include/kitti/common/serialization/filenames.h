/*
 * kitti_data_store.h
 *
 *  Created on: 22.01.2015
 *      Author: fnolden
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <kitti/common/serialization/camera_list.h>
#include <kitti/common/serialization/tf.h>
#include <tf/tf.h>
#include <string>
#include <stdio.h>
#include <pcl/io/pcd_io.h>
#include <kitti/common/io/image_io.hpp>
#include <kitti/common/io/pointcloud_io.hpp>

#ifndef INCLUDE_KITTI_COMMON_FILENAMES_H_
#define INCLUDE_KITTI_COMMON_FILENAMES_H_

namespace kitti {

namespace filenames {

	static bool create_folder(std::string dir) {
		 boost::filesystem::path path(dir);
		 boost::filesystem::path path_without_filename = path.remove_filename();

		if(!boost::filesystem::exists(path_without_filename)){
			if(boost::filesystem::create_directories(path_without_filename))
			{
				std::cerr << "Directory created:\t" << path_without_filename << std::endl;
			}
			else{
				std::cout << "Failed to create directory:\t" << path_without_filename << std::endl;
			}
		}
		return true;
	}

	static std::string sequence_number(unsigned int sequence){
				char image_name[10];
				sprintf(image_name,"%010u", sequence);
				return std::string(image_name);
	}

	static std::string filenumber(unsigned int sequence){
				return std::string("/"+sequence_number(sequence));
	}

	static void save_timestamp(std::string folder, unsigned int sequence, uint sec, uint nsec){
		folder.append("/");

		create_folder(folder);

		std::string filename_idx_file = folder + "idx.txt";
		std::string filename_timestamps = folder+ "timestamps.txt";

		{
			std::ofstream timestamps;

			timestamps.open(filename_timestamps.c_str(), std::ios_base::app);
				timestamps << sec << " \t" << nsec << "\n";
			timestamps.close();
		}
		{
			std::ofstream file_idx;

			file_idx.open(filename_idx_file.c_str(), std::ios_base::app);
				file_idx << sequence << "\n";
			file_idx.close();
		}
	}

	static bool make_folder_slash(std::string &filePath){
		if(filePath[filePath.length()-1] != '/')
		{
			filePath.append("/");
		}

		create_folder(filePath);

		return true;
	}

	static bool save_image_file(std::string data_root, cv::Mat image, int sequence, int camera_nr, uint sec, uint nsec, std::string extension=".jpg"){
		std::stringstream stream;

		char folder[9];
		sprintf(folder,"image_%02d/", camera_nr);

		stream << data_root << folder;

		save_timestamp(stream.str(), sequence, sec, nsec);

		stream << "data/";

		create_folder(stream.str());

		stream << filenumber(sequence) << extension;

		return kitti::io::save_image(stream.str(), image);
	}

	static bool save_tf(std::string data_root, std::string filename, Tf &kitti_tf){
		std::string Filenames = data_root + filename;
		return kitti_tf.save_file(Filenames);
	}

	static bool save_tf(std::string data_root, std::string filename, tf::Transform &tf){
		kitti::Tf kitti_tf;
		kitti_tf.set_transform(tf);

		return save_tf(data_root, filename, kitti_tf);
	}


	//bool save_image_file(std::string data_root, int sequence, int camera_nr, uint sec, uint nsec);
	static bool save_camera_list(std::string data_root, Camera_list &camera_list){
		std::string Filenames = data_root +"calib_cam_to_cam.txt";
		return camera_list.save_file(Filenames);
	}

	static bool save_tf_velo_to_camera0(std::string data_root, Tf &kitti_tf){
		return save_tf(data_root, "calib_velo_to_cam.txt", kitti_tf);
	}

	static bool save_tf_velo_to_camera0(std::string data_root, tf::Transform &tf){
		kitti::Tf kitti_tf;
		kitti_tf.set_transform(tf);
		return save_tf(data_root, "calib_velo_to_cam.txt", kitti_tf);
	}

	template <typename PointT>
	static bool save_pointcloud(std::string full_filepath, const pcl::PointCloud<PointT> &pointcloud){
		return kitti::io::save_pointcloud<PointT>(full_filepath, pointcloud);
	}

	template <typename PointT>
	static bool save_pointcloud(std::string data_root, pcl::PointCloud<PointT> &pointcloud, unsigned int sequence, uint sec, uint nsec, std::string folder_name="velodyne_points"){
		std::stringstream stream;

		stream << data_root << folder_name <<"/";

		create_folder(stream.str());

		save_timestamp(stream.str(), sequence, sec, nsec);

		stream << "data/";

		create_folder(stream.str());

		stream << filenumber(sequence) << ".pcd";

		save_pointcloud<PointT>(stream.str(), pointcloud);

		return true;
	}



};


} /* namespace kitti */

#endif /* INCLUDE_KITTI_COMMON_FILENAMES_H_ */

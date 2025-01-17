#include "d1_2d_topic.h"

namespace D1
{
    using namespace CygLiDARD1;

    void Topic_2D::publishScanLaser(std::string frame_id, ros::Publisher publisher_laserscan_, ros::Time start_time, uint16_t *distance_buffer_2d_)
    {
        sensor_msgs::LaserScan message_laserscan;

        assignLaserScanData(frame_id, start_time, message_laserscan);

        for (uint8_t i = 0; i < cyg_driver::DATA_LENGTH_2D; i++)
        {
            // Reverse data order of the array
            buffer_index = (cyg_driver::DATA_LENGTH_2D - 1 - i);

            if (distance_buffer_2d_[buffer_index] < Distance::Mode2D::Maximum_Depth_2D)
                message_laserscan.ranges[i] = distance_buffer_2d_[buffer_index] * MM2M;
            else
                message_laserscan.ranges[i] = std::numeric_limits<float>::infinity();
        }
        publisher_laserscan_.publish(message_laserscan);
    }
    
    void Topic_2D::assignLaserScanData(std::string frame_id_, ros::Time start_time_, sensor_msgs::LaserScan &message_laserscan_)
    {
        message_laserscan_.header.frame_id = frame_id_;
        message_laserscan_.header.stamp = start_time_;
        message_laserscan_.angle_min = -static_cast<float>(Sensor::HorizontalAngle / 2.0f * Util::ToRadian);
        message_laserscan_.angle_max =  static_cast<float>(Sensor::HorizontalAngle / 2.0f * Util::ToRadian);
        message_laserscan_.angle_increment = static_cast<float>(Sensor::AngleIncremet2D * Util::ToRadian);
        message_laserscan_.scan_time = 0;
        message_laserscan_.range_min = static_cast<float>(Distance::Mode2D::Minimum_Depth_2D * Util::MM_To_M);
        message_laserscan_.range_max = static_cast<float>(Distance::Mode2D::Maximum_Depth_2D * Util::MM_To_M);
        message_laserscan_.ranges.resize(cyg_driver::DATA_LENGTH_2D);
        message_laserscan_.intensities.resize(cyg_driver::DATA_LENGTH_2D);
    }

    void Topic_2D::publishPoint2D(std::string frame_id_, ros::Publisher publisher_point_2d_, uint16_t *distance_buffer_2d_)
    {
        pcl::PointCloud<pcl::PointXYZRGBA> pointcloud_2d;

        pointcloud_2d.header.frame_id = frame_id_;
        pointcloud_2d.is_dense = false;
        pointcloud_2d.points.resize(cyg_driver::DATA_LENGTH_2D);

        asssignPointCloud2DPosition(pointcloud_2d, distance_buffer_2d_);

        pcl_conversions::toPCL(ros::Time::now(), pointcloud_2d.header.stamp);
        publisher_point_2d_.publish(pointcloud_2d);
    }
    
    void Topic_2D::asssignPointCloud2DPosition(pcl::PointCloud<pcl::PointXYZRGBA> &pointcloud_2d_, uint16_t *distance_buffer_2d_)
    {
        float angle_increment_steps = static_cast<float>(Sensor::AngleIncremet2D);
        float point_2d_angle, point_2d_angle_variable;

        for (uint8_t i = 0; i < cyg_driver::DATA_LENGTH_2D; i++)
        {
            buffer_index = (cyg_driver::DATA_LENGTH_2D - 1 - i);

            raw_distance = distance_buffer_2d_[buffer_index];

            point_2d_angle = ((-HORIZONTAL_ANGLE / 2)+ point_2d_angle_variable) * Util::ToRadian;
            point_2d_angle_variable += angle_increment_steps;

            camera_coordinate_x = (sin(point_2d_angle) * raw_distance);
            camera_coordinate_y = (cos(point_2d_angle) * raw_distance);

            pointcloud_2d_.points[i].x = camera_coordinate_y * MM2M;
            pointcloud_2d_.points[i].y = camera_coordinate_x * MM2M;
            pointcloud_2d_.points[i].z = 0.0;

            if (distance_buffer_2d_[buffer_index] < Distance::Mode2D::Maximum_Depth_2D)
                pointcloud_2d_.points[i].rgba = 0xFFFFFF00; //ARGB
            else
                // Turn data invisible when it's greater than the maximum
                pointcloud_2d_.points[i].a = 0;
        }
    }
}

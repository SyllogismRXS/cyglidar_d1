#include "d1_node.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "cyglidar_publisher");
    ros::NodeHandle nh;

    D1::CygLiDAR_D1 node(nh);

    ros::spin();
    ros::shutdown();
}

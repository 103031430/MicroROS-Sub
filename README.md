# MicroROS-Sub

This project focuses on getting the ESP32-S3-ETH to publish data under a topic to the ROS2 stack via Micro ROS. This is done via Ethernet where ROS2 will run inside a Docker container.
Note the following IP configurations:
- ESP32-S3-ETH IP address: 192.168.0.108
- Micro ROS Agent IP address: 192.168.0.30
- Micro ROS Agent Port Number: 8888

## Requirements
- Docker (Docker or Docker Desktop)
- PlatformIO (local installation or PlatformIO IDE for VSCode)

## Board Used
The board used for this project is [Waveshare ESP32-S3-ETH](https://www.waveshare.com/esp32-s3-eth.htm)

## Libraries Used
The following Libraries is used for this project:
- [Ethernet](https://docs.arduino.cc/libraries/ethernet/#Ethernet%20Class)
- [Micro ROS for PlatformIO](https://github.com/micro-ROS/micro_ros_platformio)

## Docker Setup
The docker command used for this for this project
```
docker run -it --net=host --privileged --name=ros2 ros:jazzy
```

Once you have the docker conatiner is running, you will need to setup the Micro ROS agent in order to connect Micro ROS to ROS2. This is done by using the following commands:
```
# Source the ROS 2 installation
source /opt/ros/$ROS_DISTRO/setup.bash

# Create a workspace and download the micro-ROS tools
mkdir microros_ws
cd microros_ws
git clone -b $ROS_DISTRO https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup

# Update dependencies using rosdep
sudo apt update && rosdep update
rosdep install --from-paths src --ignore-src -y

# Install pip
sudo apt-get install python3-pip

# Build micro-ROS tools and source them
colcon build
source install/local_setup.bash

# Download micro-ROS-Agent packages
ros2 run micro_ros_setup create_agent_ws.sh

# Build step
ros2 run micro_ros_setup build_agent.sh
source install/local_setup.bash

# Add micro-ROS environment to bashrc (optional)
echo source /opt/ros/$ROS_DISTRO/setup.bash >> ~/.bashrc
echo source ~/microros_ws/install/local_setup.bash >> ~/.bashrc

# Run a micro-ROS agent
ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888

```
Once the micro ROS is running, press the reset button on the ESP32-S3-ETH and the connection between the ESP32 and the Micro ROS Agent should be established.

## Testing
Open a new terminal inside the docker container
```
docker exec -it ros2 bash
```
Publish continuous value using the following command
```
ros2 topic pub <topic-name> <msg-type> "<arg>"

e.g. ros2 topic pub /micro_ros_esp_subscriber std_msgs/msg/Int32 "{data:420}"
```
## References
The following links will provide further details each function used in the code:
- [Ethernet.h and EthernetUdp.h](https://docs.arduino.cc/libraries/ethernet/#Ethernet%20Class)
- [SPI.begin() function](https://docs.arduino.cc/language-reference/en/functions/communication/SPI/begin/)
- [Custom Transport for eProsima Micro XRCE-DDS](https://micro-xrce-dds.docs.eprosima.com/en/latest/transport.html#custom-transport)

Other links:
- [ESP32-S3-ETH](https://www.waveshare.com/esp32-s3-eth.htm)

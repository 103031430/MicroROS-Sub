#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <MicroROS_Transport.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>


// Define W5500 Ethernet Chip Pins
#define W5500_CS    14    // CS (Chip Select) PIN
#define W5500_RST   9     // Reset PIN
#define W5500_INT   10    // Interrupt PIN 
#define W5500_MISO  12    // MISO PIN
#define W5500_MOSI  11    // MOSI PIN
#define W5500_SCK   13    // Serial Clock PIN


// Define ROS entities
rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;
EthernetUDP udp;


// Define Functions
void error_loop();                                                                          
void timer_callback(rcl_timer_t * timer, int64_t last_call_time);
void CreateSubscriber();

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}     // Checks for Errors in Micro ROS Setup
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}              // 


// Network Configuration
byte esp_mac[] = { 0xDE, 0xAD, 0xAF, 0x91, 0x3E, 0xD7 };    // Mac address of ESP32
IPAddress esp_ip(192, 168, 0, 200);                         // IP address of ESP32
IPAddress dns(192, 168, 0, 1);                              // DNS Server (Modify if necessary)
IPAddress gateway(192, 168, 0, 1);                          // Default Gateway (Modify if necessary)
IPAddress agent_ip(192, 168, 0, 10);                        // IP address of Micro ROS agent        
size_t agent_port = 8888;                                   // Micro ROS Agent Port Number


// ROS Node Configurations
const char* NodeName = "micro_ros_esp_node";
const char* SubscriberTopic = "micro_ros_esp_subscriber";
const int ExecutorTimeout = 100;  // ms


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Ethernet Connection... ");


  // Initialize SPI with custom pin configuration
  SPI.begin(W5500_SCK, W5500_MISO, W5500_MOSI, W5500_CS);


  // Select CS PIN and initialize Ethernet with static IP settings (Selecting CS PIN required for ESP32 as the ardiuno default is different)
  Ethernet.init(W5500_CS);

  // Start Micro ROS Transport Connection
  set_microros_eth_transports(esp_mac, esp_ip, dns, gateway, agent_ip, agent_port);
  delay(2000);

  // Begin Publishing Message "data: <value>"
  CreateSubscriber();

}



void loop() {
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(ExecutorTimeout)));
}



// Error handle loop
void error_loop() {
  while(1) {
    delay(1000);
  }
}



// Implementation example:
void SubscriptionCallback(const void * msgin)
{
  // Cast received message to used type
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;

  // Process message
  Serial.print("[SUB] Received: ");
  Serial.println(msg->data);
}


// Initialising the ROS Node and and sending messages out from a Publisher
void CreateSubscriber() {

  // Initialize micro-ROS allocator
  allocator = rcl_get_default_allocator();


  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));


  // Create node
  RCCHECK(rclc_node_init_default(&node, NodeName, "", &support));


  // create subscriber (Quality of Service: Best Effort)
  RCCHECK(rclc_subscription_init_best_effort(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    SubscriberTopic)
  );

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &SubscriptionCallback, ON_NEW_DATA);)

}
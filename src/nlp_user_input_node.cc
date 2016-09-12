/**
 @file    nlp_user_input_node.cc
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Sep 9, 2016
 @brief   Defines the NlpUserInputNode class
 */

#include <xpp/ros/nlp_user_input_node.h>
#include <xpp/ros/ros_helpers.h>
#include <xpp/ros/marker_array_builder.h>

namespace xpp {
namespace ros {

NlpUserInputNode::NlpUserInputNode ()
{
  ::ros::NodeHandle n;
  goal_key_sub_ = n.subscribe("/keyboard/keydown", 1,
                                &NlpUserInputNode::CallbackKeyboard, this);

  goal_state_pub_ = n.advertise<StateMsg>("goal_state", 1);
  rviz_publisher_ = n.advertise<visualization_msgs::MarkerArray>("optimization_fixed", 1);
  get_goal_srv_   = n.advertiseService("get_goal_state", &NlpUserInputNode::GetGoalService, this);

  // Get Starting goal state from server
  goal_cog_.p.x() = RosHelpers::GetDoubleFromServer("/xpp/goal_state_x");
  goal_cog_.p.y() = RosHelpers::GetDoubleFromServer("/xpp/goal_state_y");

  // publish values once initially
  goal_state_pub_.publish(RosHelpers::XppToRos(goal_cog_));
}

NlpUserInputNode::~NlpUserInputNode ()
{
  // TODO Auto-generated destructor stub
}

void
NlpUserInputNode::CallbackKeyboard (const keyboard::Key& msg)
{
  const static double dx = 0.1;
  const static double dy = 0.05;

  switch (msg.code) {
    case msg.KEY_LEFT:
      goal_cog_.p.x() += dx;
      break;
    case msg.KEY_RIGHT:
      goal_cog_.p.x() -= dx;
      break;
    case msg.KEY_UP:
      goal_cog_.p.y() -= dy;
      break;
    case msg.KEY_DOWN:
      goal_cog_.p.y() += dy;
      break;
    case msg.KEY_RETURN:
      goal_state_pub_.publish(RosHelpers::XppToRos(goal_cog_));
      ROS_INFO_STREAM("Goal state set to " << goal_cog_.Get2D().p.transpose() << ".");
      break;
    default:
      break;
  }

  ROS_INFO_STREAM("Set goal state to " << goal_cog_.Get2D().p.transpose() << "?");

  // send out goal state to rviz
  visualization_msgs::MarkerArray msg_rviz;
  MarkerArrayBuilder msg_builder_;
  msg_builder_.AddPoint(msg_rviz, goal_cog_.Get2D().p, "goal",
                        visualization_msgs::Marker::CUBE);
  rviz_publisher_.publish(msg_rviz);
}

bool
NlpUserInputNode::GetGoalService (GoalSrv::Request& req, GoalSrv::Response& res)
{
  res.state = RosHelpers::XppToRos(goal_cog_);
  return true;
}

} /* namespace ros */
} /* namespace xpp */
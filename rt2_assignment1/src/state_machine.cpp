#include "ros/ros.h"
#include "rt2_assignment1/Command.h"
#include "rt2_assignment1/RandomPosition.h"
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <rt2_assignment1/PlanningAction.h>

bool start = false;
bool stop = false;

int state = 0;



bool user_interface(rt2_assignment1::Command::Request &req, rt2_assignment1::Command::Response &res){
    if (req.command == "start"){
    	state = 1;
    }
    else {
    	state = 4;
    }
    return true;
}

int main(int argc, char **argv)
{
   ros::init(argc, argv, "state_machine");
   ros::NodeHandle n;
   ros::ServiceServer service= n.advertiseService("/user_interface", user_interface);
   ros::ServiceClient client_rp = n.serviceClient<rt2_assignment1::RandomPosition>("/position_server");
   actionlib::SimpleActionClient<rt2_assignment1::PlanningAction> ac("/go_to_point", true);
   
   rt2_assignment1::RandomPosition rp;
   rp.request.x_max = 5.0;
   rp.request.x_min = -5.0;
   rp.request.y_max = 5.0;
   rp.request.y_min = -5.0;
   rt2_assignment1::PlanningGoal goal;
     //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){ // timeout can be set also for the waiforserrver
        ROS_INFO("Waiting for the go_to_point action server to come up");
    }
   
   while(ros::ok()){
       	ros::spinOnce();
       	switch (state){
           	case 1:
                client_rp.call(rp);
                goal.x = rp.response.x;
            	goal.y = rp.response.y;
               	goal.theta = rp.response.theta;
               	std::cout << "\nGoing to the position: x= " << goal.x << " y= " << goal.y << " theta = " << goal.theta << std::endl;
               	ac.sendGoal(goal);
               	state = 0;
               	break;
            case -1:
                ac.cancelGoal();
                state = 0;
                break;
                
              
                if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
                    ROS_INFO("Pose goal reached");
                    state = 1;
                }
                else if(ac.getState() == actionlib::SimpleClientGoalState::PREEMPTED){
                    ROS_INFO("Goal canceled");
                    state = 0;
                }
        }
   }
   return 0;
}

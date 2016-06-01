#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include "simple_navigation_goals_node.h"

readData::readData(string filename)
{
    std::ifstream fin(filename.c_str(), std::ios::in);
    if(!fin.is_open())
    {
        cout<<"cannot open file"<<endl;
    }

    istringstream istr;

    double oneDimension;
    vector<double> dataPointVec;
    string str;

     while(getline(fin,str))
     {
        istr.str(str);
        while(istr>>oneDimension)
        {
            dataPointVec.push_back(oneDimension);
        }
        allDataPointsVec.push_back(dataPointVec);
        dataPointVec.clear();
        istr.clear();
        str.clear();
     }
     fin.close();

    int numOfDimensions=allDataPointsVec[0].size();
    int numOfElements=allDataPointsVec.size();

    for(int i=0; i<numOfElements; i++)
    {
        for(int j=0; j<numOfDimensions; j++)
        {
            cout<<"The joystick and VO values "<<i<<" "<<j<<"t"<<setprecision(20)<<"value is "<<allDataPointsVec[i][j]<<endl;
            time.push_back(allDataPointsVec[i][0]);
            fb_joy.push_back(allDataPointsVec[i][1]);
            lr_joy.push_back(allDataPointsVec[i][2]);
            linear_vel.push_back(allDataPointsVec[i][3]);
            angular_vel.push_back(allDataPointsVec[i][4]);
        }

    }

}


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "base_link";
  goal.target_pose.header.stamp = ros::Time::now();

  double velocity = 1.5;
  double t=1.0;

  double t_tmp=0.0625;

  string data_loc="/home/lci/workspace/wheelchairTest/velocity1.txt";
  readData read_data(data_loc);


  double position=0.0;

  for(int i=0; i<read_data.linear_vel.size(); i++)
  {
    position=position+read_data.linear_vel[i]*t_tmp;
  }

  cout<<"position is "<<position<<endl;
  goal.target_pose.pose.position.x = position+6;
  //goal.target_pose.pose.position.y = 1.0;
  goal.target_pose.pose.orientation.w = 1.0;

  ROS_INFO("Sending goal");
  ac.sendGoal(goal);

  ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    if(goal.target_pose.pose.position.x>0)
    {
        ROS_INFO("Hooray, the base moved [%G] meter forward", (double)goal.target_pose.pose.position.x);
    }
    else if(goal.target_pose.pose.position.x==0)
    {
        ROS_INFO("The base does not move forward or backward");
    }
    else
    {
        ROS_INFO("Hooray, the base moved [%G] meter backward", (double)goal.target_pose.pose.position.x);
    }

    if(goal.target_pose.pose.position.y>0)
    {
        ROS_INFO("Hooray, the base moved [%G] meter right", (double)goal.target_pose.pose.position.y);
    }
    else if(goal.target_pose.pose.position.y==0)
    {
        ROS_INFO("The base does not move left or right");
    }
    else
    {
        ROS_INFO("Hooray, the base moved [%G] meter left", (double)goal.target_pose.pose.position.y);
    }

  }
  else
    ROS_INFO("The base failed to move as indicated for some reason");

  return 0;
}

/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2018, Hamburg University.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Hamburg University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/*
 * Capability to execute trajectories through a ROS action
 * that were generated by the MoveIt Task Constructor.
 *
 * Author: Michael 'v4hn' Goerner
 * */

#pragma once

#include <moveit/move_group/move_group_capability.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include <moveit_task_constructor_msgs/action/execute_task_solution.hpp>

#include <memory>

namespace move_group {

class ExecuteTaskSolutionCapability : public MoveGroupCapability
{
public:
	ExecuteTaskSolutionCapability();

	void initialize() override;

private:
	using ExecuteTaskSolutionAction = moveit_task_constructor_msgs::action::ExecuteTaskSolution;
	using ActionServerType = rclcpp_action::Server<ExecuteTaskSolutionAction>;
	bool constructMotionPlan(const moveit_task_constructor_msgs::msg::Solution& solution,
	                         plan_execution::ExecutableMotionPlan& plan);

	void execCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<ExecuteTaskSolutionAction>>& goal_handle);

	rclcpp_action::CancelResponse
	preemptCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<ExecuteTaskSolutionAction>>& goal_handle);

	/** Only accept the goal if we aren't executing one */
	rclcpp_action::GoalResponse handleNewGoal(const rclcpp_action::GoalUUID& /*uuid*/,
	                                          const ExecuteTaskSolutionAction::Goal::ConstSharedPtr& /*goal*/) const {
		if (last_goal_future_.valid() &&
		    last_goal_future_.wait_for(std::chrono::seconds::zero()) != std::future_status::ready)
			return rclcpp_action::GoalResponse::REJECT;
		return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
	}

	ActionServerType::SharedPtr as_;
	std::future<void> last_goal_future_;
};

}  // namespace move_group

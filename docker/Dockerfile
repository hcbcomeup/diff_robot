FROM osrf/ros:humble-desktop-full

RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-humble-ros2-control \
    ros-humble-ros2-controllers \
    ros-humble-gazebo-ros-pkgs \
    ros-humble-xacro \
    ros-humble-joint-state-publisher-gui \
    python3-colcon-common-extensions \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN rosdep init && rosdep update

ENV DISPLAY=:0
ENV QT_X11_NO_MITSHM=1

RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc

WORKDIR /ws_robot
COPY . .

RUN rosdep install --from-paths src --ignore-src -r -y || true
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && colcon build --symlink-install"
RUN echo "source /ws_robot/install/setup.bash" >> /root/.bashrc

CMD ["bash"]
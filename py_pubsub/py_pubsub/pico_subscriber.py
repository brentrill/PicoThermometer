#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String

# GUI Imports
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

import sys
from threading import Thread
from rclpy.executors import MultiThreadedExecutor


class PicoSubscriberNode(Node):

    def __init__(self, ui):
        super().__init__("pico_subscriber")
        self.pico_subscriber = self.create_subscription(
            String, "/pico_publisher", self.pico_callback, 10
            )
        
        # Taking the GUI window into the node
        self.ui = ui

    def pico_callback(self, msg: String):
        parsed = msg.data.split()
        number, temp, volt = parsed[0], parsed[2], parsed[5]

        self.ui.show_result(number, temp, volt)

        self.get_logger().info(parsed[2] + ' ' + parsed[5])


class Window(QMainWindow):

    def __init__(self):
        super().__init__()

        xpos, ypos, width, height = 200, 200, 300, 200
        self.setWindowTitle("Pico Thermometer")
        self.setGeometry(xpos, ypos, width, height)

        self.UiComponents()

    def UiComponents(self):
        self.label = QLabel(self)
        self.label.move(25, 0)
        self.label.resize(400, 200)
        self.label.setFont(QFont('Arial', 14))

        self.label.setText("Initializing . . .")

    def show_result(self, number, temp, volt):
        deg = '\u00b0'
        msgFormat = f"Msg {number}:  {temp} {deg}F   {volt} V"
        self.label.setText(msgFormat)


def main(args=None):
    rclpy.init(args=args)

    app = QApplication(sys.argv)
    window = Window()
    
    # Passing the window object into the node so we can update it
    node = PicoSubscriberNode(window)
    executor = MultiThreadedExecutor()
    executor.add_node(node)

    # Start ROS2 node on separate thread
    thread = Thread(target=executor.spin)
    thread.start()
    node.get_logger().info("Spinned ROS2 Node . . .")

    # Run GUI on main thread
    try:
        window.show()
        sys.exit(app.exec_())
    finally:
        node.get_logger().info("Shutting Down ROS2 Node . . .")
        node.destroy_node()
        executor.shutdown()


if __name__ == '__main__':
    main()

import os
import sys
import time
import math
import threading
import numpy as np

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
CONFIG_DIR = os.path.join(SCRIPT_DIR, "bindings_test_track.json")

sys.path.append(os.path.join(SCRIPT_DIR, '../build/'))

import pyspatzsim as ps

def main():

    settings = ps.Settings()
    settings.load()

    scene = ps.Scene(CONFIG_DIR)
    scene.paused = False
    scene.car.vesc.velocity = 1.0
    scene.car.vesc.steering_angle = 0.4
    scene.car.main_camera.image_width = 256
    scene.car.main_camera.image_height = 20

    loop = ps.Loop(800, 600, settings)
    loop.step(scene, 0.1)

    time.sleep(1)

    a = np.array([3, 2, 1])
    b = a

    b[0] = 4

    scene.car.pose.scale *= 1

    loop.step(scene, 0.5)

    scene.car.main_camera.fov = math.pi / 2 * 1.3

    while True:

        scene.car.vesc.velocity = 1.0
        scene.car.vesc.steering_angle = 0.4

        loop.step(scene, 0.05)

        time.sleep(0.1)


if __name__ == "__main__":

    main()

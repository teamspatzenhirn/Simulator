import os
import sys
import time
import threading

sys.path.append(os.path.join(os.path.abspath(
    os.path.dirname(__file__)), '../build/',))

import pyspatzsim as ps

def main():

    settings = ps.Settings()
    settings.load()

    scene = ps.Scene('/home/ruof/Projects/SpatzSim/tests/barred_area.json')
    scene.paused = False
    scene.car.vesc.velocity = 1.0
    scene.car.vesc.steering_angle = 0.4
    scene.car.main_camera.image_width = 256
    scene.car.main_camera.image_height = 256

    loop = ps.Loop(800, 600, settings)
    loop.step(scene, 0.1)

    time.sleep(1)

    loop.step(scene, 0.5)

    time.sleep(10)

if __name__ == "__main__":

    main()

import os
import sys
import time
import math

import numpy as np

import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
SCENE_PATH = os.path.join(SCRIPT_DIR, "bindings_test_track.json")

try:
    import pyspatzsim as ps
except ImportError:
    print("Module pyspatzsim could not be imported.")
    print("Try adding \"SpatzSim/build/\" to environment variable PYTHONPATH.")
    print()
    raise

def test_loop():
    """
    Simple test function, to show how SpatzSim can be run from python code.
    """

    # The Loop object holds the main simulator logic.

    loop = ps.Loop()

    # A Scene object holds all state of the simulated world.
    # The constructor takes the path to a stored "scene.json" file.

    scene = ps.Scene(SCENE_PATH)

    # With the loop(...) function the main loop of the simulator is started.
    # This function blocks until the simulator window is closed.

    loop.loop(scene)

def test_driving():
    """
    Shows how simple driving can be realized.
    """

    loop = ps.Loop()
    scene = ps.Scene(SCENE_PATH)

    # By settings the motor controller velocity and steering_angle
    # in the scene object the car can be moved.

    scene.car.vesc.velocity = 1.0
    scene.car.vesc.steering_angle = 0.4

    loop.loop(scene)

def test_step():
    """
    Shows how the simulation can be advanced in steps.
    """

    loop = ps.Loop()
    scene = ps.Scene(SCENE_PATH)

    # Because the loop.loop() function only returns after the
    # simulation is already finished, it is not very useful,
    # if we want to iteratively interact with the simulation.

    # To support stepwise interaction the SpatzSimulation includes
    # the loop.step(...) function. Calling this function advances the
    # simulation state by the given amount of seconds, then renders
    # new frames and finally returns.

    # Calling loop.step(scene, 0.0) does not advance the simulation and
    # simply renders the current simulation state.

    loop.step(scene, 0.0)
    loop.step(scene, 0.0)

    # Calling loop.step(scene, 1.0) advances the simulation by 1s and
    # renders new frames for the simulation state at t=1.

    input("Press any key to advance simulation by 1s ")

    scene.car.vesc.velocity = 1.0

    loop.step(scene, 1.0)

    # Because the car velocity as been set to 1m/s the car should
    # have travelled 1 meter after the simulation is advanced by 1s.

    input("Press any key to exit ")

def test_fast_frame_retrieval():
    """
    This function demonstrates the ability of spatzsim to
    generate a relatively long frame sequence in a short
    amount of time.
    """

    loop = ps.Loop()

    scene = ps.Scene(SCENE_PATH)
    scene.car.main_camera.image_width = 400
    scene.car.main_camera.image_height = 300

    # The first frame retrieved from the simulator will
    # always be black, as get_previous_frame() returns the
    # previously rendered frame only. Therefore we get the
    # frame here, before the actual loop.
    loop.step(scene, 1)
    black_frame = loop.get_previous_frame(scene)

    start = time.time()

    frames = []

    # Simulate 60 * 60 + 1 = 3601 frames. Which is a full minute
    # of simulation time at 60 FPS. Plus 1 frame to account for
    # get_frame returning the rendered frame for step i-1.
    for i in range(60 * 60 + 1):

        # Lets the simulated car drive in a circle
        # (counter-clockwise) with a speed of 1 m/s.
        scene.car.vesc.velocity = 1.0
        scene.car.vesc.steering_angle = 0.3

        loop.step(scene, 1/60)

        frame = loop.get_previous_frame(scene)

        frames.append(frame)

    print("Simulated {} frames in {} seconds".format(
        len(frames), time.time() - start))

    for i, frame in enumerate(frames[:60]):
        plt.clf()
        plt.imshow(frame, cmap='gray')
        plt.title('Frame sample nr. {}/59'.format(i))
        plt.pause(0.0016)

    plt.show()

def test_track_retrieval():
    """
    This function tests the sampling of a track built
    in spatzsim as discrete 2d points.
    """

    # As the track is completely stored inside the scene,
    # only the scene needs to be loaded to get the track points.

    scene = ps.Scene(SCENE_PATH)

    # Using the scene.get_path_through_track(...) function a track can
    # be sampled as discrete 2d points at a given interval (0.5).

    track_points = scene.get_path_through_track(0.5)

    # Track points are returned as a list of tuples:
    # track_points = [(x, y), ...]

    # Plot the track for visualization:

    xs, ys = list(zip(*track_points))

    plt.plot(xs, ys, "-o")

    for i, p in enumerate(track_points):
        plt.gca().annotate(str(i), p)

    #plt.axis([-10, 10, -10, 10])
    plt.gca().set_aspect('equal', adjustable='box')
    plt.show()
        
if __name__ == "__main__":

    #test_loop()
    #test_driving()
    #test_step()
    #test_fast_frame_retrieval()
    test_track_retrieval()

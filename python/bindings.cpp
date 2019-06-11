#include <string>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "scene/Settings.h"
#include "Loop.h"
#include "Scene.h"
#include "helpers/Pose.h"
#include "scene/Car.h"

PYBIND11_MODULE(pyspatzsim, m) {

    pybind11::class_<Settings>(m, "Settings")
        .def(pybind11::init())
        .def("load", &Settings::load);

    pybind11::class_<Loop>(m, "Loop")
        .def(pybind11::init<int, int, Settings>())
        .def("loop", &Loop::loop)
        .def("step", &Loop::step);

    pybind11::class_<Scene>(m, "Scene")
        .def(pybind11::init<std::string>())
        .def_readwrite("paused", &Scene::paused)
        .def_readwrite("car", &Scene::car);

    pybind11::class_<Pose>(m, "Pose")
        .def(pybind11::init())
        .def_property("position",
            [](Pose& pose) -> pybind11::array_t<float> {
                return pybind11::array_t<float>(
                        3, 
                        glm::value_ptr(pose.position), 
                        pybind11::cast(pose));
            },
            [](Pose& pose, pybind11::array_t<float> value) {
                pose.position = glm::make_vec3(value.data());
            }
        )
        .def_property("rotation",
            [](Pose& pose) -> pybind11::array_t<float> {
                return pybind11::array_t<float>(
                        4, 
                        glm::value_ptr(pose.rotation), 
                        pybind11::cast(pose));
            },
            [](Pose& pose, pybind11::array_t<float> value) {
                pose.rotation = glm::make_quat(value.data());
            }
        )
        .def_property("scale",
            [](Pose& pose) -> pybind11::array_t<float> {
                return pybind11::array_t<float>(
                        3, 
                        glm::value_ptr(pose.scale), 
                        pybind11::cast(pose));
            },
            [](Pose& pose, pybind11::array_t<float> value) {
                pose.scale = glm::make_vec3(value.data());
            }
        )
        .def("get_degrees",
            [](Pose& pose) -> pybind11::array_t<float> {
                return pybind11::array_t<float>(
                        3, 
                        glm::value_ptr(pose.getEulerAngles()));
            }
        )
        .def("set_degrees", 
            [](Pose& pose, float roll, float pitch, float yaw) {
                pose.setEulerAngles(glm::vec3(roll, pitch, yaw));
            }
        )
        .def("get_radians",
            [](Pose& pose) -> pybind11::array_t<float> {
                return pybind11::array_t<float>(
                        3, 
                        glm::value_ptr(pose.getEulerAngles() * (float)(M_PI / 180)));
            }
        )
        .def("set_radians", 
            [](Pose& pose, float roll, float pitch, float yaw) {
                float c = 180 / M_PI;
                pose.setEulerAngles(glm::vec3(roll * c, pitch * c, yaw * c));
            }
        );

    pybind11::class_<Car>(m, "Car")
        .def(pybind11::init())
        .def_readwrite("pose", &Car::modelPose)
        .def_readwrite("vesc", &Car::vesc)
        .def_readwrite("main_camera", &Car::mainCamera);

    pybind11::class_<Car::MainCamera>(m, "MainCamera")
        .def(pybind11::init())
        .def_readwrite("pose", &Car::MainCamera::pose)
        .def_readwrite("fov", &Car::MainCamera::fovy)
        .def_readwrite("image_width", &Car::MainCamera::imageWidth)
        .def_readwrite("image_height", &Car::MainCamera::imageHeight);

    pybind11::class_<Car::Vesc>(m, "Vesc")
        .def(pybind11::init())
        .def_readwrite("velocity", &Car::Vesc::velocity)
        .def_readwrite("steering_angle", &Car::Vesc::steeringAngle);
}

#include <string>
#include <iostream>
#include <thread>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "Storage.h"
#include "scene/Settings.h"
#include "Loop.h"
#include "scene/Scene.h"
#include "helpers/Pose.h"
#include "scene/Car.h"
#include "Storage.h"

PYBIND11_MODULE(pyspatzsim, m) {

    pybind11::class_<Settings>(m, "Settings")
        .def(pybind11::init())
        .def("load", [](Settings& self) { storage::load(self); })
        .def_readwrite("resource_path", &Settings::resourcePath)
        .def_readwrite("simulation_speed", &Settings::simulationSpeed);

    pybind11::class_<Loop>(m, "Loop")
        .def(pybind11::init<Settings>(), pybind11::arg("settings") = Settings())
        .def("loop", &Loop::loop)
        .def("step", &Loop::step)
        .def("get_previous_frame",
            [](Loop& loop, Scene& scene) {

                //@Optimization By using the array_t type we only pass a 
                //pointer (a numpy array to be precise) to the python world
                //and avoid copying the actual frame data.
                std::vector<size_t> dims = {
                        (long unsigned int)scene.car.mainCamera.imageHeight,
                        (long unsigned int)scene.car.mainCamera.imageWidth,
                        3
                    };

                pybind11::array_t<unsigned char> frame(dims, nullptr);

                glBindFramebuffer(GL_FRAMEBUFFER, loop.car.frameBuffer.id);

                loop.pythonMainCameraCapture.capture(
                        frame.mutable_data(), 
                        scene.car.mainCamera.imageWidth,
                        scene.car.mainCamera.imageHeight,
                        3,
                        GL_RGB, 
                        GL_UNSIGNED_BYTE);

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // Because the images are downloaded inverted into memory
                // the resulting numpy array must be flipped.
                // Which is done here by calling the flip numpy function.
                pybind11::object np = pybind11::module::import("numpy");
                frame = np.attr("flip")(frame, 0);

                return frame;
            }
        );

    pybind11::class_<Scene>(m, "Scene")
        .def(pybind11::init())
        .def(pybind11::init<std::string>())
        .def_readwrite("paused", &Scene::paused)
        .def_readwrite("car", &Scene::car)
        .def_readwrite("tracks", &Scene::tracks)
        .def_readwrite("rules", &Scene::rules);

    pybind11::class_<Scene::Rules>(m, "Rules")
        .def(pybind11::init())
        .def_readwrite("on_track", &Scene::Rules::onTrack)
        .def_readwrite("is_colliding", &Scene::Rules::isColliding);

    pybind11::class_<Tracks>(m, "Tracks")
        .def("get_path",
            [](Tracks& tracks, float distBetweenPoints) {

                std::vector<glm::vec2> pathPoints = tracks.getPath(distBetweenPoints);

                std::vector<pybind11::tuple> tupelPathPoints;

                for (glm::vec2& p : pathPoints) {
                    tupelPathPoints.push_back(pybind11::make_tuple(-p.x, p.y));
                }
                
                return tupelPathPoints;
            }
        );

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
        .def_property("x", 
            [](Car& car) { 
                return -car.modelPose.position.x; 
            },
            [](Car& car, float x) {
                return car.modelPose.position.x = -x;
            })
        .def_property("y", 
            [](Car& car) {
                return car.modelPose.position.z;
            },
            [](Car& car, float y) {
                return car.modelPose.position.z = y;
            })
        .def_property("theta", 
            [](Car& car) { 
                return car.modelPose.getEulerAngles().y 
                    * (float)(M_PI / 180)
                    + (float)(M_PI / 2);
            },
            [](Car& car, float theta) { 
                glm::vec3 angles = car.modelPose.getEulerAngles();
                angles.y = (float)(180 / M_PI) * (theta - (float)(M_PI / 2));
                car.modelPose.setEulerAngles(angles);
            }
        )
        .def_property("velocity", 
            [](Car& car) {
                return car.vesc.velocity;
            },
            [](Car& car, float velocity) { 
                return car.vesc.velocity = velocity;
            })
        .def_property("steer_angle", 
            [](Car& car) {
                return car.vesc.steeringAngle;
            },
            [](Car& car, float steerAngle) { 
                return car.vesc.steeringAngle = steerAngle;
            })
        .def_readwrite("model_pose", &Car::modelPose)
        .def_readwrite("main_camera", &Car::mainCamera);

    pybind11::class_<Car::MainCamera>(m, "MainCamera")
        .def(pybind11::init())
        .def_readwrite("pose", &Car::MainCamera::pose)
        .def_readwrite("fov", &Car::MainCamera::fovy)
        .def_readwrite("noise", &Car::MainCamera::noise)
        .def_readwrite("image_width", &Car::MainCamera::imageWidth)
        .def_readwrite("image_height", &Car::MainCamera::imageHeight);

    pybind11::class_<Car::Vesc>(m, "Vesc")
        .def(pybind11::init())
        .def_readwrite("velocity", &Car::Vesc::velocity)
        .def_readwrite("steering_angle", &Car::Vesc::steeringAngle);
}

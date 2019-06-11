#include <string>
#include <iostream>
#include <thread>

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
        .def("load", &Settings::load)
        .def_readwrite("resource_path", &Settings::resourcePath);

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
                pybind11::object np = pybind11::module::import("numpy");
                frame = np.attr("flip")(frame, 0);

                return frame;
            }
        );

    pybind11::class_<Scene>(m, "Scene")
        .def(pybind11::init<std::string>())
        .def_readwrite("paused", &Scene::paused)
        .def_readwrite("car", &Scene::car)
        .def("get_path_through_track", 
            [](Scene& scene, float distBetweenPoints) {

                std::shared_ptr<ControlPoint> cp = scene.tracks.getTracks().front();

                for (const std::shared_ptr<ControlPoint>& c : scene.tracks.getTracks()) {
                    if (c->tracks.size() == 1) {
                        cp = c;
                        break;
                    }
                }

                std::vector<std::shared_ptr<ControlPoint>> visitedCps;
                std::vector<std::shared_ptr<TrackBase>> visitedTracks;

                std::vector<glm::vec2> pathPoints;

                auto sampleLine = [&](glm::vec2 start, glm::vec2 end) {
                    float len = glm::length(end - start);
                    glm::vec2 dir = glm::normalize(end - start);

                    for(float i = 0; i < len; i += distBetweenPoints) {
                        glm::vec2 point = start + i * dir;
                        point.x = -point.x;
                        pathPoints.push_back(point);
                    }
                };

                while (std::find(visitedCps.begin(), visitedCps.end(), cp) == visitedCps.end()) {
                    visitedCps.push_back(cp);

                    if (cp->tracks.size() == 0) {
                      break;
                    }

                    for (std::shared_ptr<TrackBase>& track : cp->tracks) {

                        if (std::find(visitedTracks.begin(), visitedTracks.end(), track)
                                != visitedTracks.end()) {
                            continue; 
                        }

                        visitedTracks.push_back(track);

                        if(nullptr != dynamic_cast<TrackLine*>(track.get())) {
                          TrackLine& t = (TrackLine&)*track;

                          std::shared_ptr<ControlPoint> other = t.end.lock();
                          if(t.end.lock() == cp) {
                            other = t.start.lock();
                          }

                          sampleLine(cp->coords, other->coords);

                          cp = other;
                        }
                        else if (nullptr != dynamic_cast<TrackArc*>(track.get())) {
                          TrackArc& t = (TrackArc&)*track;

                          std::shared_ptr<ControlPoint> other = t.end.lock();
                          if(other == cp) {
                            other = t.start.lock();
                          }

                          glm::vec2 start = t.start.lock()->coords;
                          glm::vec2 end = t.end.lock()->coords;

                          float angleStart = std::atan2(start.y - t.center.y, start.x - t.center.x);
                          float angleEnd = std::atan2(end.y - t.center.y, end.x - t.center.x);
                          float baseAngle = 0;
                          float angle = 0;

                          if (t.rightArc) {
                            baseAngle = angleStart;
                            angle = angleEnd - angleStart;
                          }
                          else {
                            baseAngle = angleEnd;
                            angle = angleStart - angleEnd;
                          }
                          if (angle < 0) {
                            angle += (float)(2 * M_PI);
                          }

                          float pointRatio = t.radius * angle / distBetweenPoints;

                          std::vector<glm::vec2> points;

                          for(float i = 0; i < pointRatio; i += 1) {
                            const float currentAngle = baseAngle + i * angle / pointRatio;

                            glm::vec2 point{
                                -(t.center.x + std::cos(currentAngle) * t.radius),
                                t.center.y + std::sin(currentAngle) * t.radius };

                            points.push_back(point);
                          }
                          
                          if (other == t.end.lock() && !t.rightArc) {
                            std::reverse(points.begin(), points.end());
                          }

                          for (glm::vec2& p : points) { 
                              pathPoints.push_back(p);
                          }

                          cp = other;
                        } else if (nullptr != dynamic_cast<TrackIntersection*>(track.get())) {

                          TrackIntersection& t = (TrackIntersection&)*track;

                          std::shared_ptr<ControlPoint> other = t.link1.lock();

                          if(t.link1.lock() == cp) {
                            other = t.link3.lock();
                          } else if (t.link2.lock() == cp) {
                            other = t.link4.lock();
                          } else if (t.link3.lock() == cp) {
                            other = t.link1.lock();
                          } else if (t.link4.lock() == cp) {
                            other = t.link2.lock();
                          }

                          sampleLine(cp->coords, other->coords);

                          cp = other;
                        }

                        break;
                    }
                }

                std::vector<pybind11::tuple> tupelPathPoints;

                for (glm::vec2& p : pathPoints) {
                    tupelPathPoints.push_back(pybind11::make_tuple(p.x, p.y));
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
        .def_readwrite("pose", &Car::modelPose)
        .def_readwrite("vesc", &Car::vesc)
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

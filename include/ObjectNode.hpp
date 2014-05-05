#ifndef PILOT_OBJECT_NODE_HPP_
#define PILOT_OBJECT_NODE_HPP_

#include <list>
#include <mutex>
#include <utility>
#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Quat>
#include <osg/Vec3d>

class ObjectNode : public osg::PositionAttitudeTransform {
    typedef std::list<std::pair<osg::Vec3d, double>>
        PositionBuffer;

    static const PositionBuffer::size_type PositionBufferSize;

public:
    ObjectNode()
        : new_position_set_(false),
          new_attitude_set_(false),
          linear_velocity_set_(false) {}

    void update(double dt);

    void set_new_position(const osg::Vec3d& position, double t);

    void set_new_attitude(const osg::Quat& attitude, double t);

    void set_new_model_node(osg::ref_ptr<osg::Node> model_node);

private:
    osg::Vec3d new_position_;
    bool new_position_set_;

    PositionBuffer position_buffer_;

    osg::Quat new_attitude_;
    bool new_attitude_set_;
    osg::Vec3d linear_velocity_;
    bool linear_velocity_set_;
    osg::ref_ptr<osg::Node> new_model_node_;
    std::mutex update_mutex_;
};

#endif
